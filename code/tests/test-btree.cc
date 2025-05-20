
#include "../headers/BTreeIndex.h"
#include "../headers/Catalog.h"
#include <iostream>
using namespace std;

int main() {
    SString catalogPath("catalog.sqlite");
    Catalog catalog(catalogPath);

    BTreeIndex btree(&catalog);
    btree.Build("orders_index.bin", "orders", "o_orderkey");

    vector<int> testKeys = {1, 40, 64, 100, 128, 256};

    for (int key : testKeys) {
        int page;
        if (btree.Find(key, page)) {
            cout << "[FIND] Key " << key << " found on data page: " << page << endl;
        } else {
            cout << "[FIND] Key " << key << " NOT found in the tree." << endl;
        }
    }

    return 0;
}
