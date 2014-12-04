/*
* Copyright 2011, 2014 Range Networks, Inc.
* All Rights Reserved.
*
* This software is distributed under multiple licenses;
* see the COPYING file in the main directory for licensing
* information for this specific distribution.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef _UTILS_H
#define _UTILS_H
#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <string.h>
#include <math.h>		// for sqrtf
#include <typeinfo>
#include "Logger.h"


namespace Utils {
using namespace std;

extern double timef();					// high resolution time
// We dont use a default arg here timestr(unsigned fieldwidth=12) because g++ complains about duplicate decl in Logger.h
extern const std::string timestr();		// A timestamp to print in messages.
extern const std::string timestr(unsigned fieldwidth, bool addDate = false);		// A timestamp to print in messages.
extern void sleepf(double howlong);	// high resolution sleep
extern int gcd(int x, int y);

string format(const char *fmt, ...) __attribute__((format (printf,1,2)));

// format1 used to prevent C++ confusion over what function to call here.
string format1(const char *fmt, ...) __attribute__((format (printf,1,2)));
// We have to enumerate the cross product of argument types here.  This is fixed in C++11.
inline string format(const char *fmt, string s1) {
	return format1(fmt,s1.c_str());
}
inline string format(const char *fmt, string s1, string s2) {
	return format1(fmt,s1.c_str(),s2.c_str());
}
inline string format(const char *fmt, string s1, string s2, string s3) {
	return format1(fmt,s1.c_str(),s2.c_str(),s3.c_str());
}
inline string format(const char *fmt, string s1, string s2, string s3, string s4) {
	return format1(fmt,s1.c_str(),s2.c_str(),s3.c_str(),s4.c_str());
}
inline string format(const char *fmt, string s1, int i1) {
	return format1(fmt,s1.c_str(),i1);
}
inline string format(const char *fmt, int i1, string s1) {
	return format1(fmt,i1,s1.c_str());
}
inline string format(const char *fmt, string s1, string s2, int i1) {
	return format1(fmt,s1.c_str(),s2.c_str(),i1);
}
inline string format(const char *fmt, string s1, string s2, int i1, int i2) {
	return format1(fmt,s1.c_str(),s2.c_str(),i1,i2);
}

int myscanf(const char *str, const char *fmt, string *s1);
int myscanf(const char *str, const char *fmt, string *s1, string *s2);
int myscanf(const char *str, const char *fmt, string *s1, string *s2, string *s3);
int myscanf(const char *str, const char *fmt, string *s1, string *s2, string *s3, string *s4);

int cstrSplit(char *in, char **pargv,int maxargc, const char *splitchars=NULL);
char *cstrGetArg(const char *in, int nth, unsigned *length);

// For classes with a text() function, provide a function to return a String,
// and also a standard << stream function that takes a pointer to the object.
// We dont provide the function that takes a reference to the object
// because it is too highly overloaded and generally doesnt work.
class Text2Str {
	public:
	virtual void text(std::ostream &os) const = 0;
	std::string str() const;
};
std::ostream& operator<<(std::ostream& os, const Text2Str *val);

#if 0
// Generic Activity Timer.  Lots of controls to make everybody happy.
class ATimer {
	double mStart;
	//bool mActive;
	double mLimitTime;
	public:
	ATimer() : mStart(0), mLimitTime(0) { }
	ATimer(double wLimitTime) : mStart(0), mLimitTime(wLimitTime) { }
	void start() { mStart=timef(); }
	void stop() { mStart=0; }
	bool active() { return !!mStart; }
	double elapsed() { return timef() - mStart; }
	bool expired() { return elapsed() > mLimitTime; }
};
#endif


struct BitSet {
	unsigned mBits;
	void setBit(unsigned whichbit) { mBits |= 1<<whichbit; }
	void clearBit(unsigned whichbit) { mBits &= ~(1<<whichbit); }
	unsigned getBit(unsigned whichbit) const { return mBits & (1<<whichbit); }
	bool isSet(unsigned whichbit) const { return mBits & (1<<whichbit); }
	unsigned bits() const { return mBits; }
	operator int(void) const { return mBits; }
	BitSet() { mBits = 0; }
};

// Store current, min, max and compute running average and standard deviation.
template<class Type> struct Statistic {
	Type mCurrent, mMin, mMax;		// min,max optional initialization so you can print before adding any values.
	unsigned mCnt;
	double mSum;
	//double mSum2;	// sum of squares.
	// (Type) cast needed in case Type is an enum, stupid language.
	Statistic() : mCurrent((Type)0), mMin((Type)0), mMax((Type)0), mCnt(0), mSum(0) /*,mSum2(0)*/ {}
	// Set the current value and add a statisical point.
	void addPoint(Type val) {
		mCurrent = val;
		if (mCnt == 0 || val < mMin) {mMin = val;}
		if (mCnt == 0 || val > mMax) {mMax = val;}
		mCnt++;
		mSum += val;
		//mSum2 += val * val;
	}
	Type getCurrent() const {	// Return current value.
		return mCnt ? mCurrent : 0;
	}
	double getAvg() const { 			// Return average.
		return mCnt==0 ? 0 : mSum/mCnt; 
	};
	//float getSD() const { 	// Return standard deviation.  Use low precision square root function.
	//	return mCnt==0 ? 0 : sqrtf(mCnt * mSum2 - mSum*mSum) / mCnt;
	//}

	void text(std::ostream &os) const {	// Print everything in parens.
		os << "("<<mCurrent;
		if (mMin != mMax) {	// No point in printing all this stuff if min == max.
			os <<LOGVAR2("min",mMin)<<LOGVAR2("max",mMax)<<LOGVAR2("avg",getAvg());
			if (mCnt <= 999999) {
				os <<LOGVAR2("N",mCnt);
			} else { // Shorten this up:
				char buf[10], *ep;
				sprintf(buf,"%.3g",round(mCnt));
				if ((ep = strchr(buf,'e')) && ep[1] == '+') { strcpy(ep+1,ep+2); }
				os << LOGVAR2("N",buf);
			}
			// os<<LOGVAR2("sd",getSD())  standard deviation not interesting
		}
		os << ")";
		// " min="<<mMin <<" max="<<mMax <<format(" avg=%4g sd=%3g)",getAvg(),getSD());
	}
	// Not sure if this works:
	//std::string statStr() const {
	//	return (std::string)mCurrent + " min=" + (std::string) mMin +" max="+(string)mMax+ format(" avg=%4g sd=%3g",getAvg(),getSD());
	//}
};

