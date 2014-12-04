/*
* Copyright 2008, 2011 Free Software Foundation, Inc.
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


#ifndef INTERTHREAD_H
#define INTERTHREAD_H

#include "Defines.h"
#include "Timeval.h"
#include "Threads.h"
#include "LinkedLists.h"
#include <map>
#include <vector>
#include <queue>
#include <list>





/**@defgroup Templates for interthread mechanisms. */
//@{


// A list designed for pointers so we can use get methods that return NULL on error.
template<class T>
class PtrList : public std::list<T*> {
	//typedef typename std::list<T*> type_t;
	public:
	typedef typename std::list<T*>::iterator iter_t;
	// Like pop_front but return the value, or NULL if none.
	T* pop_frontr() {
		if (this->empty()) { return NULL; }
		T* result = this->front();
		this->pop_front();
		return result;
	}
	// Like pop_back but return the value, or NULL if none.
	T* pop_backr() {
		if (this->empty()) { return NULL; }
		T* result = this->back();
		this->pop_back();
		return result;
	}

	// These functions are solely for use by InterthreadQueue, necessitated by backward compatibility with PointerFIFO.
	void put(T*v) { this->push_back(v); }
	T* get() { return this->pop_frontr(); }
};



// (pat 8-2013) The scoped iterators are a great idea and worked fine, but it is not KISS, so I stopped using it.
// If you want to iterate through an InterthreadQueue or InterthreadMap, just get the lock using the appropropriate qGetLock method.
#if USE_SCOPED_ITERATORS
// This ScopedIterator locks the container for as long as the iterator exists.
// You have to create the ScopedIterator using the thread-safe container object, example:
//  	ThreadSafeMap<a,b> mymap;
//  	ScopedIterator it(mymap);
//  	for (it = mymap.begin(); it != mymap.end(); it++) {}
// Then begin() and end() are passed through to the normal underlying iterator.
// An alternative implementation would have been to modify begin and end to return scoped iterators,
// but that is more complicated and would probably require multiple locks/unlocks on the Mutex.
template <class BaseType,class DerivedType,class ValueType>
class ScopedIteratorTemplate : public BaseType::iterator {
	Mutex &mLockRef;
	public:
	ScopedIteratorTemplate(DerivedType &wOwner) : mLockRef(wOwner.qGetLock()) { mLockRef.lock(); }
	~ScopedIteratorTemplate() { mLockRef.unlock(); }
	void operator=(typename BaseType::iterator it) { this->BaseType::iterator::operator=(it); }
	void operator++() { this->BaseType::iterator::operator++(); }				// ++prefix
	void operator++(int) { this->BaseType::iterator::operator++(0); }			// postfix++
	ValueType& operator*() { return this->BaseType::iterator::operator*(); }
	ValueType* operator->() { return this->BaseType::iterator::operator->(); }
};
#endif



// (pat) There was a transition period from the old to the new InterthreadQueue when the new
// one was named InterthreadQueue2, and that still exists in some versions of the SGSN/GPRS code.
#define InterthreadQueue2 InterthreadQueue

