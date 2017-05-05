// Sandbox.cpp : Defines the entry point for the console application.
//

#include <Cereal.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

typedef unsigned char byte;

TEST_CASE("Test preconditions", "[pre]")
{
	SECTION("endian")
	{
		union {
			uint32_t i;
			char c[4];
		} bint = { 0x01020304 };

		REQUIRE(bint.c[0] == 4); // CPU is little endian
	}

	SECTION("datatypes")
	{
		REQUIRE(sizeof(byte) == 1);

		REQUIRE(sizeof(char) == 1);
		REQUIRE(sizeof(short) == 2);
		REQUIRE(sizeof(int) == 4);
		REQUIRE(sizeof(float) == 4);
		REQUIRE(sizeof(long long) == 8);
		REQUIRE(sizeof(double) == 8);

		REQUIRE(sizeof(unsigned char) == 1);
		REQUIRE(sizeof(unsigned short) == 2);
		REQUIRE(sizeof(unsigned int) == 4);
		REQUIRE(sizeof(unsigned long long) == 8);
	}
}

TEST_CASE("Test core functions", "[core]")
{
	CHECK(sizeOf(Cereal::DataType::DATA_BOOL) == 1);
	CHECK(sizeOf(Cereal::DataType::DATA_CHAR) == 1);
	CHECK(sizeOf(Cereal::DataType::DATA_SHORT) == 2);
	CHECK(sizeOf(Cereal::DataType::DATA_INT) == 4);
	CHECK(sizeOf(Cereal::DataType::DATA_FLOAT) == 4);
	CHECK(sizeOf(Cereal::DataType::DATA_DOUBLE) == 8);
	CHECK(sizeOf(Cereal::DataType::DATA_LONG_LONG) == 8);
	CHECK_THROWS(sizeOf(Cereal::DataType::DATA_ARRAY));
	CHECK_THROWS(sizeOf(Cereal::DataType::DATA_FIELD));
	CHECK_THROWS(sizeOf(Cereal::DataType::DATA_OBJECT));
	CHECK_THROWS(sizeOf(Cereal::DataType::DATA_STRING));
	CHECK_THROWS(sizeOf(Cereal::DataType::DATA_UNKNOWN));
}

