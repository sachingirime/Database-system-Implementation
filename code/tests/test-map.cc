#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "Map.cc"

using namespace std;


typedef Map<KInt,SInt> MapIntInt;

int main (int argc, char* argv[]) {
	MapIntInt mii;

	KInt ki, kj;
	SInt di, dj;
	for (int i = 0; i < 20; i++) {
		ki = i;
		di = i*100;
		mii.Insert(ki, di);
	}
	cout << "map = ";
	cout << mii << endl;

	for (int i = 0; i < 20; i++) {
		ki = 20-i;
		di = i*10;
		mii.Insert(ki, di);
	}
	cout << "map = ";
	cout << mii << endl;

	for (int i = 0; i < 10; i++) {
		ki = i;

		int isIn = mii.Find(ki, di);
		if (isIn == 0)
			cout << ki << " --> {}" << endl;
		else {
			cout << ki << " --> { ";
			while (!mii.AtEnd() && mii.CurrentKey().IsEqual(ki)) {
				cout << mii.CurrentData() << " ";
				mii.Advance();
			}
			cout << "}" << endl;
		}
	}

	cout << "map = ";
	cout << mii << endl;

	for (int i = 0; i < 30; i++) {
		ki = i;

		int isIn = mii.IsThere(ki);
		if (isIn == 0)
			cout << ki << " --> {}" << endl;
		else {
			cout << ki << " --> { ";
			while (!mii.AtEnd() && mii.CurrentKey().IsEqual(ki)) {
				cout << mii.CurrentData() << " ";
				mii.Advance();
			}
			cout << "}" << endl;
		}
	}

	for (int i = 0; i < 5; i++) {
		ki = i;
		mii.Remove(ki, kj, dj);
	}
	cout << "map = ";
	cout << mii << endl;

	for (int i = 5; i < 10; i++) {
		ki = i;

		int isIn = mii.Remove(ki, kj, dj);
		if (isIn == 1) {
			while (!mii.AtEnd() && mii.CurrentKey().IsEqual(ki)) {
				mii.Remove(ki, kj, dj);
			}
		}

	}
	cout << "map = ";
	cout << mii << endl;

	Map<KInt, KInt> map;
	KInt k1(1), k2(1), k3(2), k4(1), k5(1), k6(2);
	KInt v1(0), v2(0), v3(1), v4(1), v5(3), v6(5);
	map.Insert(k1, v1);
	map.Insert(k2, v2);
	map.Insert(k3, v3);
	map.Insert(k4, v4);
	map.Insert(k5, v5);
	map.Insert(k6, v6);

	for (map.MoveToStart(); map.AtEnd() == false; map.Advance()) {
		int k = map.CurrentKey();
		int v = map.CurrentData();

		printf("%d %d\n", k, v);
	}

	printf("+++++++++++++++++++++++++++++++++++++++++++++++\n");
	
	KInt k(1);
	map.IsThere(k);
	while (map.CurrentKey() == k) {
		int kk = map.CurrentKey();
		int vv = map.CurrentData();

		printf("%d %d\n", kk, vv);
		map.Advance();		
	}

	return 0;
}
