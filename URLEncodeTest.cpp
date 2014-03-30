
#include "URLEncode.h"
#include <string>
#include <iostream>


int main(int argc, char *argv[])
{

	std::string test = std::string("Testing: !@#$%^&*() " __DATE__ " " __TIME__);
	std::cout << test << std::endl;
	std::cout << URLEncode(test) << std::endl;
}

