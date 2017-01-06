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
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;

namespace Cereal
{
	public static class Writer
	{
		public static uint writeBytes<T>(byte[] dest, uint pointer, T value)
		{
			MemoryStream ms = new MemoryStream();
			BinaryWriter writer = new BinaryWriter(ms);

			int size = Marshal.SizeOf(value);

			switch (size)
			{
				case 8:
					writer.Write(Convert.ToInt64(value)); break;

				case 4:
					writer.Write(Convert.ToInt32(value)); break;

				case 2:
					writer.Write(Convert.ToInt16(value)); break;

				case 1:
					writer.Write(Convert.ToByte(value)); break;

				default:
					throw new ArgumentOutOfRangeException("sizeof(value)", "Invalid call to Writer::writeBytes<T>");
			}

			byte[] src = ms.ToArray();

			for (int i = 0; i < size; i++)
			{
				dest[pointer + i] = src[size - 1 - i];
			}

			return pointer + (uint)size;
		}

		public static uint writeBytes(byte[] dest, uint pointer, bool value)
		{
			dest[pointer] = value != false ? (byte)1 : (byte)0;

			return pointer + 1;
		}

		public static uint writeBytes(byte[] dest, uint pointer, string str)
		{
			ushort size = (ushort)str.Length;

			Debug.Assert(size <= 65535);

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