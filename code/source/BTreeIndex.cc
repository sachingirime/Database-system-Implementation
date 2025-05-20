

// #include "../headers/BTreeIndex.h"
// #include <algorithm>
// #include <fstream>
// #include <iostream>
// #include <sstream>
// #include <string>
// #include <filesystem>
// #include <cstring>

// using namespace std;

// // Sets rootPageId and nextPageId to 0.
// //Creates a single leaf node as the root with pageId = 0.
// //Stores the node in index, a map of page IDs to IndexPage objects.
// BTreeIndex::BTreeIndex(Catalog* catalog)
//     : rootPageId(0), nextPageId(0), catalog(catalog) {
//     IndexPage root;
//     root.pageId = nextPageId++;
//     root.pageType = IndexPage::LEAF;
//     root.numKeys = 0;
//     index[root.pageId] = root;
// }

// //Clears the index map, freeing all IndexPage object
// BTreeIndex::~BTreeIndex() {
//     index.clear();
// }

// //Builds an index by reading records from a heap file and inserting key-RID pairs.
// // Sets index metadata (indexFileName, tableName, attributeName).
// // Opens the heap file (e.g., customer.heap).
// // Iterates through pages, extracting keys (via ExtractKey) and inserting key-RID pairs using a sequential recordCounter.
// // Saves the tree to disk (Write), prints stats, and generates a visualization.
// void BTreeIndex::Build(const string& indexName,
//                        const string& tableName,
//                        const string& attributeName) {
//     this->indexFileName = indexName;
//     this->tableName = tableName;
//     this->attributeName = attributeName;

//     cout << "DEBUG: Building index from table: " << tableName
//          << " on attribute: " << attributeName << endl;

//     File dataFile;
//     Page page;
//     Record rec;

//     string tablePath = "../data/" + tableName + ".heap";
//     dataFile.Open(1, const_cast<char*>(tablePath.c_str()));
//     off_t numPages = dataFile.GetLength();

//     // FIX: Use a global record counter instead of page-based encoding
//     int recordCounter = 0;
//     for (off_t i = 0; i < numPages; ++i) {
//         dataFile.GetPage(page, i);
//         while (page.GetFirst(rec)) {
//             int key = ExtractKey(rec);
//             Insert(key, recordCounter++); // Use sequential RID
//         }
//     }

//     PrintPageStats();
//     Write();
//     PrintIndexStructure();
//     ExportDOT("btree.dot");
//     dataFile.Close();
// }


// //Retrieves the key value from a record.
// //Uses catalog to get the table’s schema and find the attribute’s index.
// // Extracts the integer value from the record’s column (supports only Integer type).
// // Relevance: Converts raw records to keys for indexing (e.g., c_custkey).
// int BTreeIndex::ExtractKey(Record& rec) {
//     Schema schema;
//     SString sTable(tableName);
//     SString sAttr(attributeName);
//     if (!catalog->GetSchema(sTable, schema)) {
//         cerr << "ERROR: Table not found in catalog: " << tableName << endl;
//         exit(1);
//     }
//     int attIndex = schema.Index(sAttr);
//     if (attIndex < 0) {
//         cerr << "ERROR: Attribute not found: " << attributeName << endl;
//         exit(1);
//     }
//     Type attType = schema.FindType(sAttr);
//     char* columnPtr = rec.GetColumn(attIndex);
//     if (attType == Integer) {
//         int key;
//         memcpy(&key, columnPtr, sizeof(int));
//         return key;
//     } else {
//         cerr << "ERROR: Only INTEGER keys are supported in B+ tree index." << endl;
//         exit(1);
//     }
// }

// //Inserts keys into the tree, splitting nodes when full to maintain balance.
// // Calls InsertRecursive to insert into the appropriate leaf.
// // If a split occurs, creates a new internal root with the promoted key and child pointers.
// // Validates the tree structure.
// void BTreeIndex::Insert(int key, int pointer) {
//     cout << "DEBUG: Inserting key = " << key
//          << " pointing to record = " << pointer << endl;

//     int promotedKey = -1;
//     int newPageId = -1;
//     bool split = InsertRecursive(rootPageId, key, pointer, promotedKey, newPageId);

