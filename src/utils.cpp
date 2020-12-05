/* utils.cpp
 */
#include "utils.hh"
#include "error.hh"
#include <iomanip>
#include <sstream> // Ugh...
#include <stdint.h>
#include <stdio.h>
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

void preallocateFile(FILE* file, uint64_t fileSize, const std::string &filePath)
{
	printf("Preallocating %s for %s.\n", getHumanSize(fileSize).c_str(), filePath.c_str());
	fflush(stdout);

	if (fseeko64(file, fileSize - 1, SEEK_SET) != 0)
	{
		throwFileWrite(filePath);
	}
	fputc('\0', file); // Write past EOF to force allocation

	fseek(file, 0, SEEK_SET); // Reset position to begining of file
}

void printProgressBar(size_t width, double progress, bool newline)
{
	// Bound progress range to [0, 1]
	progress = (progress < 0) ? 0 : progress;
	progress = (progress > 1) ? 1 : progress;

	size_t barWidth = width * progress;

	std::string barString = "";
	for (size_t i = 0; i < width; i++)
	{
		if (i < barWidth)
		{
			barString += '=';
		}
		else if (i == barWidth)
		{
			barString += '>';
		}
		else
		{
			barString += ' ';
		}
	}

	char terminator = newline ? '\n' : '\r';

	printf("[%s] %.0f%%%c", barString.c_str(), 100 * progress, terminator);
	fflush(stdout);
}
