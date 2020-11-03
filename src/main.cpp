/* main.cpp
 */
#include "capy.hh"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

void printUsage();

int main (int argc, char* argv[])
{
	if (argc == 1) // Print ussage when no arguments passed
	{
		printUsage();
	}

	opterr = 0; // Ignore default error printing behavior

	size_t cellSize = 0;
	size_t outputBit = 0;
	char* filePath = nullptr;

	char flag;
	while ((flag = getopt(argc, argv, "hs:o:")) != -1)
	{
		switch (flag)
		{
		case 'h':
			printUsage();
			return 0;
		case 's':
			sscanf(optarg, "%zu", &cellSize);
			break;
		case 'o':
			sscanf(optarg, "%zu", &outputBit);
			break;
		case '?':
			printf("Flag -%c invalid or missing arguments.\nSee usage. \"capy.exe -h\"\n", optopt);
			return 1;
		default:
			return 1;
		}
	}

	if (optind < argc) // If there are non option arguments to parse
	{
		filePath = argv[optind];
	}

	// Check for valid arguments before starting capy
	if (cellSize < 1 || cellSize > 64)
	{
		printf("Automata size must be between 1 and 64.\n");
		return 1;
	}
	if (outputBit >= cellSize)
	{
		printf("Output cell must be less than automata size.\n");
		return 1;
	}
	if (filePath == nullptr)
	{
		printf("No output file provided.");
		return 1;
	}

	try
	{
		Capy capy {cellSize, outputBit, filePath};
		capy.mainLoop();
	}
	catch (const std::exception &e)
	{
		printf("Unable to generate output!\n%s", e.what());
	}

	return 0;
}

void printUsage()
{
	printf(
		"Cellular Automata Pseudorandom number generator analYzer (CAPY)\n"
		"\n"
		"Simulates a CA PRNG and maps arbitrary output streams to all possible walks\n"
		"through the machines internal state. Determines the total number of distinct\n"
		"output streams and distinct internal states the machine can be determined to\n"
		"exist in as the output stream grows in length.\n"
		"\n"
		"Usage:\n"
		"capy.exe [options] <output file>\n"
		"\n"
		"Options:\n"
		"-h          Displays this message.\n"
		"-s <size>   Uses an automata of <size> cells. Must be between 1 and 64.\n"
		"-o <cell>   Read output from the <cell>th cell. Must be between 0 and <size>.\n"
		"            Cells are counted from right to left.\n"
		"            Default: 0\n");
}
