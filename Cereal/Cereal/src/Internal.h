//  Cereal: A C++/C# Serialization library
//  Copyright (C) 2016  The Cereal Team
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#ifdef __GNUC__
#	define COMPILER_GCC
#elif defined _MSC_VER
#	define COMPILER_MSVC
#else
#	error Unknown compiler!
#endif

namespace Cereal {

	typedef unsigned char byte;

	static_assert(sizeof(byte) == 1, "Invalid byte size!");

	static_assert(sizeof(char) == 1, "Invalid char size!");
	static_assert(sizeof(short) == 2, "Invalid short size!");
	static_assert(sizeof(int) == 4, "Invalid int size!");
	static_assert(sizeof(float) == 4, "Invalid float size!");
	static_assert(sizeof(double) == 8, "Invalid double size!");

	static_assert(sizeof(unsigned char) == 1, "Invalid unsigned char size!");
	static_assert(sizeof(unsigned short) == 2, "Invalid unsigned short size!");
	static_assert(sizeof(unsigned int) == 4, "Invalid unsigned int size!");
	static_assert(sizeof(unsigned long long) == 8, "Invalid unsigned long long size!");


	enum Version
	{
		VERSION_1_0		= 0x0100, // first byte = major version, second = minor (revision)
		VERSION_2_0		= 0x0200,

		VERSION_LATEST	= VERSION_2_0,
		VERSION_INVALID = 0xFFFF
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

		DATA_FIELD		= 9,
		DATA_ARRAY		= 10,
		DATA_OBJECT		= 11,

		MOD_UNSIGNED	= (1 << 4) // unsigned modifier
	};

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

		default:
			throw std::invalid_argument("Invalid input data type!"); break;
		}
	}

}