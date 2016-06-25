#pragma once

#include <string>
#include <assert.h>

#include "Internal.h"

namespace Cereal {

	class Writer
	{
	public:
		template<typename T>
		static unsigned int writeBytes(byte* dest, int pointer, T value)
		{
			for (unsigned int i = 0; i < sizeof(T); i++)
			{
				dest[pointer++] = (value >> ((sizeof(T) - 1) * 8 - i * 8)) & 0xFF;
			}

			return pointer;
		}

		template<>
		static unsigned int writeBytes<std::string>(byte* dest, int pointer, std::string string)
		{
			const unsigned short size = (unsigned short)string.length();

			assert(size <= 65535);

			pointer = writeBytes<unsigned short>(dest, pointer, size);

			for (unsigned short i = 0; i < size; i++)
			{
				pointer = writeBytes<char>(dest, pointer, string[i]);
			}

			return pointer;
		}

		template<>
		static unsigned int writeBytes<float>(byte* dest, int pointer, float data)
		{
			unsigned int x;

			memcpy_s(&x, sizeof(unsigned int), &data, sizeof(float));

			writeBytes<unsigned int>(dest, pointer, x);

			return true;
		}

		template<>
		static unsigned int writeBytes<double>(byte* dest, int pointer, double data)
		{
			unsigned long long x;

			*(unsigned long long*)&x = (unsigned long long)data;

			writeBytes<unsigned long long>(dest, pointer, x);

			return true;
		}

	};

}
