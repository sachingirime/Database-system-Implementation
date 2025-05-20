#include <iostream>
#include "sqlite3.h"

#include "Schema.h"
#include "Catalog.h"

using namespace std;




static int tables_callback(void* data, int argc, char** argv, char** azColName) {
	//cast data into correct type
	unordered_map<string,Schema>* table_schema = static_cast<unordered_map<string,Schema>*>(data);
	//expected schema of tuple is:
	// 0 - table name (char*)
	// 1 - num_tuples (int)
	// 2 - filepath(char*)
	SString table_name(argv[0]);
	SInt num_tuples(atoi(argv[1]));
	SString filepath(argv[2]);

	//Insert to map
	Schema schema;
	schema.SetNoTuples(num_tuples);
	schema.SetDataFile(filepath);
	
// Insert the schema into schema map
	(*table_schema)[table_name] = schema;




	return 0;
}

// static int attributes_callback(void* data, int argc, char** argv, char** azColName) {
// 	//cast data into correct type
// 	unordered_map<string,Schema>* table_schema = static_cast<unordered_map<string,Schema>*>(data);
// 	//expected schema of tuple is:
// 	// 0 - attribute name (char*)
// 	// 1 - position (int)
// 	// 2 - type (char*)
// 	// 3 - num_distinct (int)
// 	// 4 - table_name (char*)
// 	SString att_name(argv[0]);
// 	SInt position(atoi(argv[1]));
// 	SString type(argv[2]);
// 	SInt num_distinct(atoi(argv[3]));
// 	SString table_name(argv[4]);

// 	//Get the table name that matches table_name
// 	Schema& schema = (*table_schema)[table_name];
// 	//Insert attribute into schema
// 	Attribute attribute;
// 	attribute.name=att_name;
// 	attribute.noDistinct = num_distinct;
// 	//type cast type to Type
// 	attribute.type = (std::string(type) == "INTEGER") ? Integer : (std::string(type) == "FLOAT") ? Float : String;
	
// 	schema.GetAtts().Append(attribute);

// 	//update schema object in map
// 	(*table_schema)[table_name] = schema;


// 	return 0;
// }

static int attributes_callback(void* data, int argc, char** argv, char** azColName) {
    // cast data into correct type
    auto table_schema = static_cast<unordered_map<string,Schema>*>(data);
    // argv: 0=name, 1=position, 2=type, 3=num_distinct, 4=table_name
    SString      att_name    (argv[0]);
    SInt         position    (atoi(argv[1]));
    std::string  tstr        (argv[2]);       // use std::string for easy compare
    SInt         num_distinct(atoi(argv[3]));
    SString      table_name  (argv[4]);

    // build the attribute
    Attribute attribute;
    attribute.name      = att_name;
    attribute.noDistinct= num_distinct;

    // map SQLite types → your enum
    if      (tstr == "INTEGER" || tstr == "INT")           attribute.type = Integer;
    else if (tstr == "FLOAT"   || tstr == "REAL")
                                           attribute.type = Float;
    else                                   attribute.type = String;

    // attach into the in‑memory schema
    Schema& schema = (*table_schema)[table_name];
    schema.GetAtts().Append(attribute);
    // (no need to re‑assign; schema is a reference)

    return 0;
}

