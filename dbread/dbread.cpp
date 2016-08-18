//  dbread: A database reader tool for Cereal

#include <Cereal.h>

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

typedef unsigned char byte;

typedef struct Node
{
	std::string text = "";
	std::vector<Node*> childs;
} Node;

void gotoxy(int x, int y)
{
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &SBInfo);

	COORD dwPos;
	x > 0 ? dwPos.X = x : dwPos.X = SBInfo.dwCursorPosition.X;
	y > 0 ? dwPos.Y = y : dwPos.Y = SBInfo.dwCursorPosition.Y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), dwPos);
}

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

	for (unsigned int i = 0; i < temp->childs.size(); i++)
	{
		print(temp->childs[i], i == (temp->childs.size() - 1), level + 1);
	}
}

void printDatabase(Cereal::Database* db)
{
	Node* firstNode = new Node;
	firstNode->text = db->getName();
	Node* current = firstNode;

	for (unsigned int j = 0; j < db->getObjects().size(); j++)
	{
		Cereal::Object* obj = db->getObjects()[j];

		current = firstNode;

		Node* temp = new Node;
		temp->text = obj->getName();

		current->childs.push_back(temp);

		for (unsigned int x = 0; x < obj->getFields().size(); x++)
		{
			const Cereal::Field* field = obj->getFields()[x];

			current = firstNode->childs[j];

			Node* temp = new Node;
			temp->text = field->getName();

			current->childs.push_back(temp);
		}

		for (unsigned int y = 0; y < obj->getArrays().size(); y++)
		{
			const Cereal::Array* array = obj->getArrays()[y];

			current = firstNode->childs[j];

			Node* temp = new Node;
			temp->text = array->getName();

			current->childs.push_back(temp);
		}
	}

	print(firstNode);
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

		std::cout << "Contents of the file '" << input << "':" << std::endl << std::endl;

		Cereal::Header* header = new Cereal::Header;

		header->read(buffer);

		Node* firstNode = new Node;
		firstNode->text = input;
		Node* current = firstNode;

		byte dbCount = 0;
		unsigned int objCount = 0;
		unsigned int fieldCount = 0;
		unsigned int arrayCount = 0;

		for (unsigned int i = 0; i < header->getDatabases().size(); i++)
		{
			Cereal::Database* db = header->getDatabases()[i];

			Node* temp = new Node;
			temp->text = db->getName();

			firstNode->childs.push_back(temp);

			dbCount++;

			for (unsigned int j = 0; j < db->getObjects().size(); j++)
			{
				Cereal::Object* obj = db->getObjects()[j];

				current = firstNode->childs[i];

				Node* temp = new Node;
				temp->text = obj->getName();

				current->childs.push_back(temp);

				objCount++;

				for (unsigned int x = 0; x < obj->getFields().size(); x++)
				{
					const Cereal::Field* field = obj->getFields()[x];

					current = firstNode->childs[i]->childs[j];

					Node* temp = new Node;
					temp->text = field->getName();

					current->childs.push_back(temp);

					fieldCount++;
				}

				for (unsigned int y = 0; y < obj->getArrays().size(); y++)
				{
					const Cereal::Array* array = obj->getArrays()[y];

					current = firstNode->childs[i]->childs[j];

					Node* temp = new Node;
					temp->text = array->getName();

					current->childs.push_back(temp);

					arrayCount++;
				}
			}
		}

		print(firstNode);

		delete firstNode; // memory leak

		std::cout << std::endl /*<< "--------------------------------------------------------------------------------"*/ << std::endl;

		std::cout << "The file contains:" << std::endl;
		std::cout << (int)dbCount << " Databases" << std::endl;
		std::cout << objCount << " Objects" << std::endl;
		std::cout << fieldCount << " Fields" << std::endl;
		std::cout << arrayCount << " Arrays" << std::endl << std::endl;

		std::cout << "The total size of the file is " << header->getSize() << " bytes" << std::endl;

		delete header;

		__debugbreak();
	}
	else
	{
		// The file doesn't have a header

		Cereal::Database* db = new Cereal::Database;

		db->read(buffer);

		printDatabase(db);

		delete db;
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