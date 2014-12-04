#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <assert.h>
using namespace std;

#include "Configuration.h"
#include "Threads.h"

ConfigurationTable gConfig;

// LockTest starts three processes that lock and unlock three mutexes at random, to make sure no deadlock occurs.

struct aprocess {
	string id;
	Thread t;
	Mutex m;
	void lockall(int lockOwnerBits);
	void runtest();
	static void *pstart(void *v);
	void start1();
	aprocess(string wid) : id(wid) {}
};

aprocess a("a"), b("b"), c("c");

void aprocess::lockall(int lockOwnerBits) {
	if (id == "a") {
		ScopedLockMultiple lock(lockOwnerBits,a.m,b.m,c.m,__FILE__,__LINE__);
	} else if (id == "b") {
		ScopedLockMultiple lock(lockOwnerBits,b.m,a.m,c.m,__FILE__,__LINE__);
	} else if (id == "c") { 
		ScopedLockMultiple lock(lockOwnerBits,c.m,a.m,b.m,__FILE__,__LINE__);
	} else {
		assert(0);
	}
}

static void waitabit() {
	// randomly return instantly.
	if (random() & 1) return;
	usleep(0xff&random());
}

void aprocess::runtest() {
	for (int i = 0; i < 10000; i++) {
		waitabit();
		//printf("%s %d here\n",id.c_str(),i);
		lockall(0);
		waitabit();
		//printf("%s1:a=%d b=%d c=%d\n",id.c_str(),a.m.lockcnt(), b.m.lockcnt(), c.m.lockcnt());
		// Add in some random locking behavior.
		if (random() & 1) {
			m.lock(__FILE__,__LINE__);
			waitabit();
			m.unlock();
		}
		//printf("%s2:a=%d b=%d c=%d\n",id.c_str(),a.m.lockcnt(), b.m.lockcnt(), c.m.lockcnt());
		waitabit();
		//printf("%s %d there\n",id.c_str(),i);
		{ ScopedLock lock(m);
			lockall(1);
			waitabit();
			//printf("%s3:a=%d b=%d c=%d\n",id.c_str(),a.m.lockcnt(), b.m.lockcnt(), c.m.lockcnt());
		}
		//printf("%s4:a=%d b=%d c=%d\n",id.c_str(),a.m.lockcnt(), b.m.lockcnt(), c.m.lockcnt());
	}
	printf("finished\n");
}

void *aprocess::pstart(void *v) {	// This is the interface for the Thread.start() method.
	aprocess *p = (aprocess*)v;
	p->runtest();
	return 0;
}

void aprocess::start1() {
	this->t.start(&this->pstart,this);
}

typedef void *(*task_t)(void*);

int main(int argc, char **argv)
{
	// Start the three processes running.
	a.start1();
	b.start1();
	c.start1();

	// And let the main process fight for the locks as well.
	// We also do a coverage check here: When we randomly sample the locks, all of them must have been locked at some point.
	int fndA=0, fndB=0, fndC=0;
	for (int n = 0; n < 1000; n++) {
		waitabit();
		fndA += a.m.lockcnt();
		fndB += b.m.lockcnt();
		fndC += c.m.lockcnt();
		//printf("loop %d: a=%d b=%d c=%d\n",n, a.m.lockcnt(), b.m.lockcnt(), c.m.lockcnt());
		printf("loop %d: a=%s b=%s c=%s\n",n,a.m.mutext().c_str(), b.m.mutext().c_str(), c.m.mutext().c_str());

		a.m.lock(__FILE__,__LINE__);
		b.m.lock(__FILE__,__LINE__);
		{ ScopedLockMultiple tmp(3,a.m,b.m,c.m); waitabit(); }
		c.m.lock(__FILE__,__LINE__);

		waitabit();
		a.m.unlock();
		a.m.lock(__FILE__,__LINE__);

		waitabit();
		b.m.unlock();
		b.m.lock(__FILE__,__LINE__);

		waitabit();
		a.m.unlock();
		b.m.unlock();
		c.m.unlock();
	}
	//a.t.start(&a.pstart,&a);
	//b.t.start((void*)&b);
	//c.t.start((void*)&c);
	a.t.join();	// Wait for it to finish.
	b.t.join();	// Wait for it to finish.
	printf("Test Finished.  During random sampling, locks held were: A %d, B %d, C %d\n", fndA, fndB, fndC);
}
