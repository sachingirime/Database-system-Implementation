#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "Vector.cc"

using namespace std;


int main (int argc, char* argv[]) {
	IntVector iv;

	for (int i = 0; i < 120; i++) {
		SInt si = i;
		iv.Append(si);
	}
	cout << "vector = ";
	cout << iv << endl;

	for (int i = 0; i < 100; i++) {
		SInt si;
		iv.Remove(0, si);
	}
	cout << "vector = ";
	cout << iv << endl;

	for (int i = 0; i < 100; i++) {
		SInt si = i;
		iv.Insert(100-i, si);
	}
	cout << "vector = ";
	cout << iv << endl;

	for (int i = 0; i <= 200; i++) {
		cout << "v[" << 200-i << "] = " << iv[200-i] << endl;
	}

	for (int i = 0; i <= 200; i++) {
		SInt si = i;
		iv[i] = si;
	}
	cout << "vector = ";
	cout << iv << endl;

	return 0;
}


