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
using static Cereal.Global;

namespace Cereal
{
	public class Object
	{
		private string name;
		private List<Array> arrays = new List<Array>();
		private List<Field> fields = new List<Field>();

		public Object(string objName) { name = objName; }
		public Object() { }

		~Object()
		{
			for (int i = 0; i < arrays.Count; i++)
			{
				arrays[i] = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}

			for (int i = 0; i < fields.Count; i++)
			{
				fields[i] = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}
		}

		public bool write(ref Buffer buffer)
		{
			if (!buffer.hasSpace(Size)) return false;

			Debug.Assert(fields.Count < 65536);
			Debug.Assert(arrays.Count < 65536);

			buffer.writeBytes<byte>((byte)DataType.DATA_OBJECT);
			buffer.writeBytes(name);

			buffer.writeBytes<ushort>((ushort)fields.Count);

			foreach (Field field in fields)
				field.write(ref buffer);

			buffer.writeBytes<ushort>((ushort)arrays.Count);

			foreach (Array array in arrays)
				array.write(ref buffer);

			return true;
		}

		public void addField(Field field) { fields.Add(field); }
		public void addArray(Array array) { arrays.Add(array); }

		public Field getField(string name)
		{
			foreach (Field field in fields)
				if (field.Name == name) return field;

			return null;
		}

		public Array getArray(string name)
		{
			foreach(Array array in arrays)
				if (array.Name == name) return array;

			return null;
		}

		public void read(ref Buffer buffer)
		{
			byte type = buffer.readBytesByte();

			Debug.Assert(type == (byte)Global.DataType.DATA_OBJECT);

			name = buffer.readBytesString();

			ushort fieldCount = (ushort)buffer.readBytesShort();

			for (int i = 0; i < fieldCount; i++)
			{
				Field field = new Field();

				field.read(ref buffer);
				addField(field);
			}

			ushort arrayCount = (ushort)buffer.readBytesShort();

			for (int i = 0; i < arrayCount; i++)
			{
				Array array = new Array();

				array.read(ref buffer);
				addArray(array);
			}
		}

		#region Properties
		public string Name
		{
			get { return name; }
			set { if (string.IsNullOrEmpty(value) == false) name = value; }
		}

		public uint Size
		{
			get
			{
				uint ret = sizeof(byte) + sizeof(short) + (uint)name.Length + sizeof(short) + sizeof(short);

				foreach (Field field in fields)
					ret += field.Size;

				foreach (Array array in arrays)
					ret += array.Size;

				return ret;
			}
		}

		public List<Field> Fields
		{
			get { return fields; }
			set { fields = value; }
		}

		public List<Array> Arrays
		{
			get { return arrays; }
			set { arrays = value; }
		}
		#endregion
	};
}