/*
* Copyright 2014 Range Networks, Inc.
* All Rights Reserved.
*
* This software is distributed under multiple licenses;
* see the COPYING file in the main directory for licensing
* information for this specific distribuion.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef UNIXSIGNAL_H
#define UNIXSIGNAL_H

#include <list>
#include <sys/types.h>
#include <signal.h>

#include "Threads.h"

/** A C++ wrapper for managing unix signals. */
class UnixSignal
{
public:
    static const int C_NSIG = 64;
private:
    std::list<sighandler_t>  mListHandlers[C_NSIG]; // list of signal handlers to call for all signals
    Mutex mLock[C_NSIG];
    bool mAddPid;				    // if true file is file.pid
    std::string mCoreFile;			    // name of file to save to
public:
    UnixSignal(void);
    ~UnixSignal(void);

    void Register(sighandler_t handler, int sig); // register a signal handler with the system
    void Handler(int sig);    // main signal handler, iterates through mListHandlers
    void Dump(void); // debug dump of list
    inline void CoreName(const std::string &coreFile, bool addPid) { mCoreFile = coreFile; mAddPid = addPid; }
};

extern UnixSignal gSigVec;
#endif // UNIXSIGNAL_H
// vim: ts=4 sw=4
