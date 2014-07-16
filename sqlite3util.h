/* 
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

#ifndef SQLITE3UTIL_H
#define SQLITE3UTIL_H

#include <sqlite3.h>
#include <string>
#include <vector>

// (pat) Dont put statics in .h files - they generate a zillion g++ error messages.
extern const char *enableWAL;
//static const char* enableWAL = {
//	"PRAGMA journal_mode=WAL"
//};

// Pat added.
class sqlQuery {
	sqlite3 *mdb;
	sqlite3_stmt *mStmt;
	int mQueryRC;
	void queryStart(sqlite3*db, const char *tableName,const char *condition, const char*resultCols);

	public:
	std::string mQueryString;
	// Query for row(s) matching this condition.  Can request one or more result columns, or "*" for all columns.
	sqlQuery(sqlite3*db, const char *tableName,const char*resultColumns,const char *condition);
	// Query for a row where keyName == keyData.
	sqlQuery(sqlite3*db, const char *tableName,const char*resultColumns,const char *keyName, const char*keyData);
	// Query for a row where keyName == keyData.
	sqlQuery(sqlite3*db, const char *tableName,const char*resultColumns,const char *keyName, unsigned keyData);
	// Did the query succeed and find a result row?
	bool sqlSuccess() { return mStmt && mQueryRC == SQLITE_ROW; }
	// Return the results as text or integer.
	std::string getResultText(int colNum=0);
	sqlite3_int64 getResultInt(int colNum=0);
	// Return the number of columns in the result, or 0 if the result did not contain any data.
	// Note: If the table is completely empty, sqlite3_column_count returns non-0, so check mQueryRC first.
	unsigned sqlResultSize() { return mStmt && mQueryRC == SQLITE_ROW ? sqlite3_column_count(mStmt) : 0; }
	// Step to the next row.  Return false if there are no more rows.
	bool sqlStep();
	~sqlQuery();
};

// (pat) These functions should probably not begin with "sqlite3_" since that is reserved for sqlite3 itself...
int sqlite3_prepare_statement(sqlite3* DB, sqlite3_stmt **stmt, const char* query, unsigned retries = 5);

int sqlite3_run_query(sqlite3* DB, sqlite3_stmt *stmt, unsigned retries = 5);

bool sqlite3_single_lookup(sqlite3* DB, const char *tableName,
		const char* keyName, const char* keyData,
		const char* valueName, unsigned &valueData, unsigned retries = 5);

// This function returns an allocated string that must be free'd by the caller.
bool sqlite3_single_lookup(sqlite3* DB, const char* tableName,
		const char* keyName, const char* keyData,
		const char* valueName, char* &valueData, unsigned retries = 5);

// This function returns an allocated string that must be free'd by the caller.
bool sqlite3_single_lookup(sqlite3* DB, const char* tableName,
		const char* keyName, unsigned keyData,
		const char* valueName, char* &valueData, unsigned retries = 5);

bool sqlite_single_lookup(sqlite3* DB, const char* tableName,
		const char* keyName, const char* keyData,
		const char* valueName, std::string &valueData);
bool sqlite_single_lookup(sqlite3* DB, const char* tableName,
		const char* keyName, unsigned keyData,
		const char* valueName, std::string &valueData);

//std::vector<std::string> sqlite_multi_lookup_vector(sqlite3* DB, const char* tableName, const char* keyName, const char* keyData, const char *resultColumns);
std::string sqlite_single_lookup_string(sqlite3* DB, const char* tableName, const char* keyName, unsigned keyData, const char* valueName);

// Get and set attributes on an sqlite database.  Works by creating an ATTR_TABLE in the database.
bool sqlite_set_attr(sqlite3*db,const char *attr_name,const char *attr_value);
std::string sqlite_get_attr(sqlite3*db,const char *attr_name);

bool sqlite3_exists(sqlite3* DB, const char* tableName,
		const char* keyName, const char* keyData, unsigned retries = 5);

/** Run a query, ignoring the result; return true on success. */
bool sqlite_command(sqlite3* DB, const char* query, int *pResultCode = NULL, unsigned retries=5);
bool inline sqlite3_command(sqlite3* DB, const char* query, unsigned retries = 5) { return sqlite_command(DB,query,NULL,retries); }

#endif
