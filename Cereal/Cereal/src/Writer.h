#pragma once

#include <assert.h>

namespace Cereal {

	class Writer
	{
	public:
		template<typename T>
		static T readBytes(byte* src, int pointer)
		{
			T value = src[pointer] << (sizeof(T) * 8 - 8);

			for (int i = 0; i < sizeof(T); i++)
			{
				value |= (src[pointer + i] << ((sizeof(T) * 8 - 8) - (i * 8)));
			}

			return value;
		}

		template<>
		static float readBytes<float>(byte* src, int pointer)
		{
			unsigned int value = src[pointer] << (sizeof(int) * 8 - 8);

			for (int i = 0; i < (int) sizeof(float); i++)
			{
				value |= (src[pointer + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			float result;

			memcpy_s(&result, 4, &value, 4);

			return result;
		}

		template<>
		static bool readBytes<bool>(byte* src, int pointer)
		{
			bool result;

			result = src[pointer];

			return result != 0;
		}

		template<>
		static double readBytes<double>(byte* src, int pointer)
		{
			unsigned long long value = src[pointer] << (sizeof(int) * 8 - 8);

			for (int i = pointer; i < (int)pointer + (int) sizeof(float); i++)
			{
				value |= (src[pointer + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			double result;
			memcpy_s(&result, 4, &value, 4);

			return result;
		}

		template<>
		static std::string readBytes<std::string>(byte* src, int pointer)
		{
			std::string value = "";
			short size = readBytes<short>(src, pointer);

			for (int i = pointer + 2; i < pointer + size + 2; i++)
			{
				value += readBytes<char>(src, i);
			}

			value += '\0';

			return std::string(value);
		}

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