//     if (split) {
//         IndexPage newRoot;
//         newRoot.pageType = IndexPage::INTERNAL;
//         newRoot.pageId = nextPageId++;
//         newRoot.numKeys = 1;
//         newRoot.keys.push_back(promotedKey);
//         newRoot.pointers.push_back(rootPageId);
//         newRoot.pointers.push_back(newPageId);
//         index[newRoot.pageId] = newRoot;
//         rootPageId = newRoot.pageId;
//         cout << "DEBUG: Root split. New root pageId = " << rootPageId
//              << ", promoted key = " << promotedKey << endl;
//     }

//     ValidateTree();
// }

// //Recursively inserts a key-RID pair and handles node splits.
// // Leaf Node: Inserts key and RID in sorted order using lower_bound. If numKeys > MAX_KEYS, splits the node, promotes the last key of the left node, and links siblings.
// // Internal Node: Recursively inserts into the appropriate child, handles child splits, and splits the internal node if needed.

// bool BTreeIndex::InsertRecursive(int pageId,
//                                  int key,
//                                  int pointer,
//                                  int& promotedKey,
//                                  int& newPageId) {
//     IndexPage& node = index[pageId];
//     cout << "DEBUG: Processing page " << pageId
//          << " (type=" << (node.pageType == IndexPage::LEAF ? "LEAF" : "INTERNAL")
//          << ", keys=" << node.numKeys << ")" << endl;

//     if (node.pageType == IndexPage::LEAF) {
//         auto it = lower_bound(node.keys.begin(), node.keys.end(), key);
//         int pos = it - node.keys.begin();
//         node.keys.insert(it, key);
//         node.pointers.insert(node.pointers.begin() + pos, pointer);
//         node.numKeys = node.keys.size();

//         if (node.numKeys <= MAX_KEYS) return false;

//         IndexPage newLeaf;
//         newLeaf.pageType = IndexPage::LEAF;
//         newLeaf.pageId = nextPageId++;

//         int mid = node.numKeys / 2;
//         newLeaf.keys = vector<int>(node.keys.begin() + mid, node.keys.end());
//         newLeaf.pointers = vector<int>(node.pointers.begin() + mid, node.pointers.end());
//         newLeaf.numKeys = newLeaf.keys.size();

//         node.keys.resize(mid);
//         node.pointers.resize(mid);
//         node.numKeys = node.keys.size();

//         // Maintain sibling leaf links
//         newLeaf.lastPointer = node.lastPointer;
//         node.lastPointer = newLeaf.pageId;

//         // FIX: Promote the key just before the split point for B+ tree
//         promotedKey = node.keys.back(); // Last key in left node
//         newPageId = newLeaf.pageId;
//         index[newLeaf.pageId] = newLeaf;
//         return true;
//     }

//     int i = 0;
//     while (i < node.numKeys && key >= node.keys[i]) ++i;
//     int childId = node.pointers[i];

//     int childPromoted = -1;
//     int childNewPage = -1;
//     bool split = InsertRecursive(childId, key, pointer, childPromoted, childNewPage);
//     if (!split) return false;

//     auto it = upper_bound(node.keys.begin(), node.keys.end(), childPromoted);
//     int pos = it - node.keys.begin();
//     node.keys.insert(it, childPromoted);
//     node.pointers.insert(node.pointers.begin() + pos + 1, childNewPage);
//     node.numKeys = node.keys.size();

//     if (node.numKeys <= MAX_KEYS) return false;

//     IndexPage newInternal;
//     newInternal.pageType = IndexPage::INTERNAL;
//     newInternal.pageId = nextPageId++;

//     int mid = node.numKeys / 2;
//     promotedKey = node.keys[mid];

//     newInternal.keys = vector<int>(node.keys.begin() + mid + 1, node.keys.end());
//     newInternal.pointers = vector<int>(node.pointers.begin() + mid + 1, node.pointers.end());
//     newInternal.numKeys = newInternal.keys.size();

//     node.keys.resize(mid);
//     node.pointers.resize(mid + 1);
//     node.numKeys = node.keys.size();

//     newPageId = newInternal.pageId;
//     index[newInternal.pageId] = newInternal;
//     return true;
// }

// // Writes metadata (rootPageId, nextPageId) to page 0 with a length prefix.
// // Writes each IndexPage to disk at pid + 1.
// //Serialization (converting tree to disk format).
// void BTreeIndex::Write() {
//     string fullPath = "../data/" + indexFileName;
//     cout << "DEBUG: Writing index to file: " << fullPath << endl;

