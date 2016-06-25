#pragma once

#include <string>

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
		unsigned short count; // item count
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
		Array() : dataType(DataType::DATA_UNKNOWN), data(nullptr), count(0) { name = "", type = DataType::DATA_ARRAY; }
		Array(std::string name, byte* value, int count) { setData<byte>(name, DataType::DATA_CHAR, value, count); }
		Array(std::string name, bool* value, int count) { setData<bool>(name, DataType::DATA_BOOL, value, count); }
		Array(std::string name, char* value, int count) { setData<char>(name, DataType::DATA_CHAR, value, count); }
		Array(std::string name, short* value, int count) { setData<short>(name, DataType::DATA_SHORT, value, count); }
		Array(std::string name, int* value, int count) { setData<int>(name, DataType::DATA_INT, value, count); }
		Array(std::string name, float* value, int count) { setData<float>(name, DataType::DATA_FLOAT, value, count); }
		Array(std::string name, long long* value, int count) { setData<long long>(name, DataType::DATA_LONG_LONG, value, count); }
		Array(std::string name, double* value, int count) { setData<double>(name, DataType::DATA_DOUBLE, value, count); }

		~Array() { if (data) delete[] data; }

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace(this->getSize())) return false;

			buffer.writeBytes<byte>(type);
			buffer.writeBytes<std::string>(name);
			buffer.writeBytes<byte>(this->dataType); //write data type
			buffer.writeBytes<short>(this->count);

			for (int i = 0; i < sizeOf(dataType) * count; i++)
				buffer.writeBytes<byte>(data[i]);

			return true;
		}

		void read(Buffer& buffer)
		{
			this->type = (DataType)buffer.readBytes<byte>();

			assert(type == DataType::DATA_ARRAY);

			this->name = buffer.readBytes<std::string>();

			this->dataType = (DataType)buffer.readBytes<byte>();
			this->count = buffer.readBytes<short>();

			if (data) delete[] data;

			data = new byte[count * sizeOf(dataType)];

			memcpy(data, ((byte*)buffer.getStart() + buffer.getOffset()), count * sizeOf(dataType));
		}

		inline short getCount() const { return count; }
		inline DataType getContainerType() const { return type; }
		inline DataType getDataType() const  { return dataType; }
		const std::string& getName() const { return name; }

		template<class T>
		inline std::vector<T>& getArray() const
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

		inline unsigned int getSize() const { return sizeof(byte) + sizeof(short) + name.length() + sizeof(byte) + sizeof(short) + count * sizeOf(dataType); }
	};


}