// (pat) The original InterthreadQueue had a complicated threading problem that this version fixed.
// I started out using this new version only in GPRS and SGSN, for fear of breaking something in GSM,
// but in release 4 I removed the old version above.
// 5-2013: Changed the ultimate base class to a PtrList and added ScopedIterator.
// 8-2013: Removed the ScopedIterator even though it is an elegant solution, because it is easy to
// just use the internal lock directly when one needs to iterate through one of these.
//template <class T, class Fifo=PointerFIFO> class InterthreadQueue {
template <class T, class Fifo=PtrList<T> > class InterthreadQueue {
	//protected:

	Fifo mQ;	
	// (pat) DO NOT USE mLock and mWriteSignal; instead use mLockPointer and mWriteSignalPointer.
	// That allows us to connect two InterthreadQueue together such that a single thread can wait on either.
	mutable Mutex mLock, *mLockPointer;
	mutable Signal mWriteSignal, *mWriteSignalPointer;

	protected:

	public:
	InterthreadQueue() : mLockPointer(&mLock), mWriteSignalPointer(&mWriteSignal) {}

	// This connects the two InterthreadQueue permanently so they use the same lock and Signal.
	// Subsequently you can use iqWaitForEither.
	void iqConnect(InterthreadQueue &other) {
		mLockPointer = other.mLockPointer;
		mWriteSignalPointer = other.mWriteSignalPointer;
	}

	// (pat) This provides a client the ability to lock the InterthreadQueue and iterate it.
	Mutex &qGetLock() const { return mLock; }
	typedef typename Fifo::iterator iterator;
	typedef typename Fifo::const_iterator const_iterator;
	iterator begin() { assert(mLock.lockcnt()); return mQ.begin(); }
	iterator end() { assert(mLock.lockcnt()); return mQ.end(); }
	const_iterator begin() const { assert(mLock.lockcnt()); return mQ.begin(); }
	const_iterator end() const { assert(mLock.lockcnt()); return mQ.end(); }

#if USE_SCOPED_ITERATORS
	// The Iterator locks the InterthreadQueue until the Iterator falls out of scope.
	// Semantics are different from normal C++ iterators - the begin,end,erase methods are in
	// the Iterator, not the base type.
	// Use like this:
	// InterthreadQueue<T>::ScopedIterator sit(someInterthreadQueue);
	// for (T*val = sit.front(); val = *sit; sit++) ...
	//			if (something) val.erase();
	typedef typename Fifo::iterator iterator;
	class ScopedIterator {
		typedef InterthreadQueue<T,Fifo> BaseType_t;
		typedef typename Fifo::iterator iterator_t;
		BaseType_t &mParent;
		iterator_t mit;

		public:
		ScopedIterator(BaseType_t&wParent) : mParent(wParent) { mParent.mLockPointer->lock(); }
		~ScopedIterator() { mParent.mLockPointer->unlock(); }

		// Regular old iterators in case you want to use em.
		iterator_t begin() { return mParent.mQ.begin(); }
		iterator_t end() { return mParent.mQ.end(); }

		// Accessors and operators.  Accessors move the iterator, eg, using front() rewinds iter to begin().
		T* current() { return mit != end() ? *mit : NULL; }
		T* front() { mit = begin(); return current(); }
		T* next() { if (mit != end()) { ++mit; } return current(); }
		// Erase current element and advance the iterator forward.
		void erase() { if (mit != end()) mit = mParent.mQ.erase(mit); }
		T* operator++() { return next(); }		// prefix ++
		T* operator++(int) { T*result = current(); next(); return result; }	// postfix ++
		T* operator*() { return current(); }

		// And here is random access in case you want it.
		// Note that this is inefficient, so dont use it unless you know the queue is small.
		// This is inside ScopedIterator so that the entire InterthreadQueue is locked while you do whatever it is you are doing.
		// Eg:  { InterthreadQueue<T>::ScopedIterator sit(myinterthreadqueue);  for (unsigned i=0; i<10; i++) { T*foo = sit[i]; ... } }
		T* operator[](unsigned ind) {
			unsigned i = 0;
			for (iterator_t itr = begin(); itr != end(); itr++) { if (i++ == ind) return *itr; }
			return NULL;	// Out of bounds.
		}
	};
#endif

	/** Delete contents. */
	void clear()
	{
		ScopedLock lock(*mLockPointer);
		while (mQ.size()>0) delete (T*)mQ.get();
	}

	/** Empty the queue, but don't delete. */
	void flushNoDelete()
	{
		ScopedLock lock(*mLockPointer);
		while (mQ.size()>0) mQ.get();
	}


	~InterthreadQueue()
		{ clear(); }


	size_t size() const
	{
		ScopedLock lock(*mLockPointer);
		return mQ.size();
	}

	size_t totalSize() const		// pat added
	{
		ScopedLock lock(*mLockPointer);
		return mQ.totalSize();
	}

	// Wait for something on either of the two queues connected by iqConnect.  Kind of hokey, but it works.  Timeout is in msecs.
	void iqWaitForEither(InterthreadQueue &other, unsigned timeout) {
		ScopedLock lock(*mLockPointer);
		if (timeout) {
			Timeval waitTime(timeout);
			while (mQ.size() == 0 && other.mQ.size() == 0) {
				mWriteSignalPointer->wait(*mLockPointer,waitTime.remaining());
			}
		} else {	// Wait forever.
			while (mQ.size() == 0 && other.mQ.size() == 0) {
				mWriteSignalPointer->wait(*mLockPointer);
			}
		}
	}

	// (pat 8-2013) Removed.  Bad idea to use this name - conflicts with wait() in InterthreadQueueWithWait
	//void wait() {	// (pat 7-25-2013) Added.  Wait for something to appear in the queue.
	//	ScopedLock lock(*mLockPointer);
	//	while (mQ.size() == 0) {
	//		mWriteSignalPointer->wait(*mLockPointer);
	//	}
	//}

	/**
		Blocking read from back of queue.
		@return Pointer to object (will not be NULL).
	*/
	T* read()
	{
		ScopedLock lock(*mLockPointer);
		T* retVal = (T*)mQ.get();
		while (retVal==NULL) {
			mWriteSignalPointer->wait(*mLockPointer);
			retVal = (T*)mQ.get();
		}
		return retVal;
	}

	/** Non-blocking peek at the first element; returns NULL if empty. */
	T* front() const
	{
		ScopedLock lock(*mLockPointer);
		return (T*) (mQ.size() ? mQ.front() : NULL);
	}

	/**
		Blocking read with a timeout.
		@param timeout The read timeout in ms.
		@return Pointer to object or NULL on timeout.
	*/
	T* read(unsigned timeout)
	{
		if (timeout==0) return readNoBlock();
		Timeval waitTime(timeout);
		ScopedLock lock(*mLockPointer);
		while (mQ.size()==0) {
			long remaining = waitTime.remaining();
			// (pat) How high do we expect the precision here to be?  I dont think they used precision timers,
			// so dont try to wait if the remainder is just a few msecs.
			if (remaining < 2) { return NULL;	}
			mWriteSignalPointer->wait(*mLockPointer,remaining);
		}
		T* retVal = (T*)mQ.get();
		return retVal;
	}

	/**
		Non-blocking read.  aka pop_front.
		@return Pointer to object or NULL if FIFO is empty.
	*/
	T* readNoBlock()
	{
		ScopedLock lock(*mLockPointer);
		return (T*)mQ.get();
	}

	/** Non-blocking write. aka push_back */
	void write(T* val)
	{
		// (pat) The Mutex mLock must be released before signaling the mWriteSignal condition.
		// This is an implicit requirement of pthread_cond_wait() called from signal().
		// If you do not do that, the InterthreadQueue read() function cannot start
		// because the mutex is still locked by the thread calling the write(),
		// so the read() thread yields its immediate execution opportunity.
		// This recurs (and the InterthreadQueue fills up with data)
		// until the read thread's accumulated temporary priority causes it to
		// get a second pre-emptive activation over the writing thread,
		// resulting in bursts of activity by the read thread. 
		{ ScopedLock lock(*mLockPointer);
		  mQ.put(val);
		}
		mWriteSignalPointer->signal();
	}

	/** Non-block write to the front of the queue. aka push_front */
	void write_front(T* val)	// pat added
	{
		// (pat) See comments above.
		{ ScopedLock lock(*mLockPointer);
		  mQ.push_front(val);
		}
		mWriteSignalPointer->signal();
	}
};



