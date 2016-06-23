#pragma once

#include <string>
#include <vector>

#include <assert.h>

#include "..\Cereal.h"
#include "Object.h"
#include "Writer.h"
#include "Reader.h"

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

		void read(byte* dest, int pointer)
		{
			this->version = (Version)Reader::readBytes<unsigned short>(dest, pointer);

			pointer += sizeof(short);

			assert(version != Version::VERSION_INVALID && version <= Version::VERSION_LATEST);

			switch (version)
			{
			case Version::VERSION_1_0:
			{
				this->name = Reader::readBytes<std::string>(dest, pointer);

				pointer += sizeof(short) + name.length() + sizeof(unsigned int); //we skip the size (don't need it)

				unsigned short objectCount = Reader::readBytes<unsigned short>(dest, pointer);

				pointer += sizeof(short);

				for (unsigned short i = 0; i < objectCount; i++)
				{
					Object* obj = new Object;

					obj->read(dest, pointer);
					this->addObject(obj);

					pointer += obj->getSize();
				}

				break;
			}

			default:
				__debugbreak(); break;
			}
		}

		unsigned int write(byte* dest, int pointer) const
		{
			pointer = Writer::writeBytes<unsigned short>(dest, pointer, version);

			switch (version)
			{
			case Version::VERSION_1_0:
				pointer = Writer::writeBytes<std::string>(dest, pointer, name);
				pointer = Writer::writeBytes<unsigned int>(dest, pointer, this->getSize()); // I'm sure we will need databases > 64 kb
				pointer = Writer::writeBytes<unsigned short>(dest, pointer, objects.size());

				for (const Object* obj : objects)
					pointer = obj->write(dest, pointer);

				break;

			default:
				__debugbreak(); break;
			}

			return pointer;
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