/*
* Copyright 2008, 2011, 2014 Free Software Foundation, Inc.
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


#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include <unistd.h>

class Mutex;


/**@name Multithreaded access for standard streams. */
//@{

extern int gMutexLogLevel;	// The mutexes cannot call gConfig or gGetLoggingLevel so we have to get the log level indirectly.

/**@name Functions for gStreamLock. */
//@{
extern Mutex gStreamLock;	///< global lock for cout and cerr
void lockCerr();		///< call prior to writing cerr
void unlockCerr();		///< call after writing cerr
void lockCout();		///< call prior to writing cout
void unlockCout();		///< call after writing cout
//@}

/**@name Macros for standard messages. */
//@{
#define COUT(text) { lockCout(); std::cout << text; unlockCout(); }
#define CERR(text) { lockCerr(); std::cerr << __FILE__ << ":" << __LINE__ << ": " << text; unlockCerr(); }
#ifdef NDEBUG
#define DCOUT(text) {}
#define OBJDCOUT(text) {}
#else
#define DCOUT(text) { COUT(__FILE__ << ":" << __LINE__ << " " << text); }
#define OBJDCOUT(text) { DCOUT(this << " " << text); } 
#endif
//@}
//@}



/**@defgroup C++ wrappers for pthread mechanisms. */
//@{

/** A class for recursive mutexes based on pthread_mutex. */
// If at all possible, do not call lock/unlock from this class directly; use a ScopedLock instead.
class Mutex {

	private:

	pthread_mutex_t mMutex;
	pthread_mutexattr_t mAttribs;
	int mLockCnt;
	int mMutexLogLevel;	// We cant use LOG inside the Mutex because LOG itself uses mutexes, so get the LOG level at mutex creation time
						// and use it for this mutex from then on.

	static const int maxLocks = 5;		// Just the maximum number of recursive locks we report during debugging, not the max possible.
	const char *mLockerFile[maxLocks];
	unsigned mLockerLine[maxLocks];
	const char *lockerFile() { int i = mLockCnt-1; return (i >= 0 && i < maxLocks) ? mLockerFile[i] : NULL; }
	//unused: bool anyDebugging() { for (int i = 0; i < maxLocks; i++) { if (mLockerFile[i]) return true; return false; } }

	// pthread_mutex_trylock returns 0 and trylock returns true if the lock was acquired.
	public:
	bool trylock(const char *file=0, unsigned line=0);

	Mutex();

	~Mutex();

	void _lock() { pthread_mutex_lock(&mMutex); }

	// (pat) Like the above but report blocking; to see report you must set both Log.Level to DEBUG for both Threads.cpp and the file.
	void lock(const char *file=0, unsigned line=0);

	std::string mutext() const;

	// Returns true if the lock was acquired, or false if it timed out.
	bool timedlock(int msecs);

	void unlock();

	// (pat) I use this to assert that the Mutex is locked on entry to some method that requres it, but only in debug mode.
	int lockcnt() { return mLockCnt; }

	friend class Signal;

};

/** A class for reader/writer based on pthread_rwlock. */
class RWLock {

	private:

	pthread_rwlock_t mRWLock;
	pthread_rwlockattr_t mAttribs;

	public:

	RWLock();

	~RWLock();

	const char * wlock() { pthread_rwlock_wrlock(&mRWLock); return ""; }
	const char * rlock() { pthread_rwlock_rdlock(&mRWLock); return ""; }

	bool trywlock() { return pthread_rwlock_trywrlock(&mRWLock)==0; }
	bool tryrlock() { return pthread_rwlock_tryrdlock(&mRWLock)==0; }

	const char * unlock() { pthread_rwlock_unlock(&mRWLock); return ""; }

};


#if 0
// (pat) NOT FINISHED OR TESTED.  A pointer that releases a specified mutex when it goes out of scope.
template<class PointsTo>
class ScopedPointer {
	Mutex &mControllingMutex;	// A pointer to the mutex for the object being protected.
	PointsTo *mPtr;

	public:
	ScopedPointer(Mutex& wMutex) :mControllingMutex(wMutex) { mControllingMutex.lock(); }
	// Requisite Copy Constructor:  The mutex is already locked, but we need to lock it again because the
	// other ScopedPointer is about to go out of scope and will call unlock.
	ScopedPointer(ScopedPointer &other) :mControllingMutex(other.mControllingMutex) { mControllingMutex.lock(); }
	~ScopedPointer() { mControllingMutex.unlock(); }

	// You are allowed to assign and derference the underlying pointer - it still holds the Mutex locked.
	PointsTo *operator->() const { return mPtr; }
	PointsTo * operator=(PointsTo *other) { mPtr = other; }
	PointsTo& operator*() { return *mPtr; }
};
#endif

// Class to acquire a Mutex lock and release it automatically when this goes out of scope.
// ScopedLock should be used preferentially to Mutex::lock() and Mutex::unlock() in case a try-catch throw passes through
// the containing procedure while the lock is held; ScopedLock releases the lock in that case.
// "We dont use try-catch" you say?  Yes we do - C++ string and many standard containers use throw to handle unexpected arguments.
class ScopedLock {
	Mutex& mMutex;

