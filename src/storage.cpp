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
	while (!index.empty())
	{
		fclose(index.back());
		index.pop_back();
	}
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

void Storage::openIndexes(StorageMode mode)
{
	size_t indexCount = 1 << outputSize; // Calculates 2^outputSize
	const char* fileMode = mode == StorageMode::generate ? "wb" : "rb";

	printf("Opening index files.\n");

	for (size_t i = 0; i < indexCount; i++)
	{
		const std::string indexPath = dirPath + INDEX_PREFIX + std::to_string(i) + ".capy";
		FILE* indexFile = fopen(indexPath.c_str(), fileMode);
		if (indexFile == nullptr)
		{
			throwFileAccess(indexPath);
		}

		if (mode == StorageMode::generate)
		{
			// Calculates index file size
			// Number of cells * size of each cell / number of indexes
			uint64_t fileSize = (cellCount * diskSize) / indexCount;
			preallocateFile(indexFile, fileSize, indexPath);
		}

		index.push_back(indexFile);
	}
}

void Storage::setConfig(size_t cellSize, size_t outputSize)
{
	this->cellSize = cellSize;
	cellCount = 1ll << cellSize; // Calculates 2^cellSize
	this->outputSize = outputSize;
	diskSize = (cellSize + outputSize + 7) / 8; // Always round up instead of down

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
	openIndexes(StorageMode::generate);
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

void Storage::writeIndex(uint64_t state, uint64_t output)
{
	// Also relies on little endian architecture
	if (fwrite(&state, diskSize, 1, index.at(output)) != 1)
	{
		throwFileWrite(INDEX_PREFIX);
	}
}
