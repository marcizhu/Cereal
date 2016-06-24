#pragma once

#include <string>
#include <vector>

#include <assert.h>

#include "..\Cereal.h"
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
			if (!buffer.hasSpace(this->getSize())) return false;

			buffer.writeBytes<unsigned short>(version);

			switch (version)
			{
			case Version::VERSION_1_0:
				buffer.writeBytes<std::string>(name);
				buffer.writeBytes<unsigned int>(this->getSize()); // I'm sure we will need databases > 64 kb
				buffer.writeBytes<unsigned short>(objects.size());

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
			unsigned int ret = sizeof(short) + sizeof(short) + name.length() + sizeof(unsigned int) + sizeof(unsigned short);

			for (const Object* obj : objects)
				ret += obj->getSize();

			return ret;
		}

		Object* findObject(std::string name) const
		{
			for (Object* obj : objects)
				if (obj->getName() == name) return obj;

			return nullptr;
		}

		void addObject(Object* object) { objects.push_back(object); }

		const std::string& getName() const { return name; }
	};

}