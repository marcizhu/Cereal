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

#include "Internal.h"
#include "Object.h"
#include "Buffer.h"

namespace Cereal {

	class Database
	{
	private:
		Version version;
		std::string name;
		std::vector<Object*> objects;

	public:
		unsigned int crc32(const byte* message, unsigned int len) const
		{
			unsigned int byte, mask;
			signed int crc;

			crc = 0xFFFFFFFF;

			for (int i = 0; i < len; i++)
			{
				byte = message[i]; // Get next byte.
				crc = crc ^ byte;

				for (int j = 7; j >= 0; j--)
				{
					mask = -(crc & 1);
					crc = (crc >> 1) ^ (0xEDB88320 & mask);
				}
			}

			return ~crc;
		}

	public:
		Database() : name(""), version(Version::VERSION_INVALID) { }
		Database(std::string name, Version ver) : name(name), version(ver) { }
		Database(std::string name) : name(name), version(Version::VERSION_LATEST) { }

		~Database()
		{
			for (size_t i = 0; i < objects.size(); i++)
				delete objects[i];
		}

		void read(Buffer& buffer)
		{
			this->version = (Version)buffer.readBytes<unsigned short>();

#ifndef CEREAL_RELEASE
			if (version == Version::VERSION_INVALID) throw new std::invalid_argument("Invalid database version!");
			if (version > Version::VERSION_LATEST) throw new std::invalid_argument("Unsupported version!");
#endif

			switch (version)
			{
			case Version::VERSION_1_0:
			{
				this->name = buffer.readBytes<std::string>();

				buffer.addOffset(sizeof(unsigned int)); //we skip the size (don't need it)

				unsigned short objectCount = buffer.readBytes<unsigned short>();

				for (unsigned short i = 0; i < objectCount; i++)
				{
					Object* obj = new Object;

					obj->read(buffer);
					this->addObject(obj);
				}

				break;
			}

			case Version::VERSION_2_0:
			{
				this->name = buffer.readBytes<std::string>();

				unsigned int checksum = buffer.readBytes<unsigned int>();

				unsigned int p = buffer.getOffset();
				unsigned int size = buffer.readBytes<unsigned int>() - sizeof(short) - sizeof(short) - (unsigned int)name.length() - sizeof(unsigned int);

				if(crc32((byte*)buffer.getStart() + p, size) != checksum) throw new std::invalid_argument("Checksum mismatch!");

				unsigned short objectCount = buffer.readBytes<unsigned short>();

				for (unsigned short i = 0; i < objectCount; i++)
				{
					Object* obj = new Object;

					obj->read(buffer);
					this->addObject(obj);
				}

				break;
			}

			default:
				throw new std::invalid_argument("The version is not valid!"); break;
			}
		}

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace((unsigned int)this->getSize())) return false;

			buffer.writeBytes<unsigned short>(version);

#ifndef CEREAL_RELEASE
			if(version == Version::VERSION_INVALID) throw new std::invalid_argument("Invalid database version!");
#endif

			switch (version)
			{
			case Version::VERSION_1_0:
#ifndef CEREAL_RELEASE
				if(objects.size() > 65536) throw new std::out_of_range("Too many objects!");
				if(this->getSize() > 4294967296) throw new std::overflow_error("Database size is too big!"); // 2^32, maximum database size
#endif

				buffer.writeBytes<std::string>(name);
				buffer.writeBytes<unsigned int>((unsigned int)this->getSize());
				buffer.writeBytes<unsigned short>((unsigned short)objects.size());

				for (const Object* obj : objects)
					obj->write(buffer);

				break;

			case Version::VERSION_2_0:
			{
#ifndef CEREAL_RELEASE
				if(objects.size() > 65536) throw new std::out_of_range("Too many objects!");
				if(this->getSize() > 4294967296) throw new std::overflow_error("Database size is too big!"); // 2^32, maximum database size
#endif

				buffer.writeBytes<std::string>(name);

				unsigned int chkoffs = buffer.getOffset();
				unsigned int size = (unsigned int)this->getSize();

				buffer.addOffset(sizeof(unsigned int)); // checksum
				buffer.writeBytes<unsigned int>(size);
				buffer.writeBytes<unsigned short>((unsigned short)objects.size());

				for (const Object* obj : objects)
					obj->write(buffer);

				unsigned int temp = buffer.getOffset(); // end

				buffer.setOffset(chkoffs);

				// first short IS THE VERSION
				size = size - sizeof(short) - sizeof(short) - (unsigned int)name.length() - sizeof(unsigned int);

				unsigned int checksum = crc32((byte*)buffer.getStart() + chkoffs + sizeof(unsigned int), size);

				buffer.writeBytes<unsigned int>(checksum);
				buffer.setOffset(temp);

				break;
			}

			default:
				throw new std::invalid_argument("The version is not valid!"); break;
			}

			return true;
		}

		inline unsigned int getSize() const
		{
			unsigned int ret = sizeof(short);

			switch (version)
			{
			case Version::VERSION_1_0:
				ret += sizeof(short) + name.length() + sizeof(int) + sizeof(short); break;

			case Version::VERSION_2_0:
				ret += sizeof(short) + name.length() + sizeof(unsigned int) + sizeof(int) + sizeof(short); break;

			default:
				throw new std::invalid_argument("The version is not valid!"); break; // Invalid version
			}

			for (const Object* obj : objects)
				ret += obj->getSize();

			return ret;
		}

		Object* getObject(std::string name) const
		{
			for (Object* obj : objects)
				if (obj->getName() == name) return obj;

			return nullptr;
		}

		void addObject(Object* object) { objects.push_back(object); }

		const std::string& getName() const { return name; }
		const std::vector<Object*>& getObjects() { return objects; }
	};

}