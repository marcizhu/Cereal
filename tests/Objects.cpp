#include <Cereal.h>
#include <random>

#include <gtest/gtest.h>

typedef unsigned char byte;

TEST(SerializationUnits, Object)
{
	Cereal::Field* f = new Cereal::Field("f", 3);
	Cereal::Array* a = new Cereal::Array("a", new int[2]{ 1, 2 }, 2);
	Cereal::Object* obj = new Cereal::Object("object");

	obj->addArray(a);
	obj->addField(f);

	EXPECT_STREQ(obj->getName().c_str(), "object");
	EXPECT_EQ(obj->getArrays().size(), 1);
	EXPECT_EQ(obj->getFields().size(), 1);
	EXPECT_NE(obj->getField("f"), nullptr);
	EXPECT_NE(obj->getArray("a"), nullptr);

	EXPECT_EQ(obj->getField("nonexistent"), nullptr);
	EXPECT_EQ(obj->getArray("nonexistent"), nullptr);

	obj->deleteField("shgdad");
	obj->deleteArray("shgdad");

	EXPECT_STREQ(obj->getName().c_str(), "object");
	EXPECT_EQ(obj->getArrays().size(), 1);
	EXPECT_EQ(obj->getFields().size(), 1);
	EXPECT_NE(obj->getField("f"), nullptr);
	EXPECT_NE(obj->getArray("a"), nullptr);

	EXPECT_EQ(obj->getField("nonexistent"), nullptr);
	EXPECT_EQ(obj->getArray("nonexistent"), nullptr);

	obj->deleteField("f");
	obj->deleteArray("a");

	EXPECT_EQ(obj->getField("f"), nullptr);
	EXPECT_EQ(obj->getArray("a"), nullptr);
	EXPECT_EQ(obj->getArrays().size(), 0);
	EXPECT_EQ(obj->getFields().size(), 0);

	delete obj;
}