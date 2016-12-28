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

using System;
using System.Runtime.InteropServices;

namespace Cereal
{
	public static class Reader
	{
		public static T readBytes<T>(byte[] src, uint pointer)
		{
			T value;

			for (int i = 0; i < (uint)Marshal.SizeOf<T>(); i++)
			{
				value |= (src[pointer + i] << ((sizeof(T) * 8 - 8) - (i * 8)));
			}

			return value;
		}

		public static float readBytes(byte[] src, uint pointer)
		{
			uint value = 0;

			for (int i = 0; i < (int)sizeof(float); i++)
			{
				value |= (src[pointer + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			float result;

			memcpy_s(&result, sizeof(float), &value, sizeof(float));

			return result;
		}

		public static bool readBytes(byte[] src, uint pointer) { return src[pointer] != 0; }

		public static double readBytes(byte[] src, uint pointer)
		{
			UInt64 value = src[pointer] << (sizeof(int) * 8 - 8);

			for (int i = 0; i < (int)sizeof(double); i++)
			{
				value |= (src[pointer + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			double result;
			memcpy_s(&result, sizeof(double), &value, sizeof(double));

			return result;
		}

		public static string readBytes(byte[] src, uint pointer)
		{
			string value = "";

			ushort size = readBytes < ushort> (src, pointer);

			for (uint i = pointer + 2; i < pointer + size + 2; i++)
			{
				value += readBytes<char>(src, i);
			}

			return value;
		}
	};

}