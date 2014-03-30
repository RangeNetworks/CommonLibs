/*
* Copyright 2008 Free Software Foundation, Inc.
*
*
* This software is distributed under the terms of the GNU Affero Public License.
* See the COPYING file in the main directory for details.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#define ENABLE_VECTORDEBUG

#include "Vector.h"
#include <iostream>

// We must have a gConfig now to include Vector.
#include "Configuration.h"
ConfigurationTable gConfig;

typedef Vector<int> TestVector;
int barfo;
void foo(TestVector a)
{
	barfo = a.size();	// Do something so foo wont be optimized out.
}
void anotherTest()
{
	std::cout << "START Vector anotherTest" << std::endl;
	TestVector v0(10);
	TestVector atest = v0.head(3);
	std::cout << atest << std::endl;
	std::cout << "calling head" << std::endl;
	std::cout << v0.head(3) << std::endl;
	std::cout << "Passing Vector" << std::endl;
	// This calls the Vector non-const copy constructor
	foo(v0);
	std::cout << "FINISH anotherTest" << std::endl;
}

int main(int argc, char *argv[])
{
	anotherTest();
	TestVector test1(5);
	for (int i=0; i<5; i++) test1[i]=i;
	TestVector test2(5);
	for (int i=0; i<5; i++) test2[i]=10+i;

	std::cout << test1 << std::endl;
	std::cout << test2 << std::endl;

	{
		TestVector testC(test1,test2);
		std::cout << testC << std::endl;

		TestVector foo = testC.head(3);
		//std::cout << testC.head(3) << std::endl;
		std::cout << testC.tail(3) << std::endl;
		testC.fill(8);
		std::cout << testC << std::endl;
		test1.copyToSegment(testC,3);
		std::cout << testC << std::endl;

		TestVector testD(testC.segment(4,3));
		std::cout << testD << std::endl;
		testD.fill(9);
		std::cout << testC << std::endl;
		std::cout << testD << std::endl;
	}

	return 0;
}
