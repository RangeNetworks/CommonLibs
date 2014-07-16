/**@file Module for preventing two instances of a program from running. */
/*
* Copyright 2013, 2014 Range Networks, Inc.
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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "UnixSignal.h"
#include "SelfDetect.h"
#include "Logger.h"
#include "Exit.h"
#include "Configuration.h"

//SelfDetect gSelf;

extern ConfigurationTable gConfig;

static void e(void)
{
    gSelf.Exit(-999);
}

static void sigfcn(int sig)
{
    gSelf.Exit(sig);
}

void SelfDetect::RegisterProgram(const char *argv0)
{
    const char *p = strrchr((char*)argv0,'/');
	if (p == NULL) { p = argv0; }

    char buf[100];
    snprintf(buf, sizeof(buf)-1, "/var/run/%s.pid", p);
    LOG(NOTICE) << "*** Registering program " << argv0 << " to " << buf;

    // first, verify we aren't already running.
    struct stat stbuf;
    if (stat(buf, &stbuf) >= 0)
    {
	LOG(CRIT) << "*** An instance of " << p << " is already running. ";
	LOG(CRIT) << "*** If this is not the case, deleting this file will allow " << p << " to start: " << buf << " exiting...";
	Exit::exit(Exit::DETECTFILE);
    }
    
    FILE *fp = fopen(buf, "w");
    if (fp == NULL)
    {
    	LOG(CRIT) << "*** Unable to create " << buf << ": " << strerror(errno) << " exiting...";
		Exit::exit(Exit::CREATEFILE);
    }
    fprintf(fp, "%d\n", getpid());
    fclose(fp);
    atexit(e);
	gSigVec.CoreName(gConfig.getStr("Core.File"), gConfig.getBool("Core.Pid"));
	gSigVec.TarName(gConfig.getStr("Core.TarFile"), gConfig.getBool("Core.SaveFiles"));

    // Now, register for all signals to do the cleanup
    for (int i = 1; i < UnixSignal::C_NSIG; i++)
    {
		switch(i)
		{
			// Add any signals that need to bypass the signal handling behavior
			// here.  Currently, SIGCHLD is needed because a signal is generated
			// when stuff related to the transciever (which is a child process)
			// occurs.  In that case, the openbts log output was:
			//		openbts: ALERT 3073816320 05:03:50.4 OpenBTS.cpp:491:main: starting the transceiver
			//		openbts: NOTICE 3073816320 05:03:50.4 SelfDetect.cpp:91:Exit: *** Terminating because of signal 17
			//		openbts: NOTICE 3031243584 05:03:50.4 OpenBTS.cpp:165:startTransceiver: starting transceiver ./transceiver w/ 1 ARFCNs and Args:
			//		openbts: NOTICE 3073816320 05:03:50.4 SelfDetect.cpp:98:Exit: *** Terminating ./OpenBTS
			//		openbts: NOTICE 3073816320 05:03:50.4 SelfDetect.cpp:105:Exit: *** Removing pid file /var/run/OpenBTS.pid
			case SIGCONT:
			case SIGCHLD:
				break;
			default:
				gSigVec.Register(sigfcn, i);
				break;
		}
    }
    mProg = strdup(argv0);
    mFile = strdup(buf);
}

void SelfDetect::Exit(int sig)
{
    LOG(NOTICE) << "*** Terminating because of signal " << sig;

    if (mProg == NULL)
    {
	LOG(NOTICE) << "*** Terminating without registration of program";
    } else
    {
	LOG(NOTICE) << "*** Terminating " << mProg;
    }
    if (mFile == NULL)
    {
	LOG(NOTICE) << "*** Terminating without pid file";
    } else
    {
	LOG(NOTICE) << "*** Removing pid file " << mFile;
    	unlink(mFile);
    }
    for (std::list<std::string>::iterator i = mListFiles.begin();
	 i != mListFiles.end(); ++i)
    {
	LOG(NOTICE) << "*** Removing " << i->c_str();
    	unlink(i->c_str());
    }
}

void SelfDetect::RegisterFile(const char *file)
{
    LOG(NOTICE) << "*** Registering " << file << " for removal at program exit";
    std::string s(file);
    mListFiles.push_back(s);
}

