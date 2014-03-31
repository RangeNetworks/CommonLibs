/*
* Copyright 2011, 2014 Range Networks, Inc.
* All Rights Reserved.
*
* This software is distributed under multiple licenses;
* see the COPYING file in the main directory for licensing
* information for this specific distribuion.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <unistd.h>		// For usleep
#include <sys/time.h>	// For gettimeofday
#include <stdio.h>		// For vsnprintf
#include <ostream>		// For ostream
#include <sstream>		// For ostringstream
#include <string.h>		// For strcpy
#include <stdlib.h>		// For malloc
//#include "GSMCommon.h"
#include "Utils.h"
#include "MemoryLeak.h"

namespace Utils {

// (pat) This definition must be in the .cpp file to anchor the class vtable.
RefCntBase::~RefCntBase() { LOG(DEBUG) << typeid(this).name(); }

int RefCntBase::decRefCnt()
{
	int saveRefCnt;		// Passes the refcnt out of the locked block.
	{	ScopedLock lock(mRefMutex);
		assert(mRefCnt >= 0);
		mRefCnt = mRefCnt - 1;
		saveRefCnt = mRefCnt;
	}	// Must not keep locked during the delete, since the Mutex itself will be deleted.
	// The typeid(this).name() doesnt add anything because it is just the name of the class here, not the derived class.
	LOG(DEBUG) <<" "<<(void*)this <<" " <<LOGVAR2("refcnt after",saveRefCnt);
	// The refcnt is created with a value of zero, so when the last one is deleted it will come in with a value of zero.
	if (saveRefCnt <= 0) {
		LOG(DEBUG) << "deleting refcnted pointer "<<typeid(this).name();
		delete this;
	}
	return saveRefCnt;
}
void RefCntBase::incRefCnt()
{
	ScopedLock lock(mRefMutex);
	LOG(DEBUG) <<" "<<(void*)this <<" " <<LOGVAR2("refcnt before",mRefCnt);
	assert(mRefCnt >= 0);
	mRefCnt++;
}


MemStats gMemStats;
int gMemLeakDebug = 0;
static Mutex memChkLock;

MemStats::MemStats()
{
	memset(mMemNow,0,sizeof(mMemNow));
	memset(mMemTotal,0,sizeof(mMemTotal));
	memset(mMemName,0,sizeof(mMemName));
}

void MemStats::text(ostream &os)
{
	os << "Structs current total:\n";
	for (int i = 0; i < mMax; i++) {
		os << "\t" << (mMemName[i] ? mMemName[i] : "unknown") << " " << mMemNow[i] << " " << mMemTotal[i] << "\n";
	}
}

void MemStats::memChkNew(MemoryNames memIndex, const char *id)
{
	/*std::cout << "new " #type "\n";*/
	ScopedLock lock(memChkLock);
	mMemNow[memIndex]++;
	mMemTotal[memIndex]++;
	mMemName[memIndex] = id;
}

void MemStats::memChkDel(MemoryNames memIndex, const char *id)
{
	ScopedLock lock(memChkLock);
	/*std::cout << "del " #type "\n";*/
	mMemNow[memIndex]--;
	if (mMemNow[memIndex] < 0) {
		LOG(ERR) << "Memory reference count underflow on type "<<id;
		if (gMemLeakDebug) assert(0);
		mMemNow[memIndex] += 100;	// Prevent another message for a while.
	}
}

ostream& operator<<(std::ostream& os, std::ostringstream& ss)
{
	return os << ss.str();
}

ostream &osprintf(std::ostream &os, const char *fmt, ...)
{
	va_list ap;
	char buf[300];
	va_start(ap,fmt);
	int n = vsnprintf(buf,300,fmt,ap);
	va_end(ap);
	if (n >= (300-4)) { strcpy(&buf[(300-4)],"..."); }
	os << buf;
	return os;
}

string format(const char *fmt, ...)
{
	va_list ap;
	char buf[200];
	va_start(ap,fmt);
	int n = vsnprintf(buf,199,fmt,ap);
	va_end(ap);
	string result;
	if (n <= 199) {
		result = string(buf);
	} else {
		if (n > 5000) { LOG(ERR) << "oversized string in format"; n = 5000; }
		// We could use vasprintf but we already computed the length...
		// We are not using alloca because it might overflow the small stacks used for our threads.
		char *buffer = (char*)malloc(n+2);	// add 1 extra superstitiously.
		va_start(ap,fmt);
		vsnprintf(buffer,n+1,fmt,ap);
		va_end(ap);
		//if (n >= (2000-4)) { strcpy(&buf[(2000-4)],"..."); }
		result = string(buffer);
		free(buffer);
	}
	return result;
#if 0	// Maybe ok, but not recommended.  data() is const char*
	string result;
	va_list ap;
	va_start(ap,fmt);
	result.reserve(200);
	int n = vsnprintf(result.data(),198,fmt,ap);
	va_end(ap);
	if (n > 198) {
		if (n > 5000) { LOG(ERR) << "oversized string in format"; n = 5000; }
		result.reserve(n+2);	// add 1 extra superstitiously.
		va_start(ap,fmt);
		vsnprintf(result.data(),n+1,fmt,ap);
		va_end(ap);
	}
	result.resize(n);
	return result;
#endif
}

