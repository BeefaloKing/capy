/* error.hh
 * Utility functions for handling errors
 */
#include <string>

void throwDirCreate(const std::string &directory);
void throwFileAccess(const std::string &filePath);
void throwFileWrite(const std::string &filePath);
