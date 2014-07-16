/*
* Copyright 2008 Free Software Foundation, Inc.
* Copyright 2014 Range Networks, Inc.
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


#include "BitVector.h"
#include <iostream>
#include <cstdlib>
#include <string.h>
 
using namespace std;

// We must have a gConfig now to include BitVector.
#include "Configuration.h"
ConfigurationTable gConfig;


void origTest()
{
	BitVector v0("0000111100111100101011110000");
	cout << v0 << endl;
	// (pat) The conversion from a string was inserting garbage into the result BitVector.
	// Fixed now so only 0 or 1 are inserted, but lets check:
	for (char *cp = v0.begin(); cp < v0.end(); cp++) cout << (int)*cp<<" ";
	cout << endl;

	BitVector v1(v0);
	v1.LSB8MSB();
	cout <<v1 << " (byte swapped)" << endl;

	// Test operator==
	assert(v1 == v1);
	cout <<"v0="<<v0 <<endl;
	cout <<"v1="<<v1 <<endl;
	assert(!(v0 == v1));

	BitVector v5("000011110000");
	int r1 = v5.peekField(0,8);
	int r2 = v5.peekField(4,4);
	int r3 = v5.peekField(4,8);
	cout << r1 <<  ' ' << r2 << ' ' << r3 << endl;
	cout << v5 << endl;
	v5.fillField(0,0xa,4);
	int r4 = v5.peekField(0,8);
	cout << v5 << endl;
	cout << r4 << endl;

	v5.reverse8();
	cout << v5 << endl;

	unsigned char ts[9] = "abcdefgh";
	BitVector tp(70);
	cout << "ts=" << ts << endl;
	tp.unpack(ts);
	cout << "tp=" << tp << endl;
	tp.pack(ts);
	cout << "ts=" << ts << endl;

	BitVector v6("010101");
	BitVector v7(3);
	unsigned punk[3] = {1,2,5};
	v6.copyPunctured(v7, punk, 3);
	cout << "v7=" << v7 << endl;
}


typedef BitVector TestVector;
int barfo;
void foo(TestVector a)
{
	barfo = a.size();	// Do something so foo wont be optimized out.
}
void anotherTest()
{
	cout << "START BitVector anotherTest" << endl;
	TestVector v0("0000111100111100101011110000");
	TestVector atest = v0.head(3);
	cout << atest << endl;
	cout << "Calling head" << endl;
	cout << v0.head(3) << endl;
	cout << "Passing BitVector" << endl;
	// This calls Vector const copy constructor.
	// That is because the implicitly declared copy constructor for BitVector is of the form (BitVector const&)
	foo(v0);
	const TestVector ctest("0000");
	cout << ctest.cloneSegment(0,2) << endl;
	cout << "after"<<endl;
	cout << "FINISH anotherTest" << endl;
}

BitVector randomBitVector(int n)
{
	BitVector t(n);
	for (int i = 0; i < n; i++) t[i] = random()%2;
	return t;
}

int main(int argc, char *argv[])
{
	anotherTest();
	origTest();
}