//     File f;
//     f.Open(0, const_cast<char*>(fullPath.c_str()));

//     Page metadataPage;
//     Record metaRec;
//     // FIX: Explicitly set record size with length prefix
//     char* buffer = new char[sizeof(int) * 2 + sizeof(int)];
//     *(int*)buffer = sizeof(int) * 2; // Length prefix
//     int* metaData = reinterpret_cast<int*>(buffer + sizeof(int));
//     metaData[0] = rootPageId;
//     metaData[1] = nextPageId;
//     metaRec.CopyBits(buffer, sizeof(int) * 2 + sizeof(int));
//     metadataPage.Append(metaRec);
//     f.AddPage(metadataPage, 0);
//     delete[] buffer;

//     for (auto& [pid, page] : index) {
//         f.AddPage(page, pid + 1);
//     }

//     f.Close();
// }

// //Deserialization (reconstructing tree from disk).
// //Reads metadata from page 0, validating record size.
// //Loads each IndexPage into index.
// void BTreeIndex::Read(const string& fileName) {
//     indexFileName = fileName;
//     if (!std::filesystem::exists(fileName) || std::filesystem::file_size(fileName) == 0) {
//         std::cerr << "ERROR: index file not found or empty: " << fileName << std::endl;
//         return;
//     }

//     cout << "DEBUG: Reading index from file: " << fileName << endl;

//     File f;
//     f.Open(1, const_cast<char*>(fileName.c_str()));
//     off_t len = f.GetLength();

//     Page metadataPage;
//     f.GetPage(metadataPage, 0);
//     Record metaRec;
//     if (!metadataPage.GetFirst(metaRec)) {
//         cerr << "ERROR: Failed to read metadata from index file." << endl;
//         f.Close();
//         return;
//     }

//     // FIX: Validate metadata record size
//     char* bits = metaRec.GetBits();
//     int recSize = *(int*)bits;
//     if (recSize != sizeof(int) * 2) {
//         cerr << "ERROR: Metadata record size=" << recSize << ", expected " << sizeof(int) * 2 << endl;
//         f.Close();
//         return;
//     }

//     int* metaData = reinterpret_cast<int*>(bits + sizeof(int));
//     rootPageId = metaData[0];
//     nextPageId = metaData[1];

//     cout << "[Read] rootPageId = " << rootPageId << ", nextPageId = " << nextPageId << endl;

//     for (off_t i = 1; i < len; ++i) {
//         IndexPage page;
//         if (f.GetPage(page, i) != 0) {
//             cerr << "ERROR: Failed to read index page " << i << " from file." << endl;
//             continue;
//         }
//         cout << "[Read] Loaded page ID = " << page.pageId << ", keys = ";
//         for (int k : page.keys) cout << k << " ";
//         cout << endl;
//         index[page.pageId] = page;
//     }

//     f.Close();
// }

// //Searches for keys, returning all matching RIDs (supports duplicates).
// // Traverses from root to leaf, following pointers based on key comparisons.
// // In the leaf, collects all RIDs for the key, scanning siblings via lastPointer for duplicates.
// //Enables point queries, critical for ScanIndex.
// bool BTreeIndex::Find(int key, vector<int>& pageNumbersOut) {
//     int current = rootPageId;
//     while (true) {
//         if (index.find(current) == index.end()) {
//             cerr << "[Find] ERROR: Page " << current << " not found!" << endl;
//             return false;
//         }
//         IndexPage& node = index[current];
//         if (node.pageType == IndexPage::LEAF) {
//             bool found = false;
//             // Scan current leaf for all occurrences of key
//             for (int i = 0; i < node.numKeys; ++i) {
//                 if (node.keys[i] == key) {
//                     pageNumbersOut.push_back(node.pointers[i]);
//                     found = true;
//                 }
//                 // Stop if we pass the key (keys are sorted)
//                 else if (found && node.keys[i] != key) {
//                     break;
//                 }
//             }
//             // Check sibling leaves for more duplicates
//             while (found && node.lastPointer >= 0) {
//                 current = node.lastPointer;
//                 if (index.find(current) == index.end()) {
//                     cerr << "[Find] ERROR: Sibling page " << current << " not found!" << endl;
//                     break;
//                 }
//                 node = index[current];
//                 for (int i = 0; i < node.numKeys; ++i) {
//                     if (node.keys[i] == key) {
//                         pageNumbersOut.push_back(node.pointers[i]);
//                     } else {
//                         break; // Stop if keys increase (sorted order)
//                     }
//                 }
//             }
//             return !pageNumbersOut.empty();
//         }
//         int i = 0;
//         while (i < node.numKeys && key >= node.keys[i]) ++i;
//         current = node.pointers[i];
//     }
// }

