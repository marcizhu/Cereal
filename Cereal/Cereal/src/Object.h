#pragma once

#include <vector>
#include <string>

#include "Writer.h"
#include "Reader.h"
#include "..\Cereal.h"

#include "Field.h"
#include "Array.h"

namespace Cereal {

	class Object
	{
	private:
		byte type;
		std::string name;
		std::vector<const Array*> arrays;
		std::vector<const Field*> fields;

	public:
		//constructor for each field type
		inline Object() {};
		inline Object(std::string name) {
			this->name = name;
			this->type = DataType::DATA_OBJECT;
		}

		~Object() { }

		unsigned int write(byte* dest, int pointer) const
		{
			pointer = Writer::writeBytes<byte>(dest, pointer, type);
			pointer = Writer::writeBytes<std::string>(dest, pointer, name);

			for (const Field* field : fields)
				pointer = field->write(dest, pointer);

			for (const Array* array : arrays)
				pointer = array->write(dest, pointer);

			return pointer;
		}

		inline void addField(const Field* field) { fields.push_back(field); }
		inline void addArray(const Array* array) { arrays.push_back(array); }

		inline const Field* findField(std::string name) const
		{
			for (const Field* field : fields)
				if (field->getName() == name) return field;

			return nullptr;
		}

		inline const Array* findArray(std::string name) const
		{
			for (const Array* array : arrays)
				if (array->getName() == name) return array;

			return nullptr;
		}

		void read(byte* dest, int pointer)
		{
			this->type = Reader::readBytes<byte>(dest, pointer++);

			assert(this->type == DataType::DATA_OBJECT);

			this->name = Reader::readBytes<std::string>(dest, pointer);

			pointer += 2 + name.length();

			//Reading fields
			while (Reader::readBytes<byte>(dest, pointer) == DataType::DATA_FIELD)
			{
				Field* field = new Field;

				field->read(dest, pointer);
				this->addField(field);

				pointer += 1 + 2 + field->getName().length() + 1 + sizeOf(field->getType());
			}

			while (Reader::readBytes<byte>(dest, pointer) == DataType::DATA_ARRAY)
			{
				Array* array = new Array;

				array->read(dest, pointer);
				this->addArray(array);

				pointer += 1 + 2 + array->getName().length() + 1 + 2 + sizeOf(array->getType()) * array->getCount();
			}

		}

		const std::string& getName() const { return name; }
		byte getContainerType() const { return type; }

	};

}