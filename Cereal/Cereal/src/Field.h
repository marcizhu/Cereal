#pragma once

#include "Writer.h"
#include "Reader.h"
#include "Container.h"
#include "..\Cereal.h"

namespace Cereal {

	class Field : public Container
	{
	private:
		DataType dataType = DataType::DATA_UNKNOWN;
		byte* data = nullptr;

	public:
		//constructor for each field type
		inline Field(std::string name, byte value) { setData(name, DataType::DATA_CHAR /* | MOD_UNSIGNED*/, value); }
		inline Field(std::string name, bool value) { setData(name, DataType::DATA_BOOL, value); }
		inline Field(std::string name, char value) { setData(name, DataType::DATA_CHAR, value); }
		inline Field(std::string name, short value) { setData(name, DataType::DATA_SHORT, value); }
		inline Field(std::string name, int value) { setData(name, DataType::DATA_INT, value); }
		inline Field(std::string name, float value) { setData(name, DataType::DATA_FLOAT, value); }
		inline Field(std::string name, long long value) { setData(name, DataType::DATA_LONG_LONG, value); }
		inline Field(std::string name, double value) { setData(name, DataType::DATA_DOUBLE, value); }

		~Field() { if(data) delete[] data; }

		inline int write(byte* dest, int pointer)
		{
			pointer = this->writeContainer(dest, pointer);
			pointer = Writer::writeBytes<byte>(dest, pointer, this->dataType); //write data type

			for (int i = 0; i < sizeOf(dataType); i++)
			{
				pointer = Writer::writeBytes<byte>(dest, pointer, data[i]);
			}

			return pointer;
		}

		inline static Field read(byte* dest, int pointer) {
			byte type = Reader::readBytes<byte>(dest, pointer++);
			assert(type == ObjectType::TYPE_FIELD);
			std::string name = Reader::readBytes<std::string>(dest, pointer);
			pointer += 2 + name.length() - 1; // sizeof Short ( length) + length of string - 1 (null termination character)
			

			byte dataType = Reader::readBytes<byte>(dest, pointer++);
			switch (dataType) {
			case DataType::DATA_BOOL: return Field(name, Reader::readBytes<bool>(dest, pointer));
			case DataType::DATA_CHAR: return Field(name, Reader::readBytes<byte>(dest, pointer));
			case DataType::DATA_SHORT: return Field(name, Reader::readBytes<short>(dest, pointer));
			case DataType::DATA_INT: return Field(name, Reader::readBytes<int>(dest, pointer));
			case DataType::DATA_LONG_LONG: return Field(name, Reader::readBytes<long long>(dest, pointer));
			case DataType::DATA_FLOAT: return Field(name, Reader::readBytes<float>(dest, pointer));
			case DataType::DATA_DOUBLE: return Field(name, Reader::readBytes<double>(dest, pointer));
			}

			assert(false);
			return Field("", -1);
		}
		
		template<class T>
		inline T getValue() {
			return Reader::readBytes<T>(data, 0);
		}

	protected:
		template<class T>
		void setData(std::string name, DataType type, T value)
		{
			//Initialization of container
			this->type = ObjectType::TYPE_FIELD;
			this->name = name;

			dataType = type;

			//Setting the data
			data = new byte[sizeof(T)];
			Writer::writeBytes(data, 0, value);
		}

	};

}
