#include "Field.hpp"

using namespace arithmetic;

#define TEST_LIMBSIZE 4

int main(int argc, char* argv[])
{
	std::initializer_list<uint_default_t> a_list =
	{ 0x2370fb049d410fbe, 0x4e761a9886e50241, 0x7d023f4018000001, 0x7e80600000000001 };

	std::string a_str = "2370fb049d410fbe4e761a9886e502417d023f40180000017e80600000000001";

	bignum<TEST_LIMBSIZE> a_from_list(a_list);
	bignum<TEST_LIMBSIZE> a_from_str(a_str);
	bool flag = (a_from_list == a_from_str);

	bignum<TEST_LIMBSIZE> a = a_from_list;

	std::initializer_list<uint_default_t> b_list =
	{ 0xc1d2a1e2bccf0c52, 0x799a533a0366b901, 0xa77df6a97da46ddc, 0x952d97f7a515ce82 };

	bignum<TEST_LIMBSIZE> b(b_list);

	//c = a + b
	bignum<TEST_LIMBSIZE> c("e5439ce75a101c10c8106dd28a4bbb43248035e995a46dde13adf7f7a515ce83");
	flag = (a + b == c);
	
	//d = a - b;
	bignum<TEST_LIMBSIZE> d("619e5921e072036bd4dbc75e837e493fd58448969a5b9224e952c8085aea317f");
	flag = (a - b == d);

	//e = a * b;
	bignum<TEST_LIMBSIZE * 2> 
		e("1ad55655d6162f9d349b4e6f1a711ce49b2680f9bdf2c311b56ad18464ea3de96838b8fa17ed2c2220c4abc1a7e40f115758f8064aca60d1ff9e57f7a515ce82");
	bignum<TEST_LIMBSIZE * 2> product(a * b);
	flag = (e == product);

	//divide by single word 
	bignum<1> f(0x9884d881a5d15350);
	auto first_res = (a / f);
	bignum<TEST_LIMBSIZE> first_div("3b7cd16dc948742f39c4cf1c6f82909d1f1042c30195f1de");
	bignum<1> first_rem(0x7b72285f577b70a1);
	flag = (first_res.first == first_div);
	flag = (first_res.second == first_rem);

	//divide by long word;
	std::initializer_list<uint_default_t> g_list =
		{ 0xbaf556405498149b, 0xf44aa2af656398d0 };
	std::initializer_list<uint_default_t> second_div_list = { 0x30878833aad93fed, 0x5be45072345c562d };
	std::initializer_list<uint_default_t> second_rem_list = { 0x8edc5decab396a48, 0x13bf23a68d684371 };
	bignum<2> g(g_list);
	auto second_res = (a / g);
	bignum<3> second_div(second_div_list);
	bignum<2> second_rem(second_rem_list);
	
		
	flag = (second_res.first == second_div);
	flag = (second_res.second == second_rem);



	return flag;
}