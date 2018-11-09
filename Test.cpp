/*#define CATCH_CONFIG_MAIN  

#include "catch.hpp"

#include "Field.hpp"

using namespace arithmetic;

#define TEST_LIMBSIZE 4

TEST_CASE("Bignum test", "[bignum]")
{
	
	SECTION("check big constructors")
	{
		std::initializer_list<uint_default_t> a_list =
		{ 0x2370fb049d410fbe, 0x4e761a9886e50241, 0x7d023f4018000001, 0x7e80600000000001 };

		std::string a_str = "2370fb049d410fbe4e761a9886e502417d023f40180000017e80600000000001";

		bignum<TEST_LIMBSIZE> a_from_list(a_list);
		bignum<TEST_LIMBSIZE> a_from_str(a_str);
		REQUIRE(a_from_list == a_from_str);
	}

	SECTION("check arithmetic operations")
	{
		v.resize(0);

		REQUIRE(v.size() == 0);
		REQUIRE(v.capacity() >= 5);
	}

	SECTION("check output")
	{
		v.reserve(10);

		REQUIRE(v.size() == 5);
		REQUIRE(v.capacity() >= 10);
	}
	SECTION("reserving smaller does not change size or capacity") {
		v.reserve(0);

		REQUIRE(v.size() == 5);
		REQUIRE(v.capacity() >= 5);
	}
}


TEST_CASE("Field test", "[ff]")
{

	SECTION("check field elem constructors")
	{
		std::initializer_list<uint_default_t> a_list =
		{ 0x2370fb049d410fbe, 0x4e761a9886e50241, 0x7d023f4018000001, 0x7e80600000000001 };

		std::string a_str = "2370fb049d410fbe4e761a9886e502417d023f40180000017e80600000000001";

		bignum<TEST_LIMBSIZE> a_from_list(a_list);
		bignum<TEST_LIMBSIZE> a_from_str(a_str);
		REQUIRE(a_from_list == a_from_str);
	}

	SECTION("check arithmetic operations")
	{
		v.resize(0);

		REQUIRE(v.size() == 0);
		REQUIRE(v.capacity() >= 5);
	}

	SECTION("check output")
	{
		v.reserve(10);

		REQUIRE(v.size() == 5);
		REQUIRE(v.capacity() >= 10);
	}

	SECTION("check inversion") {
		v.reserve(0);

		REQUIRE(v.size() == 5);
		REQUIRE(v.capacity() >= 5);
	}

	SECTION("check reduction logic")
	{

	}

}*/