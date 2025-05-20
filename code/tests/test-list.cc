#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "List.cc"

using namespace std;


int main (int argc, char* argv[]) {
	IntList il;

	for (int i = 0; i < 20; i++) {
		SInt si = i;
		il.Append(si);
	}
	cout << "list = ";
	cout << il << endl;

	il.MoveToStart();
	for (int i = 0; i < 10; i++) {
		il.Advance();
	}
	cout << "list.current = " << il.Current() << endl;

	for (int i = 0; i < 10; i++) {
		SInt si = i+100;
		il.Insert(si);
		cout << "list.current = " << il.Current() << endl;
	}
	cout << "list = ";
	cout << il << endl;
	
	il.MoveToFinish();
	il.Retreat();
	for (;!il.AtStart(); il.Retreat()) {
		cout << "list.current = " << il.Current() << endl;
	}
	cout << "list.current = " << il.Current() << endl;

	cout << "list = ";
	cout << il << endl;

	il.MoveToStart();
	cout << "list.current = " << il.Current() << endl;
	for (int i = 0; i < 10; i++) {
		SInt si;
		il.Remove(si);
		cout << "list.current = " << il.Current() << endl;
	}
	cout << "list = ";
	cout << il << endl;

	return 0;
}
