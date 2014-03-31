/*
* Copyright 2010 Kestrel Signal Processing, Inc.
* All rights reserved.
*/


#include "sqlite3.h"
#include "sqlite3util.h"
#include "Logger.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <string>
#include <vector>

// Wrappers to sqlite operations.
// These will eventually get moved to commonlibs.

const char* enableWAL = {
	"PRAGMA journal_mode=WAL"
};

int sqlite3_prepare_statement(sqlite3* DB, sqlite3_stmt **stmt, const char* query, unsigned retries)
{
        int src = SQLITE_BUSY;

	for (unsigned i = 0; i < retries; i++) {
		src = sqlite3_prepare_v2(DB,query,strlen(query),stmt,NULL);
		if (src != SQLITE_BUSY && src != SQLITE_LOCKED) {
			break;
		}
		usleep(200);
	}
        if (src) {
                LOG(ERR)<< format("sqlite3_prepare_v2 failed code=%u for \"%s\": %s\n",src,query,sqlite3_errmsg(DB));
                sqlite3_finalize(*stmt);
        }
        return src;
}

int sqlite3_run_query(sqlite3* DB, sqlite3_stmt *stmt, unsigned retries)
{
	int src = SQLITE_BUSY;

        for (unsigned i = 0; i < retries; i++) {
                src = sqlite3_step(stmt);
		if (src != SQLITE_BUSY && src != SQLITE_LOCKED) {
                        break;
                }
                usleep(200);
        }
	if ((src!=SQLITE_DONE) && (src!=SQLITE_ROW)) {
		LOG(ERR) << format("sqlite3_run_query failed code=%u for: %s: %s\n", src, sqlite3_sql(stmt), sqlite3_errmsg(DB));
	}
	return src;
}

// condition buffer is size 100 minimum.
static const unsigned sqlBufferSize = 100;
static const char *condition_c(char *resultBuffer,const char *keyName, const char*keyValue)
{
	snprintf(resultBuffer,sqlBufferSize,"WHERE %s == '%s'",keyName,keyValue);
	return resultBuffer;
}

static const char *condition_u(char *resultBuffer,const char *keyName, unsigned keyValue)
{
	snprintf(resultBuffer,sqlBufferSize,"WHERE %s == %u",keyName,keyValue);
	return resultBuffer;
}

void sqlQuery::queryStart(sqlite3*db, const char *tableName,const char *resultColumns, const char*condition)
{
	int retries = 5;
	mdb = db;
	mQueryRC = SQLITE_ERROR;	// Until we know better.
	//size_t stringSize = sqlBufferSize + strlen(resultColumns) + strlen(tableName) + strlen(condition);
	//char query[stringSize];
	//snprintf(query,stringSize,"SELECT %s FROM %s %s",resultColumns,tableName,condition);
	// We save the query in a string so the caller can print it out in error messages if the query fails.
	mQueryString = format("SELECT %s FROM %s %s",resultColumns,tableName,condition);
	// Prepare the statement.
	if (sqlite3_prepare_statement(mdb,&mStmt,mQueryString.c_str(),retries)) { mStmt = NULL; return; }
	// Read the result.
	mQueryRC = sqlite3_run_query(mdb,mStmt,retries);
}

// Load the next row.  Return true if there is another row, false if finished or error.
bool sqlQuery::sqlStep()
{
	if (mQueryRC == SQLITE_ROW) {
		// Get the next row.
		mQueryRC = sqlite3_run_query(mdb,mStmt,5);
	}
	return mQueryRC == SQLITE_ROW;
}

sqlQuery::sqlQuery(sqlite3*db, const char *tableName,const char *resultColumns,const char *condition)
{
	queryStart(db,tableName,resultColumns,condition);
}

sqlQuery::sqlQuery(sqlite3*db, const char *tableName,const char *resultColumns,const char *keyName, unsigned keyData)
{
	char conditionBuffer[sqlBufferSize];
	queryStart(db,tableName,resultColumns,condition_u(conditionBuffer,keyName,keyData));
}

sqlQuery::sqlQuery(sqlite3*db, const char *tableName,const char *resultColumns,const char *keyName, const char *keyData)
{
	char conditionBuffer[sqlBufferSize];
	queryStart(db,tableName,resultColumns,condition_c(conditionBuffer,keyName,keyData));
}

sqlQuery::~sqlQuery()
{
	if (mStmt) sqlite3_finalize(mStmt);
}

std::string sqlQuery::getResultText(int colNum)
{
	if (sqlSuccess()) {
		const char* ptr = (const char*)sqlite3_column_text(mStmt,colNum);
		return ptr ? std::string(ptr,sqlite3_column_bytes(mStmt,colNum)) : std::string("");
	}
	return std::string("");
}

sqlite3_int64 sqlQuery::getResultInt(int colNum)
{
	return sqlSuccess() ? sqlite3_column_int64(mStmt,colNum) : 0;
}



