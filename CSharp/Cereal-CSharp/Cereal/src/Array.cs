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
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using static Cereal.Global;

namespace Cereal
{
	public class Array
	{
		private string name;
		private uint count; // item count
		private DataType dataType;
		private uint size = 0;
		private byte[] data;

		private void setData<T>(DataType type, T[] value, string arrayName)
		{
			if (type == DataType.DATA_UNKNOWN) return;

			name = arrayName;
			count = (uint)value.Length;
			dataType = type;

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			data = new byte[Global.sizeOf(type) * count];

			Debug.Assert((count * Marshal.SizeOf(typeof(T))) < 4294967296); // Maximum item count (overflow of pointer and buffer)

			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				pointer = Writer.writeBytes<T>(data, pointer, value[i]);
			}
		}

		private void setData(DataType type, bool[] value, string arrayName)
		{
			if (type == DataType.DATA_UNKNOWN) return;

			name = arrayName;
			count = (uint)value.Length;
			dataType = type;

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			data = new byte[count];

			Debug.Assert(count < 4294967295); // Maximum item count (overflow of pointer and buffer)

			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				pointer = Writer.writeBytes(data, pointer, value[i]);
			}
		}

		void setData(DataType type, string[] value, string name)
		{
			count = (uint)value.Length;
			dataType = type;

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			size = 0;

			for (uint i = 0; i < count; i++)
			{
				size += 2;
				size += (uint)value[i].Length;
			}

			data = new byte[size];

			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				pointer = Writer.writeBytes(data, pointer, value[i]);
			}
		}

		// public
		public Array() { setData<byte>(DataType.DATA_UNKNOWN, null, ""); }
		public Array(string name, byte[] value) { setData<byte>(DataType.DATA_CHAR, value, name); }
		public Array(string name, bool[] value) { setData(DataType.DATA_BOOL, value, name); }
		public Array(string name, char[] value) { setData<char>(DataType.DATA_CHAR, value, name); }
		public Array(string name, short[] value) { setData<short>(DataType.DATA_SHORT, value, name); }
		public Array(string name, int[] value) { setData<int>(DataType.DATA_INT, value, name); }
		public Array(string name, float[] value) { setData<float>(DataType.DATA_FLOAT, value, name); }
		public Array(string name, UInt64[] value) { setData<UInt64>(DataType.DATA_LONG_LONG, value, name); }
		public Array(string name, double[] value) { setData<double>(DataType.DATA_DOUBLE, value, name); }
		public Array(string name, string[] value) { setData(DataType.DATA_STRING, value, name); }

		~Array()
		{
			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}
		}

		public bool write(ref Buffer buffer)
		{
			if (!buffer.hasSpace(Size)) return false;

			buffer.writeBytes<byte>((byte)DataType.DATA_ARRAY);
			buffer.writeBytes(name);
			buffer.writeBytes<byte>((byte)dataType);
			buffer.writeBytes<uint>(count);

			uint s;

			if (dataType != DataType.DATA_STRING)
				s = sizeOf(dataType) * count;
			else
				s = size;

			for (uint i = 0; i < s; i++)
				buffer.writeBytes<byte>(data[i]);

			return true;
		}

		public void read(ref Buffer buffer)
		{
			DataType type = (DataType)buffer.readBytesByte();

			Debug.Assert(type == DataType.DATA_ARRAY);

			name = buffer.readBytesString();

			dataType = (DataType)buffer.readBytesByte();
			count = (uint)buffer.readBytesInt32();

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			if (dataType != DataType.DATA_STRING)
			{
				data = new byte[count * sizeOf(dataType)];

				System.Array.Copy(buffer.Data, buffer.Position, data, 0, count * sizeOf(dataType));

				buffer.addOffset(count * sizeOf(dataType));
			}
			else
			{
				uint start = buffer.Position;

				for (uint i = 0; i < count; i++)
				{
					buffer.readBytesString();
				}

				size = buffer.Position - start;

				data = new byte[size];

				System.Array.Copy(buffer.Data, start, data, 0, size);
			}
		}

		public List<T> getArray<T>()
		{
			/*List<T> ret = new List<T>();

			uint pointer = 0;

			for (int i = 0; i < count; i++)
			{
				ret.Add(Reader.readBytes<T>(data, pointer));

				pointer += (uint)Marshal.SizeOf(typeof(T));
			}

			return ret;*/
			throw new NotImplementedException();
		}

		public List<string> getArray()
		{
			List<string> ret = new List<string>();

			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				ret.Add(Reader.readBytesString(data, pointer));

				pointer += (ushort)Reader.readBytesShort(data, pointer) + (uint)sizeof(ushort);
			}

			return ret;
		}

		// This returns the data in little endian (necessary for >1 byte data types like shorts or ints)
		#region getRawArray()
		public bool[] getRawArray(bool[] mem)
		{
			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				mem[i] = Reader.readBytesBool(data, pointer);

				pointer += (uint)sizeof(bool);
			}

			return mem;
		}

		public byte[] getRawArray(byte[] mem)
		{
			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				mem[i] = Reader.readBytesByte(data, pointer);

				pointer += (uint)sizeof(byte);
			}

			return mem;
		}

		public char[] getRawArray(char[] mem)
		{
			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				mem[i] = Reader.readBytesChar(data, pointer);

				pointer += (uint)sizeof(char);
			}

			return mem;
		}

		public short[] getRawArray(short[] mem)
		{
			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				mem[i] = Reader.readBytesShort(data, pointer);

				pointer += (uint)sizeof(short);
			}

			return mem;
		}

		public float[] getRawArray(float[] mem)
		{
			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				mem[i] = Reader.readBytesFloat(data, pointer);

				pointer += (uint)sizeof(float);
			}

			return mem;
		}

		public double[] getRawArray(double[] mem)
		{
			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				mem[i] = Reader.readBytesDouble(data, pointer);

				pointer += (uint)sizeof(double);
			}

			return mem;
		}

		public int[] getRawArray(int[] mem)
		{
			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				mem[i] = Reader.readBytesInt32(data, pointer);

				pointer += (uint)sizeof(int);
			}

			return mem;
		}

		public Int64[] getRawArray(Int64[] mem)
		{
			uint pointer = 0;

			for (uint i = 0; i < count; i++)
			{
				mem[i] = Reader.readBytesInt64(data, pointer);

				pointer += (uint)sizeof(UInt64);
			}

			return mem;
		}

		public string[] getRawArray(string[] mem)
		{
			throw new NotImplementedException();
		}
		#endregion

		#region Properties
		public string Name
		{
			get { return name; }
			set { if (string.IsNullOrEmpty(value) == false) name = value; }
		}

		public uint ItemCount
		{
			get { return count; }
		}

		public uint Size
		{
			get
			{
				if (dataType != DataType.DATA_STRING)
				{
					return sizeof(byte) + sizeof(short) + (uint)Name.Length + sizeof(byte) + sizeof(int) + count * Global.sizeOf(dataType);
				}
				else
				{
					return sizeof(byte) + sizeof(short) + (uint)Name.Length + sizeof(byte) + sizeof(int) + size;
				}
			}
		}

		public DataType DataType
		{
			get { return dataType; }
		}
		#endregion
	}
}