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

#include <string>

#include "Internal.h"

namespace Cereal {

	class Writer
	{
	public:
		template<typename T>
		static unsigned int writeBytes(byte* dest, unsigned int pointer, T value)
		{
			for (unsigned int i = 0; i < sizeof(T); i++)
			{
				dest[pointer++] = (value >> ((sizeof(T) - 1) * 8 - i * 8)) & 0xFF;
			}

			return pointer;
		}

#ifdef COMPILER_MSVC
		template<>
		static unsigned int writeBytes<std::string>(byte* dest, unsigned int pointer, std::string string)
		{
			const unsigned short size = (unsigned short)string.length();

			if(size > 65535) throw std::overflow_error("String is too long!");

			pointer = writeBytes<unsigned short>(dest, pointer, size);

			for (unsigned short i = 0; i < size; i++)
			{
				pointer = writeBytes<char>(dest, pointer, string[i]);
			}

			return pointer;
		}

		template<>
		static unsigned int writeBytes<float>(byte* dest, unsigned int pointer, float data)
		{
			unsigned int x;

			*(unsigned int*)&x = *(unsigned int*)&data;

			return writeBytes<unsigned int>(dest, pointer, x);
		}

		template<>
		static unsigned int writeBytes<double>(byte* dest, unsigned int pointer, double data)
		{
			unsigned long long x;

			*(unsigned long long*)&x = *(unsigned long long*)&data;

			return writeBytes<unsigned long long>(dest, pointer, x);
		}
#endif

	};

#ifdef COMPILER_GCC
	template<>
	unsigned int Writer::writeBytes<std::string>(byte* dest, unsigned int pointer, std::string string)
	{
		const unsigned short size = (unsigned short)string.length();

		if(size > 65535) throw std::overflow_error("String is too long!");

		pointer = writeBytes<unsigned short>(dest, pointer, size);

		for (unsigned short i = 0; i < size; i++)
		{
			pointer = writeBytes<char>(dest, pointer, string[i]);
		}

		return pointer;
	}

	template<>
	unsigned int Writer::writeBytes<float>(byte* dest, unsigned int pointer, float data)
	{
		unsigned int x;

		*(unsigned int*)&x = *(unsigned int*)&data;

		return writeBytes<unsigned int>(dest, pointer, x);
	}

	template<>
	unsigned int Writer::writeBytes<double>(byte* dest, unsigned int pointer, double data)
	{
		unsigned long long x;

		*(unsigned long long*)&x = *(unsigned long long*)&data;

		return writeBytes<unsigned long long>(dest, pointer, x);
	}
#endif

}
