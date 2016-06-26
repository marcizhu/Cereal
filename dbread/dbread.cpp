// dbread.cpp : Defines the entry point for the console application.
//

#include <Cereal.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <Windows.h>

typedef unsigned char byte;

void showVerison()
{
	std::cout << std::endl << "dbread " <<
#ifdef _WIN64
		"[64] " <<
#endif
		"Version 1.0.0 beta  Copyright (c) 2016 Marc Izquierdo  26-06-2016" << std::endl << std::endl;
}

void showHelp()
{
	showVerison();

	std::cout << "USAGE: dbread <command> <input file>" << std::endl << std::endl;

	std::cout << "Commands:" << std::endl;
	std::cout << "  -h, --help                   Shows this message" << std::endl;
	std::cout << "  -v, --version                Shows the version" << std::endl << std::endl;

	std::cout << "Parameters:" << std::endl;
	std::cout << "  -i, --input [file]           Dumps the serialized contents of the file" << std::endl << std::endl;
}

int process(std::string& input)
{
	if (input == "") return -1;

	Cereal::Buffer buffer(1);

	if(buffer.readFile(input) == false) return -2;

	short headerSign = Cereal::Reader::readBytes<unsigned short>((byte*)buffer.getStart(), 0);

	if (headerSign == MAGIC_NUMBER)
	{
		// The file has a header
		Cereal::Header* header = new Cereal::Header;

		header->read(buffer);

		// TODO: Add code to read and print a tree that represents the databases
	}
	else
	{
		// The file doesn't have a header
		Cereal::Database* db = new Cereal::Database;

		db->read(buffer);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		std::string input = "";

		for (int i = 1; i < argc; i++)
		{
			for (unsigned int x = 0; x < strlen(argv[i]); x++)
			{
				argv[i][x] = tolower(argv[i][x]);
			}
		}

		for (int i = 1; i < argc; i++)
		{
			if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h") || !strcmp(argv[i], "/h") || !strcmp(argv[i], "/?"))
			{
				showHelp();
				break;
			}

			if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v") || !strcmp(argv[i], "/v"))
			{
				showVerison();
				break;
			}

			if (!strcmp(argv[i], "--input") || !strcmp(argv[i], "-i") || !strcmp(argv[i], "/i"))
			{
				input = argv[i + 1];
				i += 1;
				continue;
			}

			input = argv[i];
		}

		return process(input);
	}
	else
	{
		showHelp();
	}

	return 0;
}