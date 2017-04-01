// Sandbox.cpp : Defines the entry point for the console application.
//

#include <Cereal.h>
#include <Windows.h>

#include "ptest.h"

/*
TEST_METHOD(WriteTest32)
{
Cereal::Buffer buffer(1000);

Cereal::Header* myHeader = new Cereal::Header();
Cereal::Database* db1 = new Cereal::Database("Test"); //
Cereal::Database* db2 = new Cereal::Database("Second database"); //
Cereal::Object* obj = new Cereal::Object("object"); //
Cereal::Object* second = new Cereal::Object("second object"); //
Cereal::Field* myField = new Cereal::Field("field", 3.14f); //
Cereal::Field* other = new Cereal::Field("other", std::string("test string")); //
Cereal::Array* arr = new Cereal::Array("array", new int[4]{ 1, 2, 3, 4 }, 4); //
Cereal::Array* str = new Cereal::Array("arr2", new std::string[2]{ "t1", "t2" }, 2); //

obj->addArray(arr);
obj->addField(myField);
obj->addField(other);

second->addArray(str);

db1->addObject(obj);
db2->addObject(second);

myHeader->addDatabase(db1);
myHeader->addDatabase(db2);
myHeader->write(buffer);
buffer.shrink();
buffer.writeFile("C:\\Users\\marci\\Documents\\Visual Studio 2015\\Projects\\Cereal\\Debug\\test-out.db");

Assert::AreEqual((size_t)2, obj->getFields().size());
Assert::AreEqual((size_t)1, obj->getArrays().size());
Assert::AreEqual((size_t)1, second->getArrays().size());
Assert::AreEqual((size_t)1, db1->getObjects().size());
Assert::AreEqual((size_t)1, db2->getObjects().size());
Assert::AreEqual((size_t)2, myHeader->getDatabases().size());

delete myHeader;
}

TEST_METHOD(WriteTest64)
{
// Implement when Cereal-64 is added
}

TEST_METHOD(ReadTest32)
{
Cereal::Buffer buffer(0);
bool ret = buffer.readFile("C:\\Users\\marci\\Documents\\Visual Studio 2015\\Projects\\Cereal\\Debug\\test-out.db");
Assert::IsTrue(ret);

Cereal::Header* header = new Cereal::Header;
header->read(buffer);

Cereal::Database* db1 = header->getDatabase("Test");
Cereal::Database* db2 = header->getDatabase("Second database");
Cereal::Object* obj1 = db1->getObject("object");
Cereal::Object* obj2 = db2->getObject("second object");
Cereal::Field* f1 = obj1->getField("field");
Cereal::Field* f2 = obj1->getField("other");
Cereal::Array* arr1 = obj1->getArray("array");
Cereal::Array* arr2 = obj2->getArray("arr2");

Assert::IsNotNull(db1);
Assert::IsNotNull(db2);
Assert::IsNotNull(obj1);
Assert::IsNotNull(obj2);
Assert::IsNotNull(f1);
Assert::IsNotNull(f2);
Assert::IsNotNull(arr1);
Assert::IsNotNull(arr2);

Assert::AreEqual((size_t)2, header->getDatabases().size());
Assert::AreEqual((size_t)1, db1->getObjects().size());
Assert::AreEqual((size_t)1, db2->getObjects().size());
Assert::AreEqual((size_t)2, obj1->getFields().size());
Assert::AreEqual((size_t)1, obj1->getArrays().size());
Assert::AreEqual((size_t)0, obj2->getFields().size());
Assert::AreEqual((size_t)1, obj2->getArrays().size());
Assert::AreEqual(3.14f, f1->getValue<float>());
Assert::AreEqual(std::string("test string"), f2->getValue<std::string>());
Assert::AreEqual((unsigned int)4, arr1->getCount());
Assert::AreEqual((unsigned int)2, arr2->getCount());
Assert::AreEqual(3, arr1->getArray<int>()[2]);
Assert::AreEqual(std::string("t1"), arr2->getArray<std::string>()[0]);
Assert::AreEqual(std::string("t2"), arr2->getArray<std::string>()[1]);

delete header;
}

TEST_METHOD(ReadTest64)
{
// Implement when Cereal-64 is added
}
*/