// void BTreeIndex::ValidateTree() {
//     cout << "DEBUG: Validating tree structure..." << endl;
//     if (index.find(rootPageId) == index.end()) {
//         cerr << "ERROR: Root page " << rootPageId << " does not exist!" << endl;
//         return;
//     }
//     for (auto& [pid, page] : index) {
//         if (page.pageType == IndexPage::INTERNAL) {
//             size_t exp = page.numKeys + 1;
//             if (page.pointers.size() != exp) {
//                 cerr << "ERROR: Page " << pid << " pointer count " << page.pointers.size()
//                      << ", expected " << exp << endl;
//             }
//             for (int ptr : page.pointers) {
//                 if (index.find(ptr) == index.end()) {
//                     cerr << "ERROR: Page " << pid << " references missing child " << ptr << endl;
//                 }
//             }
//         }
//         // FIX: Validate leaf sibling links
//         if (page.pageType == IndexPage::LEAF && page.lastPointer >= 0) {
//             if (index.find(page.lastPointer) == index.end()) {
//                 cerr << "ERROR: Leaf page " << pid << " has invalid lastPointer " << page.lastPointer << endl;
//             }
//         }
//     }
//     cout << "DEBUG: Tree validation complete." << endl;
// }

// void BTreeIndex::PrintPageStats() const {
//     cout << "DEBUG: B+ Tree Stats:" << endl;
//     cout << "  Total pages: " << index.size() << endl;
//     cout << "  Root page: " << rootPageId << endl;
//     cout << "  Next page ID: " << nextPageId << endl;
//     int leafCount = 0, internalCount = 0;
//     for (auto& [pid, page] : index) {
//         if (page.pageType == IndexPage::LEAF) ++leafCount;
//         else ++internalCount;
//     }
//     cout << "  Leaf pages: " << leafCount << endl;
//     cout << "  Internal pages: " << internalCount << endl;
// }

// void BTreeIndex::PrintIndexStructure() const {
//     cout << "\nDEBUG: B+ Tree Structure (Visualized):\n";
//     for (auto& [pid, page] : index) {
//         cout << "[Page " << pid << "] "
//              << (page.pageType == IndexPage::LEAF ? "LEAF" : "INTERNAL")
//              << "\n  Keys: ";
//         for (int k : page.keys) cout << k << " | ";
//         cout << "\n  Pointers: ";
//         for (int p : page.pointers) cout << p << " -> ";
//         cout << "\n";
//         if (page.pageType == IndexPage::LEAF) {
//             cout << "  LastPointer (sibling leaf): " << page.lastPointer << "\n";
//         }
//     }
// }

// void BTreeIndex::ExportDOT(const std::string& filename) const {
//     std::ofstream out(filename);
//     out << "digraph BPlusTree {\n"
//            "  graph [rankdir=TB, splines=ortho];\n"
//            "  node [shape=record, fontsize=10];\n"
//            "  edge [arrowsize=0.7];\n\n";

//     // Emit nodes
//     for (const auto& [pid, page] : index) {
//         std::string label;
//         if (page.pageType == IndexPage::LEAF) {
//             label = "{Leaf " + std::to_string(pid) + "|";
//             for (int i = 0; i < page.numKeys; ++i) {
//                 label += std::to_string(page.keys[i]);
//                 if (i + 1 < page.numKeys) label += " | ";
//             }
//             label += "}";
//             out << "  node" << pid << " [label=\"" << label << "\"];\n";
//         } else {
//             label = "{Internal " + std::to_string(pid);
//             for (int i = 0; i <= page.numKeys; ++i) {
//                 label += "|<p" + std::to_string(i) + ">";
//                 if (i < page.numKeys) {
//                     label += std::to_string(page.keys[i]);
//                 }
//             }
//             label += "}";
//             out << "  node" << pid << " [label=\"" << label << "\"];\n";
//         }
//     }

//     out << "\n";

