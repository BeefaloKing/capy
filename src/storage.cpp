/* Storage.cpp
 */
#include "storage.hh"
#include "error.hh"
#include "utils.hh"
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

Storage::Storage(const std::string &directory, StorageMode mode) :
	dirPath(directory),
	mode(mode),
	indexFile(nullptr),
	treeFile(nullptr),
	cellSize(0),
	outputSize(0),
	diskSize(0)
{
	bool isEmpty = validateBaseDir();
	if ((mode == StorageMode::generate) && !isEmpty) // Generate must start with an empty directory
	{
		throwDirFull(directory);
	}
}

Storage::~Storage()
{
	closeSwaps();
	fclose(indexFile);
	fclose(treeFile);
}

bool Storage::validateBaseDir()
{
	struct stat dirStat = {};
	if (stat(dirPath.c_str(), &dirStat) == -1) // Directory does not exist
	{
		if (mkdir(dirPath.c_str()) == -1) // Look at my ugly nonportable code!
		{
			throwDirCreate(dirPath);
		}
	}

	bool isEmpty = true;
	DIR* baseDir = opendir(dirPath.c_str());
	dirent* entry;

	while ((entry = readdir(baseDir)) != nullptr)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue; // Skip self and parent entires "." and ".."
		}
		isEmpty = false;
		break;
	}

	closedir(baseDir);
	return isEmpty;
}

void Storage::openSwaps()
{
	size_t swapCount = 1 << outputSize; // Calculates 2^outputSize
	const char* fileMode = (mode == StorageMode::generate ? "w+b" : "rb");

	printf("Opening swap files.\n");
	fflush(stdout);

	for (size_t i = 0; i < swapCount; i++)
	{
		const std::string swapPath = dirPath + SWAP_PREFIX + std::to_string(i) + ".capy";
		FILE* swap = fopen(swapPath.c_str(), fileMode);
		if (swap == nullptr)
		{
			throwFileAccess(swapPath);
		}

		if (mode == StorageMode::generate)
		{
			// Number of cells * size of each cell / number of indexes
			uint64_t fileSize = (cellCount * diskSize) / swapCount;
			preallocateFile(swap, fileSize, swapPath);
		}

		swapFile.push_back(swap);
		swapSize.push_back(0); // Initialize count of entries in each swap file to 0
	}
}

void Storage::closeSwaps()
{
	while (!swapFile.empty())
	{
		fclose(swapFile.back());
		swapFile.pop_back();
		swapSize.pop_back();
	}

	// Delete temporary swap files used for sorting
	printf("Deleting swap files.\n");
	fflush(stdout);

	size_t swapCount = 1 << outputSize; // Calculates 2^outputSize
	for (size_t i = 0; i < swapCount; i++)
	{
		const std::string swapPath = dirPath + SWAP_PREFIX + std::to_string(i) + ".capy";
		remove(swapPath.c_str()); // We don't really care if this fails
	}
}

void Storage::openIndexFile()
{
	const char* fileMode = (mode == StorageMode::generate ? "w+b" : "rb");

	printf("Opening index file.\n");
	fflush(stdout);

	const std::string indexPath = dirPath + INDEX_NAME;
	indexFile = fopen(indexPath.c_str(), fileMode);
	if (indexFile == nullptr)
	{
		throwFileAccess(indexPath);
	}

	if (mode == StorageMode::generate)
	{
		uint64_t fileSize = cellCount * diskSize;
		preallocateFile(indexFile, fileSize, indexPath);
	}
}

void Storage::openTreeFile()
{
	const char* fileMode = (mode == StorageMode::generate ? "w+b" : "rb");

	printf("Opening tree file.\n");
	fflush(stdout);

	const std::string treePath = dirPath + TREE_NAME;
	treeFile = fopen(treePath.c_str(), fileMode);
	if (treeFile == nullptr)
	{
		throwFileAccess(treePath);
	}
}

void Storage::setConfig(size_t cellSize, size_t outputSize)
{
	this->cellSize = cellSize;
	cellCount = 1ll << cellSize; // Calculates 2^cellSize
	this->outputSize = outputSize;
	diskSize = (cellSize + 7) / 8; // Always round up instead of down

	printf("Writing configuration to disk.\n");
	fflush(stdout);

	const std::string configPath = dirPath + CONFIG_NAME;
	FILE* configFile = fopen(configPath.c_str(), "w");
	if (configFile == nullptr)
	{
		throwFileAccess(dirPath);
	}

	// Ensure that size on disk is platform agnostic
	uint64_t diskElements[] = {cellSize, outputSize, diskSize};
	size_t count = fwrite(diskElements, sizeof(diskElements), 1, configFile);
	if (count != 1)
	{
		throwFileWrite(configPath);
	}

	fclose(configFile);

	// Root node will always point to the entirety of the index array
	*tree.begin() = new StateSet {0, cellCount};
	// Initialize treeIndex to root
	treeIndex = 0;

	openSwaps();
	openIndexFile();
}

