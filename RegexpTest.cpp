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



#include "Regexp.h"
#include <iostream>


int main(int argc, char *argv[])
{

	Regexp email("^[[:graph:]]+@[[:graph:]]+ ");
	Regexp simple("^dburgess@");

	const char text1[] = "dburgess@jcis.net test message";
	const char text2[] = "no address text message";

	std::cout << email.match(text1) << " " << text1 << std::endl;
	std::cout << email.match(text2) << " " << text2 << std::endl;

	std::cout << simple.match(text1) << " " << text1 << std::endl;
	std::cout << simple.match(text2) << " " << text2 << std::endl;
}
