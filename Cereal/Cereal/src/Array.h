#pragma once

#include "Writer.h"
#include "Container.h"
#include "../Cereal.h"

namespace Cereal {

	class Array : public Container {
	private:
		DataType dataType;
		byte* data;
		short count; // item count

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

		inline int write(byte* dest, int pointer) const
		{
			pointer = this->writeContainer(dest, pointer);
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

			switch (dataType)
			{
				case DataType::DATA_BOOL: setData(name, dataType, (bool*)(dest + pointer), count); break;
				case DataType::DATA_CHAR: setData(name, dataType, (char*)(dest + pointer), count); break;
				case DataType::DATA_SHORT: setData(name, dataType, (short*)(dest + pointer), count); break;
				case DataType::DATA_INT: setData(name, dataType, (int*)(dest + pointer), count); break;
				case DataType::DATA_LONG_LONG: setData(name, dataType, (long long*)(dest + pointer), count); break;
				case DataType::DATA_FLOAT: setData(name, dataType, (float*)(dest + pointer), count); break;
				case DataType::DATA_DOUBLE: setData(name, dataType, (double*)(dest + pointer), count); break;
				default: assert(false); break;
			}
		}

		inline short getCount() const { return count; }
		inline DataType getType() const  { return dataType; }

		template<class T>
		inline T* getArray() const { return (T*)data; }
	};


}