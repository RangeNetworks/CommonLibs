
#include "URLEncode.h"
#include <string>
#include <iostream>


using namespace std;


int main(int argc, char *argv[])
{

	string test = string("Testing: !@#$%^&*() " TIMESTAMP_ISO);
	cout << test << endl;
	cout << URLEncode(test) << endl;
}

