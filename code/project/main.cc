

// // #include <iostream>
// // #include <sstream>
// // #include <string>
// // #include <cstdlib>
// // #include <chrono>
// // #include <sys/resource.h>

// // #include "Catalog.h"
// // #include "QueryCompiler.h"
// // #include "RelOp.h"
// // #include "Record.h"
// // #include "BTreeIndex.h"

// // extern "C" {
// //     #include "QueryParser.h"
// //     int yyparse();
// //     void yylex_destroy();
// //     typedef struct yy_buffer_state *YY_BUFFER_STATE;
// //     YY_BUFFER_STATE yy_scan_string(const char *str);
// //     void            yy_delete_buffer(YY_BUFFER_STATE buffer);
// // }

// // // forward‐declare the cleanup helpers from QueryCompiler.cpp:
// // void freeTableList(TableList*);
// // void freeNameList(NameList*);
// // void freeAndList(AndList*);
// // void releaseFuncOperator(FuncOperator*);

// // using namespace std;

// // extern FuncOperator* finalFunction;
// // extern TableList*    tables;
// // extern AndList*      predicate;
// // extern NameList*     groupingAtts;
// // extern NameList*     attsToSelect;
// // extern int           distinctAtts;

// // extern char* createIndexName;
// // extern char* createIndexTable;
// // extern char* createIndexAttribute;

// // void showHelp() {
// //     cout
// //       << ".help               Show this message\n"
// //       << ".tables             List all tables\n"
// //       << ".schema <table>     Show schema of a table\n"
// //       << ".quit               Exit the CLI\n";
// // }

// // int main() {
// //     cout
// //       << "Welcome to MiniSQL CLI (Phase 5)\n"
// //       << "Type .help for command list or SQL queries ending with ;\n";

// //     SString dbFile("catalog.sqlite");
// //     Catalog catalog(dbFile);
// //     QueryCompiler compiler(catalog);

// //     string line, queryBuffer;
// //     while (true) {
// //         cout << (queryBuffer.empty() ? "MiniSQL> " : "        > ");
// //         if (!getline(cin, line) || cin.eof()) break;
// //         if (line.empty()) continue;

// //         auto first = line.find_first_not_of(" \t");
// //         if (first == string::npos) continue;
// //         string trimmed = line.substr(first);

// //         // dot‑commands
// //         if (trimmed[0]=='.' && queryBuffer.empty()) {
// //             istringstream iss(trimmed);
// //             string cmd; iss >> cmd;
// //             if      (cmd==".quit")  break;
// //             else if (cmd==".help")  showHelp();
// //             else if (cmd==".tables") {
// //                 StringVector tv; catalog.GetTables(tv);
// //                 for (int i = 0; i < tv.Length(); i++)
// //                     cout << tv[i] << "\n";
// //             }
// //             else if (cmd==".schema") {
// //                 string tblName; iss>>tblName;
// //                 if (tblName.empty()) cout<<"Usage: .schema <table>\n";
// //                 else {
// //                     Schema s;
// //                     SString ss(tblName);
// //                     if (!catalog.GetSchema(ss, s))
// //                         cout<<"Table not found.\n";
// //                     else
// //                         cout<<s<<"\n";
// //                 }
// //             }
// //             else {
// //                 cout<<"Unknown command: "<<trimmed<<"\n";
// //             }
// //             continue;
// //         }

// //         // accumulate until we see ';'
// //         queryBuffer += line + "\n";
// //         if (queryBuffer.find(';') == string::npos) continue;

// //         // pull out the statement
// //         auto semi = queryBuffer.find(';');
// //         string query = queryBuffer.substr(0, semi);

// //         // reset parser globals
// //         finalFunction        = nullptr;
// //         tables               = nullptr;
// //         predicate            = nullptr;
// //         groupingAtts         = nullptr;
// //         attsToSelect         = nullptr;
// //         distinctAtts         = 0;
// //         createIndexName      = nullptr;
// //         createIndexTable     = nullptr;
// //         createIndexAttribute = nullptr;

// //         // feed the SQL into Bison/Flex
// //         YY_BUFFER_STATE buf = yy_scan_string(query.c_str());
// //         int parseStatus     = yyparse();
// //         yy_delete_buffer(buf);

