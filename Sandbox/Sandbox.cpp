// Sandbox.cpp : Defines the entry point for the console application.
//

#include <Cereal.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

typedef unsigned char byte;

TEST_CASE("Testing preconditions", "[pre]")
{
	SECTION("endian")
	{
		union {
			uint32_t i;
			char c[4];
		} bint = { 0x01020304 };

		REQUIRE(bint.c[0] == 4); // CPU is big endian
	}

	SECTION("datatypes")
	{
		REQUIRE(sizeof(byte) == 1);

		REQUIRE(sizeof(char) == 1);
		REQUIRE(sizeof(short) == 2);
		REQUIRE(sizeof(int) == 4);
		REQUIRE(sizeof(long) == 4);
		REQUIRE(sizeof(float) == 4);
		REQUIRE(sizeof(long long) == 8);
		REQUIRE(sizeof(double) == 8);

		REQUIRE(sizeof(unsigned char) == 1);
		REQUIRE(sizeof(unsigned short) == 2);
		REQUIRE(sizeof(unsigned int) == 4);
		REQUIRE(sizeof(unsigned long) == 4);
		REQUIRE(sizeof(unsigned long long) == 8);
	}
}

TEST_CASE("Testing core functions", "[core]")
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

TEST_CASE("Testing serialization units", "[lib][units]")
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

	SECTION("Writer")
	{
		byte* buff = new byte[36];

		CHECK(Cereal::Writer::writeBytes<bool>(buff, 0, true) == 1);
		CHECK(Cereal::Writer::writeBytes<char>(buff, 1, 'A') == 2);
		CHECK(Cereal::Writer::writeBytes<short>(buff, 2, 0x0102) == 4);
		CHECK(Cereal::Writer::writeBytes<int>(buff, 4, 0x12345678) == 8);
		CHECK(Cereal::Writer::writeBytes<float>(buff, 8, 3.2f) == 12);
		CHECK(Cereal::Writer::writeBytes<long long>(buff, 12, 0x123456789ABCDEF0) == 20);
		CHECK(Cereal::Writer::writeBytes<double>(buff, 20, 3.141592) == 28);
		CHECK(Cereal::Writer::writeBytes<std::string>(buff, 28, "string") == 36);

		CHECK(buff[0] == (byte)true);
		CHECK(buff[1] == 'A');
		CHECK(buff[2] == 0x01);
		CHECK(buff[3] == 0x02);
		CHECK(buff[4] == 0x12);
		CHECK(buff[5] == 0x34);
		CHECK(buff[6] == 0x56);
		CHECK(buff[7] == 0x78);
		CHECK(buff[8] == 0x40);
		CHECK(buff[9] == 0x4C);
		CHECK(buff[10] == 0xCC);
		CHECK(buff[11] == 0xCD);
		CHECK(buff[12] == 0x12);
		CHECK(buff[13] == 0x34);
		CHECK(buff[14] == 0x56);
		CHECK(buff[15] == 0x78);
		CHECK(buff[16] == 0x9A);
		CHECK(buff[17] == 0xBC);
		CHECK(buff[18] == 0xDE);
		CHECK(buff[19] == 0xF0);
		CHECK(buff[20] == 0x40);
		CHECK(buff[21] == 0x09);
		CHECK(buff[22] == 0x21);
		CHECK(buff[23] == 0xFA);
		CHECK(buff[24] == 0xFC);
		CHECK(buff[25] == 0x8B);
		CHECK(buff[26] == 0x00);
		CHECK(buff[27] == 0x7A);
		CHECK(buff[28] == 0x00);
		CHECK(buff[29] == 0x06);
		CHECK(buff[30] == 's');
		CHECK(buff[31] == 't');
		CHECK(buff[32] == 'r');
		CHECK(buff[33] == 'i');
		CHECK(buff[34] == 'n');
		CHECK(buff[35] == 'g');

		delete[] buff;
	}

	SECTION("Reader")
	{
		byte data[36] =
		{
			0x01,  'A', 0x01, 0x02, 0x12, 0x34, 0x56, 0x78, 0x40, 0x4c,
			0xcc, 0xcd, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
			0x40, 0x09, 0x21, 0xfa, 0xfc, 0x8b, 0x00, 0x7a, 0x00, 0x06,
			 's',  't',  'r',  'i',  'n',  'g'
		};

		CHECK(Cereal::Reader::readBytes<bool>(data, 0) == true);
		CHECK(Cereal::Reader::readBytes<char>(data, 1) == 'A');
		CHECK(Cereal::Reader::readBytes<short>(data, 2) == 0x0102);
		CHECK(Cereal::Reader::readBytes<int>(data, 4) == 0x12345678);
		CHECK(Cereal::Reader::readBytes<float>(data, 8) == 3.2f);
		CHECK(Cereal::Reader::readBytes<long long>(data, 12) == 0x123456789ABCDEF0);
		CHECK(Cereal::Reader::readBytes<double>(data, 20) == 3.141592);
		CHECK(Cereal::Reader::readBytes<std::string>(data, 28) == "string");
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

TEST_CASE("Testing Cereal library", "[lib]")
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
