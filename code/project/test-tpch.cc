// #include <string>
// #include <iostream>
// #include <cstdlib>
// #include <cstdio>
// #include <vector>

// #include "Schema.h"
// #include "Catalog.h"

// using namespace std;

// int main(int argc, char* argv[]) {
//     // if (argc != 2) {
//     //     cout << "Usage: " << argv[0] << " [sqlite_file]" << endl;
//     //     return -1;
//     // }

//     SString dbFile("tpch.sqlite");
//     Catalog catalog(dbFile);
//     cout << catalog << endl;
//     cout.flush();

//     // A small struct to hold table definitions
//     struct TableDefinition {
//         string name;
//         vector<string> attributes;
//         vector<string> types;
//     };

//     // Define the TPC-H schema in one place
//     vector<TableDefinition> tpchTables = {
//         {
//             "region",
//             { "r_regionkey", "r_name", "r_comment" },
//             { "INTEGER",     "STRING",  "STRING" }
//         },
//         {
//             "nation",
//             { "n_nationkey", "n_name", "n_regionkey", "n_comment" },
//             { "INTEGER",     "STRING", "INTEGER",     "STRING" }
//         },
//         {
//             "part",
//             { "p_partkey", "p_name", "p_mfgr", "p_brand", "p_type", 
//               "p_size", "p_container", "p_retailprice", "p_comment" },
//             { "INTEGER",  "STRING", "STRING", "STRING", "STRING",
//               "INTEGER",  "STRING", "FLOAT",  "STRING" }
//         },
//         {
//             "supplier",
//             { "s_suppkey", "s_name", "s_address", "s_nationkey", 
//               "s_phone", "s_acctbal", "s_comment" },
//             { "INTEGER",  "STRING",  "STRING",    "INTEGER",
//               "STRING",   "FLOAT",   "STRING" }
//         },
//         {
//             "partsupp",
//             { "ps_partkey", "ps_suppkey", "ps_availqty", "ps_supplycost", 
//               "ps_comment" },
//             { "INTEGER",    "INTEGER",    "INTEGER",     "FLOAT",
//               "STRING" }
//         },
//         {
//             "customer",
//             { "c_custkey", "c_name", "c_address", "c_nationkey",
//               "c_phone", "c_acctbal", "c_mktsegment", "c_comment" },
//             { "INTEGER",   "STRING", "STRING",    "INTEGER",
//               "STRING",    "FLOAT",  "STRING",    "STRING" }
//         },
//         {
//             "orders",
//             { "o_orderkey", "o_custkey", "o_orderstatus", "o_totalprice",
//               "o_orderdate", "o_orderpriority", "o_clerk", 
//               "o_shippriority", "o_comment" },
//             { "INTEGER",    "INTEGER",    "STRING",       "FLOAT",
//               "STRING",     "STRING",     "STRING",
//               "INTEGER",    "STRING" }
//         },
//         {
//             "lineitem",
//             { "l_orderkey", "l_partkey", "l_suppkey", "l_linenumber",
//               "l_quantity", "l_extendedprice", "l_discount", "l_tax",
//               "l_returnflag", "l_linestatus", "l_shipdate", "l_commitdate",
//               "l_receiptdate", "l_shipinstruct", "l_shipmode", "l_comment" },
//             { "INTEGER",    "INTEGER",   "INTEGER",   "INTEGER",
//               "FLOAT",      "FLOAT",     "FLOAT",     "FLOAT",
//               "STRING",     "STRING",    "STRING",    "STRING",
//               "STRING",     "STRING",    "STRING",    "STRING" }
//         }
//     };

//     // Loop over all table definitions
//     for (auto &table : tpchTables) {
//         SString tName(table.name.c_str());

//         // Convert std::vector to StringVector
//         StringVector atts;
//         StringVector types;
//         for (size_t i = 0; i < table.attributes.size(); i++) {
//             SString attr(table.attributes[i].c_str());
//             atts.Append(attr);
//             SString type(table.types[i].c_str());
//             types.Append(type);
//         }