// //         if (parseStatus != 0) {
// //             cerr<<"Error: invalid SQL!\n";
// //             queryBuffer.clear();
// //             continue;
// //         }

// //         // CREATE INDEX?
// //         if (createIndexName && createIndexTable && createIndexAttribute) {
// //             string idxFile = string(createIndexTable) + "_"
// //                            + string(createIndexAttribute) + ".bin";
// //             BTreeIndex idx(&catalog);
// //             idx.Build(idxFile,
// //                       createIndexTable,
// //                       createIndexAttribute);
// //             cout<<"Index "<<createIndexName
// //                 <<" built on "<<createIndexTable<<"."<<createIndexAttribute
// //                 <<" → file: "<<idxFile<<"\n";
// //             queryBuffer.clear();
// //             continue;
// //         }

// //         // otherwise it’s a SELECT (or GROUP) query
// //         QueryExecutionTree qTree;
// //         compiler.Compile(
// //             tables,
// //             attsToSelect,
// //             finalFunction,
// //             predicate,
// //             groupingAtts,
// //             distinctAtts,
// //             qTree
// //         );
// //         cout<<qTree<<"\n";

// //         //
// //         // ─── TIMING WRAPPER ───────────────────────────────────────────────────────
// //         //
// //         // 1) snapshot wall‑clock and resource usage
// //         auto wall_start = chrono::high_resolution_clock::now();
// //         struct rusage usage_start;
// //         getrusage(RUSAGE_SELF, &usage_start);

// //         // 2) run the query
// //         qTree.ExecuteQuery();

// //         // 3) snapshot again
// //         auto wall_end = chrono::high_resolution_clock::now();
// //         struct rusage usage_end;
// //         getrusage(RUSAGE_SELF, &usage_end);

// //         // 4) compute deltas
// //         auto wall_ns = chrono::duration_cast<chrono::nanoseconds>(wall_end - wall_start).count();
// //         double wall_ms = wall_ns / 1e6;

// //         long user_usec = (usage_end.ru_utime.tv_sec  - usage_start.ru_utime.tv_sec)*1000000L
// //                        + (usage_end.ru_utime.tv_usec - usage_start.ru_utime.tv_usec);

// //         long sys_usec  = (usage_end.ru_stime.tv_sec  - usage_start.ru_stime.tv_sec)*1000000L
// //                        + (usage_end.ru_stime.tv_usec - usage_start.ru_stime.tv_usec);

// //         long in_blocks  = usage_end.ru_inblock  - usage_start.ru_inblock;
// //         long out_blocks = usage_end.ru_oublock  - usage_start.ru_oublock;

// //         // 5) print timing breakdown
// //         cout << "\n[Timing]\n"
// //              << "  Wall clock : " << wall_ms       << " ms\n"
// //              << "  User  CPU  : " << user_usec/1000.0 << " ms\n"
// //              << "  Sys   CPU  : " << sys_usec /1000.0 << " ms\n"
// //              << "  Blk reads  : " << in_blocks    << "\n"
// //              << "  Blk writes : " << out_blocks   << "\n\n";
// //         //
// //         // ──────────────────────────────────────────────────────────────────────────
// //         //

// //         // now tear down the parse lists
// //         freeTableList(tables);
// //         freeNameList(attsToSelect);
// //         freeNameList(groupingAtts);
// //         freeAndList(predicate);
// //         releaseFuncOperator(finalFunction);

// //         tables        = nullptr;
// //         attsToSelect  = nullptr;
// //         groupingAtts  = nullptr;
// //         predicate     = nullptr;
// //         finalFunction = nullptr;
// //         distinctAtts  = 0;

// //         queryBuffer.clear();
// //     }

// //     yylex_destroy();
// //     cout<<"Goodbye.\n";
// //     return 0;
// // }

// // #include <iostream>
// // #include <sstream>
// // #include <string>
// // #include <cstdlib>
// // #include <chrono>

// // #include "Catalog.h"
// // #include "QueryCompiler.h"
// // #include "RelOp.h"
// // #include "Record.h"
// // #include "BTreeIndex.h"

// // extern "C" {
// //     #include "QueryParser.h"
// //     int yyparse();
// //     void yylex_destroy();
// //     typedef struct yy_buffer_state *YY_BUFFER_STATE;
// //     YY_BUFFER_STATE yy_scan_string(const char *str);
// //     void            yy_delete_buffer(YY_BUFFER_STATE buffer);
// // }

