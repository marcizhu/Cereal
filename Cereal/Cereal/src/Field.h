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

#include "Buffer.h"
#include "Reader.h" // Because of the internal buffer. Do we change it to be a Buffer class?
#include "Writer.h"
#include "Internal.h"

namespace Cereal {

	class Field
	{
	private:
		DataType type;
		std::string name;
		DataType dataType;
		byte* data;

		template<class T>
		void setData(std::string name, DataType type, T value)
		{
			//Initialization of container
			this->type = DataType::DATA_FIELD;
			this->name = name;

			dataType = type;

			if (data) delete[] data;

			//Setting the data
			data = new byte[sizeof(T)];
			Writer::writeBytes<T>(data, 0, value);
		}

		template<>
		void setData<std::string>(std::string name, DataType type, std::string value)
		{
			//Initialization of container
			this->type = DataType::DATA_FIELD;
			this->name = name;

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
		Field() : data(nullptr), dataType(DataType::DATA_UNKNOWN) { name = "", type = DataType::DATA_FIELD; }
		Field(std::string name, byte value) { setData<byte>(name, DataType::DATA_CHAR /* | MOD_UNSIGNED*/, value); }
		Field(std::string name, bool value) { setData<bool>(name, DataType::DATA_BOOL, value); }
		Field(std::string name, char value) { setData<char>(name, DataType::DATA_CHAR, value); }
		Field(std::string name, short value) { setData<short>(name, DataType::DATA_SHORT, value); }
		Field(std::string name, int value) { setData<int>(name, DataType::DATA_INT, value); }
		Field(std::string name, float value) { setData<float>(name, DataType::DATA_FLOAT, value); }
		Field(std::string name, long long value) { setData<long long>(name, DataType::DATA_LONG_LONG, value); }
		Field(std::string name, double value) { setData<double>(name, DataType::DATA_DOUBLE, value); }
		Field(std::string name, std::string value) { setData<std::string>(name, DataType::DATA_STRING, value); }

		~Field() { if(data) delete[] data; }

		unsigned int write(Buffer& buffer) const
		{
			if (!buffer.hasSpace(this->getSize())) return false;

			buffer.writeBytes<byte>(type);
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

			std::string name = buffer.readBytes<std::string>();

			DataType dataType = (DataType)buffer.readBytes<byte>();

			switch (dataType)
			{
				case DataType::DATA_BOOL: setData<bool>(name, dataType, buffer.readBytes<bool>()); break;
				case DataType::DATA_CHAR: setData<byte>(name, dataType, buffer.readBytes<byte>()); break;
				case DataType::DATA_SHORT: setData<short>(name, dataType, buffer.readBytes<short>()); break;
				case DataType::DATA_INT: setData<int>(name, dataType, buffer.readBytes<int>()); break;
				case DataType::DATA_LONG_LONG: setData<long long>(name, dataType, buffer.readBytes<long long>()); break;
				case DataType::DATA_FLOAT: setData<float>(name, dataType, buffer.readBytes<float>()); break;
				case DataType::DATA_DOUBLE: setData<double>(name, dataType, buffer.readBytes<double>()); break;
				case DataType::DATA_STRING: setData<std::string>(name, dataType, buffer.readBytes<std::string>()); break;
				default: assert(false); break;
			}
		}

		template<class T>
		inline T getValue() const { return Reader::readBytes<T>(data, 0); }

		byte getContainerType() const { return type; }
		const std::string& getName() const { return name; }
		inline DataType getDataType() const { return dataType; }

		inline unsigned int getSize() const
		{
			if (dataType == DataType::DATA_STRING)
			{
				return sizeof(byte) + sizeof(short) + name.length() + sizeof(byte) + sizeof(short) + Reader::readBytes<unsigned short>(data, 0);
			}

			return sizeof(byte) + sizeof(short) + name.length() + sizeof(byte) + sizeOf(dataType);
		}
	};

}
