#include <Cereal.h>

#include <gtest/gtest.h>

typedef unsigned char byte;

::testing::AssertionResult isLittleEndian()
{
	short i = 1;
	byte *p = (byte*) &i;

	if (p[0] == 1) return ::testing::AssertionSuccess() << "CPU is Little Endian";

	return ::testing::AssertionFailure() << "CPU is Big Endian";
}

TEST(Preconditions, LittleEndian)
{
	ASSERT_TRUE(isLittleEndian());
}

TEST(Preconditions, DataTypes)
{
	ASSERT_EQ(sizeof(byte), 1);

	ASSERT_EQ(sizeof(char), 1);
	ASSERT_EQ(sizeof(short), 2);
	ASSERT_EQ(sizeof(int), 4);
	ASSERT_EQ(sizeof(float), 4);
	ASSERT_EQ(sizeof(long long), 8);
	ASSERT_EQ(sizeof(double), 8);

	ASSERT_EQ(sizeof(unsigned char), 1);
	ASSERT_EQ(sizeof(unsigned short), 2);
	ASSERT_EQ(sizeof(unsigned int), 4);
	ASSERT_EQ(sizeof(unsigned long long), 8);
}

TEST(Core, CoreFuncs)
{
	ASSERT_TRUE(false);

	ASSERT_NO_THROW(sizeOf(Cereal::DataType::DATA_BOOL));
	ASSERT_NO_THROW(sizeOf(Cereal::DataType::DATA_CHAR));
	ASSERT_NO_THROW(sizeOf(Cereal::DataType::DATA_SHORT));
	ASSERT_NO_THROW(sizeOf(Cereal::DataType::DATA_INT));
	ASSERT_NO_THROW(sizeOf(Cereal::DataType::DATA_FLOAT));
	ASSERT_NO_THROW(sizeOf(Cereal::DataType::DATA_DOUBLE));
	ASSERT_NO_THROW(sizeOf(Cereal::DataType::DATA_LONG_LONG));

	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_ARRAY), std::invalid_argument);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_FIELD), std::invalid_argument);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_OBJECT), std::invalid_argument);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_STRING), std::invalid_argument);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_UNKNOWN), std::invalid_argument);

	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_BOOL), 1);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_CHAR), 1);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_SHORT), 2);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_INT), 4);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_FLOAT), 4);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_DOUBLE), 8);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_LONG_LONG), 8);
}

TEST(Core, Writing)
{
	Cereal::Header* myHeader = new Cereal::Header();
	Cereal::Database* db1 = new Cereal::Database("Test");
	Cereal::Database* db2 = new Cereal::Database("Second database");
	Cereal::Object* obj = new Cereal::Object("object");
	Cereal::Object* second = new Cereal::Object("second object");
	Cereal::Field* myField = new Cereal::Field("field", 3.14f);
	Cereal::Field* other = new Cereal::Field("other", std::string("test string"));
	Cereal::Array* arr = new Cereal::Array("array", new int[4]{ 1, 2, 3, 4 }, 4);
	Cereal::Array* str = new Cereal::Array("arr2", new std::string[2]{ "t1", "t2" }, 2);

	obj->addArray(arr);
	obj->addField(myField);
	obj->addField(other);

	second->addArray(str);

	db1->addObject(obj);
	db2->addObject(second);

	myHeader->addDatabase(db1);
	myHeader->addDatabase(db2);

	Cereal::Buffer buffer(myHeader->getSize());

	EXPECT_TRUE(myHeader->write(buffer));
	EXPECT_TRUE(buffer.writeFile("test-out.db"));

	EXPECT_EQ((size_t)2, obj->getFields().size());
	EXPECT_EQ((size_t)1, obj->getArrays().size());
	EXPECT_EQ((size_t)1, second->getArrays().size());
	EXPECT_EQ((size_t)1, db1->getObjects().size());
	EXPECT_EQ((size_t)1, db2->getObjects().size());
	EXPECT_EQ((size_t)2, myHeader->getDatabases().size());

	delete myHeader;
}

TEST(Core, Reading)
{
	Cereal::Buffer buffer(0);
	Cereal::Header* header = new Cereal::Header;

	ASSERT_TRUE(buffer.readFile("test-out.db"));

	header->read(buffer);

	Cereal::Database* db1 = header->getDatabase("Test");
	Cereal::Database* db2 = header->getDatabase("Second database");
	Cereal::Object* obj1 = db1->getObject("object");
	Cereal::Object* obj2 = db2->getObject("second object");
	Cereal::Field* f1 = obj1->getField("field");
	Cereal::Field* f2 = obj1->getField("other");
	Cereal::Array* arr1 = obj1->getArray("array");
	Cereal::Array* arr2 = obj2->getArray("arr2");

	EXPECT_NE(db1, nullptr);
	EXPECT_NE(db2, nullptr);
	EXPECT_NE(obj1, nullptr);
	EXPECT_NE(obj2, nullptr);
	EXPECT_NE(f1, nullptr);
	EXPECT_NE(f2, nullptr);
	EXPECT_NE(arr1, nullptr);
	EXPECT_NE(arr2, nullptr);

	EXPECT_EQ((size_t)2, header->getDatabases().size());
	EXPECT_EQ((size_t)1, db1->getObjects().size());
	EXPECT_EQ((size_t)1, db2->getObjects().size());
	EXPECT_EQ((size_t)2, obj1->getFields().size());
	EXPECT_EQ((size_t)1, obj1->getArrays().size());
	EXPECT_EQ((size_t)0, obj2->getFields().size());
	EXPECT_EQ((size_t)1, obj2->getArrays().size());
	EXPECT_FLOAT_EQ(3.14f, f1->getValue<float>());
	EXPECT_EQ((unsigned int)4, arr1->getCount());
	EXPECT_EQ((unsigned int)2, arr2->getCount());
	EXPECT_EQ(3, arr1->getArray<int>()[2]);
	EXPECT_STREQ("test string", f2->getValue<std::string>().c_str());
	EXPECT_STREQ("t1", arr2->getArray<std::string>()[0].c_str());
	EXPECT_STREQ("t2", arr2->getArray<std::string>()[1].c_str());

	delete header;
}