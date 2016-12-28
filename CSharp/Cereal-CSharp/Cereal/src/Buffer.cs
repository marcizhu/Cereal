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
using System.IO;
using System.Runtime.InteropServices;

namespace Cereal
{
	public class Buffer
	{
		private byte[] start;
		private uint offset;

		public Buffer(uint buffSize)
		{
			start = new byte[buffSize];

			cleanBuffer();
		}

		public Buffer(byte[] buffStart)
		{
			start = buffStart;
			cleanBuffer();
		}
		public Buffer(byte[] buffStart, uint offs, bool clean = false)
		{
			start = buffStart;
			offset = offs;

			if (clean) cleanBuffer();
		}

		~Buffer()
		{
			start = null;
			GC.Collect();
			GC.WaitForPendingFinalizers();

			offset = 0;
		}

		public void cleanBuffer() { System.Array.Clear(start, 0, start.Length); }

		public T readBytes<T>()
		{
			T value;

			for (int i = 0; i < (uint)Marshal.SizeOf<T>(); i++)
			{
				value |= (start[offset + i] << ((sizeof(T) * 8 - 8) - (i * 8)));
			}

			offset += sizeof(T);

			return value;
		}

		public float readBytes()
		{
			uint value = 0;

			for (uint i = 0; i < sizeof(float); i++)
			{
				value |= (start[offset + i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			float result;

			offset += sizeof(float);

			memcpy_s(&result, 4, &value, 4);

			return result;
		}

		public bool readBytes() { return start[offset++] != 0; }

		public double readBytes()
		{
			UInt64 value = (UInt64)(start[offset] << (sizeof(int) * 8 - 8));

			for (uint i = offset; i < offset + sizeof(float); i++)
			{
				value |= (start[i] << ((sizeof(int) * 8 - 8) - (i * 8)));
			}

			double result;
			memcpy_s(&result, 4, &value, 4);

			offset += sizeof(double);

			return result;
		}

		public string readBytes<string>()
		{
			string value = "";

			ushort size = readBytes<ushort>();

			for (int i = 0; i < size; i++)
			{
				value += readBytes<char>();
			}

			return value;
		}

		public bool writeBytes<T>(T value)
		{
			for (uint i = 0; i < (uint)Marshal.SizeOf<T>(); i++)
			{
				start[offset++] = (value >> ((sizeof(T) - 1) * 8 - i * 8)) & 0xFF;
			}

			return true;
		}

		public bool writeBytes(string str)
		{
			ushort size = (ushort)str.Length;

			//assert(size <= 65535);

			writeBytes<ushort>(size);

			for (ushort i = 0; i < size; i++)
			{
				writeBytes<char>(str[i]);
			}

			return true;
		}

		public unsafe bool writeBytes(float data)
		{
			uint x = 0;

			*(&x) = *(uint*)&data;

			writeBytes<uint>(x);

			return true;
		}

		public unsafe bool writeBytes(double data)
		{
			UInt64 x;

			*(&x) = *(UInt64*)&data;

			writeBytes<UInt64>(x);

			return true;
		}

		public void shrink()
		{
			byte[] temp = new byte[offset];

			System.Array.Copy(start, temp, offset);

			start = null;
			GC.Collect();
			GC.WaitForPendingFinalizers();

			start = temp;
		}

		#region Properties
		public uint FreeSpace
		{
			get
			{
				return (uint)start.Length - offset;
			}
		}

		public uint Offset
		{
			get
			{
				return offset;
			}

			set
			{
				if(value < start.Length) offset = value;
			}
		}

		public uint Size
		{
			get
			{
				return (uint)start.Length;
			}
		}

		public byte[] Start
		{
			get
			{
				return start;
			}
		}
		#endregion

		public byte getByte() { return start[offset++]; }
		public byte getByte(uint offs) { return start[offs]; }

		public bool hasSpace(uint amount) { return (offset + amount) <= start.Length; }

		public void addOffset(uint offs) { offset += offs; }

		public bool writeFile(string filepath)
		{
			FileStream fs = new FileStream(filepath, FileMode.OpenOrCreate);

			fs.Write(start, 0, (int)offset);

			fs.Close();
			fs.Dispose();

			return true;
		}

		bool readFile(string filepath)
		{
			if(start != null)
			{
				start = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			FileStream fs = new FileStream(filepath, FileMode.Open);

			start = new byte[fs.Length];

			fs.Read(start, 0, start.Length);

			fs.Close();
			fs.Dispose();

			offset = 0;

			return true;
		}
	};

}