/* storage.hh
 * Used for reading and writing to the stored state/output mapping
 */
#pragma once
#include <string>
#include <vector>

enum StorageMode
{
	generate,
	analyze,
};

class Storage
{
public:
	Storage(const std::string &directory, StorageMode mode);
	~Storage();

	void setConfig(size_t cellSize, size_t outputSize);
	// void readConfig();

	void preallocateMap(); // Potentially reduces file fragmentation
	void writeMapEntry(uint64_t state, uint64_t nextState, uint64_t output);
	// void readMapEntry(uint64_t state, uint64_t &nextState, uint64_t &output);
	void writeIndex(uint64_t state, uint64_t output);
	// void getIndex(uint64_t output, std::vector<uint64_t> states);

	static constexpr const char* CONFIG_NAME = "/settings.capy";
	static constexpr const char* MAP_NAME = "/statemap.capy";
	static constexpr const char* INDEX_PREFIX = "/index";
private:
	// Returns true if directory is emtpy and creates it if it does not exist
	bool validateBaseDir();
	void openIndexes(StorageMode mode);

	std::string dirPath;
	FILE* mapFile;
	std::vector<FILE*> index;

	size_t cellSize;
	size_t outputSize;
	size_t diskSize; // Size of packed data on disk in bytes
};
