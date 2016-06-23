#pragma once

#include <string>
#include <vector>

#include <assert.h>

#include "..\Cereal.h"
#include "Database.h"
#include "Writer.h"
#include "Reader.h"

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

		void read(byte* dest, int pointer)
		{
			unsigned short magic = Reader::readBytes<unsigned short>(dest, pointer);

			assert(magic == MAGIC_NUMBER);

			pointer += sizeof(short);

			byte count = Reader::readBytes<byte>(dest, pointer++);

			std::vector<unsigned int> offsets;

			for (byte i = 0; i < count; i++)
			{
				offsets.push_back(Reader::readBytes<unsigned int>(dest, pointer));

				pointer += sizeof(unsigned int);
			}

			for (unsigned int offs : offsets)
			{
				Database* db = new Database;

				db->read(dest, offs);
				this->addDatabase(db);

				//pointer += db->getSize();
			}
		}

		unsigned int write(byte* dest, int pointer) const
		{
			pointer = Writer::writeBytes<unsigned short>(dest, pointer, MAGIC_NUMBER);
			pointer = Writer::writeBytes<byte>(dest, pointer, databases.size());

			unsigned int offset = sizeof(short) + sizeof(byte) + (sizeof(unsigned int) * databases.size());

			for (unsigned int i = 0; i < databases.size(); i++)
			{
				pointer = Writer::writeBytes<unsigned int>(dest, pointer, offset);

				offset += databases[i]->getSize();
			}

			for (Database* db : databases)
				pointer = db->write(dest, pointer);

			return pointer;
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