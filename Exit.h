/*
* Copyright 2014 Range Networks, Inc.
* All Rights Reserved.
*
* This software is distributed under multiple licenses;
* see the COPYING file in the main directory for licensing
* information for this specific distribution.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef EXIT_H
#define EXIT_H

#include <list>
#include <sys/types.h>
#include <signal.h>

#include "Threads.h"

/** A C++ wrapper for managing exit codes.  Note that this is a helper class
 * without implementation.
 */
class Exit
{
public:
    enum eCodes
    {
    	SUCCESS = 0,	// no error
	DETECTFILE,	// Unable to detect a required file: CommonLibs SelfDetect
	CREATEFILE,	// Unable to create a required file: CommonLibs SelfDetect

	// Put all error codes above this line, to prevent error number values
	// from changing, put after the previous enum value (ie, preserve
	// the order.  Also, provide a comment indicating which program(s) and
	// the purpose.

	LIMIT = 256	// all error codes must be less than this
    };
private:
    inline Exit(void) { /* cannot be constructed */; };
    inline ~Exit(void) { /* cannot be destructed */; };
public:
    inline static void exit(int status)
    {
	if (status < SUCCESS || status >= LIMIT)
	{
	    printf("INFO: Unsupported exit status of %d\n", status);
	}
	::exit(status);
	/*NOTREACHED*/
    }
};
#endif // EXIT_H
// vim: ts=4 sw=4
