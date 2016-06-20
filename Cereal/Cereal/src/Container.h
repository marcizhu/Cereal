#pragma once

#include <string>

#include "Writer.h"
#include "Reader.h"

namespace Cereal {

	struct Container
	{
	protected:
		byte type;
		std::string name;

		int writeContainer(byte* dest, int pointer)
		{
			pointer = Writer::writeBytes<byte>(dest, pointer, type);
			pointer = Writer::writeBytes<std::string>(dest, pointer, name);

			return pointer;
		}

		void readContainer(byte* dest, int pointer)
		{
			this->type = Reader::readBytes<byte>(dest, pointer);
			this->name = Reader::readBytes<std::string>(dest, pointer);
		}

	};

}