// // // forward‐declare the cleanup helpers from QueryCompiler.cpp:
// // void freeTableList(TableList*);
// // void freeNameList(NameList*);
// // void freeAndList(AndList*);
// // void releaseFuncOperator(FuncOperator*);

// // using namespace std;

// // extern FuncOperator* finalFunction;
// // extern TableList*    tables;
// // extern AndList*      predicate;
// // extern NameList*     groupingAtts;
// // extern NameList*     attsToSelect;
// // extern int           distinctAtts;

// // extern char* createIndexName;
// // extern char* createIndexTable;
// // extern char* createIndexAttribute;

// // void showHelp() {
// //     cout
// //       << ".help               Show this message\n"
// //       << ".tables             List all tables\n"
// //       << ".schema <table>     Show schema of a table\n"
// //       << ".quit               Exit the CLI\n";
// // }

// // int main() {
// //     cout
// //       << "Welcome to MiniSQL CLI (Phase 5)\n"
// //       << "Type .help for command list or SQL queries ending with ;\n";

// //     SString dbFile("/home/sachin/database/2025-spring-cse177-eecs277/code/catalog.sqlite");
// //     Catalog catalog(dbFile);
// //     QueryCompiler compiler(catalog);

// //     string line, queryBuffer;
// //     while (true) {
// //         cout << (queryBuffer.empty() ? "MiniSQL> " : "        > ");
// //         if (!getline(cin, line) || cin.eof()) break;
// //         if (line.empty()) continue;

// //         auto first = line.find_first_not_of(" \t");
// //         if (first == string::npos) continue;
// //         string trimmed = line.substr(first);

// //         // dot‑commands
// //         if (trimmed[0]=='.' && queryBuffer.empty()) {
// //             istringstream iss(trimmed);
// //             string cmd; iss >> cmd;
// //             if      (cmd==".quit")  break;
// //             else if (cmd==".help")  showHelp();
// //             else if (cmd==".tables") {
// //                 StringVector tv; catalog.GetTables(tv);
// //                 for (int i = 0; i < tv.Length(); i++)
// //                     cout << tv[i] << "\n";
// //             }
// //             else if (cmd==".schema") {
// //                 string tblName; iss>>tblName;
// //                 if (tblName.empty()) cout<<"Usage: .schema <table>\n";
// //                 else {
// //                     Schema s;
// //                     SString ss(tblName);
// //                     if (!catalog.GetSchema(ss, s))
// //                         cout<<"Table not found.\n";
// //                     else
// //                         cout<<s<<"\n";
// //                 }
// //             }
// //             else {
// //                 cout<<"Unknown command: "<<trimmed<<"\n";
// //             }
// //             continue;
// //         }

// //         // accumulate until we see ';'
// //         queryBuffer += line + "\n";
// //         if (queryBuffer.find(';') == string::npos) continue;

// //         // pull out the statement
// //         auto semi = queryBuffer.find(';');
// //         string query = queryBuffer.substr(0, semi);

// //         // reset parser globals
// //         finalFunction        = nullptr;
// //         tables               = nullptr;
// //         predicate            = nullptr;
// //         groupingAtts         = nullptr;
// //         attsToSelect         = nullptr;
// //         distinctAtts         = 0;
// //         createIndexName      = nullptr;
// //         createIndexTable     = nullptr;
// //         createIndexAttribute = nullptr;

// //         // feed the SQL into Bison/Flex
// //         YY_BUFFER_STATE buf = yy_scan_string(query.c_str());
// //         int parseStatus     = yyparse();
// //         yy_delete_buffer(buf);

// //         if (parseStatus != 0) {
// //             cerr<<"Error: invalid SQL!\n";
// //             queryBuffer.clear();
// //             continue;
// //         }

// //         // CREATE INDEX?
// //         if (createIndexName && createIndexTable && createIndexAttribute) {
// //             string idxFile = string(createIndexTable) + "_"
// //                            + string(createIndexAttribute) + ".bin";
// //             BTreeIndex idx(&catalog);
// //             idx.Build(idxFile,
// //                       createIndexTable,
// //                       createIndexAttribute);
// //             cout<<"Index "<<createIndexName
// //                 <<" built on "<<createIndexTable<<"."<<createIndexAttribute
// //                 <<" → file: "<<idxFile<<"\n";
// //             queryBuffer.clear();
// //             continue;
// //         }

