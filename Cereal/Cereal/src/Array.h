#pragma once

#include "Container.h"

namespace Cereal {

	class Array : public Container
	{
	private:
		DataType dataType = DataType::DATA_UNKNOWN;
		byte* data = nullptr;

	public:
		inline Array(std::string name, byte* value, int count) { setData(name, DataType::DATA_CHAR, value); }
		inline Array(std::string name, bool* value, int count) { setData(name, DataType::DATA_BOOL, value); }
		inline Array(std::string name, char* value, int count) { setData(name, DataType::DATA_CHAR, value); }
		inline Array(std::string name, short* value, int count) { setData(name, DataType::DATA_SHORT, value); }
		inline Array(std::string name, int* value, int count) { setData(name, DataType::DATA_INT, value); }
		inline Array(std::string name, float* value, int count) { setData(name, DataType::DATA_FLOAT, value); }
		inline Array(std::string name, long long* value, int count) { setData(name, DataType::DATA_LONG_LONG, value); }
		inline Array(std::string name, double* value, int count) { setData(name, DataType::DATA_DOUBLE, value); }
		inline Array(std::string name, std::string* value, int count) { setData(name, DataType::DATA_STRING, value); }

		~Array() { if (data) delete[] data; }

		inline int write(byte* dest, int pointer)
		{
			pointer = this->writeContainer(dest, pointer);
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

		static Field read(byte* dest, int pointer)
		{
			byte type = Reader::readBytes<byte>(dest, pointer++);

			assert(type == ObjectType::TYPE_FIELD);

			std::string name = Reader::readBytes<std::string>(dest, pointer);

			pointer += sizeof(short) + name.length() - 1; // sizeof Short ( length) + length of string - 1 (null termination character)

			byte dataType = Reader::readBytes<byte>(dest, pointer++);

			switch (dataType)
			{
			case DataType::DATA_BOOL: return Field(name, Reader::readBytes<bool>(dest, pointer));
			case DataType::DATA_CHAR: return Field(name, Reader::readBytes<byte>(dest, pointer));
			case DataType::DATA_SHORT: return Field(name, Reader::readBytes<short>(dest, pointer));
			case DataType::DATA_INT: return Field(name, Reader::readBytes<int>(dest, pointer));
			case DataType::DATA_LONG_LONG: return Field(name, Reader::readBytes<long long>(dest, pointer));
			case DataType::DATA_FLOAT: return Field(name, Reader::readBytes<float>(dest, pointer));
			case DataType::DATA_DOUBLE: return Field(name, Reader::readBytes<double>(dest, pointer));
			case DataType::DATA_STRING: return Field(name, Reader::readBytes<std::string>(dest, pointer));
			}

			assert(false);

			return Field("", -1);
		}

		template<class T>
		inline T getValue() { return Reader::readBytes<T>(data, 0); }

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

		template<>
		void setData<std::string>(std::string name, DataType type, std::string value)
		{
			//Initialization of container
			this->type = ObjectType::TYPE_FIELD;
			this->name = name;

			dataType = type;

			//Setting the data
			data = new byte[value.length() + 2];
			int ptr = Writer::writeBytes<short>(data, 0, value.length());

			for (unsigned int i = 0; i < value.length(); i++)
			{
				ptr = Writer::writeBytes<char>(data, ptr, value[i]);
			}
		}

	};
	};

}