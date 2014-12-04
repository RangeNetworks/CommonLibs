/*
* Copyright 2009, 2010 Free Software Foundation, Inc.
* Copyright 2010 Kestrel Signal Processing, Inc.
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

// (pat) Logging is via rsyslogd controlled by /etc/rsyslog.d/OpenBTS.conf

// (pat) WARNING is stupidly defined in /usr/local/include/osipparser2/osip_const.h.
// This must be outside the #ifndef LOGGER_H to fix it as long as Logger.h included after the above file.
#ifdef WARNING
#undef WARNING
#endif

#ifndef LOGGER_H
#define LOGGER_H

#include <syslog.h>
#include <stdint.h>
#include <stdio.h>
#include <sstream>
#include <list>
#include <map>
#include <string>
#include <assert.h>
#include <sys/syscall.h>
// We cannot include Utils.h because it includes Logger.h, so just declare timestr() here.
// If timestr decl is changed G++ will whine when Utils.h is included.
namespace Utils { const std::string timestr(); };

#if !defined(gettid)
# define gettid() syscall(SYS_gettid)
#endif // !defined(gettid)

extern pid_t gPid;
#define _LOG(level) \
	Log(LOG_##level).get() <<gPid <<":"<<gettid() \
	<< Utils::timestr(100,true) << " " __FILE__  ":"  << __LINE__ << ":" << __FUNCTION__ << ": "

// (pat) If you '#define LOG_GROUP groupname' before including Logger.h, then you can set Log.Level.groupname as well as Log.Level.filename.
#ifdef LOG_GROUP
//#define CHECK_GROUP_LOG_LEVEL(groupname,loglevel) gCheckGroupLogLevel(#groupname,loglevel)
//#define IS_LOG_LEVEL(wLevel) (CHECK_GROUP_LOG_LEVEL(LOG_GROUP,LOG_##wLevel) || gGetLoggingLevel(__FILE__)>=LOG_##wLevel)
#define IS_LOG_LEVEL(wLevel) (gCheckGroupLogLevel(LOG_GROUP,LOG_##wLevel) || gGetLoggingLevel(__FILE__)>=LOG_##wLevel)
#define IS_WATCH_LEVEL(wLevel) gCheckGroupWatchLevel(LOG_GROUP,LOG_##wLevel)
#else
#define IS_WATCH_LEVEL(wLevel) (gGetLoggingLevel(__FILE__)>=LOG_##wLevel)
#define IS_LOG_LEVEL(wLevel) (gGetLoggingLevel(__FILE__)>=LOG_##wLevel)
#endif

#ifdef NDEBUG
#define LOG(wLevel) \
	if (LOG_##wLevel!=LOG_DEBUG && IS_LOG_LEVEL(wLevel)) _LOG(wLevel)
#else
#define LOG(wLevel) \
	if (IS_LOG_LEVEL(wLevel)) _LOG(wLevel)
#endif

// pat: And for your edification here are the 'levels' as defined in syslog.h:
// LOG_EMERG   0  system is unusable
// LOG_ALERT   1  action must be taken immediately
// LOG_CRIT    2  critical conditions
// LOG_ERR     3  error conditions
// LOG_WARNING 4  warning conditions
// LOG_NOTICE  5  normal, but significant, condition
// LOG_INFO    6  informational message
// LOG_DEBUG   7  debug-level message

// (pat) added - print out a var and its name.
// Use like this: int descriptive_name; LOG(INFO)<<LOGVAR(descriptive_name);
#define LOGVAR2(name,val) " " << name << "=" << (val)
#define LOGVAR(var) (" " #var "=") << var
#define LOGVARM(var) " " << &#var[1] << "=" << var      // Strip the first char ("m") off the var name when printing.
#define LOGVARP2(name,val) (" " name "=(") << val <<")"		// Put value in parens; used for classes.
#define LOGVARP(var) LOGVARP2(#var,var)						// Put value in parens; used for classes.
// (pat) 3-2014: Use (unsigned long) for LOGHEX so it can be used for pointers with 64-bit compiler.  What a choke.
#define LOGHEX(var) (" " #var "=0x") << std::hex << ((unsigned long long)var) << std::dec
#define LOGHEX2(name,val) " " << name << "=0x" << std::hex << ((unsigned long)(val)) << std::dec
// These are kind of cheesy, but you can use for bitvector
#define LOGBV2(name,val) " " << name << "=(" << val<<" size:"<<val.size()<<")"
#define LOGBV(bv) LOGBV2(#bv,bv)
#define LOGVARRANGE(name,cur,lo,hi) " "<<name <<"=("<<(cur) << " range:"<<(lo) << " to "<<(hi) <<")"


#define OBJLOG(wLevel) \
	LOG(wLevel) << "obj: " << this << ' '

#define LOG_ASSERT(x) { if (!(x)) LOG(EMERG) << "assertion " #x " failed"; } assert(x);

// (pat) The WATCH and WATCHF macros print only to the console.  Pat uses them for debugging.
// The WATCHINFO macro prints an INFO level message that is also printed to the console if the log level is DEBUG.
// Beware that the arguments are evaluated multiple times.
#define WATCHF(...)  { LOG(DEBUG)<<format(__VA_ARGS__); if (IS_WATCH_LEVEL(DEBUG)) {printf("%s ",timestr(7).c_str()); printf(__VA_ARGS__);} }
#define WATCHLEVEL(level,...) if (IS_WATCH_LEVEL(level)) {std::cout << timestr(7)<<" "<<__VA_ARGS__ << std::endl;}
#define WATCH(...) { LOG(DEBUG)<<__VA_ARGS__; if (IS_WATCH_LEVEL(DEBUG)) {std::cout << timestr(7)<<" "<<__VA_ARGS__ << endl;} }
#define WATCHINFO(...) { LOG(INFO)<<__VA_ARGS__; if (IS_WATCH_LEVEL(INFO)) {std::cout << timestr(7)<<" "<<__VA_ARGS__ << endl;} }


//#include "Threads.h"		// must be after defines above, if these files are to be allowed to use LOG()
//#include "Utils.h"

/**
	A C++ stream-based thread-safe logger.
	Derived from Dr. Dobb's Sept. 2007 issue.
	Updated to use syslog.
	This object is NOT the global logger;
	every log record is an object of this class.
*/
class Log {

