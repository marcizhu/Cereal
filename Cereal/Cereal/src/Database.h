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

			assert(version != Version::VERSION_INVALID && version <= Version::VERSION_LATEST);

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

			default:
				__debugbreak(); break;
			}
		}

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace((unsigned int)this->getSize())) return false;

			buffer.writeBytes<unsigned short>(version);

			assert(version != Version::VERSION_INVALID);

			switch (version)
			{
			case Version::VERSION_1_0:
				assert(objects.size() < 65536);
				assert(this->getSize() < 4294967296); // 2^32, maximum database size

				buffer.writeBytes<std::string>(name);
				buffer.writeBytes<unsigned int>((unsigned int)this->getSize());
				buffer.writeBytes<unsigned short>((unsigned short)objects.size());

				for (const Object* obj : objects)
					obj->write(buffer);

				break;

			default:
				__debugbreak(); break;
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

			default:
				__debugbreak(); break; // Invalid version
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