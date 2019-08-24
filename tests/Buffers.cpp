#include <Cereal.h>

#include <gtest/gtest.h>
#include <random>

typedef unsigned char byte;

class BufferTest: public ::testing::Test
{
protected:
	Cereal::Buffer* buff;

	BufferTest() { buff = new Cereal::Buffer(16384); }
	~BufferTest() { delete buff; }
};

TEST(Buffer, Domain)
{
	Cereal::Buffer* buff = new Cereal::Buffer(100);

	ASSERT_EQ(buff->getOffset(), 0);
	EXPECT_NO_THROW(buff->setOffset(100));
	EXPECT_THROW(buff->setOffset(101), std::domain_error);
	EXPECT_THROW(buff->addOffset(5), std::domain_error);
	EXPECT_EQ(buff->getOffset(), 100);
	EXPECT_THROW(buff->addOffset(95), std::domain_error);
	EXPECT_THROW(buff->addOffset(96), std::domain_error);
	EXPECT_EQ(buff->getOffset(), 100);

	delete buff;
}

TEST_F(BufferTest, Bool)
{
	bool data[1000];
	srand(time(NULL));

	for(int i = 0; i < 1000; i++)
	{
		data[i] = (rand() % 2) == 0;
		ASSERT_TRUE(buff->writeBytes<bool>(data[i]));
	}

	buff->setOffset(0);

	for(int i = 0; i < 1000; i++)
		EXPECT_EQ(buff->readBytes<bool>(), data[i]);
}

TEST_F(BufferTest, String)
{
	// TODO: Add more string tests
	ASSERT_TRUE(buff->writeBytes<std::string>("asdf"));
	buff->setOffset(0);
	EXPECT_STREQ(buff->readBytes<std::string>().c_str(), "asdf");
}

TEST_F(BufferTest, Float)
{
	float fdata[1000];

	for(int i = 0; i < 1000; i++)
	{
		fdata[i] = (rand() / (float)RAND_MAX);
		ASSERT_TRUE(buff->writeBytes<float>(fdata[i]));
	}

	buff->setOffset(0);

	for(int i = 0; i < 1000; i++)
		EXPECT_FLOAT_EQ(buff->readBytes<float>(), fdata[i]);
}

TEST_F(BufferTest, Double)
{
	double data[1000];

	for(int i = 0; i < 1000; i++)
	{
		data[i] = (rand() / (double)RAND_MAX);
		ASSERT_TRUE(buff->writeBytes<double>(data[i]));
	}

	buff->setOffset(0);

	for(int i = 0; i < 1000; i++)
		EXPECT_DOUBLE_EQ(buff->readBytes<double>(), data[i]);
}

TEST_F(BufferTest, Char)
{
	for(unsigned int i = 0; i < 127; i++)
		ASSERT_TRUE(buff->writeBytes<char>((char)i));

	buff->setOffset(0);

	for(unsigned int i = 0; i < 127; i++)
		EXPECT_EQ(buff->readBytes<char>(), i);
}

TEST_F(BufferTest, Byte)
{
	for(unsigned int i = 0; i < 256; i++)
		ASSERT_TRUE(buff->writeBytes<byte>((byte)i));

	buff->setOffset(0);

	for(unsigned int i = 0; i < 256; i++)
		EXPECT_EQ(buff->readBytes<byte>(), i);
}

TEST_F(BufferTest, Short)
{
	for(unsigned int j = 0; j < 2; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			ASSERT_TRUE(buff->writeBytes<unsigned short>((unsigned short)i << (8 * j)));
		}
	}

	buff->setOffset(0);

	for(unsigned int j = 0; j < 2; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			EXPECT_EQ(buff->readBytes<unsigned short>(), i << (8 * j));
		}
	}
}

TEST_F(BufferTest, Int32)
{
	for(unsigned int j = 0; j < 4; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			ASSERT_TRUE(buff->writeBytes<unsigned int>(i << (8 * j)));
		}
	}

	buff->setOffset(0);

	for(unsigned int j = 0; j < 4; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			EXPECT_EQ(buff->readBytes<unsigned int>(), i << (8 * j));
		}
	}
}

TEST_F(BufferTest, Int64)
{
	for(unsigned int j = 0; j < 8; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			const unsigned long long n = i << (8 * j);
			ASSERT_TRUE(buff->writeBytes<unsigned long long>(n));
		}
	}

	buff->setOffset(0);

	for(unsigned int j = 0; j < 8; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			const unsigned long long n = i << (8 * j);
			EXPECT_EQ(buff->readBytes<unsigned long long>(), n);
		}
	}
}