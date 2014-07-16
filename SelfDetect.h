/*
* Copyright 2013, 2014 Range Networks, Inc.
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

#ifndef SELFDETECT_H
#define SELFDETECT_H

#include <list>
#include <string>
#include <cstdlib>
#include <cstdio>

/** A C++ wrapper for preventing 2 instances of a program from running. */
class SelfDetect
{
private:
    const char             *mProg; // program name, which will map to /tmp/%s.pid
    const char             *mFile; // file in /tmp for pid
    std::list<std::string>  mListFiles; // list of files to be removed at shutdown
public:
    SelfDetect(void)
    {
    	mProg = NULL;
    	mFile = NULL;
	mListFiles.clear();
    }

    ~SelfDetect(void)
    {
	if (mProg) { free((void *)mProg); mProg = NULL; }
	if (mFile) { free((void *)mFile); mFile = NULL; }
	mListFiles.clear();
    }

    void RegisterProgram(const char *argv0); // register the program and validate
    void RegisterFile(const char *file); // register a file to be removed at shutdown / exit

    void Exit(int);		// atexit() registration function -- called internally by real atexit() function that isn't in a class
};

extern SelfDetect gSelf;
#endif // SELFDETECT_H
// vim: ts=4 sw=4