// This I/O mechanism is so dumb:
std::ostream& operator<<(std::ostream& os, const Statistic<int> &stat);
std::ostream& operator<<(std::ostream& os, const Statistic<unsigned> &stat);
std::ostream& operator<<(std::ostream& os, const Statistic<float> &stat);
std::ostream& operator<<(std::ostream& os, const Statistic<double> &stat);


// Yes, they botched and left this out:
std::ostream& operator<<(std::ostream& os, std::ostringstream& ss);

std::ostream &osprintf(std::ostream &os, const char *fmt, ...) __attribute__((format (printf,2,3)));

std::string replaceAll(const std::string input, const std::string search, const std::string replace);

vector<string>& stringSplit(vector<string> &result,const char *input);
typedef vector<vector<string> > prettyTable_t;
void printPrettyTable(prettyTable_t &tab, ostream&os, bool tabSeparated = false);

// The need for this is eliminated in C++11.
string stringcat(string a, string b);
string stringcat(string a, string b, string c);
string stringcat(string a, string b, string c, string d);
string stringcat(string a, string b, string c, string d, string e);
string stringcat(string a, string b, string c, string d, string e, string f);
string stringcat(string a, string b, string c, string d, string e, string f, string g);

extern void stringToUint(string strRAND, uint64_t *hRAND, uint64_t *lRAND);
extern string uintToString(uint64_t h, uint64_t l);
extern string uintToString(uint32_t x);

//template <class Type> class RefCntPointer;
// The class is created with a RefCnt of 0.  The caller may assign the constructed result to a pointer
// of type RefCntPointer.  If so, then when the last RefCntPointer is freed, this struct is too.
class RefCntBase {
	template <class Type> friend class RefCntPointer;
	mutable Mutex mRefMutex;
	mutable short mRefCnt;		// signed, not unsigned!
	int setRefCnt(int val) { return mRefCnt = val; }
	// The semantics of reference counting mean you cannot copy an object that has reference counts;
	// they are only manipulated by pointers.  This constructor is private, which makes C++ whine if you do this.
	// Actually, you could copy it, if the refcnt in the new object were 0 and the pointer was assigned to a RefCntPointer.
	RefCntBase(RefCntBase &) { assert(0); }
	RefCntBase(const RefCntBase &) { assert(0); }
	RefCntBase operator=(RefCntBase &) {assert(0); mRefCnt = 0; return *this; }
	// These are not 'const' but wonderful C++ requires us to declare them const so we can use a const pointer as the RefCntPointer target,
	// then add 'mutable' to everything.  What a botched up language.
	int decRefCnt() const;	// Only RefCntPointer is permitted to use incRefCnt and decRefCnt.
	void incRefCnt() const;
	public:
	virtual ~RefCntBase();
	RefCntBase() : mRefCnt(0) {}
	int getRefCnt() const { return mRefCnt; }
};

