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
	diskSize(0),
	finishedSorting(0)
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

void Storage::setConfig(size_t cellSize, size_t outputSize)
{
	this->cellSize = cellSize;
	cellCount = 1ll << cellSize; // Calculates 2^cellSize
	this->outputSize = outputSize;
	diskSize = (cellSize + 7) / 8; // Always round up instead of down

	// Root node will always point to the entirety of the index array
	root = StateSet {0, cellCount};
	// Initialize currentSet to root
	currentSet = root.begin();

	openSwaps();
	openIndexFile();
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
	uint64_t indexStart = currentSet->index;
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

		StateSet* ancestor = currentSet;
		for (uint64_t i = 0; i < currentSet->length; i++)
		{
			ancestor = ancestor->parent;
			if (ancestor && ancestor->length == currentSet->length)
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

	if (earlyOut) // We have finised sorting as many elements are in currentSet
	{
		finishedSorting += currentSet->length;
	}
	else
	{
		currentSet->left = (leftSize > 0) ?
			new StateSet {indexStart, leftSize, currentSet} : nullptr;
		currentSet->right = (rightSize > 0) ?
			new StateSet {indexStart + leftSize, rightSize, currentSet} : nullptr;
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
	while (++currentSet != root.end())
	{
		if (currentSet->length != 0) // Next valid StateSet found
		{
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

size_t Storage::getSetDepth()
{
	return currentSet.getDepth();
}

bool Storage::getNextState(uint64_t &state)
{
	static uint64_t stateCount = 0;

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

double Storage::getSortProgress()
{
	return (double) finishedSorting / (double) root.length;
}
