// dbread.cpp : Defines the entry point for the console application.
//

#include <Cereal.h>

#include <iostream>
#include <string>
#include <vector>

typedef unsigned char byte;

typedef struct Node
{
	std::string text = "";
	std::vector<Node*> childs;
} Node;

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

void print(Node* first, bool last = false, int level = 0)
{
	Node* temp = first;

	for (int i = 0; i < (level - 1); i++)
	{
		std::cout << "     ";
	}

	if (level)
	{
		if (last)
			std::cout << (char)192;
		else
			std::cout << (char)195;

		std::cout << (char)196 << (char)196 << (char)196 << (char)196;
	}

	std::cout << first->text << std::endl;

	for (int i = 0; i < temp->childs.size(); i++)
	{
		print(temp->childs[i], i == (temp->childs.size() - 1), level + 1);
	}
}

int process(const std::string& input)
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

		Node* firstNode = new Node;
		firstNode->text = input;
		Node* current = firstNode;

		for (int i = 0; i < header->getDatabases().size(); i++)
		{
			Cereal::Database* db = header->getDatabases()[i];

			Node* temp = new Node;
			temp->text = db->getName();

			firstNode->childs.push_back(temp);

			for (int j = 0; j < db->getObjects().size(); j++)
			{
				Cereal::Object* obj = db->getObjects()[j];

				current = firstNode->childs[i];

				Node* temp = new Node;
				temp->text = obj->getName();

				current->childs.push_back(temp);

				for (int x = 0; x < obj->getFields().size(); x++)
				{
					const Cereal::Field* field = obj->getFields()[x];

					current = firstNode->childs[i]->childs[j];

					Node* temp = new Node;
					temp->text = field->getName();

					current->childs.push_back(temp);
				}

				for (int y = 0; y < obj->getArrays().size(); y++)
				{
					const Cereal::Array* array = obj->getArrays()[y];

					current = firstNode->childs[i]->childs[j];

					Node* temp = new Node;
					temp->text = array->getName();

					current->childs.push_back(temp);
				}
			}
		}

		print(firstNode);

		__debugbreak();
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