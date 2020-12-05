/* utils.hh
 * Miscellaneous utility functions
 */
#pragma once
#include <stdint.h>
#include <string>

std::string getHumanSize(uint64_t bytes);
// Reduces file fragmentation
// Needs filePath for more useful debug and progress output
void preallocateFile(FILE* file, uint64_t fileSize, const std::string &filePath);

// Progress is expected to be between 0 and 1
// If newline is false next line printed will overwrite the printed bar
// Use this to make a progress bar that updates in place
void printProgressBar(size_t width, double progress, bool newline);
