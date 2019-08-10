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

struct function_class
{
	void void_func(){}
};


TEST_CASE("Testing cpp signal 1") {
	
    using signals::signal;

	SUBCASE("signal 1 empty construction")
	{
		signal s1;
		
		//CHECK(!d1);
	}

	SUBCASE("signal 1 connect")
	{
        signal s1;

        s1.connect(&void_func);

        signals::detail::type_traits::is_bind_constructible<signal::functor, void*, void(*)(void*)>::value;

		void *ptr;
		function_class *fcl_ptr;
		s1.connect(ptr, &void_func_void_p);
	}
}