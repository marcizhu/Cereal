#pragma once

#include <string>

#include "Writer.h"

namespace Cereal {

	struct Container
	{
	protected:
		byte type;
		std::string name;

		int writeContainer(byte* dest, int pointer)
		{
			pointer = Writer::writeBytes(dest, pointer, type);
			pointer = Writer::writeBytes(dest, pointer, name);

			return pointer;
		}

		void readContainer(byte* dest, int pointer)
		{
			this->type = Writer::readBytes<byte>(dest, pointer);
			this->name = Writer::readBytes<std::string>(dest, pointer);
		}

	};

}
