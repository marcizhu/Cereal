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
	public class Database
	{
		private Global.Version version;
		private string name;
		private List<Object> objects = new List<Object>();

		public Database()
		{
			name = "";
			version = Global.Version.VERSION_INVALID;
		}
		public Database(string dbName, Global.Version ver)
		{
			name = dbName;
			version = ver;
		}
		public Database(string dbName)
		{
			name = dbName;
			version = Global.Version.VERSION_LATEST;
		}

		~Database()
		{
			for (int i = 0; i < objects.Count; i++)
			{
				objects[i] = null;
				GC.Collect();
				GC.WaitForPendingFinalizers();
			}
		}

		public void read(ref Buffer buffer)
		{
			version = (Global.Version)buffer.readBytesShort();

			Debug.Assert(version != Global.Version.VERSION_INVALID && version <= Global.Version.VERSION_LATEST);

			switch (version)
			{
				case Global.Version.VERSION_1_0:
					{
						name = buffer.readBytesString();

						buffer.addOffset(sizeof(uint)); //we skip the size (don't need it)

						ushort objectCount = (ushort)buffer.readBytesShort();

						for (ushort i = 0; i < objectCount; i++)
						{
							Object obj = new Object();

							obj.read(ref buffer);
							this.addObject(obj);
						}

						break;
					}

				default:
					throw new ArgumentOutOfRangeException("version", "Invalid database version!");
			}
		}

		public bool write(ref Buffer buffer)
		{
			if (!buffer.hasSpace((uint)Size)) return false;

			buffer.writeBytes<ushort>((ushort)version);

			Debug.Assert(version != Global.Version.VERSION_INVALID);

			switch (version)
			{
			case Global.Version.VERSION_1_0:
				Debug.Assert(objects.Count < 65536);
				Debug.Assert(Size <= 4294967295); // 2^32, maximum database size

				buffer.writeBytes(name);
				buffer.writeBytes<uint>((uint)Size);
				buffer.writeBytes<ushort>((ushort)objects.Count);

				foreach (Object obj in objects)
					obj.write(ref buffer);

				break;

			default:
				throw new ArgumentOutOfRangeException("version", "Invalid database version!");
			}

			return true;
		}

		public Object getObject(string name)
		{
			foreach(Object obj in objects)
				if (obj.Name == name) return obj;

			return null;
		}

		public void addObject(Object obj) { objects.Add(obj); }

		#region Properties
		public List<Object> Objects
		{
			get { return objects; }
		}

		public Global.Version Version
		{
			get { return version; }
			set
			{
				if (value <= Global.Version.VERSION_LATEST)
					version = value;
				else
					throw new ArgumentOutOfRangeException("version", "Invalid database version!");
			}
		}

		public string Name
		{
			get { return name; }
			set { if (string.IsNullOrEmpty(value) == false) name = value; }
		}

		public uint Size
		{
			get
			{
				uint ret = sizeof(short);

				switch (version)
				{
					case Global.Version.VERSION_1_0:
						ret += sizeof(short) + (uint)name.Length + sizeof(int) + sizeof(short); break;

					default:
						throw new ArgumentOutOfRangeException("version", "Cannot calculate the database size with an unknown database version!"); // Invalid version
				}

				foreach (Object obj in objects)
					ret += obj.Size;

				return ret;
			}
		}
		#endregion
	};
}