// //         // otherwise it’s a SELECT (or GROUP) query
// //         QueryExecutionTree qTree;
// //         compiler.Compile(
// //             tables,
// //             attsToSelect,
// //             finalFunction,
// //             predicate,
// //             groupingAtts,
// //             distinctAtts,
// //             qTree
// //         );
// //         cout<<qTree<<"\n";

// //         //
// //         // ─── SIMPLE TIMING ────────────────────────────────────────────────────────
// //         auto t0 = chrono::high_resolution_clock::now();
// //         qTree.ExecuteQuery();
// //         auto t1 = chrono::high_resolution_clock::now();

// //         double elapsed_ms = chrono::duration<double, std::milli>(t1 - t0).count();
// //         cout << "Query executed in " << elapsed_ms << " ms\n\n";
// //         // ──────────────────────────────────────────────────────────────────────────
// //         //

// //         // now tear down the parse lists
// //         freeTableList(tables);
// //         freeNameList(attsToSelect);
// //         freeNameList(groupingAtts);
// //         freeAndList(predicate);
// //         releaseFuncOperator(finalFunction);

// //         tables        = nullptr;
// //         attsToSelect  = nullptr;
// //         groupingAtts  = nullptr;
// //         predicate     = nullptr;
// //         finalFunction = nullptr;
// //         distinctAtts  = 0;

// //         queryBuffer.clear();
// //     }

// //     yylex_destroy();
// //     cout<<"Goodbye.\n";
// //     return 0;
// // }


// // // main.cc
// // #include <iostream>
// // #include <sstream>
// // #include <string>
// // #include <cstdlib>
// // #include <chrono>

// // #include "Catalog.h"
// // #include "QueryCompiler.h"
// // #include "RelOp.h"
// // #include "Record.h"
// // #include "BTreeIndex.h"

// // extern "C" {
// //     #include "QueryParser.h"
// //     int yyparse();
// //     void yylex_destroy();
// //     typedef struct yy_buffer_state *YY_BUFFER_STATE;
// //     YY_BUFFER_STATE yy_scan_string(const char *str);
// //     void            yy_delete_buffer(YY_BUFFER_STATE buffer);
// // }

// // // forward‐declare the cleanup helpers from QueryCompiler.cpp:
// // void freeTableList(TableList*);
// // void freeNameList(NameList*);
// // void freeAndList(AndList*);
// // void releaseFuncOperator(FuncOperator*);

// // using namespace std;

// // extern FuncOperator* finalFunction;
// // extern TableList*    tables;
// // extern AndList*      predicate;
// // extern NameList*     groupingAtts;
// // extern NameList*     attsToSelect;
// // extern int           distinctAtts;

// // extern char* createIndexName;
// // extern char* createIndexTable;
// // extern char* createIndexAttribute;

// // void showHelp() {
// //     cout
// //       << ".help               Show this message\n"
// //       << ".tables             List all tables\n"
// //       << ".schema <table>     Show schema of a table\n"
// //       << ".quit               Exit the CLI\n";
// // }

// // int main() {
// //     cout
// //       << "Welcome to MiniSQL CLI (Phase 5)\n"
// //       << "Type .help for command list or SQL queries ending with ;\n";

// //     SString dbFile("/home/sachin/database/2025-spring-cse177-eecs277/code/catalog.sqlite");
// //     Catalog catalog(dbFile);
// //     QueryCompiler compiler(catalog);

// //     string line, queryBuffer;
// //     while (true) {
// //         cout << (queryBuffer.empty() ? "MiniSQL> " : "        > ");
// //         if (!getline(cin, line) || cin.eof()) break;
// //         if (line.empty()) continue;

// //         auto first = line.find_first_not_of(" \t");
// //         if (first == string::npos) continue;
// //         string trimmed = line.substr(first);

