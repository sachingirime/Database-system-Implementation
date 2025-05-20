#ifndef INDEX_PAGE_H
#define INDEX_PAGE_H

#include <vector>
#include <cstdint>
#include <iostream>

#define PAGE_SIZE 131072       // 128 KB (adjust as needed)
#define MAX_KEYS 100

class IndexPage {
public:
    enum PageType { LEAF = 0, INTERNAL = 1 };

    // ─── Members ──────────────────────────────────────────────────────────────
    int pageType;              // 0 = leaf, 1 = internal
    int pageId;                // Page ID in the index
    int lastPointer;           // Rightmost child pointer or sibling leaf pointer
    int numKeys;               // Number of keys in this node

    std::vector<int> keys;     // Keys
    std::vector<int> pointers; // Pointers (same length as keys for internal, 1-to-1 for leaf)

    // ─── Constructors ────────────────────────────────────────────────────────
    IndexPage();

    // ─── Serialization ──────────────────────────────────────────────────────
    void ToBinary(char* bits) const;
    void FromBinary(const char* bits);

    // ─── Debug ──────────────────────────────────────────────────────────────
    void Print() const;
};

#endif // INDEX_PAGE_H
