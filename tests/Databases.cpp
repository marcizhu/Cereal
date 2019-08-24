#include <Cereal.h>

#include <gtest/gtest.h>

typedef unsigned char byte;

TEST(SerializationUnits, Database)
{
	Cereal::Object* obj = new Cereal::Object("obj name");
	Cereal::Database* db = new Cereal::Database("db name");

	db->addObject(obj);

	EXPECT_STREQ(db->getName().c_str(), "db name");
	EXPECT_NE(db->getObject("obj name"), nullptr);
	EXPECT_EQ(db->getObjects().size(), 1);

	delete db;
}