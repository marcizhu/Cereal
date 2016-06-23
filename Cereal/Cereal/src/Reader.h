#pragma once

namespace Cereal {

	class Reader
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
			char* value;
			short size = readBytes<short>(src, pointer);
			value = new char[size + 1]; // FIX ME: memory leak
			for (int i = pointer + 2; i < pointer + size + 2; i++) {
				value[i - pointer - 2] = readBytes<char>(src, i);
			}
			value[size] = '\0'; // null termination char, to be removed when i make my own class
			return std::string(value);
		}
	};

}