// Absolutely identical to format above.  This sucks...
string format1(const char *fmt, ...)
{
	va_list ap;
	char buf[200];
	va_start(ap,fmt);
	int n = vsnprintf(buf,199,fmt,ap);
	va_end(ap);
	string result;
	if (n <= 199) {
		result = string(buf);
	} else {
		if (n > 5000) { LOG(ERR) << "oversized string in format"; n = 5000; }
		// We could use vasprintf but we already computed the length...
		// We are not using alloca because it might overflow the small stacks used for our threads.
		char *buffer = (char*)malloc(n+2);	// add 1 extra superstitiously.
		va_start(ap,fmt);
		vsnprintf(buffer,n+1,fmt,ap);
		va_end(ap);
		//if (n >= (2000-4)) { strcpy(&buf[(2000-4)],"..."); }
		result = string(buffer);
		free(buffer);
	}
	return result;
}

int myscanf(const char *str, const char *fmt, string *s1)
{
	int maxlen = strlen(str)+1;
	char *a1 = (char*)alloca(maxlen);
	int n = sscanf(str,fmt,a1);
	s1->assign(a1);
	return n;
}
int myscanf(const char *str, const char *fmt, string *s1, string *s2)
{
	int maxlen = strlen(str)+1;
	char *a1 = (char*)alloca(maxlen);
	char *a2 = (char*)alloca(maxlen);
	int n = sscanf(str,fmt,a1,a2);
	switch (n) {
	case 2: s2->assign(a2);
	case 1: s1->assign(a1);
	}
	return n;
}
int myscanf(const char *str, const char *fmt, string *s1, string *s2, string *s3)
{
	int maxlen = strlen(str)+1;
	char *a1 = (char*)alloca(maxlen);
	char *a2 = (char*)alloca(maxlen);
	char *a3 = (char*)alloca(maxlen);
	int n = sscanf(str,fmt,a1,a2,a3);
	switch (n) {
	case 3: s3->assign(a3);
	case 2: s2->assign(a2);
	case 1: s1->assign(a1);
	}
	return n;
}
int myscanf(const char *str, const char *fmt, string *s1, string *s2, string *s3, string *s4)
{
	int maxlen = strlen(str)+1;
	char *a1 = (char*)alloca(maxlen);
	char *a2 = (char*)alloca(maxlen);
	char *a3 = (char*)alloca(maxlen);
	char *a4 = (char*)alloca(maxlen);
	int n = sscanf(str,fmt,a1,a2,a3,a4);
	switch (n) {
	case 4: s4->assign(a4);
	case 3: s3->assign(a3);
	case 2: s2->assign(a2);
	case 1: s1->assign(a1);
	}
	return n;
}

#if 0
string format(const char *fmt, string s1) {
	return format(fmt,s1.c_str());
}
string format(const char *fmt, string s1, string s2) {
	return format(fmt,s1.c_str(),s2.c_str());
}
string format(const char *fmt, string s1, string s2, string s3) {
	return format(fmt,s1.c_str(),s2.c_str(),s3.c_str());
}
string format(const char *fmt, string s1, int i1) {
	return format(fmt,s1.c_str(),i1);
}
string format(const char *fmt, int i1, string s1) {
	return format(fmt,i1,s1.c_str());
}
string format(const char *fmt, string s1, string s2, int i1) {
	return format(fmt,s1.c_str(),s2.c_str(),i1);
}
string format(const char *fmt, string s1, string s2, int i1, int i2) {
	return format(fmt,s1.c_str(),s2.c_str(),i1,i2);
}
#endif

// Return time in seconds with high resolution.
// Note: In the past I found this to be a surprisingly expensive system call in linux.
double timef()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_usec / 1000000.0 + tv.tv_sec;
}

