/* error.hh
 * Utility functions for handling errors
 */
#pragma once
#include <string>

void throwDirCreate(const std::string &directory);
void throwDirFull(const std::string &directory);
void throwFileAccess(const std::string &filePath);
void throwFileWrite(const std::string &filePath);
void throwFileRead(const std::string &filePath);
void throwRange(const std::string &name, const uint64_t &index, const uint64_t &size);
void throwInvalidArg(const std::string &name, const std::string &value);
