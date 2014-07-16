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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "Threads.h"
#include "Utils.h"
using namespace Utils;

// We must have a gConfig now to include BitVector.
#include "Configuration.h"
ConfigurationTable gConfig;

// pat 6-2014: Check how many threads we can create.

#define NUM_THREADS 100000

// Overwrite Thread::start() so we can see what is going on.

class ThreadTest {

	protected:

	pthread_t mThread;
	pthread_attr_t mAttrib;
	// FIXME -- Can this be reduced now?
	size_t mStackSize;
	

	public:

	/** Create a thread in a non-running state. */
	ThreadTest(size_t wStackSize = (65536*4)):mThread((pthread_t)0) {
		pthread_attr_init(&mAttrib);	// (pat) moved this here.
		mStackSize=wStackSize;
	}

	/**
		Destroy the Thread.
		It should be stopped and joined.
	*/
	// (pat) If the Thread is destroyed without being started, then mAttrib is undefined.  Oops.
	~ThreadTest() { pthread_attr_destroy(&mAttrib); }


	/** Start the thread on a task. */
	void start( void*(*task)(void*), void *arg) {
		assert(mThread==((pthread_t)0));
		int res;
		res = pthread_attr_setstacksize(&mAttrib, mStackSize);
		if (res) { fprintf(stderr,"pthread_setstacksize(%u) failed: error=%d %s\n",mStackSize,res,strerror(res)); }
		assert(!res);
		//res = pthread_create(&mThread, &mAttrib, &thread_main, p);
		res = pthread_create(&mThread, &mAttrib, task, arg);
		if (res) { LOG(ALERT) << "pthread_create failed, error:" <<strerror(res); }
		//if (res) { fprintf(stderr,"pthread_create failed: error=%d %s\n",res,strerror(res)); }
		assert(!res);
	}

	/** Join a thread that will stop on its own. */
	void join() { int s = pthread_join(mThread,NULL); assert(!s); mThread = 0; }

};

ThreadTest *threads[NUM_THREADS];

int outputs[NUM_THREADS];

void *serviceLoop(void *arg)
{
	int myid = *(int*)arg;
	outputs[myid]++;
	return NULL;
}


// The maximum number of threads is 32K, even though the RLIMIT_PROC is 65K.  Dont know why.


int main(int argc, char **argv)
{
	// Note: The Thread library sets the default stack size using pthread_attr_setstacksize
	memset(outputs,0,sizeof(outputs));

	// Check system limit.
	printf("System thread limit:"); fflush(stdout);
	system("cat /proc/sys/kernel/threads-max");

	// Check rlimit.
	struct rlimit limits;
	if (0 != getrlimit(RLIMIT_NPROC,&limits)) {
		perror("getrlimit failed");
		exit(2);
	}
	printf("RLIMIT_PROC = %lu / %lu\n",limits.rlim_cur,limits.rlim_max);

	double start = timef();
	double chunktime = timef();
	long stacksize = 65000;
	long long totalmem = 0;
	for (int i = 0; i < NUM_THREADS; i++) {
		// Minimum stack size is evidently 16K.
		// On my 8G system: With default stack size (65K*4) I can only create 7000 threads (2G)
		//threads[i] = new ThreadTest(17000);
		threads[i] = new ThreadTest(stacksize);
		totalmem += stacksize;
		threads[i]->start( (void*(*)(void*)) serviceLoop,&i);
		if (i && i % 1000 == 0) {
			printf("%d\n",i); fflush(stdout);
			printf("Created %d (%d total) threads in %.2g seconds mem=%lldM\n",1000,i,timef()-chunktime,totalmem/1000000);
			chunktime = timef();
		}
	}

	double elapsed = timef() - start;
	printf("Created %d threads in %.2g seconds\n",NUM_THREADS,elapsed);
}
