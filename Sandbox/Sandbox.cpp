// Sandbox.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <vector>
#include <stdio.h>

#include <Cereal.h>
#include <src/Field.h>
#include <src/Array.h>

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
	byte* dest = new byte[256];
	memset(dest, 0, 256);

	int* data = new int[4]{
		1, 2, 3, 4
	};

	Cereal::Array array("Test", data, 4);

	array.write(dest, 0);

	dump(dest, 256);

	Cereal::Array array2 = Cereal::Array::read(dest, 0);

	int* ret = array2.getArray<int>();

	for (int i = 0; i < 4; i++)
	{
		printf("%i\n", ret[i]);
	}

	delete[] dest;

	while (1) { _asm nop }

    return 0;
}