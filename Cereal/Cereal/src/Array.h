#pragma once

#include "Writer.h"
#include "Container.h"
#include "../Cereal.h"

namespace Cereal {

	class Array : public Container {
	private:
		DataType dataType = DataType::DATA_UNKNOWN;
		byte* data = nullptr;
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

		static Array read(byte* dest, int pointer)
		{
			byte type = Reader::readBytes<byte>(dest, pointer++);

			assert(type == DataType::DATA_ARRAY);

			std::string name = Reader::readBytes<std::string>(dest, pointer);

			pointer += sizeof(short) + name.length() - 1; // sizeof Short (length) + length of string - 1 (the buffer starts at 0)

			byte dataType = Reader::readBytes<byte>(dest, pointer++);
			short itemCount = Reader::readBytes<short>(dest, pointer);

			pointer += sizeof(short);

			switch (dataType)
			{
			case DataType::DATA_BOOL: return Array(name, (bool*)(dest + pointer), itemCount);
			case DataType::DATA_CHAR: return Array(name, (char*)(dest + pointer), itemCount);
			case DataType::DATA_SHORT: return Array(name, (short*)(dest + pointer), itemCount);
			case DataType::DATA_INT: return Array(name, (int*)(dest + pointer), itemCount);
			case DataType::DATA_LONG_LONG: return Array(name, (long long*)(dest + pointer), itemCount);
			case DataType::DATA_FLOAT: return Array(name, (float*)(dest + pointer), itemCount);
			case DataType::DATA_DOUBLE: return Array(name, (double*)(dest + pointer), itemCount);
			}

			assert(false);

			return Array("", (byte*)nullptr, 0);
		}

		inline short getCount() { return count; }

		template<class T>
		inline T* getArray() { return (T*)data; }

		template<>
		inline std::string* getArray<std::string>()
		{
			std::string* array = new std::string[count]; // FIXME: Memory leak!

			int pointer = 0;

			for (int i = 0; i < count; i++)
			{
				array[i] = Reader::readBytes<std::string>(data, pointer);
				pointer += 2 + array[i].length();
			}

			return array;
		}

	};


}