//     // Emit internal → child pointer edges
//     for (const auto& [pid, page] : index) {
//         if (page.pageType == IndexPage::INTERNAL) {
//             for (int i = 0; i < (int)page.pointers.size(); ++i) {
//                 out << "  node" << pid << ":p" << i
//                     << " -> node" << page.pointers[i] << ";\n";
//             }
//         }
//     }

//     // Emit leaf → leaf sibling links (dotted)
//     for (const auto& [pid, page] : index) {
//         if (page.pageType == IndexPage::LEAF && page.lastPointer >= 0) {
//             out << "  node" << pid
//                 << " -> node" << page.lastPointer
//                 << " [style=dotted, arrowhead=none];\n";
//         }
//     }

//     out << "}\n";
//     out.close();
// }

#include "../headers/BTreeIndex.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include <cstring>

using namespace std;

// Sets rootPageId and nextPageId to 0.
// Creates a single leaf node as the root with pageId = 0.
// Stores the node in index, a map of page IDs to IndexPage objects.
BTreeIndex::BTreeIndex(Catalog* catalog)
    : rootPageId(0), nextPageId(0), catalog(catalog) {
    IndexPage root;
    root.pageId = nextPageId++;
    root.pageType = IndexPage::LEAF;
    root.numKeys = 0;
    index[root.pageId] = root;
}

// Clears the index map, freeing all IndexPage object
BTreeIndex::~BTreeIndex() {
    index.clear();
}

// Builds an index by reading records from a heap file and inserting key-RID pairs.
// Sets index metadata (indexFileName, tableName, attributeName).
// Opens the heap file (e.g., customer.heap).
// Iterates through pages, extracting keys (via ExtractKey) and inserting key-RID pairs using a sequential recordCounter.
// Saves the tree to disk (Write), prints stats, and generates a visualization.
void BTreeIndex::Build(const string& indexName,
                       const string& tableName,
                       const string& attributeName) {
    this->indexFileName = indexName;
    this->tableName = tableName;
    this->attributeName = attributeName;

    // cout << "DEBUG: Building index from table: " << tableName
    //      << " on attribute: " << attributeName << endl;

    File dataFile;
    Page page;
    Record rec;

    string tablePath = "../data/" + tableName + ".heap";
    dataFile.Open(1, const_cast<char*>(tablePath.c_str()));
    off_t numPages = dataFile.GetLength();

    // FIX: Use a global record counter instead of page-based encoding
    int recordCounter = 0;
    for (off_t i = 0; i < numPages; ++i) {
        dataFile.GetPage(page, i);
        while (page.GetFirst(rec)) {
            int key = ExtractKey(rec);
            Insert(key, recordCounter++); // Use sequential RID
        }
    }

    // PrintPageStats();
    Write();
    // PrintIndexStructure();
    ExportDOT("btree.dot");
    dataFile.Close();
}

// Retrieves the key value from a record.
// Uses catalog to get the table’s schema and find the attribute’s index.
// Extracts the integer value from the record’s column (supports only Integer type).
// Relevance: Converts raw records to keys for indexing (e.g., c_custkey).
int BTreeIndex::ExtractKey(Record& rec) {
    Schema schema;
    SString sTable(tableName);
    SString sAttr(attributeName);
    if (!catalog->GetSchema(sTable, schema)) {
        // cerr << "ERROR: Table not found in catalog: " << tableName << endl;
        exit(1);
    }
    int attIndex = schema.Index(sAttr);
    if (attIndex < 0) {
        // cerr << "ERROR: Attribute not found: " << attributeName << endl;
        exit(1);
    }
    Type attType = schema.FindType(sAttr);
    char* columnPtr = rec.GetColumn(attIndex);
    if (attType == Integer) {
        int key;
        memcpy(&key, columnPtr, sizeof(int));
        return key;
    } else {
        // cerr << "ERROR: Only INTEGER keys are supported in B+ tree index." << endl;
        exit(1);
    }
}

// Inserts keys into the tree, splitting nodes when full to maintain balance.
// Calls InsertRecursive to insert into the appropriate leaf.
// If a split occurs, creates a new internal root with the promoted key and child pointers.
// Validates the tree structure.
void BTreeIndex::Insert(int key, int pointer) {
    // cout << "DEBUG: Inserting key = " << key
    //      << " pointing to record = " << pointer << endl;

    int promotedKey = -1;
    int newPageId = -1;
    bool split = InsertRecursive(rootPageId, key, pointer, promotedKey, newPageId);

    if (split) {
        IndexPage newRoot;
        newRoot.pageType = IndexPage::INTERNAL;
        newRoot.pageId = nextPageId++;
        newRoot.numKeys = 1;
        newRoot.keys.push_back(promotedKey);
        newRoot.pointers.push_back(rootPageId);
        newRoot.pointers.push_back(newPageId);
        index[newRoot.pageId] = newRoot;
        rootPageId = newRoot.pageId;
        // cout << "DEBUG: Root split. New root pageId = " << rootPageId
        //      << ", promoted key = " << promotedKey << endl;
    }

    ValidateTree();
}

