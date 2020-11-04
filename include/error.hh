/* error.hh
 * Utility functions for handling errors
 */
#include <string>

void throwFileAccess(const std::string &filePath);
void throwFileWrite(const std::string &filePath);