void testCereal64()
{
	// uninplemented
}

void testRead()
{
	Cereal::Buffer buffer(0);
	bool ret = buffer.readFile("test-out.db");
	PT_ASSERT(ret == true);

	Cereal::Header* header = new Cereal::Header;
	header->read(buffer);

	Cereal::Database* db1 = header->getDatabase("Test");
	Cereal::Database* db2 = header->getDatabase("Second database");
	Cereal::Object* obj1 = db1->getObject("object");
	Cereal::Object* obj2 = db2->getObject("second object");
	Cereal::Field* f1 = obj1->getField("field");
	Cereal::Field* f2 = obj1->getField("other");
	Cereal::Array* arr1 = obj1->getArray("array");
	Cereal::Array* arr2 = obj2->getArray("arr2");

	PT_ASSERT(db1 != nullptr);
	PT_ASSERT(db2 != nullptr);
	PT_ASSERT(obj1 != nullptr);
	PT_ASSERT(obj2 != nullptr);
	PT_ASSERT(f1 != nullptr);
	PT_ASSERT(f2 != nullptr);
	PT_ASSERT(arr1 != nullptr);
	PT_ASSERT(arr2 != nullptr);

	PT_ASSERT((size_t)2 == header->getDatabases().size());
	PT_ASSERT((size_t)1 == db1->getObjects().size());
	PT_ASSERT((size_t)1 == db2->getObjects().size());
	PT_ASSERT((size_t)2 == obj1->getFields().size());
	PT_ASSERT((size_t)1 == obj1->getArrays().size());
	PT_ASSERT((size_t)0 == obj2->getFields().size());
	PT_ASSERT((size_t)1 == obj2->getArrays().size());
	PT_ASSERT(3.14f == f1->getValue<float>());
	PT_ASSERT((unsigned int)4 == arr1->getCount());
	PT_ASSERT((unsigned int)2 == arr2->getCount());
	PT_ASSERT(3 == arr1->getArray<int>()[2]);
	PT_ASSERT_STR_EQ(std::string("test string").c_str(), f2->getValue<std::string>().c_str());
	PT_ASSERT_STR_EQ(std::string("t1").c_str(), arr2->getArray<std::string>()[0].c_str());
	PT_ASSERT_STR_EQ(std::string("t2").c_str(), arr2->getArray<std::string>()[1].c_str());

	delete header;
}

void testWrite()
{
	Cereal::Buffer buffer(1000);

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
	myHeader->write(buffer);
	buffer.shrink();
	buffer.writeFile("test-out.db");

	PT_ASSERT((size_t)2 == obj->getFields().size());
	PT_ASSERT((size_t)1 == obj->getArrays().size());
	PT_ASSERT((size_t)1 == second->getArrays().size());
	PT_ASSERT((size_t)1 == db1->getObjects().size());
	PT_ASSERT((size_t)1 == db2->getObjects().size());
	PT_ASSERT((size_t)2 == myHeader->getDatabases().size());

	delete myHeader;
}

void testHeaders()
{
	Cereal::Database* db = new Cereal::Database("test");
	Cereal::Header* header = new Cereal::Header;

	header->addDatabase(db);

	PT_ASSERT(header->getDatabase("test") != nullptr);
	PT_ASSERT(header->getDatabases().size() == 1);

	delete header;
}

void testDatabases()
{
	Cereal::Object* obj = new Cereal::Object("obj name");
	Cereal::Database* db = new Cereal::Database("db name");

	db->addObject(obj);

	PT_ASSERT_STR_EQ(db->getName().c_str(), "db name");
	PT_ASSERT(db->getObject("obj name") != nullptr);
	PT_ASSERT(db->getObjects().size() == 1);

	delete db;
}

void testObjects()
{
	Cereal::Field* f = new Cereal::Field("f", 3);
	Cereal::Array* a = new Cereal::Array("a", new int[2]{ 1, 2 }, 2);
	Cereal::Object* obj = new Cereal::Object("object");

	obj->addArray(a);
	obj->addField(f);

	PT_ASSERT_STR_EQ(obj->getName().c_str(), "object");
	PT_ASSERT(obj->getArrays().size() == 1);
	PT_ASSERT(obj->getFields().size() == 1);
	PT_ASSERT(obj->getField("f") != nullptr);
	PT_ASSERT(obj->getArray("a") != nullptr);

	delete obj;
}

