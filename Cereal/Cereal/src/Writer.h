#pragma once

#include <assert.h>

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
			const unsigned int size = (short)string.length();

			assert(size <= 65535);

			pointer = writeBytes(dest, pointer, (short)size);

			for (unsigned short i = 0; i < (short)size; i++)
			{
				pointer = writeBytes(dest, pointer, string[i]);
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

			*(unsigned long long*)&x = data;

			writeBytes<unsigned long long>(dest, pointer, x);

			return true;
		}

	};

}
