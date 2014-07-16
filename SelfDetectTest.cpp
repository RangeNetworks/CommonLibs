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
#include "SelfDetect.h"
#include "Configuration.h"
#include "Logger.h"

ConfigurationTable gConfig;

// For best effects, run this program thrice: once with a pre-created
// /var/run/argv.pid, and once without.  First one should exit with an
// error. Second should run this test.  Third, with an argument, then type
// Control-C.
int main(int argc, char *argv[])
{
    int ret;
    gLogInit("SelfDetectTest","DEBUG",LOG_LOCAL7);
    gSelf.RegisterProgram(argv[0]);

    ret = system("touch /tmp/foo.1");
    ret = system("touch /tmp/foo.2");
    ret = system("touch /tmp/foo.3");
    ret = system("touch /tmp/foo.4");
    ret = system("touch /tmp/foo.5");
    ret = system("touch /tmp/foo.6");
    ret = system("touch /tmp/foo.7");
    if (ret < 0) { ret = ret; } // warning eater

    gSelf.RegisterFile("/tmp/foo.0"); // ignored
    gSelf.RegisterFile("/tmp/foo.1"); // removed
    gSelf.RegisterFile("/tmp/foo.2"); // removed
    gSelf.RegisterFile("/tmp/foo.3"); // removed
    gSelf.RegisterFile("/tmp/foo.4"); // removed
    gSelf.RegisterFile("/tmp/foo.5"); // removed
    gSelf.RegisterFile("/tmp/foo.6"); // removed
    gSelf.RegisterFile("/tmp/foo.7"); // removed
    gSelf.RegisterFile("/tmp/foo.8"); // ignored
    gSelf.RegisterFile("/tmp/foo.9"); // ignored

    if (argv[1] != NULL)
    {
    	printf("Use a Control-C to test in this mode, make sure\n");
	printf("that the file goes away\n");
	while(1)
	    sleep(60);
    }

    return 0;
}
