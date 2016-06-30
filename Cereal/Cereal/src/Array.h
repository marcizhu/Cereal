//  Cereal: A C++ Serialization library
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

#include "Buffer.h"
#include "Reader.h"
#include "Writer.h" // Same as Field.h
#include "Internal.h"

namespace Cereal {

	class Array
	{
	private:
		DataType type;
		std::string name;
		DataType dataType;
		unsigned int count; // item count
		byte* data;

		template<class T>
		void setData(std::string name, DataType type, T* value, unsigned int count)
		{
			//Initialization of container
			this->type = DataType::DATA_ARRAY;
			this->name = name;
			this->count = count;
			this->dataType = type;

			//Setting the data
			if (data) delete[] data;

			data = new byte[sizeof(T) * count];

			unsigned int pointer = 0;

			for (unsigned int i = 0; i < count; i++)
				pointer = Writer::writeBytes<T>(data, pointer, value[i]);
		}

	public:
		Array() : dataType(DataType::DATA_UNKNOWN), data(nullptr), count(0) { name = "", type = DataType::DATA_ARRAY; }
		Array(std::string name, byte* value, unsigned int count) { setData<byte>(name, DataType::DATA_CHAR, value, count); }
		Array(std::string name, bool* value, unsigned int count) { setData<bool>(name, DataType::DATA_BOOL, value, count); }
		Array(std::string name, char* value, unsigned int count) { setData<char>(name, DataType::DATA_CHAR, value, count); }
		Array(std::string name, short* value, unsigned int count) { setData<short>(name, DataType::DATA_SHORT, value, count); }
		Array(std::string name, int* value, unsigned int count) { setData<int>(name, DataType::DATA_INT, value, count); }
		Array(std::string name, float* value, unsigned int count) { setData<float>(name, DataType::DATA_FLOAT, value, count); }
		Array(std::string name, long long* value, unsigned int count) { setData<long long>(name, DataType::DATA_LONG_LONG, value, count); }
		Array(std::string name, double* value, unsigned int count) { setData<double>(name, DataType::DATA_DOUBLE, value, count); }

		~Array() { if (data) delete[] data; }

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace(this->getSize())) return false;

			buffer.writeBytes<byte>(type);
			buffer.writeBytes<std::string>(name);
			buffer.writeBytes<byte>(this->dataType);
			buffer.writeBytes<unsigned int>(this->count);

			for (unsigned int i = 0; i < sizeOf(dataType) * count; i++)
				buffer.writeBytes<byte>(data[i]);

			return true;
		}

		void read(Buffer& buffer)
		{
			this->type = (DataType)buffer.readBytes<byte>();

			assert(type == DataType::DATA_ARRAY);

			this->name = buffer.readBytes<std::string>();

			this->dataType = (DataType)buffer.readBytes<byte>();
			this->count = buffer.readBytes<unsigned int>();

			if (data) delete[] data;

			data = new byte[count * sizeOf(dataType)];

			memcpy(data, ((byte*)buffer.getStart() + buffer.getOffset()), count * sizeOf(dataType));
		}

		inline short getCount() const { return count; }
		inline DataType getContainerType() const { return type; }
		inline DataType getDataType() const { return dataType; }
		const std::string& getName() const { return name; }

		template<class T>
		inline std::vector<T> getArray() const
		{
			std::vector<T> ret;

			unsigned int pointer = 0;

			for (int i = 0; i < count; i++)
			{
				ret.push_back(Reader::readBytes<T>(data, pointer));

				pointer += sizeof(T);
			}

			return ret;
		}

		inline byte* getRawArray() const {
			return data;
		}

		inline unsigned int getSize() const { return sizeof(byte) + sizeof(short) + name.length() + sizeof(byte) + sizeof(int) + count * sizeOf(dataType); }
	};


}