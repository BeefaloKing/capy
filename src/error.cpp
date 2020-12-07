/* error.cpp
 */
#include "error.hh"
#include <stdexcept>
#include <string>
#include <string.h>

void throwDirCreate(const std::string &directory)
{
	std::string errMsg = "Unable to create directory \"" + directory + "\": "
		+ strerror(errno) + "\n";
	throw std::runtime_error(errMsg);
}

void throwDirFull(const std::string &directory)
{
	std::string errMsg = "Directory \"" + directory + "\" is not empty.\n";
	throw std::runtime_error(errMsg);
}

void throwFileAccess(const std::string &filePath)
{
	// Can't wait until C++20 std::format
	std::string errMsg = "Unable to open \"" + filePath + "\": " + strerror(errno) + "\n";
	throw std::runtime_error(errMsg);
}

void throwFileWrite(const std::string &filePath)
{
	std::string errMsg = "Unable write to file \"" + filePath + "\": " +
		strerror(errno) + "\n";
	throw std::runtime_error(errMsg);
}

void throwFileRead(const std::string &filePath)
{
	std::string errMsg = "Unknown error reading from file \"" + filePath + "\": " +
		strerror(errno) + "\n";
	throw std::runtime_error(errMsg);
}

void throwRange(const std::string &name, const uint64_t &index, const uint64_t &size)
{
	std::string errMsg = name + " access out of range. Tried to access index " +
		std::to_string(index) + " when size is " + std::to_string(size) + ".\n";
	throw std::out_of_range(errMsg);
}

void throwInvalidArg(const std::string &name, const std::string &value)
{
	std::string errMsg = "Invalid value " + value + " for argument \"" + name + "\"\n";
	throw std::runtime_error(errMsg);
}
