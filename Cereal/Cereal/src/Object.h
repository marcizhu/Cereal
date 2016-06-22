#pragma once

#include <vector>

#include "Writer.h"
#include "Reader.h"
#include "Container.h"
#include "..\Cereal.h"

#include "Field.h"
#include "Array.h"

namespace Cereal {

	class Object : public Container
	{
	private:
		std::vector<const Array*> arrays;
		std::vector<const Field*> fields;

	public:
		//constructor for each field type
		inline Object(std::string name) {
			this->name = name;
			this->type = DataType::DATA_OBJECT;
		}

		~Object() { }

		inline void write(byte* dest, int pointer)
		{
			pointer = this->writeContainer(dest, pointer);

			for (const Field* field : fields)
				pointer = field->write(dest, pointer);

			for (const Array* array : arrays)
				pointer = array->write(dest, pointer);
		}

		inline void addField(const Field* field) { fields.push_back(field); }
		inline void addArray(const Array* array) { arrays.push_back(array); }

		inline const Field* findField(std::string name)
		{
			for (const Field* field : fields)
				if (field->name == name) return field;

			assert(false);
			return nullptr;
		}

		inline const Array* findArray(std::string name)
		{
			for (const Array* array : arrays)
				if (array->name == name) return array;

			assert(false);
			return nullptr;
		}

	};

}