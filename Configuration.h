/*
* Copyright 2009, 2010 Free Software Foundation, Inc.
* Copyright 2010 Kestrel Signal Processing, Inc.
* Copyright 2011, 2012, 2014 Range Networks, Inc.
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


#ifndef CONFIGURATION_H
#define CONFIGURATION_H


#include "sqlite3util.h"

#include <assert.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <Threads.h>
#include <stdint.h>


/** A class for configuration file errors. */
class ConfigurationTableError {};
extern char gCmdName[];	// Gotta be global, gotta be char*, gotta love it.

/** An exception thrown when a given config key isn't found. */
class ConfigurationTableKeyNotFound : public ConfigurationTableError {

	private:

	std::string mKey;

	public:

	ConfigurationTableKeyNotFound(const std::string& wKey)
		:mKey(wKey)
	{ }

	const std::string& key() const { return mKey; }

};


// (pat) 10-5-2013 Add the key to this record so we can print better error messages,
// and warn if number() of floatNumber() have non-numeric values.
class ConfigurationRecord {

	private:

	std::string mCRKey;
	std::string mValue;
	bool mDefined;
	mutable bool mCRWarned;

	public:

	ConfigurationRecord() : mDefined(false), mCRWarned(false) {}
	ConfigurationRecord(const std::string &key,bool wDefined):
		mCRKey(key),
		mDefined(wDefined),
		mCRWarned(false)
	{ }

	ConfigurationRecord(const std::string&key, const std::string& wValue):
		mCRKey(key),
		mValue(wValue),
		//mNumber(strtol(wValue.c_str(),&endptr,0)),
		mDefined(true),
		mCRWarned(false)
	{ }

	ConfigurationRecord(const std::string&key, const char* wValue):
		mCRKey(key),
		mValue(std::string(wValue)),
		//mNumber(strtol(wValue.c_str(),&endptr,0)),
		mDefined(true),
		mCRWarned(false)
	{ }


	const std::string& value() const { return mValue; }
	long number() const;
	bool defined() const { return mDefined; }

	float floatNumber() const;

};


/** A string class that uses a hash function for comparison. */
class HashString : public std::string {


	protected:

	uint64_t mHash;

	void computeHash();


	public:

	HashString(const char* src)
		:std::string(src)
	{
		computeHash();
	}

	HashString(const std::string& src)
		:std::string(src)
	{
		computeHash();
	}

	HashString()
	{
		mHash=0;
	}

	HashString& operator=(std::string& src)
	{
		std::string::operator=(src);
		computeHash();
		return *this;
	}

	HashString& operator=(const char* src)
	{
		std::string::operator=(src);
		computeHash();
		return *this;
	}

	bool operator==(const HashString& other)
	{
		return mHash==other.mHash;
	}

	bool operator<(const HashString& other)
	{
		return mHash<other.mHash;
	}

	bool operator>(const HashString& other)
	{
		return mHash<other.mHash;
	}

	uint64_t hash() const { return mHash; }

};


typedef std::map<std::string, ConfigurationRecord> ConfigurationRecordMap;
typedef std::map<HashString, ConfigurationRecord> ConfigurationMap;
class ConfigurationKey;
typedef std::map<std::string, ConfigurationKey> ConfigurationKeyMap;
ConfigurationKeyMap getConfigurationKeys();

/**
	A class for maintaining a configuration key-value table,
	based on sqlite3 and a local map-based cache.
	Thread-safe, too.
*/
class ConfigurationTable {

	private:

	sqlite3* mDB;				///< database connection
	ConfigurationMap mCache;	///< cache of recently access configuration values
	mutable Mutex mLock;		///< control for multithreaded access to the cache
	std::vector<std::string> (*mCrossCheck)(const std::string&);	///< cross check callback pointer

	public:

	ConfigurationKeyMap mSchema;///< definition of configuration default values and validation logic

	// (pat) filename is the sql file name, wCmdName is the name of the executable we are running, used for better error messages.
	ConfigurationTable(const char* filename = ":memory:", const char *wCmdName = 0, ConfigurationKeyMap wSchema = ConfigurationKeyMap());

