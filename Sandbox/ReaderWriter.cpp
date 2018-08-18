#include <Cereal.h>

#include <gtest/gtest.h>
#include <random>

typedef unsigned char byte;

TEST(SerializationUnits, ReaderWriter)
{
	byte buff[8];

	buff[0] = false;
	buff[1] = true;

	//bool
	EXPECT_FALSE(Cereal::Reader::readBytes<bool>(&buff[0], 0));
	EXPECT_TRUE(Cereal::Reader::readBytes<bool>(&buff[1], 0));

	//string
	Cereal::Writer::writeBytes<std::string>(&buff[0], 0, "asdf");
	EXPECT_STREQ(Cereal::Reader::readBytes<std::string>(&buff[0], 0).c_str(), "asdf");

	//float
	srand(time(NULL));
	float val = rand() / (float)RAND_MAX;

	Cereal::Writer::writeBytes<float>(&buff[0], 0, val);
	EXPECT_FLOAT_EQ(Cereal::Reader::readBytes<float>(&buff[0], 0), val);

	//double
	double doub = rand() / (double)RAND_MAX;

	Cereal::Writer::writeBytes<double>(&buff[0], 0, doub);
	EXPECT_DOUBLE_EQ(Cereal::Reader::readBytes<double>(&buff[0], 0), doub);

	//char
	for(unsigned int i = 0; i < 127; i++)
	{
		Cereal::Writer::writeBytes<char>(&buff[0], 0, (char)i);
		EXPECT_EQ(Cereal::Reader::readBytes<char>(&buff[0], 0), i);
	}

	//byte
	for(unsigned int i = 0; i < 256; i++)
	{
		Cereal::Writer::writeBytes<byte>(&buff[0], 0, (byte)i);
		EXPECT_EQ(Cereal::Reader::readBytes<byte>(&buff[0], 0), i);
	}

	//short
	for(unsigned int j = 0; j < 2; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			Cereal::Writer::writeBytes<unsigned short>(&buff[0], 0, (short)i << (8 * j));
			EXPECT_EQ(Cereal::Reader::readBytes<unsigned short>(&buff[0], 0), i << (8 * j));
		}
	}

	//int
	for(unsigned int j = 0; j < 4; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			Cereal::Writer::writeBytes<unsigned int>(&buff[0], 0, i << (8 * j));
			EXPECT_EQ(Cereal::Reader::readBytes<unsigned int>(&buff[0], 0), i << (8 * j));
		}
	}

	//long long
	for(unsigned int j = 0; j < 8; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			const unsigned long long n = i << (8 * j);
			Cereal::Writer::writeBytes<unsigned long long>(&buff[0], 0, n);
			EXPECT_EQ(Cereal::Reader::readBytes<unsigned long long>(&buff[0], 0), n);
		}
	}
}