bool sqlite3_exists(sqlite3* DB, const char *tableName,
		const char* keyName, const char* keyData, unsigned retries)
{
	size_t stringSize = 100 + strlen(tableName) + strlen(keyName) + strlen(keyData);
	char query[stringSize];
	sprintf(query,"SELECT * FROM %s WHERE %s == \"%s\"",tableName,keyName,keyData);
	// Prepare the statement.
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_statement(DB,&stmt,query,retries)) return false;
	// Read the result.
	int src = sqlite3_run_query(DB,stmt,retries);
	sqlite3_finalize(stmt);
	// Anything there?
	return (src == SQLITE_ROW);
}



bool sqlite3_single_lookup(sqlite3* DB, const char *tableName,
		const char* keyName, const char* keyData,
		const char* valueName, unsigned &valueData, unsigned retries)
{
	size_t stringSize = 100 + strlen(valueName) + strlen(tableName) + strlen(keyName) + strlen(keyData);
	char query[stringSize];
	sprintf(query,"SELECT %s FROM %s WHERE %s == \"%s\"",valueName,tableName,keyName,keyData);
	// Prepare the statement.
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_statement(DB,&stmt,query,retries)) return false;
	// Read the result.
	int src = sqlite3_run_query(DB,stmt,retries);
	bool retVal = false;
	if (src == SQLITE_ROW) {
		valueData = (unsigned)sqlite3_column_int64(stmt,0);
		retVal = true;
	}
	sqlite3_finalize(stmt);
	return retVal;
}

#if 0	// This code works fine, but sqlQuery is a better way.
// If result is a row return the sqlite3_stmt, else NULL.
// Pass a comma-separated list of column names to return, or if you want all the columns in the result, pass "*" as the resultColumns.
// Almost all the other functions below could use this.
sqlite3_stmt *sqlite_lookup_row(sqlite3*db, const char *tableName, const char* condition, const char *resultColumns)
{
	int retries = 5;
	size_t stringSize = sqlBufferSize + strlen(resultColumns) + strlen(tableName) + strlen(condition);
	char query[stringSize];
	snprintf(query,stringSize,"SELECT %s FROM %s %s",resultColumns,tableName,condition);
	// Prepare the statement.
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_statement(db,&stmt,query,retries)) return NULL;
	// Read the result.
	int src = sqlite3_run_query(db,stmt,retries);
	if (src == SQLITE_ROW) {
		return stmt;	// Caller must sqlite3_finalize();
	}
	sqlite3_finalize(stmt);
	return NULL;
}
sqlite3_stmt *sqlite_lookup_row_c(sqlite3*db, const char *tableName, const char* keyName, const char *keyData, const char *resultColumns)
{
	char conditionBuffer[sqlBufferSize];
	return sqlite_lookup_row(db,tableName,condition_c(conditionBuffer,keyName,keyData),resultColumns);
}
sqlite3_stmt *sqlite_lookup_row_u(sqlite3*db, const char *tableName, const char* keyName, unsigned keyValue, const char* resultColumns)
{
	char conditionBuffer[sqlBufferSize];
	return sqlite_lookup_row(db,tableName,condition_u(conditionBuffer,keyName,keyValue),resultColumns);
}
// Pass a comma-separated list of column names to return, or if you want all the columns in the result, pass "*" as the resultColumns.
std::vector<std::string> sqlite_multi_lookup_vector(sqlite3* db, const char* tableName, const char* keyName, const char* keyData, const char *resultColumns)
{
	std::vector<std::string> result;
	if (sqlite3_stmt *stmt = sqlite_lookup_row_c(db,tableName,keyName,keyData,resultColumns)) {
		int n = sqlite3_column_count(stmt);
		if (n < 0 || n > 100) { goto done; }	// Would like to LOG an error but afraid to use LOG in here.
		result.reserve(n+1);
		for (int i = 0; i < n; i++) {
			const char* ptr = (const char*)sqlite3_column_text(stmt,i);
			result.push_back(ptr ? std::string(ptr,sqlite3_column_bytes(stmt,i)) : std::string(""));
		}
		done:
		sqlite3_finalize(stmt);
	}
	return result;
}
#endif


bool sqlite_single_lookup(sqlite3* db, const char* tableName,
		const char* keyName, const char* keyData,
		const char* resultName, std::string &resultData)
{
	sqlQuery query(db,tableName,resultName,keyName,keyData);
	if (query.sqlSuccess()) {
		resultData = query.getResultText();
		return true;
	}
	return false;
#if 0
	if (sqlite3_stmt *stmt = sqlite_lookup_row_c(db,tableName,keyName,keyData,valueName)) {
		if (const char* ptr = (const char*)sqlite3_column_text(stmt,0)) {
			valueData = std::string(ptr,sqlite3_column_bytes(stmt,0));
		}
		sqlite3_finalize(stmt);
		return true;
	}
	return false;
#endif
}