	/** Generate an up-to-date example sql file for new installs. */
	std::string getDefaultSQL(const std::string& program, const std::string& version);

	/** Generate an up-to-date TeX snippet. */
	std::string getTeX(const std::string& program, const std::string& version);

	/** Return true if the key is used in the table.  */
	bool defines(const std::string& key);

	/** Return true if the application's schema knows about this key. */
	bool keyDefinedInSchema(const std::string& name);

	/** Return true if the provided value validates correctly against the defined schema. */
	bool isValidValue(const std::string& name, const std::string& val);

	/** Return true if the provided value validates correctly against the defined schema. */
	bool isValidValue(const std::string& name, const int val) { std::stringstream ss; ss << val; return isValidValue(name, ss.str()); }

	/** Return a map of all similar keys in the defined schema. */
	ConfigurationKeyMap getSimilarKeys(const std::string& snippet);

	/** Return true if this key is identified as static. */
	bool isStatic(const std::string& key);

	/**
		Get a string parameter from the table.
		Throw ConfigurationTableKeyNotFound if not found.
	*/
	std::string getStr(const std::string& key);


	/**
		Get a boolean from the table.
		Return false if NULL or 0, true otherwise.
	*/
	bool getBool(const std::string& key);

	/**
		Get a numeric parameter from the table.
		Throw ConfigurationTableKeyNotFound if not found.
	*/
	long getNum(const std::string& key);

	/**
		Get a vector of strings from the table.
	*/
	std::vector<std::string> getVectorOfStrings(const std::string& key);

	/**
		Get a float from the table.
		Throw ConfigurationTableKeyNotFound if not found.
	*/
	float getFloat(const std::string& key);

	/**
		Get a numeric vector from the table.
	*/
	std::vector<unsigned> getVector(const std::string& key);

	/** Get length of a vector */
	unsigned getVectorLength(const std::string &key) 
		{ return getVector(key).size(); }

	/** Set or change a value in the table.  */
	bool set(const std::string& key, const std::string& value);

	/** Set or change a value in the table.  */
	bool set(const std::string& key, long value);

	/**
		Remove an entry from the table.
		Will not alter required values.
		@param key The key of the item to be removed.
		@return true if anything was actually removed.
	*/
	bool remove(const std::string& key);

	/** Search the table, dumping to a stream. */
	void find(const std::string& pattern, std::ostream&) const;

	/** Return all key/value pairs stored in the ConfigurationTable */
	ConfigurationRecordMap getAllPairs() const;

	/** Define the callback to purge the cache whenever the database changes. */
	void setUpdateHook(void(*)(void *,int ,char const *,char const *,sqlite3_int64));

	/** Define the callback for cross checking. */
	void setCrossCheckHook(std::vector<std::string> (*wCrossCheck)(const std::string&));

	/** Execute the application specific value cross checking logic. */
	std::vector<std::string> crossCheck(const std::string& key);

	/** purege cache if it exceeds a certain age */
	void checkCacheAge();

	/** Delete all records from the cache. */
	void purge();


	private:

	/**
		Attempt to lookup a record, cache if needed.
		Throw ConfigurationTableKeyNotFound if not found.
		Caller should hold mLock because the returned reference points into the cache.
	*/
	const ConfigurationRecord& lookup(const std::string& key);

};


typedef std::map<HashString, std::string> HashStringMap;

class SimpleKeyValueException : public std::exception {
	std::string mWhy;
	public:
	const char *what() const throw() { return mWhy.c_str(); }
	SimpleKeyValueException(std::string wWhy) : mWhy(wWhy) {}
	// This is dorky.  We have to define the no-op destructor because the default destructor has the wrong throw() content,
	// and even though the throw content is not used by the compiler.  What a balls-up.
	~SimpleKeyValueException() throw() {}
};

class SimpleKeyValue {

	protected:

	HashStringMap mMap;

	public:

	/** Take a C string "A=B" and set map["A"]="B". */
	void addItem(const char*);

	/** Take a C string "A=B C=D E=F ..." and add all of the pairs to the map. */
	void addItems(const char*s);

	/** Return a reference to the string at map["key"]. */
	const char* get(const char*) const;

