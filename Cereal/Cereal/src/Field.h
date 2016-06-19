#pragma once

#include "Writer.h"
#include "Container.h"

namespace cereal {

	class Field {
	private:
		Container container;
		byte dataType;
		byte* data = nullptr;

	public:
		//constructor for each field type
		inline Field(std::string name, byte value) { setData(name, value, SERIALIZATION_BYTE); }

		inline Field(std::string name, bool value) { setData(name, value, SERIALIZATION_BOOL); }

		inline Field(std::string name, short value) { setData(name, value, SERIALIZATION_SHORT); }

		inline Field(std::string name, int value) { setData(name, value, SERIALIZATION_INT); }

		inline Field(std::string name, float value) { setData(name, value, SERIALIZATION_FLOAT); }

		inline Field(std::string name, long long value) { setData(name, value, SERIALIZATION_LONG); }

		inline Field(std::string name, double value) { setData(name, value, SERIALIZATION_DOUBLE); }


		inline int writeBytes(byte* dest, int pointer) {
			pointer = container.writeBytes(dest, pointer);
			pointer = Writer::writeBytes(dest, pointer, dataType);
			for (int i = 0; i < Container::sizeOf(dataType); i++) {
				pointer = Writer::writeBytes(dest, pointer, data[i]);
			}
			return pointer;
		}

	protected:
		template<class T>
		void setData(std::string name, T value, byte type) {
			//Initialization of container
			container.type = SERIALIZATION_FIELD;
			container.name = name;

			dataType = type;

			//Setting the data
			data = new byte[Container::sizeOf(type)];
			Writer::writeBytes(data, 0, value);
		}

	};

}
