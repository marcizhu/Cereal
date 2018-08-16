// Sandbox.cpp : Defines the entry point for the console application.
//

#include <Cereal.h>
#include <random>

#include <gtest/gtest.h>

typedef unsigned char byte;

TEST(Preconditions, endian)
{
	union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };

	ASSERT_EQ(bint.c[0], 4); // CPU is little endian
}

TEST(Preconditions, datatypes)
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
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_BOOL), 1);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_CHAR), 1);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_SHORT), 2);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_INT), 4);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_FLOAT), 4);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_DOUBLE), 8);
	EXPECT_EQ(sizeOf(Cereal::DataType::DATA_LONG_LONG), 8);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_ARRAY), std::invalid_argument);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_FIELD), std::invalid_argument);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_OBJECT), std::invalid_argument);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_STRING), std::invalid_argument);
	EXPECT_THROW(sizeOf(Cereal::DataType::DATA_UNKNOWN), std::invalid_argument);
}

TEST(SerializationUnits, Buffer)
{
	Cereal::Buffer* buff = new Cereal::Buffer(100);
	buff->setOffset(0);
	EXPECT_NO_THROW(buff->setOffset(100));
	EXPECT_THROW(buff->setOffset(101), std::domain_error);
	EXPECT_THROW(buff->addOffset(5), std::domain_error);
	EXPECT_EQ(buff->getOffset(), 100);
	EXPECT_THROW(buff->addOffset(95), std::domain_error);
	EXPECT_THROW(buff->addOffset(96), std::domain_error);
	EXPECT_EQ(buff->getOffset(), 100);

	delete buff;

	// TODO: Serialize all at once and then deserialize everything

	buff = new Cereal::Buffer(sizeof(unsigned long long) * 0xFF);

	buff->writeBytes<bool>(false);
	buff->writeBytes<bool>(true);

	buff->setOffset(0);

	//bool
	EXPECT_FALSE(buff->readBytes<bool>());
	EXPECT_TRUE(buff->readBytes<bool>());

	buff->setOffset(0);

	//string
	buff->writeBytes<std::string>("asdf");
	buff->setOffset(0);
	EXPECT_STREQ(buff->readBytes<std::string>().c_str(), "asdf");

	buff->setOffset(0);

	//float
	srand(time(NULL));
	float val = rand() / (float)RAND_MAX;

	buff->writeBytes<float>(val);
	buff->setOffset(0);
	EXPECT_FLOAT_EQ(buff->readBytes<float>(), val);

	buff->setOffset(0);

	//double
	double doub = rand() / (double)RAND_MAX;

	buff->writeBytes<double>(doub);
	buff->setOffset(0);
	EXPECT_DOUBLE_EQ(buff->readBytes<double>(), doub);

	buff->setOffset(0);

	//char
	for(unsigned int i = 0; i < 127; i++)
	{
		buff->writeBytes<char>((char)i);
	}

	buff->setOffset(0);

	for(unsigned int i = 0; i < 127; i++)
	{
		EXPECT_EQ(buff->readBytes<char>(), i);
	}

	//byte
	for(unsigned int i = 0; i < 256; i++)
	{
		buff->setOffset(0);
		buff->writeBytes<byte>((byte)i);
		buff->setOffset(0);
		EXPECT_EQ(buff->readBytes<byte>(), i);
	}

	//short
	for(unsigned int j = 0; j < 2; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			buff->setOffset(0);
			buff->writeBytes<unsigned short>((short)i << (8 * j));
			buff->setOffset(0);
			EXPECT_EQ(buff->readBytes<unsigned short>(), i << (8 * j));
		}
	}

	//int
	for(unsigned int j = 0; j < 4; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			buff->setOffset(0);
			buff->writeBytes<unsigned int>(i << (8 * j));
			buff->setOffset(0);
			EXPECT_EQ(buff->readBytes<unsigned int>(), i << (8 * j));
		}
	}

	//long long
	for(unsigned int j = 0; j < 8; j++)
	{
		for(unsigned int i = 0; i < 256; i++)
		{
			const unsigned long long n = i << (8 * j);
			buff->setOffset(0);
			buff->writeBytes<unsigned long long>(n);
			buff->setOffset(0);
			EXPECT_EQ(buff->readBytes<unsigned long long>(), n);
		}
	}

	delete buff;
}

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

TEST(SerializationUnits, Field)
{
	char		r0 = rand() & 0x00FF;
	short		r1 = rand() & 0xFFFF;
	int			r2 = rand();
	float		r3 = rand() / (float)RAND_MAX;
	long long	r4 = ((long long)rand() << 32) | rand();

	Cereal::Field* fbool = new Cereal::Field("field bool", (bool)true);
	Cereal::Field* fchar = new Cereal::Field("field char", r0);
	Cereal::Field* fshort = new Cereal::Field("field short", r1);
	Cereal::Field* fint = new Cereal::Field("field int", r2);
	Cereal::Field* ffloat = new Cereal::Field("field float", r3);
	Cereal::Field* flonglong = new Cereal::Field("field long long", r4);
	Cereal::Field* fdouble = new Cereal::Field("field double", (double)3.141592);
	Cereal::Field* fstring = new Cereal::Field("field string", std::string("test string"));

	EXPECT_STREQ(fbool->getName().c_str(), "field bool");
	EXPECT_STREQ(fchar->getName().c_str(), "field char");
	EXPECT_STREQ(fshort->getName().c_str(), "field short");
	EXPECT_STREQ(fint->getName().c_str(), "field int");
	EXPECT_STREQ(ffloat->getName().c_str(), "field float");
	EXPECT_STREQ(flonglong->getName().c_str(), "field long long");
	EXPECT_STREQ(fdouble->getName().c_str(), "field double");
	EXPECT_STREQ(fstring->getName().c_str(), "field string");

	EXPECT_TRUE(fbool->getValue<bool>());
	EXPECT_EQ(fchar->getValue<char>(), r0);
	EXPECT_EQ(fshort->getValue<short>(), r1);
	EXPECT_EQ(fint->getValue<int>(), r2);
	EXPECT_FLOAT_EQ(ffloat->getValue<float>(), r3);
	EXPECT_EQ(flonglong->getValue<long long>(), r4);
	EXPECT_DOUBLE_EQ(fdouble->getValue<double>(), 3.141592);
	EXPECT_STREQ(fstring->getValue<std::string>().c_str(), "test string");

	delete fbool;
	delete fchar;
	delete fshort;
	delete fint;
	delete ffloat;
	delete flonglong;
	delete fdouble;
	delete fstring;
}

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

	delete obj;
}

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

TEST(SerializationUnits, Header)
{
	Cereal::Database* db = new Cereal::Database("test");
	Cereal::Header* header = new Cereal::Header;

	header->addDatabase(db);

	EXPECT_NE(header->getDatabase("test"), nullptr);
	EXPECT_EQ(header->getDatabases().size(), 1);

	delete header;
}

TEST(CerealLib, Writing)
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

TEST(CerealLib, Reading)
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