//         // Print what we're about to create
//         cout << "CREATE TABLE " << table.name << endl;
//         cout << atts << endl;
//         cout << types << endl;

//         // Create the table in the catalog
//         bool ret = catalog.CreateTable(tName, atts, types);
//         if (ret) {
//             // For demonstration, set the number of distinct values 
//             // for each attribute to some constant (e.g. 100).
//             for (int j = 0; j < atts.Length(); j++) {
//                 SInt dist = 100; 
//                 cout << atts[j] << " distinct = " << dist << endl;
//                 catalog.SetNoDistinct(tName, atts[j], dist);
//             }

//             // Set the (dummy) total number of tuples
//             SInt tuples = 1000; 
//             cout << "tuples = " << tuples << endl;
//             catalog.SetNoTuples(tName, tuples);

//             // Set the data file path
//             string path = table.name + ".dat";
//             SString sp(path.c_str());
//             cout << "path = " << sp << endl;
//             catalog.SetDataFile(tName, sp);

//             cout << "CREATE TABLE " << table.name << " OK" << endl;
//         } else {
//             cout << "CREATE TABLE " << table.name << " FAIL" << endl;
//         }
//     }

//     // Save the catalog
//     catalog.Save();
//     cout << catalog << endl;
//     cout.flush();

//     // List all tables we have so far
//     cout << "tables" << endl;
//     {
//         StringVector tables;
//         catalog.GetTables(tables);
//         for (int i = 0; i < tables.Length(); i++) {
//             cout << tables[i] << endl;
//         }
//         cout << endl;
//     }

//     // You can add extra code here for random lookups, dropping tables, etc.
//     // following the style of your existing example.

//     return 0;
// }


#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>

#include "Schema.h"
#include "Catalog.h"

using namespace std;

// A structure that contains all information needed for a table.
struct TableDefinition {
    string name;
    vector<string> attributes;
    vector<string> types;
    int tuples;
    vector<int> distincts;
    string dataFile;
};