	std::string getStrOrBust(const char *key) const;
	long getNumOrBust(const char *key) const;
	long getNum(const char *key, bool &valid) const;
};


class ConfigurationKey {

	public:

	enum VisibilityLevel
	{
		CUSTOMER,
		CUSTOMERSITE,
		CUSTOMERTUNE,
		CUSTOMERWARN,
		DEVELOPER,
		FACTORY
	};

	enum Type
	{
		BOOLEAN,
		CHOICE_OPT,
		CHOICE,
		CIDR_OPT,		// (pat) IP Route, eg: "xxx.xxx.xxx.xxx/yy"
		CIDR,
		FILEPATH_OPT,
		FILEPATH,
		HOSTANDPORT_OPT,
		HOSTANDPORT,	// (pat) hostname is allowed.
		IPADDRESS_OPT,
		IPADDRESS,		// (pat) IP Address but hostnames not allowed.
		IPANDPORT,
		MIPADDRESS_OPT,
		MIPADDRESS,		// (pat) List of IP addresses + optional ports, used only for the Neighbors config option.
		PORT_OPT,
		PORT,			// (pat) An IP port.
		REGEX_OPT,
		REGEX,
		STRING_OPT,
		STRING,
		VALRANGE 		// (pat) string format is: <min_value> ':' <max_value> [ '(' <step> ')' ]
						// step is not currently enforced.
						// To allow floating point add a decimal point to the range, eg: "0.0:10.0"
	};

	enum Scope
	{
		GLOBALLYUNIQUE = 1,		// (pat) value must be unique on all nodes.
		GLOBALLYSAME = 2,		// (pat) value must be the same on all nodes.
		NEIGHBORSUNIQUE = 4,	// (pat) value must be unique on all neighbors
		NEIGHBORSSAME = 8,		// (pat) value must be the same on all neighbors
		NODESPECIFIC = 16
	};

	private:

	std::string mName;
	std::string mDefaultValue;
	std::string mUnits;
	VisibilityLevel mVisibility;
	Type mType;
	std::string mValidValues;
	bool mIsStatic;
	std::string mDescription;
	unsigned mScope;


	public:

	ConfigurationKey(const std::string& wName, const std::string& wDefaultValue, const std::string& wUnits, const VisibilityLevel wVisibility, const Type wType, const std::string& wValidValues, bool wIsStatic, const std::string& wDescription, unsigned wScope = 0):
		mName(wName),
		mDefaultValue(wDefaultValue),
		mUnits(wUnits),
		mVisibility(wVisibility),
		mType(wType),
		mValidValues(wValidValues),
		mIsStatic(wIsStatic),
		mDescription(wDescription),
		mScope(wScope)
	{ }

	ConfigurationKey()
	{ }

	const std::string& getName() const { return mName; }
	const std::string& getDefaultValue() const { return mDefaultValue; }
	void updateDefaultValue(const std::string& newValue) { mDefaultValue = newValue; }
	void updateDefaultValue(const int newValue) { std::stringstream ss; ss << newValue; updateDefaultValue(ss.str()); }
	void updateValidValues(const std::string& newValue) { mValidValues = newValue; }
	const std::string& getUnits() const { return mUnits; }
	const VisibilityLevel& getVisibility() const { return mVisibility; }
	const Type& getType() const { return mType; }
	const std::string& getValidValues() const { return mValidValues; }
	bool isStatic() const { return mIsStatic; }
	const std::string& getDescription() const { return mDescription; }
	unsigned getScope() const { return mScope; }

	static bool isValidIP(const std::string& ip);
	static void getMinMaxStepping(const ConfigurationKey &key, std::string &min, std::string &max, std::string &stepping);
	template<class T> static bool isInValRange(const ConfigurationKey &key, const std::string& val, const bool isInteger);
	static const std::string visibilityLevelToString(const VisibilityLevel& visibility);
	static const std::string typeToString(const ConfigurationKey::Type& type);
	static void printKey(const ConfigurationKey &key, const std::string& currentValue, std::ostream& os);
	static void printDescription(const ConfigurationKey &key, std::ostream& os);
};


#endif


// vim: ts=4 sw=4
