#pragma once

#define VERSION		Cereal::Version::VERSION_LATEST

//#include "src/Writer.h"
//#include "src/Container.h"
//#include "src/Field.h"

namespace Cereal {

	typedef unsigned char byte;

	enum Version
	{
		VERSION_1_0		= 0x0100, // first byte = major version, second = minor (revision)

		VERSION_LATEST	= VERSION_1_0,
		VERSION_INVALID = 0xFFFF
	};

	enum ObjectType
	{
		TYPE_UNKNOWN	= 0,
		TYPE_FIELD		= 1,
		TYPE_ARRAY		= 2,
		TYPE_OBJECT		= 3
		//TYPE_DATABASE	= 4 -> databases don't have a type, right? We don't need  this one
	};

	enum DataType
	{
		DATA_UNKNOWN	= 0,
		DATA_BOOL		= 1,
		DATA_CHAR		= 2,
		DATA_SHORT		= 3,
		DATA_INT		= 4,
		DATA_LONG_LONG	= 5,
		DATA_FLOAT		= 6,
		DATA_DOUBLE		= 7,
		DATA_STRING		= 8,
		MOD_UNSIGNED	= (1 << 4) // unsigned modifier
	};

	inline unsigned short sizeOf(DataType type)
	{
		switch (type)
		{
		case DataType::DATA_CHAR: return sizeof(byte); break;
		case DataType::DATA_BOOL: return sizeof(bool); break;
		case DataType::DATA_SHORT: return sizeof(short); break;
		case DataType::DATA_INT: return sizeof(int); break;
		case DataType::DATA_FLOAT: return sizeof(float); break;
		case DataType::DATA_LONG_LONG: return sizeof(long long); break;
		case DataType::DATA_DOUBLE: return sizeof(double); break;

		default: return 0; break;
		}
	}

	inline unsigned short sizeOf(byte type)
	{
		switch (type)
		{
		case DataType::DATA_CHAR: return sizeof(byte); break;
		case DataType::DATA_BOOL: return sizeof(bool); break;
		case DataType::DATA_SHORT: return sizeof(short); break;
		case DataType::DATA_INT: return sizeof(int); break;
		case DataType::DATA_FLOAT: return sizeof(float); break;
		case DataType::DATA_LONG_LONG: return sizeof(long long); break;
		case DataType::DATA_DOUBLE: return sizeof(double); break;

		default: return 0; break;
		}
	}

}