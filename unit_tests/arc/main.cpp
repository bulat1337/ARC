#include <gtest/gtest.h>  // for InitGoogleTest, RUN_ALL_TESTS

#include <exception>      // for exception
#include <iostream>       // for char_traits, basic_ostream, basic_ios, oper...

int main(int argc, char **argv)
{
	try
	{
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
	catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}
