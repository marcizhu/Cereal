#pragma once

#include <string>

#include "Writer.h"
#include "../Cereal.h"

namespace Cereal {

	class Array
	{
	private:
		byte type;
		std::string name;
		DataType dataType;
		short count; // item count
		byte* data;

		template<class T>
		void setData(std::string name, DataType type, T* value, short count)
		{
			//Initialization of container
			this->type = DataType::DATA_ARRAY;
			this->name = name;
			this->count = count;
			this->dataType = type;

			//Setting the data
			if (data) delete[] data;

			data = new byte[sizeof(T) * count];

			int pointer = 0;

			for (short i = 0; i < count; i++)
				pointer = Writer::writeBytes<T>(data, pointer, value[i]);
		}

	public:
		inline Array() : dataType(DataType::DATA_UNKNOWN), data(nullptr), count(0) { name = "", type = DataType::DATA_ARRAY; }
		inline Array(std::string name, byte* value, int count) { setData<byte>(name, DataType::DATA_CHAR, value, count); }
		inline Array(std::string name, bool* value, int count) { setData<bool>(name, DataType::DATA_BOOL, value, count); }
		inline Array(std::string name, char* value, int count) { setData<char>(name, DataType::DATA_CHAR, value, count); }
		inline Array(std::string name, short* value, int count) { setData<short>(name, DataType::DATA_SHORT, value, count); }
		inline Array(std::string name, int* value, int count) { setData<int>(name, DataType::DATA_INT, value, count); }
		inline Array(std::string name, float* value, int count) { setData<float>(name, DataType::DATA_FLOAT, value, count); }
		inline Array(std::string name, long long* value, int count) { setData<long long>(name, DataType::DATA_LONG_LONG, value, count); }
		inline Array(std::string name, double* value, int count) { setData<double>(name, DataType::DATA_DOUBLE, value, count); }

		~Array() { if (data) delete[] data; }

		unsigned int write(byte* dest, int pointer) const
		{
			pointer = Writer::writeBytes<byte>(dest, pointer, type);
			pointer = Writer::writeBytes<std::string>(dest, pointer, name);
			pointer = Writer::writeBytes<byte>(dest, pointer, this->dataType); //write data type
			pointer = Writer::writeBytes<short>(dest, pointer, this->count);

			for (int i = 0; i < sizeOf(dataType) * count; i++)
				pointer = Writer::writeBytes<byte>(dest, pointer, data[i]);

			return pointer;
		}

		void read(byte* dest, int pointer)
		{
			this->type = Reader::readBytes<byte>(dest, pointer++);

			assert(type == DataType::DATA_ARRAY);

			this->name = Reader::readBytes<std::string>(dest, pointer);

			pointer += sizeof(short) + name.length(); // sizeof Short (length) + length of string - 1 (the buffer starts at 0)

			this->dataType = (DataType)Reader::readBytes<byte>(dest, pointer++);
			this->count = Reader::readBytes<short>(dest, pointer);

			pointer += sizeof(short);

			if (data) delete[] data;

			data = new byte[count * sizeOf(dataType)];

			memcpy(data, (dest + pointer), count * sizeOf(dataType));
		}

		inline short getCount() const { return count; }
		inline DataType getType() const  { return dataType; }
		const std::string& getName() const { return name; }
		byte getContainerType() const { return type; }

		template<class T>
		inline T* getArray() const
		{
			T* ret = new T[count];

			unsigned int pointer = 0;

			for (int i = 0; i < count; i++)
			{
				ret[i] = Reader::readBytes<T>(data, pointer);

				pointer += sizeof(T);
			}

			return ret;
		}

		inline unsigned int getSize() const { return sizeof(byte) + sizeof(short) + name.length() + sizeof(byte) + sizeof(short) + count * sizeOf(dataType); }
	};


}