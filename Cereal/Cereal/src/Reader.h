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

	class Reader
	{
	public:
		template<typename T>
		static T readBytes(byte* src, unsigned int pointer)
		{
			T value = 0;

			for (unsigned int i = 0; i < sizeof(T); i++)
			{
				value |= ((T)src[pointer + i] << ((sizeof(T) * 8 - 8) - (i * 8)));
			}

			return value;
		}

#ifdef COMPILER_MSVC
		template<>
		static float readBytes<float>(byte* src, unsigned int pointer)
		{
			unsigned int value = 0;

			for (int i = 0; i < (int) sizeof(float); i++)
			{
				value |= (src[pointer + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			float result;

			memcpy(&result, &value, sizeof(float));

			return result;
		}

		template<>
		static bool readBytes<bool>(byte* src, unsigned int pointer) { return src[pointer] != 0; }

		template<>
		static double readBytes<double>(byte* src, unsigned int pointer)
		{
			unsigned long long value = src[pointer] << (sizeof(int) * 8 - 8);

			for (int i = 0; i < (int) sizeof(double); i++)
			{
				value |= ((unsigned long long)src[pointer + i] << ((sizeof(unsigned long long) * 8 - 8) - (i * 8)));
			}

			double result;
			memcpy(&result, &value, sizeof(double));

			return result;
		}

		template<>
		static std::string readBytes<std::string>(byte* src, unsigned int pointer)
		{
			std::string value = "";

			unsigned short size = readBytes<unsigned short>(src, pointer);

			for (unsigned int i = pointer + 2; i < pointer + size + 2; i++)
			{
				value += readBytes<char>(src, i);
			}

			return value;
		}
#endif
	};

#ifdef COMPILER_GCC
	template<>
	float Reader::readBytes<float>(byte* src, unsigned int pointer)
	{
		unsigned int value = 0;

		for (int i = 0; i < (int) sizeof(float); i++)
		{
			value |= (src[pointer + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
		}

		float result;

		memcpy(&result, &value, sizeof(float));

		return result;
	}

	template<>
	bool Reader::readBytes<bool>(byte* src, unsigned int pointer) { return src[pointer] != 0; }

	template<>
	double Reader::readBytes<double>(byte* src, unsigned int pointer)
	{
		unsigned long long value = src[pointer] << (sizeof(int) * 8 - 8);

		for (int i = 0; i < (int) sizeof(double); i++)
		{
			value |= ((unsigned long long)src[pointer + i] << ((sizeof(unsigned long long) * 8 - 8) - (i * 8)));
		}

		double result;
		memcpy(&result, &value, sizeof(double));

		return result;
	}

	template<>
	std::string Reader::readBytes<std::string>(byte* src, unsigned int pointer)
	{
		std::string value = "";

		unsigned short size = readBytes<unsigned short>(src, pointer);

		for (unsigned int i = pointer + 2; i < pointer + size + 2; i++)
		{
			value += readBytes<char>(src, i);
		}

		return value;
	}
#endif

}
