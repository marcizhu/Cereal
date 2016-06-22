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
		
		std::vector<Array> arrays;
		unsigned short arrayCount = 0;
		std::vector<Field> fields;
		unsigned short fieldCount = 0;

	public:
		//constructor for each field type
		inline Object(std::string name) {
			this->name = name;
			this->type = DataType::DATA_OBJECT;
		}

		~Object() { }

		inline void write(byte* dest, int pointer) {
			pointer = this->writeContainer(dest, pointer);
			for (unsigned short i = 0; i < fieldCount; i++) {
				pointer = fields[i].write(dest, pointer);
			}

			for (unsigned short i = 0; i < arrayCount; i++) {
				pointer = arrays[i].write(dest, pointer);
			}
		}

		inline void addField(const Field& field) {
			fields[fieldCount++] = field;
		}
		
		inline void addArray(const Array& array) {
			arrays[arrayCount++] = array;
		}

		inline Field findField(std::string name) {
			for (unsigned short i = 0; i < fieldCount; i++) {
				if (fields[i].name == name) {
					return fields[i];
				}
			}
			assert(false);
			return Field("", nullptr);
		}

		inline Array findArray(std::string name) {
			for (unsigned short i = 0; i < arrayCount; i++) {
				if (arrays[i].name == name) {
					return arrays[i];
				}
			}
			assert(false);
			return Array("", (int*) nullptr, 1);
		}

	};

}
