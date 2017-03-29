// Sandbox.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <vector>
#include <stdio.h>
#include <sstream>

#include <Cereal.h>

#include <src/Writer.h>

void gotoxy(int x, int y)
{
	CONSOLE_SCREEN_BUFFER_INFO SBInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &SBInfo);

	COORD dwPos;
	x > 0 ? dwPos.X = x : dwPos.X = SBInfo.dwCursorPosition.X;
	y > 0 ? dwPos.Y = y : dwPos.Y = SBInfo.dwCursorPosition.Y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), dwPos);
}

void setConsoleColor(int color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void dump(const void* object, unsigned int size, int color = 0x03)
{
	byte* x = (byte*)object;
	std::vector<byte> data;

	for (unsigned int i = 1; i <= size; i++)
	{
		x[i - 1] >= 32 ? x[i - 1] < 127 ? setConsoleColor(color) : setConsoleColor(0x07) : setConsoleColor(0x07);

		printf("%02X ", x[i - 1]);
		data.push_back(x[i - 1]);

		if (((i % 16 == 0) && (i > 0)) || (i == size))
		{
			gotoxy(64, -1);

			for (byte chars : data)
			{
				chars >= 32 ? chars < 127 ? setConsoleColor(color) : setConsoleColor(0x07) : setConsoleColor(0x07);
				chars >= 32 ? chars < 127 ? printf("%c", chars) : printf(".") : printf(".");
			}

			data.clear();
		}
	}

	printf("\n\n");
	setConsoleColor(0x07);
}

std::vector<std::string> splitString(const std::string& str, char delimiter)
{
	std::vector<std::string> split;
	std::stringstream ss(str);
	std::string item;

	while (std::getline(ss, item, delimiter)) split.push_back(item);

	return split;
}

int main()
{
	Cereal::Buffer dest(1024 * 1024 * 10);

	std::ifstream infile("words.txt", std::ifstream::binary);

	printf("Reading file...");

	if (!infile.good()) return false;

	infile.seekg(0, std::ios::end);
	unsigned int size = (unsigned int)infile.tellg();
	infile.seekg(0, std::ios::beg);

	char* data = new char[size];

	infile.read((char*)data, size);
	infile.close();

	printf("OK!\nSeparating words...");

	std::vector<std::string> words = splitString(std::string(data), '\n');

	delete[] data;

	printf("OK!\nCopying data...");

	std::string* strs = new std::string[words.size()];

	for (int i = 0; i < words.size(); i++)
	{
		strs[i] = words[i];
	}

	printf("OK!\nCreating database...");

	Cereal::Database* db = new Cereal::Database("Dictionaries");

	db->addObject(new Cereal::Object("English"));

	db->getObject("English")->addArray(new Cereal::Array("words", strs, words.size()));

	printf("OK!\nWriting...");

	db->write(dest);

	dest.shrink();

	printf("OK!\nWriting to a file...");

	dest.writeFile(std::string("dictionaries-english.db"));

	printf("OK!\nFreeing memory...");

	words.erase(words.begin(), words.end());

	delete[] strs;
	delete db;

	printf("OK!\n\nJob done.");

	dump(dest.getStart(), dest.getSize());

	/**Cereal::Buffer dest(0);

	dest.readFile("dictionaries.db");

	Cereal::Database* db = new Cereal::Database;

	db->read(dest);

	//float retf = header2->getDatabase("Second database")->getObject("Test object")->getField("xpos")->getValue<float>();

	//std::string ret = header2->getDatabase("Database name")->getObject("Object name")->getField("Field name")->getValue<std::string>();

	std::vector<std::string> array = db->getObject("English")->getArray("words")->getArray<std::string>();

	printf("Done.");

	//delete header;
	delete db;*/

	while (1) { Sleep(1000); }

    return 0;
}