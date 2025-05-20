#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "Schema.h"
#include "Catalog.h"

using namespace std;


int main (int argc, char* argv[]) {
	if (argc != 4) {
		cout << "Usage: main [sqlite_file] [no_tables] [no_atts]" << endl;
		return -1;
	}

	SString dbFile(argv[1]);
	int tNo = atoi(argv[2]);
	int aNo = atoi(argv[3]);

	Catalog catalog(dbFile);
	cout << catalog << endl; cout.flush();


	////////////////////////////////
	for (int i = 0; i < tNo; i++) {
		char tN[20]; sprintf(tN, "T_%d", i);
		SString tName(tN);

		int taNo = (i+1) * aNo;
		StringVector atts;
		StringVector types;
		//attributes for table, 
		for (int j = 0; j < taNo; j++) {
			char aN[20]; sprintf(aN, "A_%d_%d", i, j);
			SString aName(aN);
			atts.Append(aName);

			SString aType;
			int at = j % 3;
			if (0 == at) aType = SString("INTEGER");
			else if (1 == at) aType = SString("FLOAT");
			else if (2 == at) aType = SString("STRING");
			types.Append(aType);
		}

		cout << "CREATE TABLE " << tName << endl;
		cout << atts << endl;
		cout << types << endl;

		bool ret = catalog.CreateTable(tName, atts, types);
		if (true == ret) {
			for (int j = 0; j < taNo; j++) {
				SInt dist = (i+1) * 10 + j;
				cout << atts[j] << " distinct = " << dist << endl;
				catalog.SetNoDistinct(tName, atts[j], dist);
			}

			SInt tuples = (i+1) * 1000;
			cout << "tuples = " << tuples << endl;
			catalog.SetNoTuples(tName, tuples);

			string path = tName; path = path + ".dat"; SString sp(path);
			cout << "path = " << sp << endl;
			catalog.SetDataFile(tName, sp);

			cout << "CREATE TABLE " << tName << " OK" << endl;
		}
		else {
			cout << "CREATE TABLE " << tName << " FAIL" << endl;
		}
	}


	////////////////////////////////
	catalog.Save();
	cout << catalog << endl; cout.flush();


	////////////////////////////////
	cout << "tables" << endl;

	StringVector tables;
	catalog.GetTables(tables);
	for (int i = 0; i < tables.Length(); i++) {
		cout << tables[i] << endl;
	}
	cout << endl;


	////////////////////////////////
	for (int i = 0; i < 10; i++) {
		cout << i << "+++++++++++++" << endl;

		int r = rand() % tNo;
		char tN[20]; sprintf(tN, "T_%d", r);
		SString tName(tN);

		SInt tuples = 0;
		catalog.GetNoTuples(tName, tuples);
		cout << tName << " tuples = " << tuples << endl;

		SString path(".");
		catalog.GetDataFile(tName, path);
		cout << tName << " path = " << path << endl;

		StringVector atts;
		catalog.GetAttributes(tName, atts);
		for (int i = 0; i < atts.Length(); i++) {
			cout << atts[i] << " ";
		}
		cout << endl;

		Schema schema;
		catalog.GetSchema(tName, schema);
		cout << schema << endl;

		////////////////////////////////
		for (int j = 0; j < 2; j++) {
			int s = rand() % ((r+1) * aNo);
			char aN[20]; sprintf(aN, "A_%d_%d", r, s);
			SString aName(aN);

			SInt distinct = -1;
			catalog.GetNoDistinct(tName, aName, distinct);
			cout << tName << "." << aName << " distinct = " << distinct << endl;
		}
	}


	////////////////////////////////
	for (int i = 0; i < tNo/2; i++) {
		char tN[20]; sprintf(tN, "T_%d", i);
		SString tName(tN);

		bool ret = catalog.DropTable(tName);
		if (true == ret) {
			cout << "DROP TABLE " << tName << " OK" << endl;
		}
		else {
			cout << "DROP TABLE " << tName << " FAIL" << endl;
		}
	}

	return 0;
}