// Not done
Catalog::Catalog(SString& _fileName) {
	//opens/creates a database with the given filename
	filename = _fileName;
	int return_code = sqlite3_open(filename.c_str(), &db_handle);

	if(return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") opening database: " << filename << endl;
		exit(1);
	}

	/* Recommended schema of the database:
	* Tables(id INT, name VARCHAR, num_tuples INT, path VARCHAR)
	* Attributes (name VARCHAR, position INT, type VARCHAR, num_distinct INT, table_id INT)
	
	More can be added for later phases, 
	numeric join keys */

	char* error_buffer;
	string sql = "CREATE TABLE IF NOT EXISTS Tables (name VARCHAR, num_tuples INT, path VARCHAR);";
	return_code = sqlite3_exec(db_handle, sql.c_str(), NULL, 0, &error_buffer);

	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") creating table: " << error_buffer << endl;
		//free error to prevent memory leak
		sqlite3_free(error_buffer);
		exit(1);
	}

	sql = "CREATE TABLE IF NOT EXISTS Attributes (name VARCHAR, position INT, type VARCHAR, num_distinct INT, table_name VARCHAR);";
	return_code = sqlite3_exec(db_handle, //database handle
				  sql.c_str(), //sql statement buffer
				  NULL,  //callback function
				  0, //callback argument
				  &error_buffer); //buffer for error message

	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") when creating table: " << error_buffer << endl;
		//free error to prevent memory leak
		sqlite3_free(error_buffer);
		exit(1);
	}

	// Database schema is now created
	// Next step is to read in the data inro memory

	sql = "SELECT name,num_tuples,path FROM Tables;";
	return_code = sqlite3_exec(db_handle, sql.c_str(), tables_callback,(void*)&(table_schema), &error_buffer);

	if(return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") when creating Table " << error_buffer << endl;
		sqlite3_free(error_buffer);
	}

	sql = "SELECT name,position,type,num_distinct,table_name FROM Attributes ORDER BY position;";
	return_code = sqlite3_exec(db_handle, sql.c_str(), attributes_callback,(void*)&(table_schema), &error_buffer);

	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") when creating Tables " << error_buffer << endl;
		sqlite3_free(error_buffer);
	}


}



Catalog::~Catalog() {

}

bool Catalog::Save() {
	// Saves data in Catalog back into database
	// optionally group all statements into a single transaction to speed up

	sqlite3_exec(db_handle,"BEGIN TRANSACTION;",nullptr,nullptr,nullptr);

	// 1. Clear existing Catalog data
	char* error_buffer;
	string sql = "DELETE FROM Tables;";
	int return_code = sqlite3_exec(db_handle,sql.c_str(),NULL,0,&error_buffer);
	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") when clearing Tables: " << error_buffer << endl;
		sqlite3_free(error_buffer);
	
		return false;
	}

	sql = "DELETE FROM Attributes;";
	return_code = sqlite3_exec(db_handle,sql.c_str(),NULL,0,&error_buffer);
	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") when clearing Attributes: " << error_buffer << endl;
		sqlite3_free(error_buffer);
	
		return false;
	}

	// 2. Prepare INSERT INTO Tables
	sql = "INSERT INTO Tables VALUES(?,?,?);";
	return_code = sqlite3_prepare_v2(db_handle, //database handle
									sql.c_str(), // sql statement buffer
									-1, //max number of bytes to read from buffer
									&stmt_handle, //handle for prepared statement
									&stmt_leftover); //pointer to end of current statement in buffer
	
	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code <<") compiling INSERT statement: " << sqlite3_errmsg(db_handle) << endl;
		exit(1);
		return false;
	}

	// 3. Insert schemas from Catalog into Tables
	for (unordered_map<string,Schema>::iterator it = table_schema.begin(); it != table_schema.end(); ++it) {
		
		//Get data to insert
		string name = it->first;
		Schema schema = it->second;
		int num_tuples = schema.GetNoTuples();
		string filepath = schema.GetDataFile();

		// 4. Bind data into INSERT statement
		sqlite3_reset(stmt_handle);
		sqlite3_clear_bindings(stmt_handle);

		sqlite3_bind_text(stmt_handle,1,name.c_str(),-1,0);
		sqlite3_bind_int(stmt_handle,2,num_tuples);
		sqlite3_bind_text(stmt_handle,3,filepath.c_str(),-1,0);

		//Run INSERT
		return_code = sqlite3_step(stmt_handle);
		if (return_code != SQLITE_DONE) {
			cerr << "Error (" << return_code << ") running INSERT INTO Tables VALUES ("
			<< name << ", "
			<< num_tuples << ", "
			<< filepath << "): "
			<< sqlite3_errmsg(db_handle) << endl;
			
			return false;
		}


	}
	sqlite3_finalize(stmt_handle);

	// 5. Prepare INSERT INTO Attributes
	sql = "INSERT INTO Attributes VALUES(?,?,?,?,?);";
	return_code = sqlite3_prepare_v2(db_handle, //database handle
									sql.c_str(), // sql statement buffer
									-1, //max number of bytes to read from buffer
									&stmt_handle, //handle for prepared statement
									&stmt_leftover); //pointer to end of current statement in buffer

	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code <<") compiling INSERT statement: " << sqlite3_errmsg(db_handle) << endl;
		return false;
	}

	// 6. Insert schemas from Catalog into Attributes
	for (unordered_map<string,Schema>::iterator it = table_schema.begin(); it != table_schema.end(); ++it) {
		// Get data to insert
		Schema schema = it->second;
		string table_name = it->first;
		AttributeVector& attributes = schema.GetAtts();

		// Iterate through attributes
		for (int pos = 0; pos < schema.GetNumAtts(); pos++){
			string name = attributes[pos].name;
			int num_distinct = attributes[pos].noDistinct;

			string type;
			switch (attributes[pos].type){

				//missing code from here to 
				case Integer:
					type = "INTEGER";
					break;
				case Float:
					type = "FLOAT";
					break;
				case String:
					type = "STRING";
					break;
			}
			sqlite3_reset(stmt_handle);
			sqlite3_clear_bindings(stmt_handle);

			
			
			sqlite3_bind_text(stmt_handle,1,name.c_str(),-1,SQLITE_STATIC);
			sqlite3_bind_int(stmt_handle, 2, pos);
			sqlite3_bind_text(stmt_handle,3,type.c_str(),-1,0);
			sqlite3_bind_int(stmt_handle,4,num_distinct);
			sqlite3_bind_text(stmt_handle,5,table_name.c_str(),-1,0);

			cout<<sqlite3_expanded_sql(stmt_handle)<<endl;

			//Run INSERT
			return_code = sqlite3_step(stmt_handle);
			if (return_code != SQLITE_DONE){
				cerr << "Error (" << return_code << ") running INSERT INTO Attributes VALUES("
					<< name << ", "
					<< pos << ", " 
					<< type << ", "
					<< num_distinct << ", "
					<< table_name << "): "
					<< sqlite3_errmsg(db_handle) << endl;

				return false;
			}

			
		}
	}

	sqlite3_finalize(stmt_handle);

	//commit transaction 
	sqlite3_exec(db_handle,"COMMIT;",nullptr,nullptr,nullptr);
	return true;
}

