/*
* Copyright 2008, 2014 Free Software Foundation, Inc.
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





#include "Threads.h"
#include "Timeval.h"
#include "Logger.h"
#include <errno.h>


using namespace std;

int gMutexLogLevel = LOG_INFO;	// The mutexes cannot call gConfig or gGetLoggingLevel so we have to get the log level indirectly.


#define LOCKLOG(level,fmt,...) \
	if (gMutexLogLevel >= LOG_##level) syslog(LOG_##level,"%lu %s %s:%u:%s:lockid=%p " fmt,(unsigned long)pthread_self(),Utils::timestr().c_str(),__FILE__,__LINE__,__FUNCTION__,this,##__VA_ARGS__);
	//printf("%u %s %s:%u:%s:lockid=%u " fmt "\n",(unsigned)pthread_self(),Utils::timestr().c_str(),__FILE__,__LINE__,__FUNCTION__,(unsigned)this,##__VA_ARGS__);




Mutex gStreamLock;		///< Global lock to control access to cout and cerr.

void lockCout()
{
	gStreamLock.lock();
	Timeval entryTime;
	cout << entryTime << " " << pthread_self() << ": ";
}


void unlockCout()
{
	cout << dec << endl << flush;
	gStreamLock.unlock();
}


void lockCerr()
{
	gStreamLock.lock();
	Timeval entryTime;
	cerr << entryTime << " " << pthread_self() << ": ";
}

void unlockCerr()
{
	cerr << dec << endl << flush;
	gStreamLock.unlock();
}







Mutex::Mutex() : mLockCnt(0) //, mLockerFile(0), mLockerLine(0)
{
	memset(mLockerFile,0,sizeof(mLockerFile));
	// Must use getLoggingLevel, not gGetLoggingLevel, to avoid infinite recursion.
	bool res;
	res = pthread_mutexattr_init(&mAttribs);
	assert(!res);
	res = pthread_mutexattr_settype(&mAttribs,PTHREAD_MUTEX_RECURSIVE);
	assert(!res);
	res = pthread_mutex_init(&mMutex,&mAttribs);
	assert(!res);
}


Mutex::~Mutex()
{
	pthread_mutex_destroy(&mMutex);
	bool res = pthread_mutexattr_destroy(&mAttribs);
	assert(!res);
}

bool Mutex::trylock()
{
	if (pthread_mutex_trylock(&mMutex)==0) {
		if (mLockCnt < maxLocks) { mLockerFile[mLockCnt] = NULL; }
		mLockCnt++;
		return true;
	} else {
		return false;
	}
}

// Returns true if the lock was acquired within the timeout, or false if it timed out.
bool Mutex::timedlock(int msecs) // Wait this long in milli-seconds.
{
	Timeval future(msecs);
	struct timespec timeout = future.timespec();
	return ETIMEDOUT != pthread_mutex_timedlock(&mMutex, &timeout);
}

string Mutex::mutext() const
{
	string result;
	result.reserve(100);
	//result += format("lockid=%u lockcnt=%d",(unsigned)this,mLockCnt);
	result += format("lockcnt=%d",mLockCnt);
	for (int i = 0; i < mLockCnt && i < maxLocks; i++) {
		if (mLockerFile[i]) {
			result += format(" %s:%u",mLockerFile[i],mLockerLine[i]);
		} else {
			result += " ?";
		}
	}
	return result;
}

void Mutex::lock() {
	if (lockerFile()) LOCKLOG(DEBUG,"lock unchecked");
	_lock();
	mLockCnt++;
}

// WARNING:  The LOG facility calls lock, so to avoid infinite recursion do not call LOG if file == NULL,
// and the file argument should never be used from the Logger facility.
void Mutex::lock(const char *file, unsigned line)
{
	// (pat 10-25-13) This is now going to be the default behavior so we can detect and report deadlocks at customer sites.
	//if (file && gGetLoggingLevel(file)>=LOG_DEBUG)
	//if (file) OBJLOG(DEBUG) <<"start at "<<file<<" "<<line;
	if (file) {
		LOCKLOG(DEBUG,"start at %s %u",file,line);
		// If we wait more than a second, print an error message.
		if (!timedlock(1000)) {
			// We have to use a temporary variable here because there is a chance here that the mLockerFile&mLockerLine
			// could change while we are printing it if multiple other threads are contending for the lock
			// and swapping the lock around while we are in here.
			// There is still some chance that mLockerFile&mLockerLine will be out of phase with each other, but at least it wont crash.
			// Granted, if we have already been stalled for a second, this is all unlikely.
			//const char *oldFile = NULL; unsigned oldLine = 0;
			//if (mLockCnt < maxLocks) { oldFile = mLockerFile[mLockCnt]; oldLine = mLockerLine[mLockCnt]; }
			//OBJLOG(ERR) << "Blocked more than one second at "<<file<<" "<<line<<" by "<<(oldFile?oldFile:"?")<<" "<<oldLine;
			LOCKLOG(ERR, "Blocked more than one second at %s %u by %s",file,line,mutext().c_str());
			printf("WARNING: %s Blocked more than one second at %s %u by %s\n",timestr(4).c_str(),file,line,mutext().c_str());
			_lock();					// If timedlock failed we are probably now entering deadlock.
		}
#if older_version
		mLockerFile = file; mLockerLine = line;
		if (!trylock()) {
			// This is not 100% for sure, because lock might be released in this little interval,
			// or there could be multiple callers waiting on the lock overwriting mLockerFile, mLockerLine,
			// but it is not worth adding yet another mutex just to perfect this debug message.
			OBJLOG(DEBUG) << "Blocking at "<<file<<" "<<line<<" by "<<oldFile<<" "<<oldLine;
			lock();
			OBJLOG(DEBUG) << "Unblocking at "<<file<<" "<<line;
		}
#endif
	} else {
		//LOCKLOG(DEBUG,"unchecked lock");
		_lock();
	}
	if (mLockCnt >= 0 && mLockCnt < maxLocks) {
		mLockerFile[mLockCnt] = file; mLockerLine[mLockCnt] = line;		// Now our thread has it locked from here.
	}
	mLockCnt++;
	if (file) { LOCKLOG(DEBUG,"lock by %s",mutext().c_str()); }
	//else { LOCKLOG(DEBUG,"lock no file"); }
}

void Mutex::unlock()
{
	if (lockerFile()) { LOCKLOG(DEBUG,"unlock at %s",mutext().c_str()); }
	//else { LOCKLOG(DEBUG,"unlock unchecked"); }
	mLockCnt--;
	pthread_mutex_unlock(&mMutex);
}

RWLock::RWLock()
{
	bool res;
	res = pthread_rwlockattr_init(&mAttribs);
	assert(!res);
	res = pthread_rwlock_init(&mRWLock,&mAttribs);
	assert(!res);
}


RWLock::~RWLock()
{
	pthread_rwlock_destroy(&mRWLock);
	bool res = pthread_rwlockattr_destroy(&mAttribs);
	assert(!res);
}



/** Block for the signal up to the cancellation timeout in msecs. */
// (pat 8-2013) Our code had places (InterthreadQueue) that passed in negative timeouts which create deadlock.
// To prevent that, use signed, not unsigned timeout.
void Signal::wait(Mutex& wMutex, long timeout) const
{
	if (timeout <= 0) { return; }	// (pat) Timeout passed already
	Timeval then(timeout);
	struct timespec waitTime = then.timespec();
	pthread_cond_timedwait(&mSignal,&wMutex.mMutex,&waitTime);
}


void Thread::start(void *(*task)(void*), void *arg)
{
	assert(mThread==((pthread_t)0));
	bool res;
	// (pat) Moved initialization to constructor to avoid crash in destructor.
	//res = pthread_attr_init(&mAttrib);
	//assert(!res);
	res = pthread_attr_setstacksize(&mAttrib, mStackSize);
	assert(!res);
	res = pthread_create(&mThread, &mAttrib, task, arg);
	assert(!res);
}



// vim: ts=4 sw=4
