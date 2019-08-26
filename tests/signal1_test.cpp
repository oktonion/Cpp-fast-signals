#include "./testsuit/testsuit.h"

#include "..//signals/signal.hpp"


bool func_called = false;

void void_func_int(int)
{
    func_called = true;
}

void void_func_void_p(void*)
{
    func_called = true;
}

void void_func_int_p(int*)
{
    func_called = true;
}

struct function_class
{
	void void_func(){}
};


TEST_CASE("Testing cpp signal 1") {
	
    typedef signals::signal<> signal;

	SUBCASE("signal 1 empty construction")
	{
		signal s1;
		
		CHECK(!s1);
		CHECK((s1 == NULL));
		CHECK_FALSE((s1  != NULL));
		CHECK(s1.empty() == true);
		CHECK_FALSE(s1);
	}

	SUBCASE("signal 1 comparison")
	{
		signal s1, ss1;
		
		CHECK((s1 == s1));
		CHECK((s1 == ss1));
		CHECK((ss1 == s1));
		
		CHECK_FALSE((s1 != s1));
		CHECK_FALSE((s1 != ss1));
		CHECK_FALSE((ss1 != s1));

		CHECK_FALSE((s1 < s1));
		CHECK_FALSE((s1 < ss1));
		CHECK_FALSE((ss1 < s1));

		CHECK_FALSE((s1 > s1));
		CHECK_FALSE((s1 > ss1));
		CHECK_FALSE((ss1 > s1));
	}

	SUBCASE("signal 1 connect and disconnect")
	{
        signal s1;

		CHECK(s1.empty() == true);
        s1.connect(&void_func);
		CHECK(s1.empty() == false);
		CHECK(s1.disconnect(&void_func) == true);
		CHECK(s1.empty() == true);

		void *ptr;
		function_class *fcl_ptr;

		signal::value_type
			d1(ptr, &void_func_void_p),
			dd1(ptr, &void_func_void_p),
			ddd1(d1);

		CHECK(d1 == dd1);
		CHECK(dd1 == d1);
		CHECK(d1 == ddd1);
		CHECK(ddd1 == d1);
		CHECK(dd1 == ddd1);
		CHECK(ddd1 == dd1);

		CHECK(s1.empty() == true);
		s1.connect(d1);
		CHECK(s1.empty() == false);
		CHECK(s1.disconnect(d1) == true);
		CHECK(s1.empty() == true);

		CHECK(s1.empty() == true);
		s1.connect(d1);
		CHECK(s1.empty() == false);
		CHECK(s1.disconnect(dd1) == true);
		CHECK(s1.empty() == true);

		CHECK(s1.empty() == true);
		s1.connect(d1);
		CHECK(s1.empty() == false);
		CHECK(s1.disconnect(ddd1) == true);
		CHECK(s1.empty() == true);
        
		CHECK(s1.empty() == true);
		s1.connect(ptr, &void_func_void_p);
		CHECK(s1.empty() == false);
		CHECK(s1.disconnect(ptr, &void_func_void_p) == true);
		CHECK(s1.empty() == true);
	}

	SUBCASE("signal 1 emit")
	{
		signal s1;

		int *ptr;
		
		func_called = false;

		CHECK(s1.empty() == true);
        s1.connect(ptr, &void_func_int_p);
		CHECK(s1.empty() == false);

		s1.emit();

		CHECK(func_called == true);

		func_called = false;

		s1();

		CHECK(func_called == true);
	}
}