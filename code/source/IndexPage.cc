

#include "IndexPage.h"
#include <cstring>
#include <cassert>
#include <cstdint>
#include <iostream>

using namespace std;

IndexPage::IndexPage() : pageType(LEAF), pageId(-1), lastPointer(-1), numKeys(0) {}

void IndexPage::ToBinary(char* bits) const {
    // Unchanged: Already correctly zeroes buffer and writes data
    std::memset(bits, 0, PAGE_SIZE);
    static_assert(sizeof(int)==4, "This on-disk format assumes 4-byte ints");
    size_t offset = 0;
    auto writeInt = [&](int32_t v) {
        std::memcpy(bits + offset, &v, sizeof(v));
        offset += sizeof(v);
    };
    writeInt(static_cast<int32_t>(pageType));
    writeInt(static_cast<int32_t>(pageId));
    writeInt(static_cast<int32_t>(numKeys));
    for (int i = 0; i < numKeys; ++i) {
        writeInt(static_cast<int32_t>(keys[i]));
        writeInt(static_cast<int32_t>(pointers[i]));
    }
    writeInt(static_cast<int32_t>(lastPointer));
    assert(offset <= PAGE_SIZE);
}

void IndexPage::FromBinary(const char* bits) {
    int offset = 0;
    memcpy(&pageType, bits + offset, sizeof(int)); offset += sizeof(int);
    memcpy(&pageId,   bits + offset, sizeof(int)); offset += sizeof(int);
    memcpy(&numKeys,  bits + offset, sizeof(int)); offset += sizeof(int);

    // FIX: Validate numKeys to prevent corruption
    if (numKeys < 0 || numKeys > MAX_KEYS) {
        cerr << "ERROR: Invalid numKeys=" << numKeys << " in page " << pageId << endl;
        numKeys = 0; // Reset to safe value
    }

    keys.clear();
    pointers.clear();

    // Read numKeys pairs of (key, pointer)
    for (int i = 0; i < numKeys; ++i) {
        int k, p;
        memcpy(&k, bits + offset, sizeof(int)); offset += sizeof(int);
        memcpy(&p, bits + offset, sizeof(int)); offset += sizeof(int);
        keys.push_back(k);
        pointers.push_back(p);
    }

    // Read the final pointer (child or sibling)
    int extra;
    memcpy(&extra, bits + offset, sizeof(int));
    lastPointer = extra;

    // FIX: Enhanced sanity check
    if ((int)pointers.size() != numKeys || (int)keys.size() != numKeys) {
        cerr << "ERROR: Corrupt page " << pageId << ": pointers=" << pointers.size()
             << ", keys=" << keys.size() << ", expected=" << numKeys << endl;
        keys.clear();
        pointers.clear();
        numKeys = 0;
    }
}

void IndexPage::Print() const {
    cout << "IndexPage [pageId=" << pageId << ", pageType=" << (pageType == 0 ? "Leaf" : "Internal")
         << ", numKeys=" << numKeys << "]\n";
    for (int i = 0; i < numKeys; ++i) {
        cout << "  Key: " << keys[i] << " -> Page: " << pointers[i] << "\n";
    }
    cout << "  LastPointer: " << lastPointer << endl;
}