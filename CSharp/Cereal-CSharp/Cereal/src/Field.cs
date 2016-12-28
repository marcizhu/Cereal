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

			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			//Setting the data
			data = new byte[Marshal.SizeOf<T>()];
			Writer.writeBytes<T>(data, 0, value);
		}

		private void setData(DataType type, string value, string fName)
		{
			dataType = type;

			//Setting the data
			if (data != null)
			{
				data = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			data = new byte[value.Length + 2];

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
		public Field(string name, bool value) { setData<bool>(DataType.DATA_BOOL, value, name); }
		public Field(string name, char value) { setData<char>(DataType.DATA_CHAR, value, name); }
		public Field(string name, short value) { setData<short>(DataType.DATA_SHORT, value, name); }
		public Field(string name, int value) { setData<int>(DataType.DATA_INT, value, name); }
		public Field(string name, float value) { setData<float>(DataType.DATA_FLOAT, value, name); }
		public Field(string name, UInt64 value) { setData<UInt64>(DataType.DATA_LONG_LONG, value, name); }
		public Field(string name, double value) { setData<double>(DataType.DATA_DOUBLE, value, name); }
		public Field(string name, string value) { setData<string>(DataType.DATA_STRING, value, name); }

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
			buffer.writeBytes<string>(name);
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
				short len = Reader.readBytes<short>(data, 0);
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
			byte type = buffer.readBytes<byte>();

			//assert(type == DataType::DATA_FIELD);

			string sname = buffer.readBytes<string>();

			DataType dataType = (DataType)buffer.readBytes<byte>();

			switch (dataType)
			{
				case DataType.DATA_BOOL: setData<bool>(dataType, buffer.readBytes<bool>(), sname); break;
				case DataType.DATA_CHAR: setData<byte>(dataType, buffer.readBytes<byte>(), sname); break;
				case DataType.DATA_SHORT: setData<short>(dataType, buffer.readBytes<short>(), sname); break;
				case DataType.DATA_INT: setData<int>(dataType, buffer.readBytes<int>(), sname); break;
				case DataType.DATA_LONG_LONG: setData<UInt64> (dataType, buffer.readBytes<UInt64>(), sname); break;
				case DataType.DATA_FLOAT: setData<float>(dataType, buffer.readBytes<float>(), sname); break;
				case DataType.DATA_DOUBLE: setData<double>(dataType, buffer.readBytes<double>(), sname); break;
				case DataType.DATA_STRING: setData<string> (dataType, buffer.readBytes<string>(), sname); break;
				default: throw new ArgumentException();
			}
		}

		#region Properties
		public string Name
		{
			get
			{
				return name;
			}
		}

		public uint Size
		{
			get
			{
				if (dataType == DataType.DATA_STRING)
				{
					return (uint)(sizeof(byte) + sizeof(short) + name.Length + sizeof(byte) + sizeof(short) + Reader.readBytes<ushort>(data, 0));
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

		public T getValue<T>() { return Reader.readBytes<T>(data, 0); }
	};

}
