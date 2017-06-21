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
#include <assert.h>

#include "Buffer.h"
#include "Reader.h" // Because of the internal buffer. Do we change it to be a Buffer class?
#include "Writer.h"
#include "Internal.h"

namespace Cereal {

	class Field
	{
	private:
		std::string name;
		DataType dataType;
		byte* data = nullptr;

		template<class T>
		void setData(DataType type, T value)
		{
			dataType = type;

			if (data) delete[] data;

			//Setting the data
			data = new byte[sizeof(T)];
			Writer::writeBytes<T>(data, 0, value);
		}

#ifdef COMPILER_MSVC
		template<>
		void setData<std::string>(DataType type, std::string value)
#elif defined COMPILER_GCC
		void setDataString(DataType type, std::string value)
#endif
		{
			dataType = type;

			//Setting the data
			if (data) delete[] data;

			data = new byte[value.length() + 2];

			int ptr = Writer::writeBytes<unsigned short>(data, 0, (unsigned short)value.length());

			for (unsigned int i = 0; i < value.length(); i++)
			{
				ptr = Writer::writeBytes<char>(data, ptr, value[i]);
			}
		}

	public:
		//constructor for each field type
		Field() : data(nullptr), dataType(DataType::DATA_UNKNOWN), name("") { }
		Field(std::string name, byte value) : name(name) { setData<byte>(DataType::DATA_CHAR /* | MOD_UNSIGNED*/, value); }
		Field(std::string name, bool value) : name(name) { setData<bool>(DataType::DATA_BOOL, value); }
		Field(std::string name, char value) : name(name) { setData<char>(DataType::DATA_CHAR, value); }
		Field(std::string name, short value) : name(name) { setData<short>(DataType::DATA_SHORT, value); }
		Field(std::string name, int value) : name(name) { setData<int>(DataType::DATA_INT, value); }
		Field(std::string name, float value) : name(name) { setData<float>(DataType::DATA_FLOAT, value); }
		Field(std::string name, long long value) : name(name) { setData<long long>(DataType::DATA_LONG_LONG, value); }
		Field(std::string name, double value) : name(name) { setData<double>(DataType::DATA_DOUBLE, value); }
#ifdef COMPILER_MSVC
		Field(std::string name, std::string value) : name(name) { setData<std::string>(DataType::DATA_STRING, value); }
#elif defined COMPILER_GCC
		Field(std::string name, std::string value) : name(name) { setDataString(DataType::DATA_STRING, value); }
#endif

		~Field() { if(data) delete[] data; }

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace(this->getSize())) return false;

			buffer.writeBytes<byte>(DataType::DATA_FIELD);
			buffer.writeBytes<std::string>(name);
			buffer.writeBytes<byte>(this->dataType); //write data type

			if (dataType != DataType::DATA_STRING)
			{
				for (int i = 0; i < sizeOf(dataType); i++)
				{
					buffer.writeBytes<byte>(data[i]);
				}
			}
			else
			{
				short len = Reader::readBytes<short>(data, 0) + 2;

				for (int i = 0; i < len; i++)
				{
					buffer.writeBytes<byte>(data[i]);
				}
			}

			return true;
		}

		void read(Buffer& buffer)
		{
			byte type = buffer.readBytes<byte>();

			assert(type == DataType::DATA_FIELD);

			this->name = buffer.readBytes<std::string>();

			DataType dataType = (DataType)buffer.readBytes<byte>();

			switch (dataType)
			{
				case DataType::DATA_BOOL: setData<bool>(dataType, buffer.readBytes<bool>()); break;
				case DataType::DATA_CHAR: setData<byte>(dataType, buffer.readBytes<byte>()); break;
				case DataType::DATA_SHORT: setData<short>(dataType, buffer.readBytes<short>()); break;
				case DataType::DATA_INT: setData<int>(dataType, buffer.readBytes<int>()); break;
				case DataType::DATA_LONG_LONG: setData<long long>(dataType, buffer.readBytes<long long>()); break;
				case DataType::DATA_FLOAT: setData<float>(dataType, buffer.readBytes<float>()); break;
				case DataType::DATA_DOUBLE: setData<double>(dataType, buffer.readBytes<double>()); break;
#ifdef COMPILER_MSVC
				case DataType::DATA_STRING: setData<std::string>(dataType, buffer.readBytes<std::string>()); break;
#elif defined COMPILER_GCC
				case DataType::DATA_STRING: setDataString(dataType, buffer.readBytes<std::string>()); break;
#endif
				default: throw std::logic_error("Invalid data type!"); break;
			}
		}

		template<class T>
		inline T getValue() const { return Reader::readBytes<T>(data, 0); }

		inline const std::string& getName() const { return name; }
		inline DataType getDataType() const { return dataType; }

		inline unsigned int getSize() const
		{
			if (dataType == DataType::DATA_STRING)
			{
				return sizeof(byte) + sizeof(short) + (unsigned int)name.length() + sizeof(byte) + sizeof(short) + Reader::readBytes<unsigned short>(data, 0);
			}

			return sizeof(byte) + sizeof(short) + (unsigned int)name.length() + sizeof(byte) + sizeOf(dataType);
		}
	};

}