// Recursively inserts a key-RID pair and handles node splits.
// Leaf Node: Inserts key and RID in sorted order using lower_bound. If numKeys > MAX_KEYS, splits the node, promotes the last key of the left node, and links siblings.
// Internal Node: Recursively inserts into the appropriate child, handles child splits, and splits the internal node if needed.
bool BTreeIndex::InsertRecursive(int pageId,
                                 int key,
                                 int pointer,
                                 int& promotedKey,
                                 int& newPageId) {
    IndexPage& node = index[pageId];
    // cout << "DEBUG: Processing page " << pageId
    //      << " (type=" << (node.pageType == IndexPage::LEAF ? "LEAF" : "INTERNAL")
    //      << ", keys=" << node.numKeys << ")" << endl;

    if (node.pageType == IndexPage::LEAF) {
        auto it = lower_bound(node.keys.begin(), node.keys.end(), key);
        int pos = it - node.keys.begin();
        node.keys.insert(it, key);
        node.pointers.insert(node.pointers.begin() + pos, pointer);
        node.numKeys = node.keys.size();

        if (node.numKeys <= MAX_KEYS) return false;

        IndexPage newLeaf;
        newLeaf.pageType = IndexPage::LEAF;
        newLeaf.pageId = nextPageId++;

        int mid = node.numKeys / 2;
        newLeaf.keys = vector<int>(node.keys.begin() + mid, node.keys.end());
        newLeaf.pointers = vector<int>(node.pointers.begin() + mid, node.pointers.end());
        newLeaf.numKeys = newLeaf.keys.size();

        node.keys.resize(mid);
        node.pointers.resize(mid);
        node.numKeys = node.keys.size();

        // Maintain sibling leaf links
        newLeaf.lastPointer = node.lastPointer;
        node.lastPointer = newLeaf.pageId;

        // FIX: Promote the key just before the split point for B+ tree
        promotedKey = node.keys.back(); // Last key in left node
        newPageId = newLeaf.pageId;
        index[newLeaf.pageId] = newLeaf;
        return true;
    }

    int i = 0;
    while (i < node.numKeys && key >= node.keys[i]) ++i;
    int childId = node.pointers[i];

    int childPromoted = -1;
    int childNewPage = -1;
    bool split = InsertRecursive(childId, key, pointer, childPromoted, childNewPage);
    if (!split) return false;

    auto it = upper_bound(node.keys.begin(), node.keys.end(), childPromoted);
    int pos = it - node.keys.begin();
    node.keys.insert(it, childPromoted);
    node.pointers.insert(node.pointers.begin() + pos + 1, childNewPage);
    node.numKeys = node.keys.size();

    if (node.numKeys <= MAX_KEYS) return false;

    IndexPage newInternal;
    newInternal.pageType = IndexPage::INTERNAL;
    newInternal.pageId = nextPageId++;

    int mid = node.numKeys / 2;
    promotedKey = node.keys[mid];

    newInternal.keys = vector<int>(node.keys.begin() + mid + 1, node.keys.end());
    newInternal.pointers = vector<int>(node.pointers.begin() + mid + 1, node.pointers.end());
    newInternal.numKeys = newInternal.keys.size();

    node.keys.resize(mid);
    node.pointers.resize(mid + 1);
    node.numKeys = node.keys.size();

    newPageId = newInternal.pageId;
    index[newInternal.pageId] = newInternal;
    return true;
}

