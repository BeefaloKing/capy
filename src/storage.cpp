/* Storage.cpp
 */
#include "storage.hh"
#include "error.hh"
#include <string>

Storage::Storage(const std::string &directory) :
	dirPath(directory),
	cellSize(0),
	outputSize(0),
	diskSize(0)
{}

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
