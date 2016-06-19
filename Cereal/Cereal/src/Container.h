#pragma once

#include <string>
#include "Writer.h"

#define SERIALIZATION_UNKNOWN	0
#define SERIALIZATION_FIELD		1
#define SERIALIZATION_ARRAY		2
#define SERIALIZATION_OBJECT	3	
#define SERIALIZATION_DATABASE	4

#define SERIALIZATION_BYTE		0
#define SERIALIZATION_BOOL		1
#define SERIALIZATION_SHORT		2
#define SERIALIZATION_INT		3
#define SERIALIZATION_FLOAT		4
#define SERIALIZATION_LONG		5
#define SERIALIZATION_DOUBLE	6

namespace cereal {

	struct Container {
		byte type;
		std::string name;

		int writeBytes(byte* dest, int pointer) {
			pointer = Writer::writeBytes(dest, pointer, type);
			pointer = Writer::writeBytes(dest, pointer, name);
			return pointer;
		}

		static Container readBytes(byte* dest, int pointer) {
			Container container;
			container.type = Writer::readBytes<byte>(dest, pointer);
			container.name = Writer::readBytes<std::string>(dest, pointer);
			return container;
		}

		static short sizeOf(byte type) {
			switch (type) {
			case SERIALIZATION_BYTE: return sizeof(byte);
			case SERIALIZATION_BOOL: return sizeof(byte);
			case SERIALIZATION_SHORT: return sizeof(short);
			case SERIALIZATION_INT: return sizeof(int);
			case SERIALIZATION_FLOAT: return sizeof(float);
			case SERIALIZATION_LONG: return sizeof(long long);
			case SERIALIZATION_DOUBLE: return sizeof(double);
			}
			assert(false);
			return -1;
		}

	};

}
