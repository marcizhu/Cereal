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

namespace Cereal {

	typedef unsigned char byte;

	enum Version
	{
		VERSION_1_0		= 0x0100, // first byte = major version, second = minor (revision)

		VERSION_LATEST	= VERSION_1_0,
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

		default: return 0; break;
		}
	}

}