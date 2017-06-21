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

#include <string>
#include <vector>
#include <assert.h>

#include "Buffer.h"
#include "Reader.h"
#include "Writer.h" // Same as Field.h
#include "Internal.h"

namespace Cereal {

	class Array
	{
	private:
		std::string name;
		DataType dataType;
		unsigned int count; // item count
		unsigned int size = 0;
		byte* data;

		template<class T>
		void setData(DataType type, T* value, unsigned int count)
		{
			this->count = count;
			this->dataType = type;

			//Setting the data
			if (data) delete[] data;

			data = new byte[sizeof(T) * count];

			if((count * sizeof(T)) > 4294967296) throw std::overflow_error("Array size is too big!"); // Maximum item count (overflow of pointer and buffer)

			unsigned int pointer = 0;

			for (unsigned int i = 0; i < count; i++)
				pointer = Writer::writeBytes<T>(data, pointer, value[i]);
		}

#ifdef COMPILER_MSVC
		template<>
		void setData<std::string>(DataType type, std::string* value, unsigned int count)
#elif defined COMPILER_GCC
		void setDataString(DataType type, std::string* value, unsigned int count)
#endif
		{
			this->count = count;
			this->dataType = type;

			//Setting the data
			if (data) delete[] data;

			size = 0;

			for (unsigned int i = 0; i < count; i++)
			{
				size += 2;
				size += (unsigned int)value[i].length();
			}

			data = new byte[size];

			unsigned int pointer = 0;

			for (unsigned int i = 0; i < count; i++)
				pointer = Writer::writeBytes<std::string>(data, pointer, value[i]);
		}

	public:
		Array() : dataType(DataType::DATA_UNKNOWN), data(nullptr), count(0), name("") { }
		Array(std::string name, byte* value, unsigned int count) : name(name), data(nullptr) { setData<byte>(DataType::DATA_CHAR, value, count); }
		Array(std::string name, bool* value, unsigned int count) : name(name), data(nullptr) { setData<bool>(DataType::DATA_BOOL, value, count); }
		Array(std::string name, char* value, unsigned int count) : name(name), data(nullptr) { setData<char>(DataType::DATA_CHAR, value, count); }
		Array(std::string name, short* value, unsigned int count) : name(name), data(nullptr) { setData<short>(DataType::DATA_SHORT, value, count); }
		Array(std::string name, int* value, unsigned int count) : name(name), data(nullptr) { setData<int>(DataType::DATA_INT, value, count); }
		Array(std::string name, float* value, unsigned int count) : name(name), data(nullptr) { setData<float>(DataType::DATA_FLOAT, value, count); }
		Array(std::string name, long long* value, unsigned int count) : name(name), data(nullptr) { setData<long long>(DataType::DATA_LONG_LONG, value, count); }
		Array(std::string name, double* value, unsigned int count) : name(name), data(nullptr) { setData<double>(DataType::DATA_DOUBLE, value, count); }
#ifdef COMPILER_MSVC
		Array(std::string name, std::string* value, unsigned int count) : name(name), data(nullptr) { setData<std::string>(DataType::DATA_STRING, value, count); }
#elif defined COMPILER_GCC
		Array(std::string name, std::string* value, unsigned int count) : name(name), data(nullptr) { setDataString(DataType::DATA_STRING, value, count); }
#endif
		~Array() { if (data) delete[] data; }

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace(this->getSize())) return false;

			buffer.writeBytes<byte>(DataType::DATA_ARRAY);
			buffer.writeBytes<std::string>(name);
			buffer.writeBytes<byte>(this->dataType);
			buffer.writeBytes<unsigned int>(this->count);

			unsigned int s;

			if (dataType != DataType::DATA_STRING)
				s = sizeOf(dataType) * count;
			else
				s = size;

			buffer.copy(data, s);

			return true;
		}

		void read(Buffer& buffer)
		{
			DataType type = (DataType)buffer.readBytes<byte>();

			assert(type == DataType::DATA_ARRAY);

			this->name = buffer.readBytes<std::string>();
			this->dataType = (DataType)buffer.readBytes<byte>();
			this->count = buffer.readBytes<unsigned int>();

			if (data) delete[] data;

			if (dataType != DATA_STRING)
			{
				data = new byte[count * sizeOf(dataType)];

				memcpy(data, ((byte*)buffer.getStart() + buffer.getOffset()), count * sizeOf(dataType));

				buffer.addOffset(count * sizeOf(dataType));
			}
			else
			{
				size = 0;

				for (unsigned int i = 0; i < count; i++)
				{
					size += Reader::readBytes<unsigned short>((byte*)buffer.getStart(), buffer.getOffset() + size) + sizeof(unsigned short);
				}

				data = new byte[size];

				memcpy(data, ((byte*)buffer.getStart() + buffer.getOffset()), size);

				buffer.addOffset(size);
			}
		}

		inline unsigned int getCount() const { return count; }
		inline DataType getDataType() const { return dataType; }
		inline const std::string& getName() const { return name; }

		template<class T>
		inline std::vector<T> getArray() const
		{
			std::vector<T> ret;

			unsigned int pointer = 0;

			for (unsigned int i = 0; i < count; i++)
			{
				ret.push_back(Reader::readBytes<T>(data, pointer));

				pointer += sizeof(T);
			}

			return ret;
		}

#ifdef COMPILER_MSVC
		template<>
		inline std::vector<std::string> getArray() const
		{
			std::vector<std::string> ret;

			unsigned int pointer = 0;

			for (unsigned int i = 0; i < count; i++)
			{
				ret.push_back(Reader::readBytes<std::string>(data, pointer));

				pointer += Reader::readBytes<unsigned short>(data, pointer) + sizeof(unsigned short);
			}

			return ret;
		}
#endif

		// This returns the data in little endian (necessary for >1 byte data types like shorts or ints)
		template<typename T>
		inline T* getRawArray(T* mem) const
		{
			unsigned int pointer = 0;

			for (unsigned int i = 0; i < count; i++)
			{
				mem[i] = Reader::readBytes<T>(data, pointer);

				pointer += sizeof(T);
			}

			return mem;
		}

		inline unsigned int getSize() const
		{
			if(dataType != DataType::DATA_STRING)
				return sizeof(byte) + sizeof(short) + (unsigned int)name.length() + sizeof(byte) + sizeof(int) + count * sizeOf(dataType);
			else
				return sizeof(byte) + sizeof(short) + (unsigned int)name.length() + sizeof(byte) + sizeof(int) + size;
		}
	};

#ifdef COMPILER_GCC
	template<>
	inline std::vector<std::string> Array::getArray() const
	{
		std::vector<std::string> ret;

		unsigned int pointer = 0;

		for (unsigned int i = 0; i < count; i++)
		{
			ret.push_back(Reader::readBytes<std::string>(data, pointer));

			pointer += Reader::readBytes<unsigned short>(data, pointer) + sizeof(unsigned short);
		}

		return ret;
	}
#endif

}
