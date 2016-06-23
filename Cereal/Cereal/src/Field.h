#pragma once

#include <string>

#include "Writer.h"
#include "Reader.h"
#include "..\Cereal.h"

namespace Cereal {

	class Field
	{
	private:
		byte type;
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

		// This code is unnecessary

		/*template<>
		void setData<std::string>(std::string name, DataType type, std::string value)
		{
			//Initialization of container
			this->type = DataType::DATA_FIELD;
			this->name = name;

			dataType = type;

			//Setting the data
			data = new byte[value.length() + 2];
			int ptr = Writer::writeBytes<unsigned short>(data, 0, value.length());

			for (unsigned int i = 0; i < value.length(); i++)
			{
				ptr = Writer::writeBytes<char>(data, ptr, value[i]);
			}
		}*/

	public:
		//constructor for each field type
		inline Field() : data(nullptr), dataType(DataType::DATA_UNKNOWN) { name = "", type = DataType::DATA_FIELD; }
		inline Field(std::string name, byte value) { setData(name, DataType::DATA_CHAR /* | MOD_UNSIGNED*/, value); }
		inline Field(std::string name, bool value) { setData(name, DataType::DATA_BOOL, value); }
		inline Field(std::string name, char value) { setData(name, DataType::DATA_CHAR, value); }
		inline Field(std::string name, short value) { setData(name, DataType::DATA_SHORT, value); }
		inline Field(std::string name, int value) { setData(name, DataType::DATA_INT, value); }
		inline Field(std::string name, float value) { setData(name, DataType::DATA_FLOAT, value); }
		inline Field(std::string name, long long value) { setData(name, DataType::DATA_LONG_LONG, value); }
		inline Field(std::string name, double value) { setData(name, DataType::DATA_DOUBLE, value); }
		inline Field(std::string name, std::string value) { setData(name, DataType::DATA_STRING, value); }

		~Field() { if(data) delete[] data; }

		unsigned int write(byte* dest, int pointer) const
		{
			pointer = Writer::writeBytes<byte>(dest, pointer, type);
			pointer = Writer::writeBytes<std::string>(dest, pointer, name);
			pointer = Writer::writeBytes<byte>(dest, pointer, this->dataType); //write data type

			if (dataType != DataType::DATA_STRING)
			{
				for (int i = 0; i < sizeOf(dataType); i++)
				{
					pointer = Writer::writeBytes<byte>(dest, pointer, data[i]);
				}
			}
			else
			{
				short len = Reader::readBytes<short>(data, 0) + 2;

				for (int i = 0; i < len; i++)
				{
					pointer = Writer::writeBytes<byte>(dest, pointer, data[i]);
				}
			}

			return pointer;
		}

		void read(byte* dest, int pointer)
		{
			byte type = Reader::readBytes<byte>(dest, pointer++);

			assert(type == DataType::DATA_FIELD);

			std::string name = Reader::readBytes<std::string>(dest, pointer);

			pointer += sizeof(short) + name.length(); // sizeof Short ( length) + length of string - 1 (the buffer starts at 0)

			DataType dataType = (DataType)Reader::readBytes<byte>(dest, pointer++);

			switch (dataType)
			{
				case DataType::DATA_BOOL: setData<bool>(name, dataType, Reader::readBytes<bool>(dest, pointer)); break;
				case DataType::DATA_CHAR: setData<byte>(name, dataType, Reader::readBytes<byte>(dest, pointer)); break;
				case DataType::DATA_SHORT: setData<short>(name, dataType, Reader::readBytes<short>(dest, pointer)); break;
				case DataType::DATA_INT: setData<int>(name, dataType, Reader::readBytes<int>(dest, pointer)); break;
				case DataType::DATA_LONG_LONG: setData<long long>(name, dataType, Reader::readBytes<long long>(dest, pointer)); break;
				case DataType::DATA_FLOAT: setData<float>(name, dataType, Reader::readBytes<float>(dest, pointer)); break;
				case DataType::DATA_DOUBLE: setData<double>(name, dataType, Reader::readBytes<double>(dest, pointer)); break;
				case DataType::DATA_STRING: setData<std::string>(name, dataType, Reader::readBytes<std::string>(dest, pointer)); break;
				default: assert(false); break;
			}
		}

		template<class T>
		inline T getValue() const { return Reader::readBytes<T>(data, 0); }

		byte getContainerType() const { return type; }
		const std::string& getName() const { return name; }
		inline DataType getType() const { return dataType; }

		inline unsigned int getSize() const { return sizeof(byte) + sizeof(short) + name.length() + sizeof(byte) + sizeOf(dataType); }
	};

}
