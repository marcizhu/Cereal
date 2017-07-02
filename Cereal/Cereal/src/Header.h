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

#pragma once

#include <string>
#include <vector>
#include <assert.h>

#include "Internal.h"
#include "Database.h"
#include "Buffer.h"

#define MAGIC_NUMBER	0x524d

namespace Cereal {

	class Header
	{
	private:
		std::vector<Database*> databases;

	public:
		Header() { }

		~Header()
		{
			for (size_t i = 0; i < databases.size(); i++)
				delete databases[i];
		}

		void read(Buffer& buffer)
		{
			unsigned short magic = buffer.readBytes<unsigned short>();

			assert(magic == MAGIC_NUMBER);

			byte count = buffer.readBytes<byte>();

			std::vector<unsigned int> offsets;

			for (byte i = 0; i < count; i++)
			{
				offsets.push_back(buffer.readBytes<unsigned int>());
			}

			for (unsigned int offs : offsets)
			{
				assert(buffer.getOffset() == offs); // database offset mismatch

				buffer.setOffset(offs);

				Database* db = new Database;

				db->read(buffer);
				this->addDatabase(db);
			}
		}

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace(this->getSize())) return false;

			if(databases.size() > 256) throw std::overflow_error("Too many databases!");

			buffer.writeBytes<unsigned short>(MAGIC_NUMBER);
			buffer.writeBytes<byte>((byte)databases.size());

			unsigned int offset = sizeof(unsigned short) + sizeof(byte) + (sizeof(unsigned int) * (unsigned int)databases.size());

			for (unsigned int i = 0; i < databases.size(); i++)
			{
				buffer.writeBytes<unsigned int>(offset);

				offset += (unsigned int)databases[i]->getSize();
			}

			for (Database* db : databases)
				db->write(buffer);

			return true;
		}

		inline unsigned int getSize() const
		{
			unsigned int ret = sizeof(short) + sizeof(byte) + (sizeof(unsigned int) * (unsigned int)databases.size());

			for (const Database* db : databases)
				ret += (unsigned int)db->getSize();

			return ret;
		}

		Database* getDatabase(std::string name) const
		{
			for (Database* db : databases)
				if (db->getName() == name) return db;

			return nullptr;
		}

		void addDatabase(Database* db) { databases.push_back(db); }

		const std::vector<Database*>& getDatabases() const { return databases; }
	};

}