TEST_CASE("Test serialization units", "[lib][units]")
{
	SECTION("Buffer")
	{
		Cereal::Buffer* buff = new Cereal::Buffer(100);
		buff->setOffset(0);
		CHECK_NOTHROW(buff->setOffset(100));
		CHECK_THROWS(buff->setOffset(101));
		CHECK_THROWS(buff->addOffset(5));
		CHECK(buff->getOffset() == 100);
		CHECK_THROWS(buff->addOffset(95));
		CHECK_THROWS(buff->addOffset(96));
		CHECK(buff->getOffset() == 100);
	}

	SECTION("Reader/Writer")
	{
		byte buff[8];

		buff[0] = false;
		buff[1] = true;

		//bool
		CHECK(Cereal::Reader::readBytes<bool>(&buff[0], 0) == false);
		CHECK(Cereal::Reader::readBytes<bool>(&buff[1], 0) == true);

		//string
		Cereal::Writer::writeBytes<std::string>(&buff[0], 0, "asdf");
		CHECK(Cereal::Reader::readBytes<std::string>(&buff[0], 0) == "asdf");

		//float
		Cereal::Writer::writeBytes<float>(&buff[0], 0, 3.2f);
		CHECK(Cereal::Reader::readBytes<float>(&buff[0], 0) == 3.2f);

		//double
		Cereal::Writer::writeBytes<double>(&buff[0], 0, 3.141592);
		CHECK(Cereal::Reader::readBytes<double>(&buff[0], 0) == 3.141592);

		//char
		for(unsigned int i = 0; i < 127; i++)
		{
			Cereal::Writer::writeBytes<char>(&buff[0], 0, (char)i);
			CHECK(Cereal::Reader::readBytes<char>(&buff[0], 0) == i);
		}

		//byte
		for(unsigned int i = 0; i < 256; i++)
		{
			Cereal::Writer::writeBytes<byte>(&buff[0], 0, (byte)i);
			CHECK(Cereal::Reader::readBytes<byte>(&buff[0], 0) == i);
		}

		//short
		for(unsigned int j = 0; j < 2; j++)
		{
			for(unsigned int i = 0; i < 256; i++)
			{
				Cereal::Writer::writeBytes<unsigned short>(&buff[0], 0, (short)i << (8 * j));
				CHECK(Cereal::Reader::readBytes<unsigned short>(&buff[0], 0) == i << (8 * j));
			}
		}

		//int
		for(unsigned int j = 0; j < 4; j++)
		{
			for(unsigned int i = 0; i < 256; i++)
			{
				Cereal::Writer::writeBytes<unsigned int>(&buff[0], 0, i << (8 * j));
				CHECK(Cereal::Reader::readBytes<unsigned int>(&buff[0], 0) == i << (8 * j));
			}
		}

		//long long
		for(unsigned int j = 0; j < 8; j++)
		{
			for(unsigned int i = 0; i < 256; i++)
			{
				Cereal::Writer::writeBytes<unsigned long long>(&buff[0], 0, i << (8 * j));
				CHECK(Cereal::Reader::readBytes<unsigned long long>(&buff[0], 0) == i << (8 * j));
			}
		}
	}

	SECTION("Fields")
	{
		Cereal::Field* fbool = new Cereal::Field("field bool", (bool)true);
		Cereal::Field* fchar = new Cereal::Field("field char", (char)'M');
		Cereal::Field* fshort = new Cereal::Field("field short", (short)9000);
		Cereal::Field* fint = new Cereal::Field("field int", (int)42);
		Cereal::Field* ffloat = new Cereal::Field("field float", 3.2f);
		Cereal::Field* flonglong = new Cereal::Field("field long long", (long long)0x123456789ABCDEF0);
		Cereal::Field* fdouble = new Cereal::Field("field double", (double)3.141592);
		Cereal::Field* fstring = new Cereal::Field("field string", std::string("test string"));

		CHECK(fbool->getName() == "field bool");
		CHECK(fchar->getName() == "field char");
		CHECK(fshort->getName() == "field short");
		CHECK(fint->getName() == "field int");
		CHECK(ffloat->getName() == "field float");
		CHECK(flonglong->getName() == "field long long");
		CHECK(fdouble->getName() == "field double");
		CHECK(fstring->getName() == "field string");

		CHECK(fbool->getValue<bool>() == true);
		CHECK(fchar->getValue<char>() == 'M');
		CHECK(fshort->getValue<short>() == 9000);
		CHECK(fint->getValue<int>() == 42);
		CHECK(ffloat->getValue<float>() == 3.2f);
		CHECK(flonglong->getValue<long long>() == 0x123456789ABCDEF0);
		CHECK(fdouble->getValue<double>() == 3.141592);
		CHECK(fstring->getValue<std::string>() == "test string");

		delete fbool;
		delete fchar;
		delete fshort;
		delete fint;
		delete ffloat;
		delete flonglong;
		delete fdouble;
		delete fstring;
	}

	SECTION("Arrays")
	{
		Cereal::Array* a = new Cereal::Array("array name", new int[4]{ 1, 2, 3, 4 }, 4);
		Cereal::Array* a2 = new Cereal::Array("second", new std::string[2]{ "another string", "string no 2" }, 2);
		int* ret = a->getRawArray(new int[a->getCount()]);

		CHECK(a->getName() == "array name");
		CHECK(a2->getName() == "second");
		CHECK(a2->getArray<std::string>()[0] == "another string");
		CHECK(a2->getArray<std::string>()[1] == "string no 2");

		CHECK(a->getCount() == 4);
		CHECK(ret[0] == 1);
		CHECK(ret[1] == 2);
		CHECK(ret[2] == 3);
		CHECK(ret[3] == 4);
		CHECK(a->getArray<int>().size() == a->getCount());

		delete a;
		delete a2;
		delete ret;
	}

	SECTION("Objects")
	{
		Cereal::Field* f = new Cereal::Field("f", 3);
		Cereal::Array* a = new Cereal::Array("a", new int[2]{ 1, 2 }, 2);
		Cereal::Object* obj = new Cereal::Object("object");

		obj->addArray(a);
		obj->addField(f);

		CHECK(obj->getName() == "object");
		CHECK(obj->getArrays().size() == 1);
		CHECK(obj->getFields().size() == 1);
		CHECK(obj->getField("f") != nullptr);
		CHECK(obj->getArray("a") != nullptr);

		delete obj;
	}

	SECTION("Databases")
	{
		Cereal::Object* obj = new Cereal::Object("obj name");
		Cereal::Database* db = new Cereal::Database("db name");

		db->addObject(obj);

		CHECK(db->getName() == "db name");
		CHECK(db->getObject("obj name") != nullptr);
		CHECK(db->getObjects().size() == 1);

		delete db;
	}

	SECTION("Headers")
	{
		Cereal::Database* db = new Cereal::Database("test");
		Cereal::Header* header = new Cereal::Header;

		header->addDatabase(db);

		CHECK(header->getDatabase("test") != nullptr);
		CHECK(header->getDatabases().size() == 1);

		delete header;
	}
}

