#include <Cereal.h>

#include <gtest/gtest.h>

typedef unsigned char byte;

TEST(SerializationUnits, Array)
{
	Cereal::Array* a = new Cereal::Array("array name", new int[4]{ 1, 2, 3, 4 }, 4);
	Cereal::Array* a2 = new Cereal::Array("second", new std::string[2]{ "another string", "string no 2" }, 2);
	int* ret = a->getRawArray(new int[a->getCount()]);

	EXPECT_STREQ(a->getName().c_str(), "array name");
	EXPECT_STREQ(a2->getName().c_str(), "second");
	EXPECT_STREQ(a2->getArray<std::string>()[0].c_str(), "another string");
	EXPECT_STREQ(a2->getArray<std::string>()[1].c_str(), "string no 2");

	EXPECT_EQ(a->getCount(), 4);
	EXPECT_EQ(ret[0], 1);
	EXPECT_EQ(ret[1], 2);
	EXPECT_EQ(ret[2], 3);
	EXPECT_EQ(ret[3], 4);
	EXPECT_EQ(a->getArray<int>().size(), a->getCount());

	delete a;
	delete a2;
	delete ret;
}