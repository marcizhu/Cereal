#pragma once

#include "Writer.h"
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

	protected:
		template<class T>
		void setData(std::string name, DataType type, T value)
		{
			//Initialization of container
			this->type = ObjectType::TYPE_FIELD;
			this->name = name;

			dataType = type;

			//Setting the data
			data = new byte[sizeOf(type)];
			Writer::writeBytes(data, 0, value);
		}

	};

}