// Writes metadata (rootPageId, nextPageId) to page 0 with a length prefix.
// Writes each IndexPage to disk at pid + 1.
// Serialization (converting tree to disk format).
void BTreeIndex::Write() {
    string fullPath = "../data/" + indexFileName;
    // cout << "DEBUG: Writing index to file: " << fullPath << endl;

    File f;
    f.Open(0, const_cast<char*>(fullPath.c_str()));

    Page metadataPage;
    Record metaRec;
    // FIX: Explicitly set record size with length prefix
    char* buffer = new char[sizeof(int) * 2 + sizeof(int)];
    *(int*)buffer = sizeof(int) * 2; // Length prefix
    int* metaData = reinterpret_cast<int*>(buffer + sizeof(int));
    metaData[0] = rootPageId;
    metaData[1] = nextPageId;
    metaRec.CopyBits(buffer, sizeof(int) * 2 + sizeof(int));
    metadataPage.Append(metaRec);
    f.AddPage(metadataPage, 0);
    delete[] buffer;

    for (auto& [pid, page] : index) {
        f.AddPage(page, pid + 1);
    }

    f.Close();
}

// Deserialization (reconstructing tree from disk).
// Reads metadata from page 0, validating record size.
// Loads each IndexPage into index.
void BTreeIndex::Read(const string& fileName) {
    indexFileName = fileName;
    if (!std::filesystem::exists(fileName) || std::filesystem::file_size(fileName) == 0) {
        // std::cerr << "ERROR: index file not found or empty: " << fileName << std::endl;
        return;
    }

    // cout << "DEBUG: Reading index from file: " << fileName << endl;

    File f;
    f.Open(1, const_cast<char*>(fileName.c_str()));
    off_t len = f.GetLength();

    Page metadataPage;
    f.GetPage(metadataPage, 0);
    Record metaRec;
    if (!metadataPage.GetFirst(metaRec)) {
        // cerr << "ERROR: Failed to read metadata from index file." << endl;
        f.Close();
        return;
    }

    // FIX: Validate metadata record size
    char* bits = metaRec.GetBits();
    int recSize = *(int*)bits;
    if (recSize != sizeof(int) * 2) {
        // cerr << "ERROR: Metadata record size=" << recSize << ", expected " << sizeof(int) * 2 << endl;
        f.Close();
        return;
    }

    int* metaData = reinterpret_cast<int*>(bits + sizeof(int));
    rootPageId = metaData[0];
    nextPageId = metaData[1];

    // cout << "[Read] rootPageId = " << rootPageId << ", nextPageId = " << nextPageId << endl;

    for (off_t i = 1; i < len; ++i) {
        IndexPage page;
        if (f.GetPage(page, i) != 0) {
            //  // cerr << "ERROR: Failed to read index page " << i << " from file." << endl;
            continue;
        }
        // cout << "[Read] Loaded page ID = " << page.pageId << ", keys = ";
        // for (int k : page.keys) cout << k << " ";
        // cout << endl;
        index[page.pageId] = page;
    }

    f.Close();
}

// Searches for keys, returning all matching RIDs (supports duplicates).
// Traverses from root to leaf, following pointers based on key comparisons.
// In the leaf, collects all RIDs for the key, scanning siblings via lastPointer for duplicates.
// Enables point queries, critical for ScanIndex.
bool BTreeIndex::Find(int key, vector<int>& pageNumbersOut) {
    int current = rootPageId;
    while (true) {
        if (index.find(current) == index.end()) {
            // cerr << "[Find] ERROR: Page " << current << " not found!" << endl;
            return false;
        }
        IndexPage& node = index[current];
        if (node.pageType == IndexPage::LEAF) {
            bool found = false;
            // Scan current leaf for all occurrences of key
            for (int i = 0; i < node.numKeys; ++i) {
                if (node.keys[i] == key) {
                    pageNumbersOut.push_back(node.pointers[i]);
                    found = true;
                }
                // Stop if we pass the key (keys are sorted)
                else if (found && node.keys[i] != key) {
                    break;
                }
            }
            // Check sibling leaves for more duplicates
            while (found && node.lastPointer >= 0) {
                current = node.lastPointer;
                if (index.find(current) == index.end()) {
                    // cerr << "[Find] ERROR: Sibling page " << current << " not found!" << endl;
                    break;
                }
                node = index[current];
                for (int i = 0; i < node.numKeys; ++i) {
                    if (node.keys[i] == key) {
                        pageNumbersOut.push_back(node.pointers[i]);
                    } else {
                        break; // Stop if keys increase (sorted order)
                    }
                }
            }
            return !pageNumbersOut.empty();
        }
        int i = 0;
        while (i < node.numKeys && key >= node.keys[i]) ++i;
        current = node.pointers[i];
    }
}

