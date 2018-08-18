#include <Cereal.h>

#include <gtest/gtest.h>

typedef unsigned char byte;

TEST(SerializationUnits, Header)
{
	Cereal::Database* db = new Cereal::Database("test");
	Cereal::Header* header = new Cereal::Header;

	header->addDatabase(db);

	EXPECT_NE(header->getDatabase("test"), nullptr);
	EXPECT_EQ(header->getDatabases().size(), 1);

	delete header;
}