int main(int argc, char* argv[]) {
    // Open the catalog on the given sqlite file.
    SString dbFile("tpch.sqlite");
    Catalog catalog(dbFile);
    cout << catalog << endl;
    cout.flush();

    // Define the TPC-H schema along with tuple counts and distinct values,
    // following the values in your reference file.
    vector<TableDefinition> tpchTables = {
        {
            "region",
            { "r_regionkey", "r_name", "r_comment" },
            { "INTEGER",     "STRING",  "STRING" },
            5,
            { 5, 5, 5 },
            "region.dat"
        },
        {
            "nation",
            { "n_nationkey", "n_name", "n_regionkey", "n_comment" },
            { "INTEGER",     "STRING", "INTEGER",     "STRING" },
            25,
            { 25, 25, 5, 25 },
            "nation.dat"
        },
        {
            "part",
            { "p_partkey", "p_name", "p_mfgr", "p_brand", "p_type", 
              "p_size", "p_container", "p_retailprice", "p_comment" },
            { "INTEGER",  "STRING", "STRING", "STRING", "STRING",
              "INTEGER",  "STRING", "FLOAT",  "STRING" },
            2000,
            { 2000, 2000, 5, 25, 150, 50, 40, 1099, 2000 },
            "part.dat"
        },
        {
            "supplier",
            { "s_suppkey", "s_name", "s_address", "s_nationkey", 
              "s_phone", "s_acctbal", "s_comment" },
            { "INTEGER",  "STRING",  "STRING",    "INTEGER",
              "STRING",   "FLOAT",   "STRING" },
            100,
            { 100, 100, 100, 25, 100, 100, 100 },
            "supplier.dat"
        },
        {
            "partsupp",
            { "ps_partkey", "ps_suppkey", "ps_availqty", "ps_supplycost", 
              "ps_comment" },
            { "INTEGER",    "INTEGER",    "INTEGER",     "FLOAT",
              "STRING" },
            8000,
            { 2000, 2000, 5497, 7665, 8000 },
            "partsupp.dat"
        },
        {
            "customer",
            { "c_custkey", "c_name", "c_address", "c_nationkey",
              "c_phone", "c_acctbal", "c_mktsegment", "c_comment" },
            { "INTEGER",   "STRING", "STRING",    "INTEGER",
              "STRING",    "FLOAT",  "STRING",    "STRING" },
            1500,
            { 1500, 1500, 1500, 25, 1500, 1499, 5, 1500 },
            "customer.dat"
        },
        {
            "orders",
            { "o_orderkey", "o_custkey", "o_orderstatus", "o_totalprice",
              "o_orderdate", "o_orderpriority", "o_clerk", 
              "o_shippriority", "o_comment" },
            { "INTEGER",    "INTEGER",    "STRING",       "FLOAT",
              "STRING",     "STRING",     "STRING",
              "INTEGER",    "STRING" },
            15000,
            { 15000, 1000, 3, 14996, 2401, 5, 1000, 1, 15000 },
            "orders.dat"
        },
        {
            "lineitem",
            { "l_orderkey", "l_partkey", "l_suppkey", "l_linenumber",
              "l_quantity", "l_extendedprice", "l_discount", "l_tax",
              "l_returnflag", "l_linestatus", "l_shipdate", "l_commitdate",
              "l_receiptdate", "l_shipinstruct", "l_shipmode", "l_comment" },
            { "INTEGER",    "INTEGER",   "INTEGER",   "INTEGER",
              "FLOAT",      "FLOAT",     "FLOAT",     "FLOAT",
              "STRING",     "STRING",    "STRING",    "STRING",
              "STRING",     "STRING",    "STRING",    "STRING" },
            60175,
            { 15000, 2000, 100, 7, 50, 35921, 11, 9, 3, 2, 2518, 2460, 2529, 4, 7, 60175 },
            "lineitem.dat"
        }
    };

    // Loop over all table definitions and create them in the catalog.
    for (auto &table : tpchTables) {
        SString tName(table.name.c_str());

        // Convert std::vector to StringVector
        StringVector atts;
        StringVector types;
        for (size_t i = 0; i < table.attributes.size(); i++) {
            SString attr(table.attributes[i].c_str());
            atts.Append(attr);
            SString type(table.types[i].c_str());
            types.Append(type);
        }

        // Print what we're about to create
        cout << "CREATE TABLE " << table.name << endl;
        cout << atts << endl;
        cout << types << endl;

        // Create the table in the catalog.
        bool ret = catalog.CreateTable(tName, atts, types);
        if (ret) {
            // Set the number of distinct values per attribute.
            for (size_t j = 0; j < table.attributes.size(); j++) {
                SString attr(table.attributes[j].c_str());
                SInt distinct = table.distincts[j];
                cout << attr << " distinct = " << distinct << endl;
                catalog.SetNoDistinct(tName, attr, distinct);
            }

            // Set the total number of tuples.
            SInt tuples = table.tuples;
            cout << "tuples = " << tuples << endl;
            catalog.SetNoTuples(tName, tuples);

            // Set the data file path.
            SString sp(table.dataFile.c_str());
            cout << "path = " << sp << endl;
            catalog.SetDataFile(tName, sp);

            cout << "CREATE TABLE " << table.name << " OK" << endl;
        } else {
            cout << "CREATE TABLE " << table.name << " FAIL" << endl;
        }
    }

    // Save the catalog.
    catalog.Save();
    cout << catalog << endl;
    cout.flush();

    // List all tables we have so far.
    cout << "tables" << endl;
    {
        StringVector tables;
        catalog.GetTables(tables);
        for (int i = 0; i < tables.Length(); i++) {
            cout << tables[i] << endl;
        }
        cout << endl;
    }

    return 0;
}