bool sqlite_single_lookup(sqlite3* db, const char* tableName,
		const char* keyName, unsigned keyValue,
		const char* resultName, std::string &resultData)
{
	sqlQuery query(db,tableName,resultName,keyName,keyValue);
	if (query.sqlSuccess()) {
		resultData = query.getResultText();
		return true;
	}
	return false;
#if 0
	if (sqlite3_stmt *stmt = sqlite_lookup_row_u(db,tableName,keyName,keyValue,valueName)) {
		if (const char* ptr = (const char*)sqlite3_column_text(stmt,0)) {
			valueData = std::string(ptr,sqlite3_column_bytes(stmt,0));
		}
		sqlite3_finalize(stmt);
		return true;
	}
	return false;
#endif
}

// Do the lookup and just return the string.
// For this function an empty value is indistinguishable from failure - both return an empty string.
std::string sqlite_single_lookup_string(sqlite3* db, const char* tableName,
		const char* keyName, const char* keyData, const char* resultName)
{
	return sqlQuery(db,tableName,resultName,keyName,keyData).getResultText();
#if 0
	std::string result;
	(void) sqlite_single_lookup(db,tableName,keyName,keyData,valueName,result);
	return result;
#endif
}


// This function returns an allocated string that must be free'd by the caller.
bool sqlite3_single_lookup(sqlite3* DB, const char* tableName,
		const char* keyName, const char* keyData,
		const char* valueName, char* &valueData, unsigned retries)
{
	valueData=NULL;
	size_t stringSize = 100 + strlen(valueName) + strlen(tableName) + strlen(keyName) + strlen(keyData);
	char query[stringSize];
	snprintf(query,stringSize,"SELECT %s FROM %s WHERE %s == \"%s\"",valueName,tableName,keyName,keyData);
	// Prepare the statement.
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_statement(DB,&stmt,query,retries)) return false;
	// Read the result.
	int src = sqlite3_run_query(DB,stmt,retries);
	bool retVal = false;
	if (src == SQLITE_ROW) {
		const char* ptr = (const char*)sqlite3_column_text(stmt,0);
		if (ptr) valueData = strdup(ptr);
		retVal = true;
	}
	sqlite3_finalize(stmt);
	return retVal;
}


// This function returns an allocated string that must be free'd by tha caller.
bool sqlite3_single_lookup(sqlite3* DB, const char* tableName,
		const char* keyName, unsigned keyData,
		const char* valueName, char* &valueData, unsigned retries)
{
	valueData=NULL;
	size_t stringSize = 100 + strlen(valueName) + strlen(tableName) + strlen(keyName) + 20;
	char query[stringSize];
	sprintf(query,"SELECT %s FROM %s WHERE %s == %u",valueName,tableName,keyName,keyData);
	// Prepare the statement.
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_statement(DB,&stmt,query,retries)) return false;
	// Read the result.
	int src = sqlite3_run_query(DB,stmt,retries);
	bool retVal = false;
	if (src == SQLITE_ROW) {
		const char* ptr = (const char*)sqlite3_column_text(stmt,0);
		if (ptr) valueData = strdup(ptr);
		retVal = true;
	}
	sqlite3_finalize(stmt);
	return retVal;
}

bool sqlite_set_attr(sqlite3*db,const char *attr_name,const char*attr_value)
{
	if (! sqlite3_command(db,"CREATE TABLE IF NOT EXISTS ATTR_TABLE (ATTR_NAME TEXT PRIMARY KEY, ATTR_VALUE TEXT)")) {
		const char *fn = sqlite3_db_filename(db,"main");
		LOG(WARNING) << "Could not create ATTR_TABLE in database file " <<(fn?fn:"");
		return false;
	}
	char query[100];
	snprintf(query,100,"REPLACE INTO ATTR_TABLE (ATTR_NAME,ATTR_VALUE) VALUES('%s','%s')",attr_name,attr_value);
	if (! sqlite3_command(db,query)) {
		const char *fn = sqlite3_db_filename(db,"main");
		LOG(WARNING) << "Could not set attribute: "<<attr_name<<"="<<attr_value <<" in database "<<(fn?fn:"");
		return false;
	}
	return true;
}

std::string sqlite_get_attr(sqlite3*db,const char *attr_name)
{
	return sqlite_single_lookup_string(db,"ATTR_TABLE","ATTR_NAME",attr_name,"ATTR_VALUE");
}




bool sqlite_command(sqlite3* DB, const char* query, int *pResultCode, unsigned retries)
{
	// Prepare the statement.
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_statement(DB,&stmt,query,retries)) {
		if (pResultCode) { *pResultCode = -2; }		// made up value.
		return false;
	}
	// Run the query.
	int src = sqlite3_run_query(DB,stmt,retries);
	if (pResultCode) { *pResultCode = src; }
	sqlite3_finalize(stmt);
	return (src==SQLITE_DONE || src==SQLITE_OK || src==SQLITE_ROW);
}



