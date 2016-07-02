// Sandbox.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <vector>
#include <stdio.h>

#include <Cereal.h>

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

int main()
{
	Cereal::Buffer dest(1024);

	int data[4] = { 1, 2, 3, 4 };

	Cereal::Database* db = new Cereal::Database("Database name");
	Cereal::Database* db2 = new Cereal::Database("Second database");

	db->addObject(new Cereal::Object("Object name"));
	db2->addObject(new Cereal::Object("Test object"));

	db->getObject("Object name")->add(new Cereal::Array("Array name", data, 4));
	db->getObject("Object name")->add(new Cereal::Field("Field name", std::string("test!")));
	db2->getObject("Test object")->add(new Cereal::Field("xpos", 3.141592f));

	Cereal::Header* header = new Cereal::Header;

	header->add(db);
	header->add(db2);
	header->write(dest);

	dest.shrink();

	dump(dest.getStart(), dest.getSize());

	dest.writeFile(std::string("test.db"));

	Cereal::Header* header2 = new Cereal::Header;

	dest.setOffset(0);

	header2->read(dest);

	//float ret = header2.getDatabase("Second database")->getObject("Test object")->getField("xpos")->getValue<float>();

	std::string ret = header2->getDatabase("Database name")->getObject("Object name")->getField("Field name")->getValue<std::string>();

	//int* array = new int[4];

	//array = header2.getDatabase("Database name")->getObject("Object name")->getArray("Array name")->getRawArray<int>(array);

	printf("%s", ret.c_str());

	delete header;
	delete header2;

	while (1) { Sleep(1000); }

    return 0;
}