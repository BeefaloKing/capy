/* Storage.cpp
 */
#include "storage.hh"
#include "error.hh"
#include <string>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

Storage::Storage(const std::string &directory) :
	dirPath(directory),
	cellSize(0),
	outputSize(0),
	diskSize(0)
{}

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

void Storage::setConfig(size_t cellSize, size_t outputSize)
{
	this->cellSize = cellSize;
	this->outputSize = outputSize;
	diskSize = (cellSize + outputSize + 7) / 8; // Always round up instead of down.

	const std::string configPath = dirPath + "/settings.capy";
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
}
