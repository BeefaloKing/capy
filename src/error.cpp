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
