#pragma once

#include <fstream>
#include <string>
#include <assert.h>

#include "Internal.h"

namespace Cereal {

	class Buffer
	{
	private:
		byte* start;
		unsigned int size;
		unsigned int offset;

	public:
		Buffer(unsigned int size) : size(size), start(new byte[size]) { clean(); }
		Buffer(byte* start, unsigned int size) : size(size), start(start) { clean(); }
		Buffer(byte* start, unsigned int size, unsigned int offset) : size(size), start(start), offset(offset) {}

		~Buffer()
		{
			offset = 0;
			size = 0;
			delete[] start;
		}

		void clean()
		{
			offset = 0;

			for (unsigned int i = 0; i < size; i++)
			{
				start[i] = 0;
			}
		}

		template<typename T>
		T readBytes()
		{
			T value = 0;

			for (int i = 0; i < sizeof(T); i++)
			{
				value |= (start[offset + i] << ((sizeof(T) * 8 - 8) - (i * 8)));
			}

			offset += sizeof(T);

			return value;
		}

		template<>
		float readBytes<float>()
		{
			unsigned int value = 0;

			for (int i = 0; i < (int) sizeof(float); i++)
			{
				value |= (start[offset + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			float result;

			offset += sizeof(float);

			memcpy_s(&result, 4, &value, 4);

			return result;
		}

		template<>
		bool readBytes<bool>() { return start[offset++] != 0; }

		template<>
		double readBytes<double>()
		{
			unsigned long long value = start[offset] << (sizeof(int) * 8 - 8);

			for (int i = offset; i < (int)offset + (int)sizeof(float); i++)
			{
				value |= (start[offset + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			double result;
			memcpy_s(&result, 4, &value, 4);

			offset += sizeof(double);

			return result;
		}

		template<>
		std::string readBytes<std::string>()
		{
			std::string value = "";

			unsigned short size = readBytes<unsigned short>();

			for (int i = 0; i < size; i++)
			{
				value += readBytes<char>();
			}

			return value;
		}

		template<typename T>
		bool writeBytes(T value)
		{
			for (unsigned int i = 0; i < sizeof(T); i++)
			{
				start[offset++] = (value >> ((sizeof(T) - 1) * 8 - i * 8)) & 0xFF;
			}

			return true;
		}

		template<>
		bool writeBytes<std::string>(std::string string)
		{
			const unsigned short size = (unsigned short)string.length();

			assert(size <= 65535);

			writeBytes<unsigned short>(size);

			for (unsigned short i = 0; i < size; i++)
			{
				writeBytes<char>(string[i]);
			}

			return true;
		}

		template<>
		bool writeBytes<float>(float data)
		{
			unsigned int x;

			memcpy_s(&x, sizeof(unsigned int), &data, sizeof(float));

			writeBytes<unsigned int>(x);

			return true;
		}

		template<>
		bool writeBytes<double>(double data)
		{
			unsigned long long x;

			*(unsigned long long*)&x = (unsigned long long)data;

			writeBytes<unsigned long long>(x);

			return true;
		}

		void shrink()
		{
			byte* temp = new byte[offset];

			memcpy(temp, start, offset);

			delete[] start;

			start = temp;
			size = offset;
		}

		void setOffset(unsigned int off) { offset = off; }

		unsigned int getFreeSpace() const { return size - offset; }
		unsigned int getOffset() const { return offset; }
		unsigned int getSize() const { return size; }
		const void* getStart() const { return start; }
		byte getByte() { return start[offset++]; }
		byte getByte(unsigned int offs) const { return start[offs]; }

		bool hasSpace(unsigned int amount) const { return (offset + amount) <= size; }

		void addOffset(unsigned int offs) { offset += offs; }

		bool writeFile(std::string& filepath)
		{
			std::ofstream outfile(filepath, std::ofstream::binary);

			if (!outfile.good()) return false;

			outfile.write((char*)this->start, offset);
			outfile.close();

			return true;
		}

		bool readFile(std::string& filepath)
		{
			std::ifstream infile(filepath, std::ifstream::binary);

			if (!infile.good()) return false;

			delete[] start;

			infile.seekg(0, std::ios::end);
			size_t size = infile.tellg();
			infile.seekg(0, std::ios::beg);

			start = new byte[size];

			infile.read((char*)start, size);
			infile.close();

			this->size = size;
			this->offset = 0;

			return true;
		}
	};

}