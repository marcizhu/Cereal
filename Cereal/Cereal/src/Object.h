//  Cereal: A C++ Serialization library
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

#include <vector>
#include <string>

#include "Buffer.h"
#include "Internal.h"

#include "Field.h"
#include "Array.h"

namespace Cereal {

	class Object
	{
	private:
		byte type;
		std::string name;
		std::vector<Array*> arrays;
		std::vector<Field*> fields;

	public:
		Object() {};
		Object(std::string name)
		{
			this->name = name;
			this->type = DataType::DATA_OBJECT;
		}

		~Object()
		{
			for (size_t i = 0; i < arrays.size(); i++)
				delete arrays[i];

			for (size_t i = 0; i < fields.size(); i++)
				delete fields[i];
		}

		bool write(Buffer& buffer) const
		{
			if (!buffer.hasSpace(this->getSize())) return false;

			assert(fields.size() < 65536);
			assert(arrays.size() < 65536);

			buffer.writeBytes<byte>(type);
			buffer.writeBytes<std::string>(name);

			buffer.writeBytes<unsigned short>((unsigned short)fields.size());

			for (const Field* field : fields)
				field->write(buffer);

			buffer.writeBytes<unsigned short>((unsigned short)arrays.size());

			for (const Array* array : arrays)
				array->write(buffer);

			return true;
		}

		inline void add(Field* field) { fields.push_back(field); }
		inline void add(Array* array) { arrays.push_back(array); }

		inline Field* getField(std::string name) const
		{
			for (Field* field : fields)
				if (field->getName() == name) return field;

			return nullptr;
		}

		inline Array* getArray(std::string name) const
		{
			for (Array* array : arrays)
				if (array->getName() == name) return array;

			return nullptr;
		}

		void read(Buffer& buffer)
		{
			this->type = buffer.readBytes<byte>();

			assert(this->type == DataType::DATA_OBJECT);

			this->name = buffer.readBytes<std::string>();

			unsigned short fieldCount = buffer.readBytes<unsigned short>();

			for (int i = 0; i < fieldCount; i++)
			{
				Field* field = new Field;

				field->read(buffer);
				this->add(field);
			}

			unsigned short arrayCount = buffer.readBytes<unsigned short>();

			for (int i = 0; i < arrayCount; i++)
			{
				Array* array = new Array;

				array->read(buffer);
				this->add(array);

				buffer.addOffset(array->getCount() * array->getDataType());
			}
		}

		const std::string& getName() const { return name; }
		byte getContainerType() const { return type; }

		inline unsigned int getSize() const
		{
			unsigned int ret = sizeof(byte) + sizeof(short) + name.length() + sizeof(short) + sizeof(short);

			for (const Field* field : fields)
				ret += field->getSize();

			for (const Array* array : arrays)
				ret += array->getSize();

			return ret;
		}

		const std::vector<Field*>& getFields() { return fields; }
		const std::vector<Array*>& getArrays() { return arrays; }
	};

}