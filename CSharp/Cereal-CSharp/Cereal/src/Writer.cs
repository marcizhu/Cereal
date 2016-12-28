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
	public static class Writer
	{
		public static uint writeBytes<T>(byte[] dest, uint pointer, T value)
		{
			byte size = (byte)Marshal.SizeOf<T>();

			for (uint i = 0; i < size; i++)
			{
				dest[pointer++] = (value >> ((sizeof(T) - 1) * 8 - i * 8)) & 0xFF;
			}

			return pointer;
		}

		public static uint writeBytes(byte[] dest, uint pointer, string str)
		{
			ushort size = (ushort)str.Length;

			//assert(size <= 65535);

			pointer = writeBytes<ushort>(dest, pointer, size);

			for (ushort i = 0; i < size; i++)
			{
				pointer = writeBytes<char>(dest, pointer, str[i]);
			}

			return pointer;
		}

		public static unsafe uint writeBytes(byte[] dest, uint pointer, float data)
		{
			uint x;

			*(uint*)&x = *(uint*)&data;

			return writeBytes<uint>(dest, pointer, x);
		}

		public static unsafe uint writeBytes(byte[] dest, uint pointer, double data)
		{
			UInt64 x;

			*(UInt64*)&x = *(UInt64*)&data;

			return writeBytes<UInt64>(dest, pointer, x);
		}

	};

}