void Storage::preallocateFile(FILE* file, uint64_t fileSize, const std::string &filePath)
{
	printf("Preallocating %s for %s.\n", getHumanSize(fileSize).c_str(), filePath.c_str());
	fflush(stdout);

	if (fseeko64(file, fileSize - 1, SEEK_SET) != 0)
	{
		throwFileWrite(filePath);
	}
	fputc('\0', file); // Write past EOF to force allocation

	fseek(file, 0, SEEK_SET); // Reset position to begining of file
}

void Storage::writeSwap(uint64_t state, uint64_t output)
{
	// Also relies on little endian architecture
	if (fwrite(&state, diskSize, 1, swapFile.at(output)) != 1)
	{
		throwFileWrite(SWAP_PREFIX);
	}
	swapSize.at(output)++;
}

void Storage::mergeSwap()
{
	StateNode it = tree.begin() + treeIndex;

	uint64_t indexStart = (*it)->index;
	fseeko64(indexFile, indexStart, SEEK_SET); // Seek to index position pointed to by tree data

	// Here is where we killed the idea of having more than 1 bit of output at a time
	uint64_t leftSize = swapSize.at(0);
	uint64_t rightSize = swapSize.at(1);

	// If one child contains no states (and the other contains the same states as the parent),
	// then this particular bit of output does not contain any information.
	// If we recieved no information from the last P bits, where P is equal to the number of
	// states in our current set, then no further bits will provide information either.
	// Effectively, we are unable to determine the state of the machine,
	// but can determine the entire rest of the bitstream.
	// Early out, as there is no need to continue generating nodes down this path.
	bool earlyOut = false;
	if (leftSize == 0 || rightSize == 0)
	{
		earlyOut = true;

		StateNode ancestor = it;
		for (size_t i = 0; i < (*it)->length; i++)
		{
			ancestor = it.parent();
			if ((*ancestor)->length == (*it)->length)
			{
				continue;
			}
			else
			{
				// We recieved information less than P bits ago
				// It is not yet safe to early out
				earlyOut = false;
				break;
			}
		}
	}

	if (!earlyOut)
	{
		*it.left() = new StateSet {indexStart, leftSize};
		*it.right() = new StateSet {indexStart + leftSize, rightSize};
	}

	char entryBuffer[diskSize];
	for (size_t i = 0; i < swapSize.size(); i++)
	{
		fseeko64(swapFile.at(i), 0, SEEK_SET); // Return to begining of each swap file
		for (uint64_t j = 0; j < swapSize.at(i); j++)
		{
			if (fread(entryBuffer, diskSize, 1, swapFile.at(i)) != 1)
			{
				throwFileRead(SWAP_PREFIX);
			}

			if (fwrite(entryBuffer, sizeof(entryBuffer), 1, indexFile) != 1)
			{
				throwFileWrite(INDEX_NAME);
			}
		}
	}
}

bool Storage::advStateSet()
{
	while (tree.begin() + treeIndex != tree.end())
	{
		treeIndex++;

		StateNode it = tree.begin() + treeIndex;
		if (*it && (*it)->length != 0)	// Next valid StateSet found
		{
			StateSet* currentSet = *it;

			// Seek to indexFile position from the index specified by the current StateSet
			fseeko64(indexFile, currentSet->index, SEEK_SET);
			for (size_t i = 0; i < swapFile.size(); i++)
			{
				rewind(swapFile.at(i)); // Seek to begining of each swap file
				swapSize.at(i) = 0; // Reset count of swap entries to 0
			}

			return true;
		}
	}

	return false; // No next valid StateSet
}

size_t Storage::getDepth()
{
	size_t depth = 0;
	size_t dividend = treeIndex;
	while (dividend > 0)
	{
		dividend = (dividend - 1) / 2; // Iterate up parent nodes until we reach root
		depth++;
	}

	return depth;
}

bool Storage::getNextState(uint64_t &state)
{
	static size_t stateCount = 0;

	StateSet* currentSet = *(tree.begin() + treeIndex);
	if (stateCount < currentSet->length)
	{
		if (fread(&state, diskSize, 1, indexFile) != 1)
		{
			throwFileRead(INDEX_NAME);
		}

		stateCount++;
		return true;
	}

	// Reset stateCount to 0 when no next state in current set remains
	stateCount = 0;
	return false;
}
