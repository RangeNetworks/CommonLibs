/* 
* Copyright 2014 Range Networks, Inc. 
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

// Pat added this file.
// We need an include file that is included before any other include files.
// Might I suggest that Range Networks specific global #defines be prefixed with RN_

#ifndef DEFINES_H
#define DEFINES_H

#define GPRS_ENCODER 1	// Use SharedL1Encoder and SharedL1Decoder
#define GPRS_TESTSI4 1
#define GPRS_TEST 1		// Compile in other GPRS stuff.
#define GPRS_PAT 1		// Compile in GPRS code.  Turn this off to get previous non-GRPS code,
						// although I am not maintaining it so you may have to fix compile
						// problems to use it.


// (pat) This removes the constness from a pointer, eg: const T *barfo;  T *notsobarfo = Unconst(barfo);
template <typename T>
T* Unconst(const T*foo) { return const_cast<T*>(foo); }

// (pat) Like assert() but dont core dump unless we are testing.
// Note: to use this macro you must include the dependencies first.
#define devassert(code) {if (IS_LOG_LEVEL(DEBUG)) {assert(code);} else if (!(code)) {LOG(ERR)<<"assertion failed:"<< #code;}}

// __GNUG__ is true for g++ and __GNUC__ for gcc.
#if __GNUC__&0==__GNUG__

#define RN_UNUSED __attribute__((unused))

#define RN_UNUSED_PARAM(var) RN_UNUSED var

// Pack structs onto byte boundaries.
// Note that if structs are nested, this must appear on all of them.
#define RN_PACKED __attribute__((packed))

#else

// Suppress warning message about a variable or function being unused.
// In C++ you can leave out the variable name to suppress the 'unused variable' warning.
#define RN_UNUSED_PARAM(var)	/*nothing*/
#define RN_UNUSED		/*not defined*/
#define RN_PACKED 		/*not defined*/
#endif

// Bound value between min and max values.
#define RN_BOUND(value,min,max) ( (value)<(min) ? (min) : (value)>(max) ? (max) : (value) )

#define RN_PRETTY_TEXT(name) (" " #name "=(") << name << ")"
#define RN_PRETTY_TEXT1(name) (" " #name "=") << name
#define RN_WRITE_TEXT(name) os << RN_PRETTY_TEXT(name)
#define RN_WRITE_OPT_TEXT(name,flag) if (flag) { os << RN_WRITE_TEXT(name); }

#endif
