#include "CppUnitTest.h"

#include "Cereal.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(UnitTest1)
	{
	public:
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
	};
}