/** Pointer FIFO for interthread operations.  */
// Pat thinks this should be combined with InterthreadQueue by simply moving the wait method there.
template <class T> class InterthreadQueueWithWait {

	protected:

	PointerFIFO mQ;	
	mutable Mutex mLock;
	mutable Signal mWriteSignal;
	mutable Signal mReadSignal;

	virtual void freeElement(T* element) const { delete element; };

	public:

	/** Delete contents. */
	void clear()
	{
		ScopedLock lock(mLock);
		while (mQ.size()>0) freeElement((T*)mQ.get());
		mReadSignal.signal();
	}



	virtual ~InterthreadQueueWithWait()
		{ clear(); }


	size_t size() const
	{
		ScopedLock lock(mLock);
		return mQ.size();
	}

	/**
		Blocking read.
		@return Pointer to object (will not be NULL).
	*/
	T* read()
	{
		ScopedLock lock(mLock);
		T* retVal = (T*)mQ.get();
		while (retVal==NULL) {
			mWriteSignal.wait(mLock);
			retVal = (T*)mQ.get();
		}
		mReadSignal.signal();
		return retVal;
	}

	/**
		Blocking read with a timeout.
		@param timeout The read timeout in ms.
		@return Pointer to object or NULL on timeout.
	*/
	T* read(unsigned timeout)
	{
		if (timeout==0) return readNoBlock();
		Timeval waitTime(timeout);
		ScopedLock lock(mLock);
		// (pat 8-2013) This commented out code has a deadlock problem.
		//while ((mQ.size()==0) && (!waitTime.passed()))
		//	mWriteSignal.wait(mLock,waitTime.remaining());
		while (mQ.size()==0) {
			long remaining = waitTime.remaining();
			// (pat) How high do we expect the precision here to be?  I dont think they are used as precision timers,
			// so dont try to wait if the remainder is just a few msecs.
			if (remaining < 2) { return NULL; }
			mWriteSignal.wait(mLock,remaining);
		}
		T* retVal = (T*)mQ.get();
		if (retVal!=NULL) mReadSignal.signal();
		return retVal;
	}

	/**
		Non-blocking read.
		@return Pointer to object or NULL if FIFO is empty.
	*/
	T* readNoBlock()
	{
		ScopedLock lock(mLock);
		T* retVal = (T*)mQ.get();
		if (retVal!=NULL) mReadSignal.signal();
		return retVal;
	}

	/** Non-blocking write. */
	void write(T* val)
	{
		{
			ScopedLock lock(mLock);
			mQ.put(val);
		}
		mWriteSignal.signal();
	}

	/** Wait until the queue falls below a low water mark. */
	// (pat) This function suffers from the same problem as documented
	// at InterthreadQueue.write(), but I am not fixing it because I cannot test it.
	// The caller of this function will eventually get to run, just not immediately
	// after the mReadSignal condition is fulfilled.
	void wait(size_t sz=0)
	{
		ScopedLock lock(mLock);
		while (mQ.size()>sz) mReadSignal.wait(mLock);
	}

};


