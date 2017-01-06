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

		public Int64 readBytesInt64()
		{
			int hiByte = readBytesInt32();
			int loByte = readBytesInt32();

			Int64 temp = (uint)hiByte;
			temp = temp << (sizeof(int) * 8);
			temp |= (uint)loByte;

			return temp;
		}

		public int readBytesInt32()
		{
			int ret = 0;

			for (int i = 0; i < sizeof(int); i++)
			{
				ret |= start[offset++] << ((sizeof(int) * 8 - 8) - (i * 8));
			}

			return ret;
		}

		public short readBytesShort()
		{
			int ret = 0;

			for (int i = 0; i< sizeof(short); i++)
			{
				ret |= start[offset++] << ((sizeof(short) * 8 - 8) - (short)(i* 8));
			}

			return (short)ret;
		}

		public char readBytesChar() { return (char)start[offset++]; }

		public byte readBytesByte() { return start[offset++]; }

		public float readBytes()
		{
			uint value = (uint)readBytesInt32();

			byte[] result = new byte[sizeof(float)];

			for (int i = 0; i < sizeof(float); i++)
			{
				result[i] = BitConverter.GetBytes(value)[i];
			}

			return BitConverter.ToSingle(result, 0);
		}

		public bool readBytesBool() { return start[offset++] != 0; }

		public float readBytesFloat()
		{
			uint value = (uint)readBytesInt32();

			byte[] result = new byte[sizeof(float)];

			for (int i = 0; i < sizeof(float); i++)
			{
				result[i] = BitConverter.GetBytes(value)[i];
			}

			return BitConverter.ToSingle(result, 0);
		}

		public double readBytesDouble()
		{
			UInt64 value = (UInt64)readBytesInt64();

			byte[] result = new byte[sizeof(double)];

			for (int i = 0; i < sizeof(double); i++)
			{
				result[i] = BitConverter.GetBytes(value)[i];
			}

			return BitConverter.ToDouble(result, 0);
		}

		public string readBytesString()
		{
			string value = "";

			ushort size = (ushort)readBytesShort();

			for (uint i = 0; i < size; i++)
			{
				value += readBytesChar();
			}

			return value;
		}

		public bool writeBytes<T>(T value)
		{
			if(typeof(T) == typeof(string))
			{
				throw new Exception("Invalid call to Buffer::writeBytes<T>(...)"); // debug exception. It will be removed once everything is tested properly
			}

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
				start[offset++] = src[size - 1 - i];
			}

			return true;
		}

		public bool writeBytes(string value)
		{
			ushort size = (ushort)value.Length;

			Debug.Assert(size <= 65535);

			writeBytes<ushort>(size);

			for (ushort i = 0; i < size; i++)
			{
				writeBytes<char>(value[i]);
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

		public bool readFile(string filepath)
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

		#region Properties
		public uint FreeSpace
		{
			get { return (uint)start.Length - offset; }
		}

		public uint Position
		{
			get { return offset; }
			set { if (value < start.Length) offset = value; }
		}

		public uint Length
		{
			get { return (uint)start.Length; }
		}

		public byte[] Data
		{
			get { return start; }
		}
		#endregion
	};
}