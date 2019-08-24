#include <Cereal.h>

#include <gtest/gtest.h>
#include <random>

typedef unsigned char byte;

class WriterTest : public ::testing::Test
{
protected:
	byte* buff;
	unsigned int offset;

	WriterTest() { buff = new byte[16384]; }
	~WriterTest() { delete[] buff; }

	void SetUp() override { offset = 0; }
};

TEST_F(WriterTest, Bool)
{
	srand(time(NULL));
	bool data[1000];

	for(int i = 0; i < 1000; i++)
	{
		data[i] = (rand() % 2) == 0;
		offset = Cereal::Writer::writeBytes<bool>(buff, offset, data[i]);
	}

	offset = 0;

	for(int i = 0; i < 1000; i++)
	{
		EXPECT_EQ(Cereal::Reader::readBytes<bool>(buff, offset), data[i]);
		offset += sizeof(bool);
	}
}

TEST_F(WriterTest, String)
{
	// TODO: Add more string tests
	Cereal::Writer::writeBytes<std::string>(buff, 0, "asdf");
	EXPECT_STREQ(Cereal::Reader::readBytes<std::string>(buff, 0).c_str(), "asdf");
}

TEST_F(WriterTest, Float)
{
	float fdata[1000];

	for(int i = 0; i < 1000; i++)
	{
		fdata[i] = (rand() / (float)RAND_MAX);
		offset = Cereal::Writer::writeBytes<float>(buff, offset, fdata[i]);
	}

	offset = 0;

	for(int i = 0; i < 1000; i++)
	{
		EXPECT_FLOAT_EQ(Cereal::Reader::readBytes<float>(buff, offset), fdata[i]);
		offset += sizeof(float);
	}
}

TEST_F(WriterTest, Double)
{
	double data[1000];

	for(int i = 0; i < 1000; i++)
	{
		data[i] = (rand() / (double)RAND_MAX);
		offset = Cereal::Writer::writeBytes<double>(buff, offset, data[i]);
	}

	offset = 0;

	for(int i = 0; i < 1000; i++)
	{
		EXPECT_DOUBLE_EQ(Cereal::Reader::readBytes<double>(buff, offset), data[i]);
		offset += sizeof(double);
	}
}

TEST_F(WriterTest, Char)
{
	for(unsigned int i = 0; i < 127; i++)
		offset = Cereal::Writer::writeBytes<char>(buff, offset, (char)i);

	offset = 0;

	for(unsigned int i = 0; i < 127; i++)
	{
		EXPECT_EQ(Cereal::Reader::readBytes<char>(buff, offset), i);
		offset += sizeof(char);
	}
}

TEST_F(WriterTest, Byte)
{
	for(unsigned int i = 0; i < 256; i++)
		offset = Cereal::Writer::writeBytes<byte>(buff, offset, (byte)i);

	offset = 0;

	for(unsigned int i = 0; i < 256; i++)
	{
		EXPECT_EQ(Cereal::Reader::readBytes<byte>(buff, offset), i);
		offset += sizeof(byte);
	}
}

TEST_F(WriterTest, Short)
{
	for(unsigned int j = 0; j < 2; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			offset = Cereal::Writer::writeBytes<unsigned short>(buff, offset, (unsigned short)i << (8 * j));
		}
	}

	offset = 0;

	for(unsigned int j = 0; j < 2; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			EXPECT_EQ(Cereal::Reader::readBytes<unsigned short>(buff, offset), i << (8 * j));
			offset += sizeof(short);
		}
	}
}

TEST_F(WriterTest, Int32)
{
	for(unsigned int j = 0; j < 4; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			offset = Cereal::Writer::writeBytes<unsigned int>(buff, offset, i << (8 * j));
		}
	}

	offset = 0;

	for(unsigned int j = 0; j < 4; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			EXPECT_EQ(Cereal::Reader::readBytes<unsigned int>(buff, offset), i << (8 * j));
			offset += sizeof(int);
		}
	}
}

TEST_F(WriterTest, Int64)
{
	for(unsigned int j = 0; j < 8; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			offset = Cereal::Writer::writeBytes<unsigned long long>(buff, offset, (unsigned long long)i << (8 * j));
		}
	}

	offset = 0;

	for(unsigned int j = 0; j < 8; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			EXPECT_EQ(Cereal::Reader::readBytes<unsigned long long>(buff, offset), (unsigned long long)i << (8 * j));
			offset += sizeof(long long);
		}
	}
}