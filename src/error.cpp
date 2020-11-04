/* error.cpp
 */
#include "error.hh"
#include <stdexcept>
#include <string>
#include <string.h>

void throwFileAccess(const std::string &filePath)
{
	// Can't wait until C++20 std::format
	const std::string errMsg = "Unable to open \"" + filePath + "\": " + strerror(errno) + "\n";
	throw std::runtime_error(errMsg);
}

void throwFileWrite(const std::string &filePath)
{
	const std::string errMsg = "Unable write to file \"" + filePath + "\": " +
		strerror(errno) + "\n";
	throw std::runtime_error(errMsg);
}
