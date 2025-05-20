#include <string>

#include "Config.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "DBFile.h"

using namespace std;


DBFile::DBFile () : fileName(""),currentPagePos(0) {
}

DBFile::~DBFile () {
}

DBFile::DBFile(const DBFile& _copyMe) :
	file(_copyMe.file),	fileName(_copyMe.fileName) {}

DBFile& DBFile::operator=(const DBFile& _copyMe) {
	// handle self-assignment first
	if (this == &_copyMe) return *this;

	file = _copyMe.file;
	fileName = _copyMe.fileName;

	return *this;
}

int DBFile::Create (char* f_path, FileType f_type) { 
	//create a file name of Heap type.
	//check if heap
	fileName = string(f_path);
	if (f_type != Heap) {
        cerr << "ERROR: Only Heap file type is supported.\n";
        return 0;
    }

	// Create the binary file using File class
	
	int status = file.Open(0, f_path);  // 0 => create new file
	if (status < 0) {
		cerr << "ERROR: File.Open failed while creating DBFile.\n";
		return 0;
	}
	else{
		cout << "File created." << status << endl;
	}
	return 0;
}

int DBFile::Open (char* f_path) {
	fileName = string(f_path);
	int status = file.Open(-1,f_path); // 1 - since we dont want any truncation , just open in rd/wr mode
	if (status < 0) {
		cerr << "ERROR: File.Open failed while opening DBFile.\n";
		return 0;
	}
	else{
		cout << "File Opened." << status << endl;
	}
	return 0;
}

int DBFile::Close () {
	int status = file.Close();
	cout << "File closed. Length is " << status << endl;
	return 0;
}

void DBFile::MoveFirst () {
	currentPagePos = 0;
	//cout<<"Running MoveFirst in DBFile.cc" << endl;
	file.GetPage(currentPage, currentPagePos); // currentPage is the first page in the file.(technically seconds, since first gives total number of pages in the file.)
	//let Get next handle the position to first record in the page.
}

int DBFile::GetNext (Record& rec) {
	
	//cout<<"Running GetNext in DBFile.cc with file length "<< file.GetLength() << endl;
	
	if (currentPage.GetFirst(rec) == 0) {
		// move to the next page in File , 0 means last record,no record left in this page

		currentPagePos++;
		
		
		if (currentPagePos < file.GetLength()) {
			file.GetPage(currentPage, currentPagePos);
			return GetNext(rec);

		}
		return 0; // No more records

	}
	
	


	return 1;
}

void DBFile::AppendRecord (Record& rec) {
	//cout<<"Running Append Record in DBFile.cc" << endl;
	if (0 == currentPage.Append(rec)) {
		// save the current page to File
		file.AddPage(currentPage,currentPagePos);
		// start a new page and add the record to this new page
		
		currentPage.EmptyItOut();
		// increment currentPagepos
		currentPagePos += 1;
		currentPage.Append(rec);
	}
	

}

void DBFile::Load (Schema& schema, char* textFile) {
	//cout<<"Running Load in DBFile.cc" << endl;
	currentPagePos = 0;
	currentPage.EmptyItOut();

	FILE* f = fopen(textFile, "rt");
	
	//write the code to load tuples from the text file to the heap file
	

	Record rec;
	while (rec.ExtractNextRecord (schema, *f)) {
		//AppendRecord(rec);
		if (0 == currentPage.Append(rec)) {
			file.AddPage(currentPage, currentPagePos);
			currentPage.EmptyItOut();
			currentPagePos += 1;
	  
			currentPage.Append(rec);
		  }
	
	}

	file.AddPage(currentPage, currentPagePos); // save content of last page to heap file


	fclose(f);
}