	public:

	protected:

	std::ostringstream mStream;		///< This is where we buffer up the log entry.
	int mPriority;					///< Priority of current report.
	bool mDummyInit;

	public:

	Log(int wPriority)
		:mPriority(wPriority), mDummyInit(false)
	{ }

	// (pat) This constructor is not used to construct a Log record, it is called once per application
	// to init the syslog facility.  This is a very poor use of C++.
	Log(const char* name, const char* level=NULL, int facility=LOG_USER);

	// Most of the work is in the destructor.
	/** The destructor actually generates the log entry. */
	~Log();

	std::ostringstream& get();
};
extern bool gLogToConsole;	// Pat added for easy debugging.


// (pat) Added logging by explicit group name.
class LogGroup {
	public:
	// These must exactly match LogGroup::mGroupNames:
	// That kinda sucks, but using static data prevents any constructor race.
	enum Group {
		Control,
		SIP,
		GSM,
		GPRS,
		Layer2,
		SMS,
		_NumberOfLogGroups
	};
	void setAll();	// Update mDebugLevel from the Log.Group.... config database options.

	int8_t mDebugLevel[_NumberOfLogGroups];	// use int in case a -1 value gets in here.
	int8_t mWatchLevel[_NumberOfLogGroups];	// use int in case a -1 value gets in here.
	LogGroup();
	void LogGroupInit();
	private:
	static const char *mGroupNames[_NumberOfLogGroups+1];	// We add a NULL at the end.
	Group groupNameToIndex(const char *) const;		// unused.
};
extern LogGroup gLogGroup;

// We inline this:
static __inline__ bool gCheckGroupLogLevel(LogGroup::Group group, unsigned level) {
	assert(group < LogGroup::_NumberOfLogGroups);
	//_LOG(DEBUG) << LOGVAR(group)<<LOGVAR(level)<<LOGVAR2("stashed",(unsigned) gLogGroup.mDebugLevel[group]);
	return gLogGroup.mDebugLevel[group] >= (int) level;
}
static __inline__ bool gCheckGroupWatchLevel(LogGroup::Group group, unsigned level) {
	assert(group < LogGroup::_NumberOfLogGroups);
	//_LOG(DEBUG) << LOGVAR(group)<<LOGVAR(level)<<LOGVAR2("stashed",(unsigned) gLogGroup.mDebugLevel[group]);
	return gLogGroup.mWatchLevel[group] >= (int) level;
}



std::list<std::string> gGetLoggerAlarms();		///< Get a copy of the recent alarm list.


/**@ Global control and initialization of the logging system. */
//@{


/** Initialize the global logging system with filename  test  10*/
void gLogInitWithFile(const char* name, const char* level, int facility, char* LogFilePath=NULL);

/** Initialize the global logging system. */
void gLogInit(const char* name, const char* level=NULL, int facility=LOG_USER);
/** Get the logging level associated with a given file. */
int gGetLoggingLevel(const char *filename=NULL);
/** Allow early logging when still in constructors */
void gLogEarly(int level, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
//@}

// (pat) This is historical, some files include Logger.h and expect to get these too.  These should be removed.
#include "Threads.h"		// must be after defines above, if these files are to be allowed to use LOG()
#include "Utils.h"

#endif

// vim: ts=4 sw=4