// //         // dot‑commands
// //         if (trimmed[0]=='.' && queryBuffer.empty()) {
// //             istringstream iss(trimmed);
// //             string cmd; iss >> cmd;
// //             if      (cmd==".quit")  break;
// //             else if (cmd==".help")  showHelp();
// //             else if (cmd==".tables") {
// //                 StringVector tv; catalog.GetTables(tv);
// //                 for (int i = 0; i < tv.Length(); i++)
// //                     cout << tv[i] << "\n";
// //             }
// //             else if (cmd==".schema") {
// //                 string tblName; iss>>tblName;
// //                 if (tblName.empty()) cout<<"Usage: .schema <table>\n";
// //                 else {
// //                     Schema s;
// //                     SString ss(tblName);
// //                     if (!catalog.GetSchema(ss, s))
// //                         cout<<"Table not found.\n";
// //                     else
// //                         cout<<s<<"\n";
// //                 }
// //             }
// //             else {
// //                 cout<<"Unknown command: "<<trimmed<<"\n";
// //             }
// //             continue;
// //         }

// //         // accumulate until we see ';'
// //         queryBuffer += line + "\n";
// //         if (queryBuffer.find(';') == string::npos) continue;

// //         // pull out the statement
// //         auto semi = queryBuffer.find(';');
// //         string query = queryBuffer.substr(0, semi);

// //         // reset parser globals
// //         finalFunction        = nullptr;
// //         tables               = nullptr;
// //         predicate            = nullptr;
// //         groupingAtts         = nullptr;
// //         attsToSelect         = nullptr;
// //         distinctAtts         = 0;
// //         createIndexName      = nullptr;
// //         createIndexTable     = nullptr;
// //         createIndexAttribute = nullptr;

// //         // feed the SQL into Bison/Flex
// //         YY_BUFFER_STATE buf = yy_scan_string(query.c_str());
// //         int parseStatus     = yyparse();
// //         yy_delete_buffer(buf);

// //         if (parseStatus != 0) {
// //             cerr<<"Error: invalid SQL!\n";
// //             queryBuffer.clear();
// //             continue;
// //         }

// //         // CREATE INDEX?
// //         if (createIndexName && createIndexTable && createIndexAttribute) {
// //             string idxFile = string(createIndexTable) + "_"
// //                            + string(createIndexAttribute) + ".bin";
// //             BTreeIndex idx(&catalog);
// //             idx.Build(idxFile,
// //                       createIndexTable,
// //                       createIndexAttribute);
// //             cout<<"Index "<<createIndexName
// //                 <<" built on "<<createIndexTable<<"."<<createIndexAttribute
// //                 <<" → file: "<<idxFile<<"\n";
// //             queryBuffer.clear();
// //             continue;
// //         }

// //         // otherwise it’s a SELECT (or GROUP) query
// //         QueryExecutionTree qTree;
// //         compiler.Compile(
// //             tables,
// //             attsToSelect,
// //             finalFunction,
// //             predicate,
// //             groupingAtts,
// //             distinctAtts,
// //             qTree
// //         );
// //         cout<<qTree<<"\n";

// //         //
// //         // ─── SIMPLE TIMING ────────────────────────────────────────────────────────
// //         auto t0 = chrono::high_resolution_clock::now();
// //         qTree.ExecuteQuery();
// //         auto t1 = chrono::high_resolution_clock::now();

// //         double elapsed_ms = chrono::duration<double, std::milli>(t1 - t0).count();
// //         cout << "Query executed in " << elapsed_ms << " ms\n\n";

// //         //
// //         // ─── RENDER QUERY PLAN ────────────────────────────────────────────────────
// //         // write out the .dot file
// //         qTree.ToDotFile("plan.dot");

// //         // now launch GraphViz’s X11 viewer in the background
// //         cout << "Opening plan.dot in GraphViz viewer...\n";
// //         if (std::system("dot -Tx11 plan.dot &") != 0) {
// //             cerr << "⚠️  could not launch GraphViz viewer; ensure `dot` was built with X11 support\n";
// //         }
// //         // ──────────────────────────────────────────────────────────────────────────
// //         //

// //         // tear down the parse lists
// //         freeTableList(tables);
// //         freeNameList(attsToSelect);
// //         freeNameList(groupingAtts);
// //         freeAndList(predicate);
// //         releaseFuncOperator(finalFunction);

// //         tables        = nullptr;
// //         attsToSelect  = nullptr;
// //         groupingAtts  = nullptr;
// //         predicate     = nullptr;
// //         finalFunction = nullptr;
// //         distinctAtts  = 0;

