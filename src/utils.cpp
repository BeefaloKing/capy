/* utils.cpp
 */
#include "utils.hh"
#include <iomanip>
#include <sstream> // Ugh...
#include <stdint.h>
#include <string>

std::string getHumanSize(uint64_t bytes)
{
	static const char* suffixArray[] = {"Bytes", "KiB", "MiB", "GiB", "TiB"};
	static const size_t suffixSize = sizeof(suffixArray) / sizeof(suffixArray[0]);

	size_t suffix = 0;
	double size = bytes;
	while (size > 1024 && suffix < suffixSize)
	{
		size /= 1024;
		suffix++;
	}

	std::ostringstream humanSize;
	humanSize << std::fixed << std::setprecision(1);
	humanSize << size << " " << suffixArray[suffix];

	return humanSize.str();
}
