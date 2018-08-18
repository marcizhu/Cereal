#include <Cereal.h>

void write()
{
	Cereal::Header* myHeader = new Cereal::Header();
	Cereal::Database* db1 = new Cereal::Database("Test");
	Cereal::Database* db2 = new Cereal::Database("Second database");
	Cereal::Object* obj = new Cereal::Object("object");
	Cereal::Object* second = new Cereal::Object("second object");
	Cereal::Field* myField = new Cereal::Field("field", 3.141592f);
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

	myHeader->write(buffer);
	buffer.writeFile("test-out.db");

	delete myHeader;
}

void read()
{
	Cereal::Buffer buffer2(0);
	Cereal::Header* header = new Cereal::Header;

	buffer2.readFile("test-out.db");

	header->read(buffer2);

	Cereal::Database* db1 = header->getDatabase("Test");
	Cereal::Database* db2 = header->getDatabase("Second database");
	Cereal::Object* obj1 = db1->getObject("object");
	Cereal::Object* obj2 = db2->getObject("second object");
	Cereal::Field* f1 = obj1->getField("field");
	Cereal::Field* f2 = obj1->getField("other");
	Cereal::Array* arr2 = obj2->getArray("arr2");

	printf("Pi is: %f\n",f1->getValue<float>());

	printf("%s\n", f2->getValue<std::string>().c_str());
	printf("%s\n", arr2->getArray<std::string>()[0].c_str());
	printf("%s\n", arr2->getArray<std::string>()[1].c_str());

	delete header;
}

int main()
{
	write();
	read();

	return 0;
}