const string timestr(unsigned fieldwidth)	// Use to pick the number of chars in the output.
{
	struct timeval tv;
	struct tm tm;
	gettimeofday(&tv,NULL);
	localtime_r(&tv.tv_sec,&tm);
	unsigned tenths = tv.tv_usec / 100000;	// Rounding down is ok.
	string result = format(" %02d:%02d:%02d.%1d",tm.tm_hour,tm.tm_min,tm.tm_sec,tenths);
	return result.substr(fieldwidth >= result.size() ? 0 : result.size() - fieldwidth);
	//switch (maxfield) {
	//case 'h': case 'H': return format("%02d:%02d:%02d.%1d",tm.tm_hour,tm.tm_min,tm.tm_sec,tenths);
	//case 'm': case 'M': return format("%02d:%02d.%1d",tm.tm_min,tm.tm_sec,tenths);
	//case 's': case 'S': return format("%02d.%1d",tm.tm_sec,tenths);
	//default:            return format(" %02d:%02d:%02d.%1d",tm.tm_hour,tm.tm_min,tm.tm_sec,tenths);
	//}
}

const string timestr() { return timestr(12); }

// High resolution sleep for the specified time.
// Return FALSE if time is already past.
void sleepf(double howlong)
{
	if (howlong <= 0.00001) return;		// Less than 10 usecs, forget it.
	usleep((useconds_t) (1000000.0 * howlong));
}

//bool sleepuntil(double untilwhen)
//{
	//double now = timef();
	//double howlong = untilwhen - now;		// Fractional time in seconds.
	// We are not worrying about overflow because all times should be in the near future.
	//if (howlong <= 0.00001) return false;		// Less than 10 usecs, forget it.
	//sleepf(sleeptime);
//}

string Text2Str::str() const
{
	ostringstream ss;
	text(ss);
	return ss.str();
}

ostream& operator<<(std::ostream& os, const Text2Str *val)
{
	ostringstream ss;
	if (val) {
		val->text(ss);
		os << ss.str(); 
	} else {
		os << "(null)";
	}
	return os;
}

// Greatest Common Denominator.
// This is by Doug Brown.
int gcd(int x, int y)
{
	if (x > y) {
		return x % y == 0 ? y : gcd(y, x % y);
	} else {
		return y % x == 0 ? x : gcd(x, y % x);
	}
}


// Split a C string into an argc,argv array in place; the input string is modified.
// Returns argc, and places results in argv, up to maxargc elements.
// The final argv receives the rest of the input string from maxargc on,
// even if it contains additional splitchars.
// The correct idiom for use is to make a copy of your string, like this:
// char *copy = strcpy((char*)alloca(the_string.length()+1),the_string.c_str());
// char *argv[2];
// int argc = cstrSplit(copy,argv,2,NULL);
// If you want to detect the error of too many arguments, add 1 to argv, like this:
// char *argv[3];
// int argc = cstrSplit(copy,argv,3,NULL);
// if (argc == 3) { error("too many arguments"; }
int cstrSplit(char *in, char **pargv,int maxargc, const char *splitchars)
{
	if (splitchars == NULL) { splitchars = " \t\r\n"; }	// Default is any space.
	int argc = 0;
	while (argc < maxargc) {
		while (*in && strchr(splitchars,*in)) {in++;}	// scan past any splitchars
		if (! *in) return argc;					// return if finished.
		pargv[argc++] = in;						// save ptr to start of arg.
		in = strpbrk(in,splitchars);			// go to end of arg.
		if (!in) return	argc;					// return if finished.
		*in++ = 0;								// zero terminate this arg.
	}
	return argc;
}


// Return pointer to the nth (1 for first) space-separated non-empty argument from this string, and length in plength.
// Note that strtok is not thread safe.
char *cstrGetArg(const char *in, int nth, unsigned *length)
{
	const char *result, *cp = in;
	while (*cp && nth-- > 0) {
		while (*cp && isspace(*cp)) { cp++; }
		result = cp;
		while (*cp && !isspace(*cp)) { cp++; }
		if (nth == 0) {
			*length = cp - result;
			// remove the ever-to-be-hated const for the convenience of our callers.
			return *length ? const_cast<char*>(result) : NULL;
		}
	}
	return NULL;
}


vector<string>& stringSplit(vector<string> &result,const char *input)
{
	char *argv[40];
	//char buf[202];
	//strncpy(buf,input,200); buf[200] = 0;
	char *buf = strdup(input);
	int cnt = cstrSplit(buf,argv,40,NULL);
	for (int i = 0; i < cnt; i++) {
		result.push_back(string(argv[i]));
	}
	free(buf);
	return result;
}

// Print a table formatted as a vector of vector of strings.
// The columns will be aligned.
// Column size is determined from the columns.
// An entry of "_" is suppressed.

