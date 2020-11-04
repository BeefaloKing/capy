/* main.cpp
 */
#include "storage.hh"

int main (int argc, char* argv[])
{
	Storage myStorage {"."};
	myStorage.setConfig(14, 3);
	return 0;
}
