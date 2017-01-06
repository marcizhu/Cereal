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

namespace Cereal
{
	public class Header
	{
		List<Database> databases = new List<Database>();

		public Header() { }

		~Header()
		{
			for (int i = 0; i < databases.Count; i++)
			{
				databases[i] = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}
		}

		public void read(ref Buffer buffer)
		{
			ushort magic = (ushort)buffer.readBytesShort();

			Debug.Assert(magic == Global.MAGIC_NUMBER);

			byte count = buffer.readBytesByte();

			List<uint> offsets = new List<uint>();

			for (byte i = 0; i < count; i++)
			{
				offsets.Add((uint)buffer.readBytesInt32());
			}

			foreach (uint offs in offsets)
			{
				Debug.Assert(buffer.Position == offs);

				buffer.Position = offs;

				Database db = new Database();

				db.read(ref buffer);
				addDatabase(db);
			}
		}

		public bool write(ref Buffer buffer)
		{
			if (!buffer.hasSpace(Size)) return false;

			Debug.Assert(databases.Count < 256);

			buffer.writeBytes<ushort>(Global.MAGIC_NUMBER);
			buffer.writeBytes<byte>((byte)databases.Count);

			uint offset = (uint)(sizeof(short) + sizeof(byte) + (sizeof(uint) * databases.Count));

			for (int i = 0; i < databases.Count; i++)
			{
				buffer.writeBytes<uint>(offset);

				offset += (uint)databases[i].Size;
			}

			foreach (Database db in databases)
				db.write(ref buffer);

			return true;
		}

		public void addDatabase(Database db) { databases.Add(db); }

		public Database getDatabase(string name)
		{
			foreach (Database db in databases)
				if (db.Name == name) return db;

			return null;
		}

		#region Properties
		public uint Size
		{
			get
			{
				uint ret = (uint)(sizeof(short) + sizeof(byte) + (sizeof(uint) * databases.Count));

				foreach (Database db in databases)
					ret += (uint)db.Size;

				return ret;
			}
		}

		public List<Database> Databases
		{
			get { return databases; }
		}
		#endregion
	};
}