// //         queryBuffer.clear();
// //     }

// //     yylex_destroy();
// //     cout<<"Goodbye.\n";
// //     return 0;
// // }


// main.cc
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <chrono>

#include "Catalog.h"
#include "QueryCompiler.h"
#include "RelOp.h"
#include "Record.h"
#include "BTreeIndex.h"

extern "C" {
    #include "QueryParser.h"
    int yyparse();
    void yylex_destroy();
    typedef struct yy_buffer_state *YY_BUFFER_STATE;
    YY_BUFFER_STATE yy_scan_string(const char *str);
    void            yy_delete_buffer(YY_BUFFER_STATE buffer);
}

// forward‐declare the cleanup helpers from QueryCompiler.cpp:
void freeTableList(TableList*);
void freeNameList(NameList*);
void freeAndList(AndList*);
void releaseFuncOperator(FuncOperator*);

using namespace std;

extern FuncOperator* finalFunction;
extern TableList*    tables;
extern AndList*      predicate;
extern NameList*     groupingAtts;
extern NameList*     attsToSelect;
extern int           distinctAtts;

extern char* createIndexName;
extern char* createIndexTable;
extern char* createIndexAttribute;

void showHelp() {
    cout
      << ".help               Show this message\n"
      << ".tables             List all tables\n"
      << ".schema <table>     Show schema of a table\n"
      << ".quit               Exit the CLI\n";
}

int main() {
    cout
      << "Welcome to MiniSQL CLI (Phase 5)\n"
      << "Type .help for command list or SQL queries ending with ;\n";

    SString dbFile("/home/sachin/database/2025-spring-cse177-eecs277/code/catalog.sqlite");
    Catalog catalog(dbFile);
    QueryCompiler compiler(catalog);

    string line, queryBuffer;
    while (true) {
        cout << (queryBuffer.empty() ? "MiniSQL> " : "        > ");
        if (!getline(cin, line) || cin.eof()) break;
        if (line.empty()) continue;

        auto first = line.find_first_not_of(" \t");
        if (first == string::npos) continue;
        string trimmed = line.substr(first);

        // dot‑commands
        if (trimmed[0]=='.' && queryBuffer.empty()) {
            istringstream iss(trimmed);
            string cmd; iss >> cmd;
            if      (cmd==".quit")  break;
            else if (cmd==".help")  showHelp();
            else if (cmd==".tables") {
                StringVector tv; catalog.GetTables(tv);
                for (int i = 0; i < tv.Length(); i++)
                    cout << tv[i] << "\n";
            }
            else if (cmd==".schema") {
                string tblName; iss>>tblName;
                if (tblName.empty()) cout<<"Usage: .schema <table>\n";
                else {
                    Schema s;
                    SString ss(tblName);
                    if (!catalog.GetSchema(ss, s))
                        cout<<"Table not found.\n";
                    else
                        cout<<s<<"\n";
                }
            }
            else {
                cout<<"Unknown command: "<<trimmed<<"\n";
            }
            continue;
        }

        // accumulate until we see ';'
        queryBuffer += line + "\n";
        if (queryBuffer.find(';') == string::npos) continue;

        // pull out the statement
        auto semi = queryBuffer.find(';');
        string query = queryBuffer.substr(0, semi);

        // reset parser globals
        finalFunction        = nullptr;
        tables               = nullptr;
        predicate            = nullptr;
        groupingAtts         = nullptr;
        attsToSelect         = nullptr;
        distinctAtts         = 0;
        createIndexName      = nullptr;
        createIndexTable     = nullptr;
        createIndexAttribute = nullptr;

        // feed the SQL into Bison/Flex
        YY_BUFFER_STATE buf = yy_scan_string(query.c_str());
        int parseStatus     = yyparse();
        yy_delete_buffer(buf);

        if (parseStatus != 0) {
            cerr<<"Error: invalid SQL!\n";
            queryBuffer.clear();
            continue;
        }

        // CREATE INDEX?
        if (createIndexName && createIndexTable && createIndexAttribute) {
            // build the index file
            string idxFile = string(createIndexTable) + "_"
                           + string(createIndexAttribute) + ".bin";
            BTreeIndex idx(&catalog);
            idx.Build(idxFile,
                      createIndexTable,
                      createIndexAttribute);
            cout<<"Index "<<createIndexName
                <<" built on "<<createIndexTable<<"."<<createIndexAttribute
                <<" → file: "<<idxFile<<"\n";

            // ─── RENDER THE B+‑TREE ─────────────────────────────────────────────
            // we exported "btree.dot" inside Build()
            cout << "Opening btree.dot in GraphViz viewer...\n";
            if (std::system("dot -Tx11 btree.dot &") != 0) {
                cerr << "⚠️  could not launch GraphViz viewer; ensure `dot` was built with X11 support\n";
            }
            // ─────────────────────────────────────────────────────────────────────

            queryBuffer.clear();
            continue;
        }

        // otherwise it’s a SELECT (or GROUP) query
        QueryExecutionTree qTree;
        compiler.Compile(
            tables,
            attsToSelect,
            finalFunction,
            predicate,
            groupingAtts,
            distinctAtts,
            qTree
        );
        cout<<qTree<<"\n";

        //
        // ─── SIMPLE TIMING ────────────────────────────────────────────────────────
        auto t0 = chrono::high_resolution_clock::now();
        qTree.ExecuteQuery();
        auto t1 = chrono::high_resolution_clock::now();

        double elapsed_ms = chrono::duration<double, std::milli>(t1 - t0).count();
        cout << "Query executed in " << elapsed_ms << " ms\n\n";

        //
        // ─── RENDER QUERY PLAN ────────────────────────────────────────────────────
        qTree.ToDotFile("plan.dot");
        cout << "Opening plan.dot in GraphViz viewer...\n";
        if (std::system("dot -Tx11 plan.dot &") != 0) {
            cerr << "⚠️  could not launch GraphViz viewer; ensure `dot` was built with X11 support\n";
        }
        // ──────────────────────────────────────────────────────────────────────────

        // tear down the parse lists
        freeTableList(tables);
        freeNameList(attsToSelect);
        freeNameList(groupingAtts);
        freeAndList(predicate);
        releaseFuncOperator(finalFunction);

        tables        = nullptr;
        attsToSelect  = nullptr;
        groupingAtts  = nullptr;
        predicate     = nullptr;
        finalFunction = nullptr;
        distinctAtts  = 0;

        queryBuffer.clear();
    }

    yylex_destroy();
    cout<<"Goodbye.\n";
    return 0;
}


