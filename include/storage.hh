/* storage.hh
 * Used for reading and writing to the stored state/output mapping
 */
#pragma once
#include <string>

class Storage
{
public:
	Storage(const std::string &directory);

	// Returns true if directory is emtpy and creates it if it does not exist
	bool validateBaseDir();

	void setConfig(size_t cellSize, size_t outputSize);

	// Potentially reduces file fragmentation
	void preallocateMap();

	static constexpr const char* CONFIG_NAME = "/settings.capy";
	static constexpr const char* MAP_NAME = "/statemap.capy";
private:

	std::string dirPath;

	size_t cellSize;
	size_t outputSize;
	size_t diskSize; // Size of packed data on disk in bytes
};
