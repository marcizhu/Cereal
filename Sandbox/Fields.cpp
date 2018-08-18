#include <Cereal.h>

#include <gtest/gtest.h>
#include <random>

typedef unsigned char byte;

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
	Cereal::Field* fdouble = new Cereal::Field("field double", (double)3.14159265);
	Cereal::Field* fstring = new Cereal::Field("field string", std::string("test string"));

	EXPECT_STREQ(fbool->getName().c_str(), "field bool");
	EXPECT_STREQ(fchar->getName().c_str(), "field char");
	EXPECT_STREQ(fshort->getName().c_str(), "field short");
	EXPECT_STREQ(fint->getName().c_str(), "field int");
	EXPECT_STREQ(ffloat->getName().c_str(), "field float");
	EXPECT_STREQ(flonglong->getName().c_str(), "field long long");
	EXPECT_STREQ(fdouble->getName().c_str(), "field double");
	EXPECT_STREQ(fstring->getName().c_str(), "field string");

	EXPECT_EQ(fbool->getDataType(), Cereal::DataType::DATA_BOOL);
	EXPECT_EQ(fchar->getDataType(), Cereal::DataType::DATA_CHAR);
	EXPECT_EQ(fshort->getDataType(), Cereal::DataType::DATA_SHORT);
	EXPECT_EQ(fint->getDataType(), Cereal::DataType::DATA_INT);
	EXPECT_EQ(ffloat->getDataType(), Cereal::DataType::DATA_FLOAT);
	EXPECT_EQ(flonglong->getDataType(), Cereal::DataType::DATA_LONG_LONG);
	EXPECT_EQ(fdouble->getDataType(), Cereal::DataType::DATA_DOUBLE);
	EXPECT_EQ(fstring->getDataType(), Cereal::DataType::DATA_STRING);

	EXPECT_TRUE(fbool->getValue<bool>());
	EXPECT_EQ(fchar->getValue<char>(), r0);
	EXPECT_EQ(fshort->getValue<short>(), r1);
	EXPECT_EQ(fint->getValue<int>(), r2);
	EXPECT_FLOAT_EQ(ffloat->getValue<float>(), r3);
	EXPECT_EQ(flonglong->getValue<long long>(), r4);
	EXPECT_DOUBLE_EQ(fdouble->getValue<double>(), 3.14159265);
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