void testArrays()
{
	Cereal::Array* a = new Cereal::Array("array name", new int[4]{ 1, 2, 3, 4 }, 4);
	Cereal::Array* a2 = new Cereal::Array("second", new std::string[2]{ "another string", "string no 2" }, 2);
	int* ret = a->getRawArray(new int[a->getCount()]);

	PT_ASSERT_STR_EQ(a->getName().c_str(), "array name");
	PT_ASSERT_STR_EQ(a2->getName().c_str(), "second");
	PT_ASSERT_STR_EQ(a2->getArray<std::string>()[0].c_str(), "another string");
	PT_ASSERT_STR_EQ(a2->getArray<std::string>()[1].c_str(), "string no 2");

	PT_ASSERT(a->getCount() == 4);
	PT_ASSERT(ret[0] == 1);
	PT_ASSERT(ret[1] == 2);
	PT_ASSERT(ret[2] == 3);
	PT_ASSERT(ret[3] == 4);
	PT_ASSERT(a->getArray<int>().size() == a->getCount());

	delete a;
	delete a2;
	delete ret;
}

void testFields()
{
	Cereal::Field* fbool = new Cereal::Field("field bool", (bool)true);
	Cereal::Field* fchar = new Cereal::Field("field char", (char)'M');
	Cereal::Field* fshort = new Cereal::Field("field short", (short)9000);
	Cereal::Field* fint = new Cereal::Field("field int", (int)42);
	Cereal::Field* ffloat = new Cereal::Field("field float", 3.2f);
	Cereal::Field* flonglong = new Cereal::Field("field long long", (long long)0x123456789ABCDEF0);
	Cereal::Field* fdouble = new Cereal::Field("field double", (double)3.141592);
	Cereal::Field* fstring = new Cereal::Field("field string", std::string("test string"));

	PT_ASSERT_STR_EQ(fbool->getName().c_str(), "field bool");
	PT_ASSERT_STR_EQ(fchar->getName().c_str(), "field char");
	PT_ASSERT_STR_EQ(fshort->getName().c_str(), "field short");
	PT_ASSERT_STR_EQ(fint->getName().c_str(), "field int");
	PT_ASSERT_STR_EQ(ffloat->getName().c_str(), "field float");
	PT_ASSERT_STR_EQ(flonglong->getName().c_str(), "field long long");
	PT_ASSERT_STR_EQ(fdouble->getName().c_str(), "field double");
	PT_ASSERT_STR_EQ(fstring->getName().c_str(), "field string");

	PT_ASSERT(fbool->getValue<bool>() == true);
	PT_ASSERT(fchar->getValue<char>() == 'M');
	PT_ASSERT(fshort->getValue<short>() == 9000);
	PT_ASSERT(fint->getValue<int>() == 42);
	PT_ASSERT(ffloat->getValue<float>() == 3.2f);
	PT_ASSERT(flonglong->getValue<long long>() == 0x123456789ABCDEF0);
	PT_ASSERT(fdouble->getValue<double>() == 3.141592);
	PT_ASSERT_STR_EQ(fstring->getValue<std::string>().c_str(), "test string");

	delete fbool;
	delete fchar;
	delete fshort;
	delete fint;
	delete ffloat;
	delete flonglong;
	delete fdouble;
	delete fstring;
}

void testUnits()
{
	pt_add_test(testFields, "Test fields", "Test units");
	pt_add_test(testArrays, "Test arrays", "Test units");
	pt_add_test(testObjects, "Test objects", "Test units");
	pt_add_test(testDatabases, "Test objects", "Test units");
	pt_add_test(testHeaders, "Test objects", "Test units");
}

void testLib()
{
	pt_add_test(testWrite, "Test writing", "Test library");
	pt_add_test(testRead, "Test reading", "Test library");
}

int main()
{
	pt_add_suite(testUnits);
	pt_add_suite(testLib);

	pt_run();

	while (1) { Sleep(1000); }

    return 0;
}