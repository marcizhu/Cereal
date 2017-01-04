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
using System.Runtime.InteropServices;
using static Cereal.Global;

namespace Cereal
{
	public class Field
	{
		private string name;
		private DataType dataType;
		private byte[] data = null;

		private void setData<T>(DataType type, T value, string fName)
		{
			dataType = type;
			name = fName;

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			//Setting the data
			data = new byte[Marshal.SizeOf(typeof(T))];
			Writer.writeBytes<T>(data, 0, value);
		}

		private void setData(DataType type, bool value, string fName)
		{
			dataType = type;
			name = fName;

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			//Setting the data
			data = new byte[1];
			Writer.writeBytes(data, 0, value);
		}

		private void setData(DataType type, float value, string fName)
		{
			dataType = type;
			name = fName;

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			//Setting the data
			data = new byte[sizeof(float)];
			Writer.writeBytes(data, 0, value);
		}

		private void setData(DataType type, double value, string fName)
		{
			dataType = type;
			name = fName;

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			//Setting the data
			data = new byte[sizeof(double)];
			Writer.writeBytes(data, 0, value);
		}

		private void setData(DataType type, string value, string fName)
		{
			dataType = type;
			name = fName;

			//Setting the data
			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			data = new byte[value.Length + sizeof(short)];

			uint ptr = Writer.writeBytes<ushort>(data, 0, (ushort)value.Length);

			for (int i = 0; i < value.Length; i++)
			{
				ptr = Writer.writeBytes<char>(data, ptr, value[i]);
			}
		}

		//constructor for each field type
		public Field()
		{
			data = null;
			dataType = DataType.DATA_UNKNOWN;
			name = "";
		}

		public Field(string name, byte value) { setData<byte>(DataType.DATA_CHAR /* | MOD_UNSIGNED*/, value, name); }
		public Field(string name, bool value) { setData(DataType.DATA_BOOL, value, name); }
		public Field(string name, char value) { setData<char>(DataType.DATA_CHAR, value, name); }
		public Field(string name, short value) { setData<short>(DataType.DATA_SHORT, value, name); }
		public Field(string name, int value) { setData<int>(DataType.DATA_INT, value, name); }
		public Field(string name, UInt64 value) { setData<UInt64>(DataType.DATA_LONG_LONG, value, name); }
		public Field(string name, float value) { setData(DataType.DATA_FLOAT, value, name); }
		public Field(string name, double value) { setData(DataType.DATA_DOUBLE, value, name); }
		public Field(string name, string value) { setData(DataType.DATA_STRING, value, name); }

		~Field()
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

			buffer.writeBytes<byte>((byte)DataType.DATA_FIELD);
			buffer.writeBytes(name);
			buffer.writeBytes<byte>((byte)dataType); //write data type

			if (dataType != DataType.DATA_STRING)
			{
				for (int i = 0; i<sizeOf(dataType); i++)
				{
					buffer.writeBytes<byte>(data[i]);
				}
}
			else
			{
				short len = Reader.readBytesShort(data, 0);
				len += 2;

				for (int i = 0; i<len; i++)
				{
					buffer.writeBytes<byte>(data[i]);
				}
			}

			return true;
		}

		public void read(ref Buffer buffer)
		{
			byte type = buffer.readBytesByte();

			Debug.Assert(type == (byte)Global.DataType.DATA_FIELD);

			string sname = buffer.readBytesString();

			DataType dataType = (DataType)buffer.readBytesByte();

			switch (dataType)
			{
				case DataType.DATA_BOOL: setData(dataType, buffer.readBytesBool(), sname); break;
				case DataType.DATA_CHAR: setData<byte>(dataType, buffer.readBytesByte(), sname); break;
				case DataType.DATA_SHORT: setData<short>(dataType, buffer.readBytesShort(), sname); break;
				case DataType.DATA_INT: setData<int>(dataType, buffer.readBytesInt32(), sname); break;
				case DataType.DATA_LONG_LONG: setData<UInt64> (dataType, (ulong)buffer.readBytesInt64(), sname); break;
				case DataType.DATA_FLOAT: setData(dataType, buffer.readBytesFloat(), sname); break;
				case DataType.DATA_DOUBLE: setData(dataType, buffer.readBytesDouble(), sname); break;
				case DataType.DATA_STRING: setData(dataType, buffer.readBytesString(), sname); break;
				default: throw new ArgumentOutOfRangeException("dataType", "Invalid data type!");
			}
		}

		#region Properties
		public string Name
		{
			get
			{
				return name;
			}

			set
			{
				if (value.Length > 0) name = value;
			}
		}

		public uint Size
		{
			get
			{
				if (dataType == DataType.DATA_STRING)
				{
					return (uint)(sizeof(byte) + sizeof(short) + name.Length + sizeof(byte) + sizeof(short) + (ushort)Reader.readBytesShort(data, 0));
				}

				return (uint)(sizeof(byte) + sizeof(short) + name.Length + sizeof(byte) + sizeOf(dataType));
			}
		}

		public DataType DataType
		{
			get
			{
				return DataType;
			}
		}
		#endregion

		public byte getByte() { return Reader.readBytesByte(data, 0); }
		public bool getBool() { return Reader.readBytesBool(data, 0); }
		public char getChar() { return Reader.readBytesChar(data, 0); }
		public short getShort() { return Reader.readBytesShort(data, 0); }
		public int getInt32() { return Reader.readBytesInt32(data, 0); }
		public float getFloat() { return Reader.readBytesFloat(data, 0); }
		public Int64 getInt64() { return Reader.readBytesInt64(data, 0); }
		public double getDouble() { return Reader.readBytesDouble(data, 0); }
		public string getString() { return Reader.readBytesString(data, 0); }
	};
}