/*
* Copyright 2009, 2010 Free Software Foundation, Inc.
* Copyright 2010 Kestrel Signal Processing, Inc.
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



#include "Configuration.h"
#include <iostream>
#include <string>

ConfigurationKeyMap getConfigurationKeys();
ConfigurationTable gConfig("exampleconfig.db","test", getConfigurationKeys());

void purgeConfig(void*,int,char const*, char const*, sqlite3_int64)
{
	//std::cout << "update hook" << std::endl;
	gConfig.purge();
}


int main(int argc, char *argv[])
{

	gConfig.setUpdateHook(purgeConfig);

	const char *keys[5] = {"key1", "key2", "key3", "key4", "key5"};

	for (int i=0; i<5; i++) {
		gConfig.set(keys[i],i);
	}

	for (int i=0; i<5; i++) {
		std::cout << "table[" << keys[i] << "]=" << gConfig.getStr(keys[i]) <<  std::endl;
		std::cout << "table[" << keys[i] << "]=" << gConfig.getNum(keys[i]) <<  std::endl;
	}

	for (int i=0; i<5; i++) {
		std::cout << "defined table[" << keys[i] << "]=" << gConfig.defines(keys[i]) <<  std::endl;
	}

	gConfig.set("key5","100 200 300  400 ");
	std::vector<unsigned> vect = gConfig.getVector("key5");
	std::cout << "vect length " << vect.size() << ": ";
	for (unsigned i=0; i<vect.size(); i++) std::cout << " " << vect[i];
	std::cout << std::endl;
	std::vector<std::string> svect = gConfig.getVectorOfStrings("key5");
	std::cout << "vect length " << svect.size() << ": ";
	for (unsigned i=0; i<svect.size(); i++) std::cout << " " << svect[i] << ":";
	std::cout << std::endl;

	std::cout << "bool " << gConfig.getBool("booltest") << std::endl;
	gConfig.set("booltest",1);
	std::cout << "bool " << gConfig.getBool("booltest") << std::endl;
	gConfig.set("booltest",0);
	std::cout << "bool " << gConfig.getBool("booltest") << std::endl;

	gConfig.getStr("newstring");
	gConfig.getNum("numnumber");


	SimpleKeyValue pairs;
	pairs.addItems(" a=1 b=34 dd=143 ");
	std::cout<< pairs.get("a") << std::endl;
	std::cout<< pairs.get("b") << std::endl;
	std::cout<< pairs.get("dd") << std::endl;

	gConfig.set("fkey","123.456");
	float fval = gConfig.getFloat("fkey");
	std::cout << "fkey " << fval << std::endl;

	std::cout << "search fkey:" << std::endl;
	gConfig.find("fkey",std::cout);
	std::cout << "search fkey:" << std::endl;
	gConfig.find("fkey",std::cout);
	gConfig.remove("fkey");
	std::cout << "search fkey:" << std::endl;
	gConfig.find("fkey",std::cout);

	try {
		gConfig.getNum("supposedtoabort");
	} catch (ConfigurationTableKeyNotFound) {
		std::cout << "ConfigurationTableKeyNotFound exception successfully caught." << std::endl;
	}
}

ConfigurationKeyMap getConfigurationKeys()
{
	ConfigurationKeyMap map;
	ConfigurationKey *tmp;

	tmp = new ConfigurationKey("booltest","0",
		"",
		ConfigurationKey::DEVELOPER,
		ConfigurationKey::BOOLEAN,
		"",
		false,
		""
	);
	map[tmp->getName()] = *tmp;
	delete tmp;

	tmp = new ConfigurationKey("numnumber","42",
		"",
		ConfigurationKey::DEVELOPER,
		ConfigurationKey::VALRANGE,
		"0-100",
		false,
		""
	);
	map[tmp->getName()] = *tmp;
	delete tmp;

	tmp = new ConfigurationKey("newstring","new string value",
		"",
		ConfigurationKey::DEVELOPER,
		ConfigurationKey::STRING,
		"",
		false,
		""
	);
	map[tmp->getName()] = *tmp;
	delete tmp;

	return map;
}