void BTreeIndex::ValidateTree() {
    // cout << "DEBUG: Validating tree structure..." << endl;
    if (index.find(rootPageId) == index.end()) {
        // cerr << "ERROR: Root page " << rootPageId << " does not exist!" << endl;
        return;
    }
    for (auto& [pid, page] : index) {
        if (page.pageType == IndexPage::INTERNAL) {
            size_t exp = page.numKeys + 1;
            if (page.pointers.size() != exp) {
                // cerr << "ERROR: Page " << pid << " pointer count " << page.pointers.size()
                //      << ", expected " << exp << endl;
            }
            for (int ptr : page.pointers) {
                if (index.find(ptr) == index.end()) {
                    // cerr << "ERROR: Page " << pid << " references missing child " << ptr << endl;
                }
            }
        }
        // FIX: Validate leaf sibling links
        if (page.pageType == IndexPage::LEAF && page.lastPointer >= 0) {
            if (index.find(page.lastPointer) == index.end()) {
                // cerr << "ERROR: Leaf page " << pid << " has invalid lastPointer " << page.lastPointer << endl;
            }
        }
    }
    // cout << "DEBUG: Tree validation complete." << endl;
}

void BTreeIndex::PrintPageStats() const {
    // cout << "DEBUG: B+ Tree Stats:" << endl;
    // cout << "  Total pages: " << index.size() << endl;
    // cout << "  Root page: " << rootPageId << endl;
    // cout << "  Next page ID: " << nextPageId << endl;
    int leafCount = 0, internalCount = 0;
    for (auto& [pid, page] : index) {
        if (page.pageType == IndexPage::LEAF) ++leafCount;
        else ++internalCount;
    }
    // cout << "  Leaf pages: " << leafCount << endl;
    // cout << "  Internal pages: " << internalCount << endl;
}

void BTreeIndex::PrintIndexStructure() const {
    // cout << "\nDEBUG: B+ Tree Structure (Visualized):\n";
    for (auto& [pid, page] : index) {
        // cout << "[Page " << pid << "] "
        //      << (page.pageType == IndexPage::LEAF ? "LEAF" : "INTERNAL")
        //      << "\n  Keys: ";
        // for (int k : page.keys) cout << k << " | ";
        // cout << "\n  Pointers: ";
        // for (int p : page.pointers) cout << p << " -> ";
        // cout << "\n";
        if (page.pageType == IndexPage::LEAF) {
            // cout << "  LastPointer (sibling leaf): " << page.lastPointer << "\n";
        }
    }
}

void BTreeIndex::ExportDOT(const std::string& filename) const {
    std::ofstream out(filename);
    out << "digraph BPlusTree {\n"
           "  graph [rankdir=TB, splines=ortho];\n"
           "  node [shape=record, fontsize=10];\n"
           "  edge [arrowsize=0.7];\n\n";

    // Emit nodes
    for (const auto& [pid, page] : index) {
        std::string label;
        if (page.pageType == IndexPage::LEAF) {
            label = "{Leaf " + std::to_string(pid) + "|";
            for (int i = 0; i < page.numKeys; ++i) {
                label += std::to_string(page.keys[i]);
                if (i + 1 < page.numKeys) label += " | ";
            }
            label += "}";
            out << "  node" << pid << " [label=\"" << label << "\"];\n";
        } else {
            label = "{Internal " + std::to_string(pid);
            for (int i = 0; i <= page.numKeys; ++i) {
                label += "|<p" + std::to_string(i) + ">";
                if (i < page.numKeys) {
                    label += std::to_string(page.keys[i]);
                }
            }
            label += "}";
            out << "  node" << pid << " [label=\"" << label << "\"];\n";
        }
    }

    out << "\n";

    // Emit internal → child pointer edges
    for (const auto& [pid, page] : index) {
        if (page.pageType == IndexPage::INTERNAL) {
            for (int i = 0; i < (int)page.pointers.size(); ++i) {
                out << "  node" << pid << ":p" << i
                    << " -> node" << page.pointers[i] << ";\n";
            }
        }
    }

    // Emit leaf → leaf sibling links (dotted)
    for (const auto& [pid, page] : index) {
        if (page.pageType == IndexPage::LEAF && page.lastPointer >= 0) {
            out << "  node" << pid
                << " -> node" << page.lastPointer
                << " [style=dotted, arrowhead=none];\n";
        }
    }

    out << "}\n";
    out.close();
}