bool Catalog::GetNoTuples(SString& _table, SInt& _noTuples) {
	if (table_schema.count(_table) == 0){return false;}

	_noTuples = table_schema[_table].GetNoTuples();
	return true;
}

void Catalog::SetNoTuples(SString& _table, SInt& _noTuples) {
	table_schema[_table].SetNoTuples(_noTuples);

}

bool Catalog::GetDataFile(SString& _table, SString& _path) {
	if(table_schema.count(_table) == 0){return false;}
	_path = table_schema[_table].GetDataFile();
	

	return true;
}

void Catalog::SetDataFile(SString& _table, SString& _path) {
	table_schema[_table].SetDataFile(_path);

}

bool Catalog::GetNoDistinct(SString& _table, SString& _attribute, SInt& _noDistinct) {
	if(table_schema.count(_table) == 0){return false;}
	SInt d = table_schema[_table].GetDistincts(_attribute);
	_noDistinct = table_schema[_table].GetDistincts(_attribute);

	return true;
}

void Catalog::SetNoDistinct(SString& _table, SString& _attribute, SInt& _noDistinct) {
	table_schema[_table].SetDistincts(_attribute,_noDistinct);

}

void Catalog::GetTables(StringVector& _tables) {
	//print out all the table names
	for(unordered_map<string,Schema>::iterator it = table_schema.begin(); it != table_schema.end(); ++it){
		SString table(it->first);
		_tables.Append(table);
	}


}

