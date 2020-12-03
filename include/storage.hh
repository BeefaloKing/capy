/* storage.hh
 * Used for reading and writing to the stored state/output mapping
 */
#pragma once
#include <string>
#include <vector>

enum class StorageMode
{
	generate,
	analyze,
};

class Storage
{
public:
	Storage() = delete;
	Storage(const std::string &directory, StorageMode mode);
	~Storage();

	inline uint64_t getCellCount()
	{
		return cellCount;
	}

	void setConfig(size_t cellSize, size_t outputSize);
	// void readConfig();

	void writeSwap(uint64_t state, uint64_t output);
	// void getIndex(uint64_t output, std::vector<uint64_t> states);

	void mergeSwap();

	static constexpr const char* CONFIG_NAME = "/settings.capy";
	static constexpr const char* SWAP_PREFIX = "/swap";
	static constexpr const char* INDEX_NAME = "/index.capy";
	static constexpr const char* TREE_NAME = "/tree.capy";
private:
	// Returns true if directory is emtpy and creates it if it does not exist
	bool validateBaseDir();
	void openIndex();
	void openTree();

	void openSwaps();
	void closeSwaps();

	// Reduces file fragmentation
	// Needs filePath for more useful debug and progress output
	void preallocateFile(FILE* file, uint64_t fileSize, const std::string &filePath = "");

	std::string dirPath;
	StorageMode mode;
	FILE* indexFile;
	FILE* treeFile;
	std::vector<FILE*> swapFile;
	std::vector<size_t> swapSize; // Counts number of entries in each swap file

	size_t cellSize;
	uint64_t cellCount; // Must be able to hold 64 bits which may be larger than size_t
	size_t outputSize;
	size_t diskSize; // Size of packed data on disk in bytes
};