// #include <iostream>
// #include <sstream>
// #include <string>
// #include <cstdlib>
// #include <chrono>

// #include "Catalog.h"
// #include "QueryCompiler.h"
// #include "RelOp.h"
// #include "Record.h"
// #include "BTreeIndex.h"

// extern "C" {
//     #include "QueryParser.h"
//     int yyparse();
//     void yylex_destroy();
//     typedef struct yy_buffer_state *YY_BUFFER_STATE;
//     YY_BUFFER_STATE yy_scan_string(const char *str);
//     void yy_delete_buffer(YY_BUFFER_STATE buffer);
// }

// // forward‐declare the cleanup helpers from QueryCompiler.cpp:
// void freeTableList(TableList*);
// void freeNameList(NameList*);
// void freeAndList(AndList*);
// void releaseFuncOperator(FuncOperator*);

// using namespace std;

// extern FuncOperator* finalFunction;
// extern TableList*    tables;
// extern AndList*      predicate;
// extern NameList*     groupingAtts;
// extern NameList*     attsToSelect;
// extern int           distinctAtts;

// extern char* createIndexName;
// extern char* createIndexTable;
// extern char* createIndexAttribute;

// void showHelp() {
//     cout
//       << ".help               Show this message\n"
//       << ".tables             List all tables\n"
//       << ".schema <table>     Show schema of a table\n"
//       << ".quit               Exit the CLI\n";
// }

// int main() {
//     cout
//       << "Welcome to MiniSQL CLI (Phase 5)\n"
//       << "Type .help for command list or SQL queries ending with ;\n";

//     // SString dbFile("/home/sachin/database/2025 spring-cse177-eecs277/code/catalog.sqlite");
//     // Catalog catalog(dbFile);
//     // QueryCompiler compiler(catalog);

//     SString dbFile("/home/sachin/database/2025-spring-cse177-eecs277/code/catalog.sqlite");
//     Catalog catalog(dbFile);
//     QueryCompiler compiler(catalog);