bool Catalog::GetAttributes(SString& _table, StringVector& _attributes) {
	// check for remaining code here
	unordered_map<string,Schema>::iterator it = table_schema.find(_table);
	if(it == table_schema.end()){return false;}

	for (int i = 0; i < it->second.GetAtts().Length();i++){
		SString att(it->second.GetAtts()[i].name);
		_attributes.Append(att);
	}
	return true;
}

bool Catalog::GetSchema(SString& _table, Schema& _schema) {
	if(table_schema.count(_table) == 0){return false;}

	_schema = table_schema[_table];


	return true;
}

bool Catalog::CreateTable(SString& _table, StringVector& _attributes,
	StringVector& _attributeTypes) {
	
	if (table_schema.count(_table) != 0){return false;}

	//Initialize the number of distinct values to 0
	IntVector distincts;
	SInt t(0);
	for(int i = 0; i < _attributes.Length(); i++){
		distincts.Append(t);

		//Validate attribute type
		if (!((string)_attributeTypes[i] == "INTEGER"  ||
					(string)_attributeTypes[i] == "FLOAT" || 
					(string)_attributeTypes[i] == "STRING")){
						return false;
					}
	}

	table_schema[_table] = Schema(_attributes, _attributeTypes, distincts);

	return true;
}

bool Catalog::DropTable(SString& _table) {
	if(table_schema.erase(_table) == 0){return false;}
	// delete the tables and attributes associated from sqlite file as well, similarly as above, bind and execute the statement similarly as above
	char* error_buffer;
	string sql = "DELETE FROM Tables WHERE name = ?;";


	int return_code = sqlite3_prepare_v2(db_handle,sql.c_str(),-1,&stmt_handle,&stmt_leftover);
	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") when dropping table " << error_buffer << endl;
		sqlite3_free(error_buffer);
		return false;
	}
	//bind and execute the statement
	sqlite3_bind_text(stmt_handle,1,((std::string)_table).c_str(),-1,0);
	return_code = sqlite3_step(stmt_handle);
	if (return_code != SQLITE_DONE) {
		cerr << "Error (" << return_code << ") when deleting table: " << error_buffer << endl;
		sqlite3_free(error_buffer);
		return false;
	}
	sqlite3_finalize(stmt_handle);

	//also delete attrutes associated with the table name
	string sql2 = "DELETE FROM Attributes WHERE table_name = ?;";
	sqlite3_stmt* stmt_handle;
	const char* stmt_leftover;
	//prepare the statement
	return_code = sqlite3_prepare_v2(db_handle,sql2.c_str(),-1,&stmt_handle,&stmt_leftover);
	if (return_code != SQLITE_OK) {
		cerr << "Error (" << return_code << ") when dropping table " << error_buffer << endl;
		sqlite3_free(error_buffer);
		return false;
	}
	//bind and execute the statement
	sqlite3_bind_text(stmt_handle,1,((std::string)_table).c_str(),-1,0);
	return_code = sqlite3_step(stmt_handle);
	if (return_code != SQLITE_DONE) {
		cerr << "Error (" << return_code << ") when deleting table: " << error_buffer << endl;
		sqlite3_free(error_buffer);
		return false;
	}
	sqlite3_finalize(stmt_handle);



	return true;
}

ostream& operator<<(ostream& _os, Catalog& _c) {
	//iterate over the map and print in the format

	//use nested loop, first over table then over attributes
	for(unordered_map<string,Schema>::iterator it = _c.table_schema.begin(); it != _c.table_schema.end(); ++it){
		_os << it->first << "\t" << it->second.GetNoTuples() << "\t" << it->second.GetDataFile() << endl;
		
		for(int i = 0; i < it->second.GetAtts().Length(); i++){
			_os << "\t" << it->second.GetAtts()[i].name << "\t";
			switch(it->second.GetAtts()[i].type){
				case Integer:
					_os << "INTEGER";
					break;
				case Float:
					_os << "FLOAT";
					break;
				case String:
					_os << "STRING";
					break;
				default:
					_os << "UNKNOWN";
					break;
			}
			_os << "\t" << it->second.GetAtts()[i].noDistinct << endl;
		}
	}

	return _os;
}