void printPrettyTable(prettyTable_t &tab, ostream&os, bool tabSeparated)
{
	LOG(DEBUG);
	const unsigned maxcols = 30;
	// Determine the maximum width of each column.
	int width[maxcols]; memset(width,0,sizeof(width));
	if (!tabSeparated) {
	  for (prettyTable_t::iterator it = tab.begin(); it != tab.end(); ++it) {
		std::vector<std::string> &row = *it;
		for (unsigned col = 0; col<maxcols && col<row.size(); col++) {
			int colwidth = row[col].size();
			if (colwidth > 100) colwidth = 100;
			width[col] = std::max(width[col],colwidth);
		}
	  }
	}
	// Now print it.
	for (unsigned nrow = 0; nrow < tab.size(); nrow++) {
		vector<string> &row = tab[nrow];

		// DEBUG: print the column widths.
		if (0 && IS_LOG_LEVEL(DEBUG) && nrow == 0) {
			for (unsigned col = 0; col<maxcols && col<row.size(); col++) {
				os << format("%-*s ",width[col],format("%d",width[col]));
			}
			os << "\n";
		}

		for (unsigned col = 0; col<maxcols && col<row.size(); col++) {
			char buf[252];
			const char *val = row[col].c_str();
			if (0 == strcmp(val,"_")) { val = ""; }	// Dont print "_", used to easily format the header fields.
			if (tabSeparated) {
				if (col) os << "\t";
				os << val;
			} else {
				// (pat) This code centered the headers, but I took it out.
				//int pad = 0;
				//if (nrow < 2) { pad = (width[col] - strlen(val)) / 2; }
				//sprintf(buf,"%*s%.*s ",(width[col]-pad),val,pad,"                         ");
				snprintf(buf,250,"%-*s ",width[col],val);
				os << buf;
			}
		}
		os << "\n";
	}
	os << std::endl;
}



ostream& operator<<(std::ostream& os, const Statistic<int> &stat) { stat.text(os); return os; }
ostream& operator<<(std::ostream& os, const Statistic<unsigned> &stat) { stat.text(os); return os; }
ostream& operator<<(std::ostream& os, const Statistic<float> &stat) { stat.text(os); return os; }
ostream& operator<<(std::ostream& os, const Statistic<double> &stat) { stat.text(os); return os; }

string replaceAll(const std::string input, const std::string search, const std::string replace)
{
	string output = input;
 	unsigned index1 = 0;

	while (index1 < output.size()) {
	  try {
		index1 = output.find(search, index1);
		if (index1 == string::npos) {
			break;
		}

		output.replace(index1, search.length(), replace);
		// We want to scan past the piece we just replaced.
		index1 += replace.length();
	  } catch (...) {
		LOG(ERR) << "string replaceAll error"<<LOGVAR(index1)<<LOGVAR(input)<<LOGVAR(search)<<LOGVAR(replace)<<LOGVAR(output);
	  	break;
	  }
	}

	return output;
}

// Efficient string concatenation.
string stringcat(string a, string b, string c, string d, string e, string f, string g)
{
	string result;
	result.reserve(a.size() + b.size() + c.size() + d.size() + e.size() + f.size() + g.size());
	result.append(a);
	result.append(b);
	result.append(c);
	result.append(d);
	result.append(e);
	result.append(f);
	result.append(g);
	return result;
}

static string emptystring("");

string stringcat(string a, string b) {
	return stringcat(a,b,emptystring,emptystring,emptystring,emptystring,emptystring);
}
string stringcat(string a, string b, string c) {
	return stringcat(a,b,c,emptystring,emptystring,emptystring,emptystring);
}
string stringcat(string a, string b, string c, string d) {
	return stringcat(a,b,c,d,emptystring,emptystring,emptystring);
}
string stringcat(string a, string b, string c, string d, string e) {
	return stringcat(a,b,c,d,e,emptystring,emptystring);
}
string stringcat(string a, string b, string c, string d, string e, string f) {
	return stringcat(a,b,c,d,e,f,emptystring);
}

void stringToUint(string strRAND, uint64_t *hRAND, uint64_t *lRAND)
{
	assert(strRAND.size() == 32);
	string strhRAND = strRAND.substr(0, 16);
	string strlRAND = strRAND.substr(16, 16);
	stringstream ssh;
	ssh << hex << strhRAND;
	ssh >> *hRAND;
	stringstream ssl;
	ssl << hex << strlRAND;
	ssl >> *lRAND;
}

string uintToString(uint64_t h, uint64_t l)
{
	ostringstream os1;
	os1.width(16);
	os1.fill('0');
	os1 << hex << h;
	ostringstream os2;
	os2.width(16);
	os2.fill('0');
	os2 << hex << l;
	ostringstream os3;
	os3 << os1.str() << os2.str();
	return os3.str();
}

string uintToString(uint32_t x)
{
	ostringstream os;
	os.width(8);
	os.fill('0');
	os << hex << x;
	return os.str();
}
};