TEST_CASE("Test Cereal library", "[lib]")
{
	SECTION("Writing")
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

		CHECK(myHeader->write(buffer) == true);
		CHECK(buffer.writeFile("test-out.db") == true);

		CHECK((size_t)2 == obj->getFields().size());
		CHECK((size_t)1 == obj->getArrays().size());
		CHECK((size_t)1 == second->getArrays().size());
		CHECK((size_t)1 == db1->getObjects().size());
		CHECK((size_t)1 == db2->getObjects().size());
		CHECK((size_t)2 == myHeader->getDatabases().size());

		delete myHeader;
	}

	SECTION("Reading")
	{
		Cereal::Buffer buffer(0);
		Cereal::Header* header = new Cereal::Header;

		REQUIRE(buffer.readFile("test-out.db") == true);

		header->read(buffer);

		Cereal::Database* db1 = header->getDatabase("Test");
		Cereal::Database* db2 = header->getDatabase("Second database");
		Cereal::Object* obj1 = db1->getObject("object");
		Cereal::Object* obj2 = db2->getObject("second object");
		Cereal::Field* f1 = obj1->getField("field");
		Cereal::Field* f2 = obj1->getField("other");
		Cereal::Array* arr1 = obj1->getArray("array");
		Cereal::Array* arr2 = obj2->getArray("arr2");

		CHECK(db1 != nullptr);
		CHECK(db2 != nullptr);
		CHECK(obj1 != nullptr);
		CHECK(obj2 != nullptr);
		CHECK(f1 != nullptr);
		CHECK(f2 != nullptr);
		CHECK(arr1 != nullptr);
		CHECK(arr2 != nullptr);

		CHECK((size_t)2 == header->getDatabases().size());
		CHECK((size_t)1 == db1->getObjects().size());
		CHECK((size_t)1 == db2->getObjects().size());
		CHECK((size_t)2 == obj1->getFields().size());
		CHECK((size_t)1 == obj1->getArrays().size());
		CHECK((size_t)0 == obj2->getFields().size());
		CHECK((size_t)1 == obj2->getArrays().size());
		CHECK(3.14f == f1->getValue<float>());
		CHECK((unsigned int)4 == arr1->getCount());
		CHECK((unsigned int)2 == arr2->getCount());
		CHECK(3 == arr1->getArray<int>()[2]);
		CHECK(std::string("test string") == f2->getValue<std::string>());
		CHECK(std::string("t1") == arr2->getArray<std::string>()[0]);
		CHECK(std::string("t2") == arr2->getArray<std::string>()[1]);

		delete header;
	}
}