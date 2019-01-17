//  Cereal: A C++/C# Serialization library
//  Copyright (C) 2016  The Cereal Team
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <fstream>
#include <string>
#include <cstring>

#include "Internal.h"

namespace Cereal {

	class Buffer
	{
	private:
		byte* start;
		unsigned int size;
		unsigned int offset;

	public:
		Buffer(const Buffer& other) = delete;
		Buffer(unsigned int size) : start(new byte[size]), size(size) { clean(); }
		Buffer(byte* start, unsigned int size) : start(start), size(size) { clean(); }
		Buffer(byte* start, unsigned int size, unsigned int offset, bool clean = false) : start(start), size(size), offset(offset) { if (clean) this->clean(); }

		~Buffer()
		{
			delete[] start;
			offset = size = 0;
		}

		void clean() { memset(start, 0, size); offset = 0; }

		template<typename T>
		inline T readBytes()
		{
			T value = 0;

			for (unsigned int i = 0; i < sizeof(T); i++)
			{
				value |= ((T)start[offset + i] << ((sizeof(T) * 8 - 8) - (i * 8)));
			}

			offset += sizeof(T);

			return value;
		}

#ifdef COMPILER_MSVC
		template<>
		float readBytes<float>()
		{
			unsigned int value = 0;

			for (unsigned int i = 0; i < sizeof(float); i++)
			{
				value |= ((unsigned int)start[offset + i] << ((sizeof(unsigned int) * 8 - 8) - (i * 8)));
			}

			float result;

			offset += sizeof(float);

			memcpy(&result, &value, sizeof(float));

			return result;
		}

		template<>
		bool readBytes<bool>() { return start[offset++] != 0; }

		template<>
		double readBytes<double>()
		{
			unsigned long long value = 0;

			for (unsigned int i = 0; i < sizeof(double); i++)
			{
				value |= ((unsigned long long)start[offset + i] << ((sizeof(unsigned long long) * 8 - 8) - (i * 8)));
			}

			double result;
			memcpy(&result, &value, sizeof(double));

			offset += sizeof(double);

			return result;
		}

		template<>
		std::string readBytes<std::string>()
		{
			std::string value = "";

			unsigned short size = readBytes<unsigned short>();

			for (unsigned int i = 0; i < size; i++)
			{
				value += readBytes<char>();
			}

			return value;
		}
#endif

		template<typename T>
		inline bool writeBytes(T value)
		{
			if(!hasSpace(sizeof(T))) return false;

			for (unsigned int i = 0; i < sizeof(T); i++)
			{
				start[offset++] = (value >> ((sizeof(T) - 1) * 8 - i * 8)) & 0xFF;
			}

			return true;
		}

#ifdef COMPILER_MSVC
		template<>
		bool writeBytes<std::string>(std::string string)
		{
			const unsigned short size = (unsigned short)string.length();

			if(string.length() > 0xFFFF) throw std::overflow_error("String is too long!");
			if(!hasSpace(size + sizeof(short))) return false;

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

			*(unsigned int*)&x = *(unsigned int*)&data;

			return writeBytes<unsigned int>(x);
		}

		template<>
		bool writeBytes<double>(double data)
		{
			unsigned long long x;

			*(unsigned long long*)&x = *(unsigned long long*)&data;

			return writeBytes<unsigned long long>(x);
		}
#endif

		bool copy(byte* data, unsigned int size)
		{
			if (!hasSpace(size)) return false;

			memcpy((byte*)start + offset, data, size);

			offset += size;

			return true;
		}

		bool copy(const Buffer* buffer)
		{
			if (!hasSpace(buffer->offset)) return false;

			memcpy((byte*)start + offset, buffer->start, buffer->offset);

			offset += buffer->offset;

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

		void setOffset(unsigned int off) { if (off > size) throw std::domain_error("Offset value is too large"); else offset = off; }

		unsigned int getFreeSpace() const  { return size - offset; }
		unsigned int getOffset() const { return offset; }
		unsigned int getSize() const { return size; }
		const void* getStart() const { return start; }
		byte getByte() { return start[offset++]; }
		byte getByte(unsigned int offs) const { return start[offs]; }

		bool hasSpace(unsigned int amount) const { return (offset + amount) <= size; }

		void addOffset(unsigned int offs) { if ((offs + offset) > size) throw std::domain_error("Offset value is too large"); offset += offs; }

		bool writeFile(const std::string& filepath)
		{
			std::ofstream outfile(filepath, std::ofstream::binary);

			if (!outfile.good()) return false;

			outfile.write((char*)start, offset);
			outfile.close();

			return true;
		}

		bool readFile(const std::string& filepath)
		{
			std::ifstream infile(filepath, std::ifstream::binary);

			if (!infile.good()) return false;

			delete[] start;

			infile.seekg(0, std::ios::end);
			this->size = (unsigned int)infile.tellg();
			infile.seekg(0, std::ios::beg);

			start = new byte[size];

			infile.read((char*)start, size);
			infile.close();

			this->offset = 0;

			return true;
		}
	};

#ifdef COMPILER_GCC
	template<>
	inline float Buffer::readBytes<float>()
	{
		unsigned int value = 0;

		for (unsigned int i = 0; i < sizeof(float); i++)
		{
			value |= ((unsigned int)start[offset + i] << ((sizeof(unsigned int) * 8 - 8) - (i * 8)));
		}

		float result;

		offset += sizeof(float);

		memcpy(&result, &value, sizeof(float));

		return result;
	}

	template<>
	inline bool Buffer::readBytes<bool>() { return start[offset++] != 0; }

	template<>
	inline double Buffer::readBytes<double>()
	{
		unsigned long long value = 0;

		for (unsigned int i = 0; i < sizeof(double); i++)
		{
			value |= ((unsigned long long)start[offset + i] << ((sizeof(unsigned long long) * 8 - 8) - (i * 8)));
		}

		double result;
		memcpy(&result, &value, sizeof(double));

		offset += sizeof(double);

		return result;
	}

	template<>
	inline std::string Buffer::readBytes<std::string>()
	{
		std::string value = "";

		unsigned short size = readBytes<unsigned short>();

		for (unsigned int i = 0; i < size; i++)
		{
			value += readBytes<char>();
		}

		return value;
	}

	template<>
	inline bool Buffer::writeBytes<std::string>(std::string string)
	{
		const unsigned short size = (unsigned short)string.length();

		if(string.length() > 0xFFFF) throw std::overflow_error("String is too long!");
		if(!hasSpace(size + sizeof(short))) return false;

		writeBytes<unsigned short>(size);

		for (unsigned short i = 0; i < size; i++)
		{
			writeBytes<char>(string[i]);
		}

		return true;
	}

	template<>
	inline bool Buffer::writeBytes<float>(float data)
	{
		unsigned int x;

		*(unsigned int*)&x = *(unsigned int*)&data;

		return writeBytes<unsigned int>(x);
	}

	template<>
	inline bool Buffer::writeBytes<double>(double data)
	{
		unsigned long long x;

		*(unsigned long long*)&x = *(unsigned long long*)&data;

		return writeBytes<unsigned long long>(x);
	}
#endif
}
