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

#include <cstdlib>
#include "UnixSignal.h"
#include "Configuration.h"
#include "Logger.h"

ConfigurationTable gConfig;

void test1(int sig) { printf("Test 1, signal %d\n", sig); }
void test2(int sig) { printf("Test 2, signal %d\n", sig); }
void test3(int sig) { printf("Test 3, signal %d\n", sig); }
void test4(int sig) { printf("Test 4, signal %d\n", sig); }
void test5(int sig) { printf("Test 5, signal %d\n", sig); }
void test6(int sig) { printf("Test 6, signal %d\n", sig); }

void registerFuncs(int sig)
{
    gSigVec.Register(test1, sig);
    gSigVec.Register(test2, sig);
    gSigVec.Register(test3, sig);
    gSigVec.Register(test4, sig);
    gSigVec.Register(test5, sig);
    gSigVec.Register(test6, sig);
}

int main(int argc, char *argv[])
{
    gLogInit("UnixSignalTest","DEBUG",LOG_LOCAL7);
    registerFuncs(SIGINT);
    registerFuncs(SIGHUP);
    registerFuncs(SIGTERM);
    printf("Test1 @ %p\n", test1);
    printf("Test2 @ %p\n", test2);
    printf("Test3 @ %p\n", test3);
    printf("Test4 @ %p\n", test4);
    printf("Test5 @ %p\n", test5);
    printf("Test6 @ %p\n", test6);
    printf("\n");
    gSigVec.Dump();

    printf("Run this three times.  First time, do a 'kill %d' to test SIGTERM\n", getpid());
    printf("Second time, do a 'kill -%d %d' to test SIGHUP\n", SIGHUP, getpid());
    printf("Third time, do a Control-C to test SIGINT\n");
    printf("\n");
    printf("Output should iterate through tests 1-6 with the correct signal number\n");
    printf("%d for SIGTERM, %d for SIGHUP, %d for SIGINT\n",
    	SIGTERM, SIGHUP, SIGINT);
    while(true)
    	sleep(60);

    return 0;
}
