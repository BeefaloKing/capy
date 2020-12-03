/* Storage.cpp
 */
#include "storage.hh"
#include "error.hh"
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
}

void Storage::openIndex()
{
	const char* fileMode = (mode == StorageMode::generate ? "w+b" : "rb");

	printf("Opening index file.\n");

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

void Storage::openTree()
{
	const char* fileMode = (mode == StorageMode::generate ? "w+b" : "rb");

	printf("Opening tree file.\n");

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

	openSwaps();
	openIndex();
	openTree();
}

void Storage::preallocateFile(FILE* file, uint64_t fileSize, const std::string &filePath)
{
	// TODO: Human readable sizes
	printf("Preallocating %llu bytes for %s.\n", fileSize, filePath.c_str());

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

// TODO: Determine position in tree and only overwrite that portion of the index
void Storage::mergeSwap()
{
	char entryBuffer[diskSize];

	// uint64_t indexPosition = tree...
	fseeko64(indexFile, 0, SEEK_SET); // Seek to index position pointed to by tree data

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

	// Clear swap files
	closeSwaps();
	openSwaps();
}