// This is basically the same as a C++11 shared_ptr, but we cannot use C++ 11 yet.
// The basic idea is that once you put an object into a RefCntPointer, then destruction of that
// object is controlled by and assured by RefCntPointer using reference counts; when the last
// RefCntPointer referring to the object is destroyed, the object is too.
// You can use RefCntPointer<object> similarly to an object* for most data-access purposes.
// The auto-destruction should be optional - there should be a version that just maintains reference counts for you.
// Semantics:
// SomeDescendentOfRefCntBase foo;
// RefCntPointer<SomeDescendentOfRefCntBase> ptrToFoo;
// ptrToFoo = &foo;		// increment the reference count of foo.
// ... ptrToFoo->...	// Normal code using ptrToFoo as a (SomeDescendentOfRefCntBase*)
// ptrToFoo = 0;		// release pointer and decrement reference count of foo.
// ptrToFoo.free();		// A better way to release it.
template<class Type>	// Type must be a descendent of RefCntBase.
class RefCntPointer {
	Type *rcPointer;
	void rcInc() { if (rcPointer) rcPointer->incRefCnt(); }
	void rcDec() {
		if (rcPointer) if (rcPointer->decRefCnt() <= 0) { rcPointer = NULL; };
	}
	public:
	RefCntPointer() : rcPointer(NULL) { }
	RefCntPointer(const RefCntPointer &other) { rcPointer = other.rcPointer; rcInc(); }
	RefCntPointer(RefCntPointer &other) { rcPointer = other.rcPointer; rcInc(); }
	RefCntPointer(Type*other) { rcPointer = other; rcInc(); }
	// (pat) Making this virtual ~RefCntPointer makes ~MMContext crash, dont know why.
	// (pat) Update: lets try making it virtual again; it should work, but no one derives off RefCntPointer so I dont know why it crashes..
	~RefCntPointer() {
		if (rcPointer) { LOG(DEBUG) <<" rcPointer="<<((void*)rcPointer) <<" refcnt="<<(rcPointer?rcPointer->getRefCnt():0); }
		rcDec();
		rcPointer = NULL;	// should be redundant
	}
	Type* self() const { return rcPointer; }
	// The operator=(Type*) is called if the argument is NULL, so we dont need int.
	//Type* operator=(int value) { assert(value == 0); rcDec(); rcPointer = 0; return rcPointer; }
	// We increment before decrement if possible so that a = a; does not crash.
	RefCntPointer<Type>& operator=(RefCntPointer other) { other.rcInc(); rcDec(); rcPointer = other.rcPointer; return *this; }
	// The old is the previous value of this pointer.  We do not decrement the refcnt in the other that we are taking charge of.
	RefCntPointer<Type>& operator=(Type* other) { Type *old = rcPointer;  rcPointer = other; rcInc(); if (old) {old->decRefCnt();} return *this; }
	bool operator==(const RefCntPointer &other) const { return rcPointer == other.rcPointer; }
	bool operator==(const Type* other) const { return rcPointer == other; }
	bool operator!=(const RefCntPointer &other) const { return rcPointer != other.rcPointer; }
	bool operator!=(const Type* other) const { return rcPointer != other; }
	Type* operator->() const { return rcPointer; }
	Type& operator*() const { return *rcPointer; }
	// This auto-conversion causes gcc warning messages, which are of no import, but to avoid them I now use self() everywhere instead.
	//operator Type*() const { return rcPointer; }
	// free is a synonym for *this = NULL; but it is a better comment in the code what is happening.  RefCntBase is only freed if this was the last pointer to it.
	void free() {
		int refcnt = rcPointer ? rcPointer->getRefCnt() : 0;
		rcDec();
		if (refcnt > 1) { LOG(DEBUG)<<"RefCntPointer "<<(void*)rcPointer<<" refcnt before="<<refcnt<< " did not free structure."; }
		else { LOG(DEBUG)<<"RefCntPointer "<<(void*)rcPointer<<"  refcnt before="<<refcnt; }
		rcPointer = NULL;
	}
	bool isNULL() const { return self() == NULL; }
};

extern string firstlines(string msgstr, int n=2);

extern std::string rn_backtrace();
extern string decodeToString(const char *buf, unsigned buflen, string encodingArg, string &errorMessage);
extern string encodeToString(const char *data, unsigned datalen, string encodingArg, string &errorMessage);

};	// namespace

using namespace Utils;

#endif
