#pragma once

#include <array>
#include <assert.h>

namespace cereal {

	typedef unsigned char byte;

	class Writer {
		

	public:
		template<typename T>
		static T readBytes(byte* src, int pointer) {
			T value = src[pointer] << (sizeof(T) * 8 - 8);
			for (int i = 0; i < (int) sizeof(T); i++) {
				value |= (src[pointer + i] << ((sizeof(T) * 8 - 8) - (i * 8)));
			}
			return value;
		}

		template<>
		static float readBytes<float>(byte* src, int pointer) {
			unsigned int value = src[pointer] << (sizeof(int) * 8 - 8);
			for (int i = 0; i < (int) sizeof(float); i++) {
				value |= (src[pointer + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}
			float result;
			memcpy_s(&result, 4, &value, 4);
			return result;
		}

		template<>
		static bool readBytes<bool>(byte* src, int pointer) {
			bool result;
			result = src[pointer];
			return result != 0;
		}

		template<>
		static double readBytes<double>(byte* src, int pointer) {
			unsigned long long value = src[pointer] << (sizeof(int) * 8 - 8);
			for (int i = pointer; i < (int)pointer + (int) sizeof(float); i++) {
				value |= (src[pointer + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}
			double result;
			memcpy_s(&result, 4, &value, 4);
			return result;
		}

		template<>
		static std::string readBytes<std::string>(byte* src, int pointer) {
			char* value;
			short size = readBytes<short>(src, pointer);
			value = new char[size + 1];
			for (int i = pointer + 2; i < pointer + size + 2; i++) {
				value[i - pointer - 2] = readBytes<char>(src, i);
			}
			value[size] = 0; // null termination char, to be removed when i make my own class
			return std::string(value);
		}

		template<typename T>
		static int writeBytes(byte* dest, int pointer, const T& value) {
			auto asBytes = toBytes(value);
			for (byte c : asBytes) {
				dest[pointer++] = c;
			}
			return pointer;
		}
		static int writeBytes(byte* dest, int pointer, std::string string) {
			const unsigned int size = (short)string.length();

			assert(size <= 65535);

			pointer = writeBytes(dest, pointer, (short)size);
			for (unsigned short i = 0; i < (short)size; i++) {
				pointer = writeBytes(dest, pointer, string[i]);
			}
			return pointer;
		}

	protected:
		template<typename T>
		static std::array<byte, sizeof(T)> toBytes(const T& object) {
			std::array<byte, sizeof(T)> bytes;

			const byte* begin = reinterpret_cast<const byte*>(std::addressof(object));
			const byte* end = begin + sizeof(T);
			std::reverse_copy(begin, end, std::begin(bytes));

			return bytes;
		}


	};

}