// (pat) Same as an InterthreadMap but the mapped type is "D" instead of "D*";
// the only difference is that we cannot automatically delete the content on destruction (no clear method).
template <class K, class D > class InterthreadMap1
{
public:
	typedef std::map<K,D> Map;

protected:

	Map mMap;
	mutable Mutex mLock;
	Signal mWriteSignal;

public:
	// User can over-ride this method if they want to delete type D elements.
	virtual void vdelete(D) {}

	~InterthreadMap1() {
		ScopedLock lock(mLock);
		typename Map::iterator iter = mMap.begin();
		while (iter != mMap.end()) {
			vdelete(iter->second);
			++iter;
		}
		mMap.clear();
	}

	/**
		Non-blocking write.  WARNING: This deletes any pre-existing element!
		@param key The index to write to.
		@param wData Pointer to data, not to be deleted until removed from the map.
	*/
	void write(const K &key, D wData)
	{
		ScopedLock lock(mLock);
		typename Map::iterator iter = mMap.find(key);
		if (iter!=mMap.end()) {
			vdelete(iter->second);
			iter->second = wData;
		} else {
			mMap[key] = wData;
		}
		mWriteSignal.broadcast();
	}

	/**
		Identical to readNoBlock but with element removal.
		@param key Key to read from.
		@return Pointer at key or NULL if key not found, to be deleted by caller.
	*/
	bool getNoBlock(const K& key, D &result, bool bRemove = true)
	{
		ScopedLock lock(mLock);
		typename Map::iterator iter = mMap.find(key);
		if (iter==mMap.end()) return false;
		result = iter->second;
		if (bRemove) { mMap.erase(iter); }
		return true;
	}

	/**
		Blocking read with a timeout and element removal.
		@param key The key to read from.
		@param timeout The blocking timeout in ms.
		@return Pointer at key or NULL on timeout, to be deleted by caller.
	*/
	bool get(const K &key, D &result, unsigned timeout, bool bRemove = true)
	{
		if (timeout==0) return getNoBlock(key,result,bRemove);
		ScopedLock lock(mLock);
		Timeval waitTime(timeout);
		while (1) {
			typename Map::iterator iter = mMap.find(key);
			if (iter!=mMap.end()) {
				result = iter->second;
				if (bRemove) { mMap.erase(iter); }
				return true;
			}
			long remaining = waitTime.remaining();
			if (remaining < 2) { return false; }
			mWriteSignal.wait(mLock,remaining);
		}
	}

	/**
		Blocking read with and element removal.
		@param key The key to read from.
		@return Pointer at key, to be deleted by caller.
		This always returns true.
	*/
	bool get(const K &key, D &result, bool bRemove = true)
	{
		ScopedLock lock(mLock);
		typename Map::iterator iter = mMap.find(key);
		while (iter==mMap.end()) {
			mWriteSignal.wait(mLock);
			iter = mMap.find(key);
		}
		result = iter->second;
		if (bRemove) { mMap.erase(iter); }
		return true;
	}


	/**
		Remove an entry and delete it.
		@param key The key of the entry to delete.
		@return True if it was actually found and deleted.
		(pat) If you just want remove without deleting, see getNoBlock.
	*/
	bool remove(const  K &key )
	{
		D val;
		if (getNoBlock(key,val,true)) {
			vdelete(val);
			return true;
		} else {
			return false;
		}
	}


	/**
		Non-blocking read.  (pat) Actually, it blocks until the map is available.
		@param key Key to read from.
		@return Pointer at key or NULL if key not found.
	*/
	D readNoBlock(const K& key) const
	{
		D result = NULL;
		Unconst(this)->getNoBlock(key,result,false);
		return result;
	}

	/**
		Blocking read with a timeout.
		@param key The key to read from.
		@param timeout The blocking timeout in ms.
		@return Pointer at key or NULL on timeout.
	*/
	D read(const K &key, unsigned timeout) const
	{
		D result = NULL;
		Unconst(this)->get(key,result,timeout,false);
		return result;
	}

	/**
		Blocking read.  Blocks until the key exists.
		@param key The key to read from.
		@return Pointer at key.
	*/
	D read(const K &key) const
	{
		D result;
		Unconst(this)->get(key,result,false);
		return result;
	}

	// pat added.
	unsigned size() const { ScopedLock(mLock); return mMap.size(); }

	// WARNING: These iterators are not intrinsically thread safe.
	// Caller must use ScopedIterator or the modification lock or enclose the entire iteration in some higher level lock.
	Mutex &qGetLock() { return mLock; }
	typedef typename Map::iterator iterator;
	typename Map::iterator begin() { return mMap.begin(); }
	typename Map::iterator end() { return mMap.end(); }
};