//     string line, queryBuffer;
//     while (true) {
//         cout << (queryBuffer.empty() ? "MiniSQL> " : "        > ");
//         if (!getline(cin, line) || cin.eof()) break;
//         if (line.empty()) continue;

//         auto first = line.find_first_not_of(" \t");
//         if (first == string::npos) continue;
//         string trimmed = line.substr(first);

//         // dot‑commands
//         if (trimmed[0]=='.' && queryBuffer.empty()) {
//             istringstream iss(trimmed);
//             string cmd; iss >> cmd;
//             if      (cmd==".quit")  break;
//             else if (cmd==".help")  showHelp();
//             else if (cmd==".tables") {
//                 StringVector tv; catalog.GetTables(tv);
//                 for (int i = 0; i < tv.Length(); i++)
//                     cout << tv[i] << "\n";
//             }
//             else if (cmd==".schema") {
//                 string tblName; iss>>tblName;
//                 if (tblName.empty()) cout<<"Usage: .schema <table>\n";
//                 else {
//                     Schema s;
//                     SString ss(tblName);
//                     if (!catalog.GetSchema(ss, s))
//                         cout<<"Table not found.\n";
//                     else
//                         cout<<s<<"\n";
//                 }
//             }
//             else {
//                 cout<<"Unknown command: "<<trimmed<<"\n";
//             }
//             continue;
//         }

//         // accumulate until we see ';'
//         queryBuffer += line + "\n";
//         if (queryBuffer.find(';') == string::npos) continue;

//         // pull out the statement
//         auto semi = queryBuffer.find(';');
//         string query = queryBuffer.substr(0, semi);

//         // reset parser globals
//         finalFunction        = nullptr;
//         tables               = nullptr;
//         predicate            = nullptr;
//         groupingAtts         = nullptr;
//         attsToSelect         = nullptr;
//         distinctAtts         = 0;
//         createIndexName      = nullptr;
//         createIndexTable     = nullptr;
//         createIndexAttribute = nullptr;

//         // feed the SQL into Bison/Flex
//         YY_BUFFER_STATE buf = yy_scan_string(query.c_str());
//         int parseStatus     = yyparse();
//         yy_delete_buffer(buf);

//         if (parseStatus != 0) {
//             cerr<<"Error: invalid SQL!\n";
//             queryBuffer.clear();
//             continue;
//         }

//         // CREATE INDEX?
//         if (createIndexName && createIndexTable && createIndexAttribute) {
//             string idxFile = string(createIndexTable) + "_"
//                            + string(createIndexAttribute) + ".bin";
//             BTreeIndex idx(&catalog);
//             idx.Build(idxFile,
//                       createIndexTable,
//                       createIndexAttribute);
//             cout<<"Index "<<createIndexName
//                 <<" built on "<<createIndexTable<<"."<<createIndexAttribute
//                 <<" → file: "<<idxFile<<"\n";
//             queryBuffer.clear();
//             continue;
//         }

//         // otherwise it’s a SELECT (or GROUP) query
//         QueryExecutionTree qTree;
//         compiler.Compile(
//             tables,
//             attsToSelect,
//             finalFunction,
//             predicate,
//             groupingAtts,
//             distinctAtts,
//             qTree
//         );
//         // cout<<qTree<<"\n";

//         // ─── SIMPLE TIMING ────────────────────────────────────────────────────────
//         auto t0 = chrono::high_resolution_clock::now();
//         qTree.ExecuteQuery();
//         auto t1 = chrono::high_resolution_clock::now();

//         double elapsed_ms = chrono::duration<double, std::milli>(t1 - t0).count();
//         cout << "Query executed in " << elapsed_ms << " ms\n\n";
//         // ──────────────────────────────────────────────────────────────────────────

//         // tear down the parse lists
//         freeTableList(tables);
//         freeNameList(attsToSelect);
//         freeNameList(groupingAtts);
//         freeAndList(predicate);
//         releaseFuncOperator(finalFunction);

//         tables        = nullptr;
//         attsToSelect  = nullptr;
//         groupingAtts  = nullptr;
//         predicate     = nullptr;
//         finalFunction = nullptr;
//         distinctAtts  = 0;

//         queryBuffer.clear();
//     }

//     yylex_destroy();
//     cout<<"Goodbye.\n";
//     return 0;
// }