	public:
	ScopedLock(Mutex& wMutex) :mMutex(wMutex) { mMutex.lock(); }
	// Like the above but report blocking; to see report you must set both Log.Level to DEBUG for both Threads.cpp and the file.
	ScopedLock(Mutex& wMutex,const char *file, unsigned line):mMutex(wMutex) { mMutex.lock(file,line); }
	~ScopedLock() { mMutex.unlock(); }
};

// Lock multiple mutexes simultaneously.
class ScopedLockMultiple {
	Mutex garbage;	// Someplace to point mC if only two mutexes are specified.
	Mutex *mA[3];
	bool ownA[3];		// If set, expect mA to be locked by this thread on entry.
	bool state[3];	// Current state, true if our thread has locked the associated Mutex; doesnt say if Mutex is locked by other threads.
	const char *_file; unsigned _line;

	void _lock(int which);
	bool _trylock(int which);
	void _unlock(int which);
	void _saveState();
	void _restoreState();
	void _lockAll();
	void _init(int wOwner, Mutex& wA, Mutex&wB, Mutex&wCa);

	public:

	// Do not return until all three mutexes are locked.
	// On entry, the caller may optionally already have locked mutexes, as specified by the wOwner flag bits.
	// If owner&1, caller owns wA, if owner&2 caller owns wB, if owner&4 caller owns wC.
	// There wouldnt be much point of this class if the caller already owned all three mutexes.
	// Note that the mutexes may be temporarily surrendered during this call as the methodology to avoid deadlock,
	// but in that case all will be re-acquired before this returns.

	ScopedLockMultiple(int wOwner, Mutex&wA, Mutex&wB, Mutex&wC) : _file(NULL), _line(0) {
		_init(wOwner,wA,wB,wC);
		_lockAll();
	}
	// Like the above but report blocking; to see report you must set both Log.Level to DEBUG for both Threads.cpp and the file.
	// Use like this:  ScopedLockMultiple lock(bits,mutexa,mutexb,__FILE__,__LINE__);
	ScopedLockMultiple(int wOwner, Mutex&wA, Mutex&wB, Mutex&wC, const char *wFile, int wLine) : _file(wFile), _line(wLine) {
		_init(wOwner,wA,wB,wC);
		_lockAll();
	}

	// Like the above but for two mutexes intead of three.
	ScopedLockMultiple(int wOwner, Mutex& wA, Mutex&wB) : _file(NULL), _line(0) {
		_init(wOwner,wA,wB,garbage);
		_lockAll();
	}
	ScopedLockMultiple(int wOwner, Mutex&wA, Mutex&wB, const char *wFile, int wLine) : _file(wFile), _line(wLine) {
		_init(wOwner,wA,wB,garbage);
		_lockAll();
	}
	~ScopedLockMultiple() { _restoreState(); }
};




/** A C++ interthread signal based on pthread condition variables. */
class Signal {

	private:

	mutable pthread_cond_t mSignal;

	public:

	Signal() { int s = pthread_cond_init(&mSignal,NULL); assert(!s); }

	~Signal() { pthread_cond_destroy(&mSignal); }

	/**
		Block for the signal up to the cancellation timeout.
		Under Linux, spurious returns are possible.
	*/
	void wait(Mutex& wMutex, long timeout) const;

	/**
		Block for the signal.
		Under Linux, spurious returns are possible.
	*/
	void wait(Mutex& wMutex) const
		{ pthread_cond_wait(&mSignal,&wMutex.mMutex); }

	void signal() { pthread_cond_signal(&mSignal); }

	void broadcast() { pthread_cond_broadcast(&mSignal); }

};



#define START_THREAD(thread,function,argument) \
	thread.start((void *(*)(void*))function, (void*)argument);

/** A C++ wrapper for pthread threads.  */
class Thread {

	private:

	pthread_t mThread;
	pthread_attr_t mAttrib;
	// FIXME -- Can this be reduced now?
	size_t mStackSize;
	

	public:
	// (pat) This is the type of the function argument to pthread_create.
	typedef void *(*Task_t)(void*);

	/** Create a thread in a non-running state. */
	Thread(size_t wStackSize = (65536*4)):mThread((pthread_t)0) {
		pthread_attr_init(&mAttrib);	// (pat) moved this here.
		mStackSize=wStackSize;
	}

	/**
		Destroy the Thread.
		It should be stopped and joined.
	*/
	// (pat) If the Thread is destroyed without being started, then mAttrib is undefined.  Oops.
	~Thread() { pthread_attr_destroy(&mAttrib); }


	/** Start the thread on a task. */
	void start(Task_t task, void *arg);
	void start2(Task_t task, void *arg, int stacksize);

	/** Join a thread that will stop on its own. */
	void join() { int s = pthread_join(mThread,NULL); assert(!s); mThread = 0; }

};




#endif
// vim: ts=4 sw=4