// (pat) The original InterthreadMap works only for pointers; now it is derived from the more general version above.
/** Thread-safe map of pointers to class D, keyed by class K. */
template <class K, class D > class InterthreadMap : public InterthreadMap1<K,D*>
{
	void vdelete(D* foo) { delete foo; }
};








/** This class is used to provide pointer-based comparison in priority_queues. */
// The priority_queue sorts the largest element to the 'top' of the priority_queue, which is the back of the underlying vector.
template <class T> class PointerCompare {

	public:

	/** Compare the objects pointed to, not the pointers themselves. */
	bool operator()(const T *v1, const T *v2)
		{ return (*v1)>(*v2); }

};



/**
	Priority queue for interthread operations.
	Passes pointers to objects.
*/
template <class T, class C = std::vector<T*>, class Cmp = PointerCompare<T> > class InterthreadPriorityQueue
{

	protected:

	std::priority_queue<T*,C,Cmp> mQ;
	mutable Mutex mLock;
	mutable Signal mWriteSignal;

	// Assumes caller holds the lock.
	T*ipqGet() {
		if (!mQ.size()) { return NULL; }
		T*result = mQ.top();
		mQ.pop();
		return result;
	}

	public:


	/** Clear the FIFO. */
	void clear()
	{
		ScopedLock lock(mLock);
		while (mQ.size()>0)	{
			T* ptr = mQ.top();
			mQ.pop();
			delete ptr;
		}
	}


	~InterthreadPriorityQueue()
	{
		clear();
	}

	size_t size() const
	{
		ScopedLock lock(mLock);
		return mQ.size();
	}


	/** Non-blocking read. */
	T* readNoBlock()
	{
		ScopedLock lock(mLock);
		return ipqGet();
	}

	/** Blocking read. */
	T* read()
	{
		ScopedLock lock(mLock);
		while (mQ.size()==0) mWriteSignal.wait(mLock);
		return ipqGet();
	}

	T* read(unsigned timeout)
	{
		if (timeout==0) return readNoBlock();
		Timeval waitTime(timeout);
		ScopedLock lock(mLock);
		while (mQ.size()==0) {
			long remaining = waitTime.remaining();
			// (pat) How high do we expect the precision here to be?  I dont think they used precision timers,
			// so dont try to wait if the remainder is just a few msecs.
			if (remaining < 2) { return NULL;	}
			mWriteSignal.wait(mLock,remaining);
		}
		return ipqGet();
	}

	// pat added 4-2014.  Return but do not pop the top element, if any, or NULL.
	T* peek()
	{
		ScopedLock lock(mLock);
		return mQ.size() ? mQ.top() : NULL;
	}

	/** Non-blocking write. */
	void write(T* val)
	{
		{	ScopedLock lock(mLock);
			mQ.push(val);
		}
		mWriteSignal.signal();
	}

};





class Semaphore {

	private:

	bool mFlag;
	Signal mSignal;
	mutable Mutex mLock;

	public:

	Semaphore()
		:mFlag(false)
	{ }

	void post()
	{
		ScopedLock lock(mLock);
		mFlag=true;
		mSignal.signal();
	}

	void get()
	{
		ScopedLock lock(mLock);
		while (!mFlag) mSignal.wait(mLock);
		mFlag=false;
	}

	bool semtry()
	{
		ScopedLock lock(mLock);
		bool retVal = mFlag;
		mFlag = false;
		return retVal;
	}

};





//@}




#endif
// vim: ts=4 sw=4
