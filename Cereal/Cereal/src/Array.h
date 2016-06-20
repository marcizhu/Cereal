#pragma once

#include "Writer.h"
#include "Container.h"
#include "../Cereal.h"


namespace Cereal {

	class Array : public Container {
	private:
		DataType dataType = DataType::DATA_UNKNOWN;
		byte* data = nullptr;
		short count;
		int dataSize = 0; //used for string array only!

	public:
		inline Array(std::string name, byte* value, int count) { setData(name, DataType::DATA_CHAR, value, count); }
		inline Array(std::string name, bool* value, int count) { setData(name, DataType::DATA_BOOL, value, count); }
		inline Array(std::string name, char* value, int count) { setData(name, DataType::DATA_CHAR, value, count); }
		inline Array(std::string name, short* value, int count) { setData(name, DataType::DATA_SHORT, value, count); }
		inline Array(std::string name, int* value, int count) { setData(name, DataType::DATA_INT, value, count); }
		inline Array(std::string name, float* value, int count) { setData(name, DataType::DATA_FLOAT, value, count); }
		inline Array(std::string name, long long* value, int count) { setData(name, DataType::DATA_LONG_LONG, value, count); }
		inline Array(std::string name, double* value, int count) { setData(name, DataType::DATA_DOUBLE, value, count); }
		inline Array(std::string name, std::string* value, int count) { setData(name, DataType::DATA_STRING, value, count); }

		~Array() { if (data) delete[] data; }

		inline int write(byte* dest, int pointer)
		{
			pointer = this->writeContainer(dest, pointer);
			pointer = Writer::writeBytes<byte>(dest, pointer, this->dataType); //write data type
			pointer = Writer::writeBytes<short>(dest, pointer, this->count);

			

			int size;

			if (sizeOf(dataType) != 0) 
				size = sizeof(dataType) * count; 
			else {
				pointer = Writer::writeBytes(dest, pointer, dataSize);
				size = dataSize;
			}

			for (int i = 0; i < size; i++) {
				pointer = Writer::writeBytes<byte>(dest, pointer, data[i]);
			}

			return pointer;
		}

		/*static Array read(byte* dest, int pointer)
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
		}*/

		template<class T>
		inline T* getArray() {
			T* array = new T[count];
			short size = sizeOf(dataType);
			int pointer = 0;
			for (int i = 0; i < count; i++) {
				array[i] = Reader::readBytes<T>(data, pointer);
				pointer += size;
			}
			return array;
		}

		template<>
		inline std::string* getArray() {
			std::string* array = new std::string[count];
			int pointer = 0;
			for (int i = 0; i < count; i++) {
				array[i] = Reader::readBytes<std::string>(data, pointer);
				pointer += 2 + array[i].length();
			}
			return array;
		}

		inline short getCount() { return count; }

	protected:
		template<class T>
		void setData(std::string name, DataType type, T* value, short count)
		{
			//Initialization of container
			this->type = ObjectType::TYPE_ARRAY;
			this->name = name;
			this->count = count;

			dataType = type;

			//Setting the data
			data = new byte[sizeof(T) * count];
			int pointer = 0;
			for (short i = 0; i < count; i++) {
				pointer = Writer::writeBytes(data, pointer, value[i]);
			}
		}

		template<>
		void setData<std::string>(std::string name, DataType type, std::string* value, short count)
		{
			//Initialization of container
			this->type = ObjectType::TYPE_ARRAY;
			this->name = name;
			this->count = count;

			dataType = type;

			//Setting the data
			int dataSize = 0;
			for (short i = 0; i < count; i++) {
				dataSize += 2;
				dataSize += value[i].length() - 1;
			}

			this->dataSize = dataSize;

			data = new byte[dataSize];
			int pointer = 0;
			for (short i = 0; i < count; i++) {
				pointer = Writer::writeBytes(data, pointer, value[i]);
			}

		}

	};


}