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
				Database* db = new Database;

				db->read(buffer);
				this->addDatabase(db);
			}
		}

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace(this->getSize())) return false;

			buffer.writeBytes<unsigned short>(MAGIC_NUMBER);
			buffer.writeBytes<byte>((byte)databases.size());

			unsigned int offset = sizeof(short) + sizeof(byte) + (sizeof(unsigned int) * databases.size());

			for (unsigned int i = 0; i < databases.size(); i++)
			{
				buffer.writeBytes<unsigned int>(offset);

				offset += databases[i]->getSize();
			}

			for (Database* db : databases)
				db->write(buffer);

			return true;
		}

		inline unsigned int getSize() const
		{
			unsigned int ret = sizeof(short) + sizeof(short);

			for (const Database* db : databases)
				ret += db->getSize();

			return ret;
		}

		Database* findDatabase(std::string name) const
		{
			for (Database* db : databases)
				if (db->getName() == name) return db;

			return nullptr;
		}

		void addDatabase(Database* db) { databases.push_back(db); }

	};

}