/*
* Copyright 2008 Free Software Foundation, Inc.
* Copyright 2014 Range Networks, Inc.
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
#include "Interthread.h"
#include <iostream>
#include "Configuration.h"
ConfigurationTable gConfig;

using namespace std;


InterthreadQueue<int> gQ;
InterthreadMap<int,int> gMap;

void* qWriter(void*)
{
	int *p;
	for (int i=0; i<20; i++) {
		p = new int;
		*p = i;
		COUT("queue write " << *p);
		gQ.write(p);
		if (random()%2) sleep(1);
	}
	p = new int;
	*p = -1;
	gQ.write(p);
	return NULL;
}

void* qReader(void*)
{
	bool done = false;
	while (!done) {
		int *p = gQ.read();
		COUT("queue read " << *p);
		if (*p<0) done=true;
		delete p;
	}
	return NULL;
}


void* mapWriter(void*)
{
	int *p;
	for (int i=0; i<20; i++) {
		p = new int;
		*p = i;
		COUT("map write " << *p);
		gMap.write(i,p);
		if (random()%2) sleep(1);
	}
	return NULL;
}

void* mapReader(void*)
{
	for (int i=0; i<20; i++) {
		int *p = gMap.read(i);
		COUT("map read " << *p);
		// InterthreadMap will delete the pointers
		// delete p;
	}
	return NULL;
}

static const uint32_t Hyperframe = 1024;

static int32_t FNDelta(int32_t v1, int32_t v2)
{
	static const int32_t halfModulus = Hyperframe/2;
	int32_t delta = v1-v2;
	if (delta>=halfModulus) delta -= Hyperframe;
	else if (delta<-halfModulus) delta += Hyperframe;
	return (int32_t) delta;
}

static int FNCompare(int32_t v1, int32_t v2)
{
	int32_t delta = FNDelta(v1,v2);
	if (delta>0) return 1;
	if (delta<0) return -1;
	return 0;
}

struct TestTime {


	int mFN;				///< frame number in the hyperframe
	int mTN;			///< timeslot number

	public:

	TestTime(int wFN=0, int wTN=0)
		:mFN(wFN),mTN(wTN)
	{ }

	bool operator<(const TestTime& other) const
	{
		if (mFN==other.mFN) return (mTN<other.mTN);
		return FNCompare(mFN,other.mFN)<0;
	}

	bool operator>(const TestTime& other) const
	{
		if (mFN==other.mFN) return (mTN>other.mTN);
		return FNCompare(mFN,other.mFN)>0;
	}

	bool operator<=(const TestTime& other) const
	{
		if (mFN==other.mFN) return (mTN<=other.mTN);
		return FNCompare(mFN,other.mFN)<=0;
	}

	bool operator>=(const TestTime& other) const
	{
		if (mFN==other.mFN) return (mTN>=other.mTN);
		return FNCompare(mFN,other.mFN)>=0;
	}

	bool operator==(const TestTime& other) const
	{
		return (mFN == other.mFN) && (mTN==other.mTN);
	}

};


// Welcome to wonderful C++.
struct CompareAdapter {
	/** Compare the objects pointed to, not the pointers themselves. */
	// (pat) This is used when a RachInfo is placed in a priority_queue.
	// Return true if rach1 should appear before rach2 in the priority_queue,
	// meaning that rach1 will be serviced before rach2.
	bool operator()(const TestTime *rach1, const TestTime *rach2) {
		return *rach1 > *rach2;
	}
};

void priority_queue_test()
{
	typedef InterthreadPriorityQueue<TestTime,std::vector<TestTime*>,CompareAdapter> PQ_t;
	PQ_t pq;

	pq.write(new TestTime(2,0));
	pq.write(new TestTime(1,0));
	pq.write(new TestTime(3,0));
	pq.write(new TestTime(2,3));
	pq.write(new TestTime(2,2));
	pq.write(new TestTime(2,1));
	pq.write(new TestTime(0,0));
	pq.write(new TestTime(1021,1));
	pq.write(new TestTime(1023,1));
	pq.write(new TestTime(1022,1));
	pq.write(new TestTime(1024,1));
	while (1) {
		TestTime *peek = pq.peek();
		TestTime *ptime = pq.readNoBlock();
		if (!ptime) { break; }	// Done.
		assert(*peek == *ptime);
		printf("TestTime(%d,%d)\n",ptime->mFN,ptime->mTN);
	}
}

int main(int argc, char *argv[])
{
	priority_queue_test();

	Thread qReaderThread;
	qReaderThread.start(qReader,NULL);
	Thread mapReaderThread;
	mapReaderThread.start(mapReader,NULL);

	Thread qWriterThread;
	qWriterThread.start(qWriter,NULL);
	Thread mapWriterThread;
	mapWriterThread.start(mapWriter,NULL);

	qReaderThread.join();
	qWriterThread.join();
	mapReaderThread.join();
	mapWriterThread.join();
}


// vim: ts=4 sw=4
