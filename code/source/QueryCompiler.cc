#include "QueryCompiler.h"
#include "Schema.h"
#include "ParseTree.h"
#include "Record.h"
#include "DBFile.h"
#include "Comparison.h"
#include "Function.h"
#include "RelOp.h"
#include <vector>
#include <iostream>
 // include ofstream
#include <fstream>
#include <sstream>
#include <limits> // Required for numeric_limits
// include graphviz



using namespace std;



QueryCompiler::QueryCompiler(Catalog& _catalog) : catalog(&_catalog) {
       
}

QueryCompiler::~QueryCompiler() {
      
}



void releaseFuncOperator(FuncOperator* op) {
        if (op == nullptr) {
            return;
        }
        releaseFuncOperator(op->leftOperator);
        if (op->leftOperand != nullptr) {
            free(op->leftOperand);
        }
        if (op->right != nullptr) {
            releaseFuncOperator(op->right);
        }
        free(op);
    }

            // Recursive helper to free the TableList linked list.
void freeTableList(TableList* table) {
        if (table == NULL)
            return;
        freeTableList(table->next);
        free(table->tableName);
        free(table);
    }
    
    // Recursive helper to free the NameList linked list (used for both _attsToSelect and _groupingAtts).
    void freeNameList(NameList* list) {
        if (list == NULL)
            return;
        freeNameList(list->next);
        free(list->name);
        free(list);
    }
    
    // Recursive helper to free the AndList linked list for the predicates.
    // void freeAndList(AndList* predicate) {
    //     if (predicate == NULL)
    //         return;
    //     freeAndList(predicate->rightAnd);
    //     if (predicate->left != NULL) {
    //         free(predicate->left->left->value);
    //         free(predicate->left->left);
    //         free(predicate->left->right->value);
    //         free(predicate->left->right);
    //         free(predicate->left);
    //     }
    //     free(predicate);
    // }

    void freeAndList(AndList* head) {
        while (head) {
            AndList* next = head->rightAnd;
    
            // each AndList holds a ComparisonOp* in head->left
            ComparisonOp* cmp = head->left;
            if (cmp) {
                // left operand
                Operand* lhs = cmp->left;
                if (lhs) {
                    if (lhs->value) {
                        free(lhs->value);      // only free if you strdup'd it
                    }
                    free(lhs);
                }
    
                // right operand
                Operand* rhs = cmp->right;
                if (rhs) {
                    if (rhs->value) {
                        free(rhs->value);
                    }
                    free(rhs);
                }
    
                free(cmp);
            }
    
            free(head);
            head = next;
        }
    }
    

    string swapextension(const string& filename, const string& ext) {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == string::npos) {
           return filename + ext;
        }
        return filename.substr(0, dotPos)  + ext; // Append new extension
    }

//------------------------------------------------------------------------------
// freeAndList
//
// Recursively free an entire AndList linked list (including its OR‐clause).
//------------------------------------------------------------------------------
// static void freeAndList(AndList* predicate) {
//     if (!predicate) return;
//     // first free the rest of the list
//     freeAndList(predicate->rightAnd);
//     // then free this node’s single OR‐clause
//     if (predicate->left) {
//         free(predicate->left->left->value);
//         free(predicate->left->left);
//         free(predicate->left->right->value);
//         free(predicate->left->right);
//         free(predicate->left);
//     }
//     // finally free the AndList node itself
//     free(predicate);
// }


//------------------------------------------------------------------------------
// removeUsedJoins
//
// Given the global predicate list, rip out exactly those AND‐clauses that
// describe a pure join between leftSch and rightSch.  Everything else
// remains untouched.
// void removeMatchedPredicates(AndList*& predicate, CNF& usedJoinCNF, Schema& leftSch, Schema& rightSch) {
//     AndList* cur = predicate;
//     AndList* prev = nullptr;

//     while (cur != nullptr) {
//         AndList* next = cur->rightAnd;
//         ComparisonOp* cmpOp = cur->left;

//         if (!cmpOp || !cmpOp->left || !cmpOp->right) {
//             prev = cur;
//             cur = next;
//             continue;
//         }

//         // For each condition in the CNF used during the join
//         for (int i = 0; i < usedJoinCNF.numAnds; ++i) {
//             Comparison& cmp = usedJoinCNF.andList[i];

//             // Check operator match
//             bool opMatch = false;
//             if (cmpOp->code == EQUALS && cmp.op == Equals) opMatch = true;
//             if (cmpOp->code == LESS_THAN && cmp.op == LessThan) opMatch = true;
//             if (cmpOp->code == GREATER_THAN && cmp.op == GreaterThan) opMatch = true;

//             if (!opMatch) continue;

//             // Check that the condition involves attributes from both schemas
//             if (!ConditionOnSchemas(*cur, leftSch, rightSch)) continue;

//             // We assume it's a match if op type and attribute names match
//             // safer match could use attribute positions but here we use names
//             string leftVal = cmpOp->left->value;
//             string rightVal = cmpOp->right->value;

//             SString left_temp = leftVal;
//             SString right_temp = rightVal;

//             // NOTE: attribute name match assumes no aliasing
//             bool attrMatch =
//                 (leftSch.Index(left_temp) != -1 && rightSch.Index(right_temp) != -1) ||
//                 (rightSch.Index(left_temp) != -1 && leftSch.Index(right_temp) != -1);

//             // if (attrMatch) {
//             //     // Remove this node from predicate
//             //     if (prev) prev->rightAnd = next;
//             //     else      predicate = next;

//             //     cur->rightAnd = nullptr;
//             //     freeAndList(cur);
//             //     cur = nullptr;
//             //     break; // break inner loop
//             // }

//             // inside removeMatchedPredicates, when you find a matching predicate:
// if (attrMatch) {
//     // 1) unlink it:
//     if (prev) prev->rightAnd = cur->rightAnd;
//     else      predicate    = cur->rightAnd;
  
//     // 2) free just that one node + its ComparisonOp & Operands:
//     ComparisonOp* cmp = cur->left;
//     if (cmp) {
//       if (cmp->left) {
//         free(cmp->left->value);
//         free(cmp->left);
//       }
//       if (cmp->right) {
//         free(cmp->right->value);
//         free(cmp->right);
//       }
//       free(cmp);
//     }
//     free(cur);
  
//     // 3) advance without calling freeAndList
//     cur = (prev ? prev->rightAnd : predicate);
//     continue;
//   }
  
//         }

//         if (cur != nullptr) {
//             prev = cur;
//             cur = next;
//         }
//     }
// }



void removeMatchedPredicates(AndList*& predicate,
    CNF& usedJoinCNF,
    Schema& leftSch,
    Schema& rightSch)
{
AndList* cur  = predicate;
AndList* prev = nullptr;

while (cur != nullptr) {
AndList* next = cur->rightAnd;
ComparisonOp* cmpOp = cur->left;

// skip any malformed node
if (!cmpOp || !cmpOp->left || !cmpOp->right) {
prev = cur;
cur  = next;
continue;
}

bool removed = false;

// try each condition in the join‐CNF
for (int i = 0; i < usedJoinCNF.numAnds; ++i) {
Comparison& cmp = usedJoinCNF.andList[i];

// operator must match
bool opMatch = (cmpOp->code == EQUALS      && cmp.op == Equals)
|| (cmpOp->code == LESS_THAN   && cmp.op == LessThan)
|| (cmpOp->code == GREATER_THAN && cmp.op == GreaterThan);
if (!opMatch) continue;

// must involve one attribute from each schema
if (!ConditionOnSchemas(*cur, leftSch, rightSch))
continue;

// extract the two attribute‐names
std::string Lv = cmpOp->left->value;
std::string Rv = cmpOp->right->value;

// create lvalue SStrings for Index()
SString leftName(Lv.c_str());
SString rightName(Rv.c_str());

// attribute‐name match on either ordering
bool attrMatch =
( leftSch.Index(leftName)  != -1
&& rightSch.Index(rightName) != -1 )
||  ( rightSch.Index(leftName) != -1
&& leftSch.Index(rightName)  != -1 );
if (!attrMatch) continue;

// ** unlink only — do NOT free here **
if (prev) prev->rightAnd = cur->rightAnd;
else       predicate     = cur->rightAnd;

// advance to next without updating prev
cur     = (prev ? prev->rightAnd : predicate);
removed = true;
break;
}

// if we did not remove this node, advance prev + cur as normal
if (!removed) {
prev = cur;
cur  = next;
}
}
}

// void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect,
// 	FuncOperator* _finalFunction, AndList* _predicate,
// 	NameList* _groupingAtts, int& _distinctAtts,
// 	QueryExecutionTree& _queryTree) {




//     // create a SCAN operator for each table in the query
//     int nTbl = 0;
//     for (TableList* node = _tables; node != NULL; node = node->next) nTbl += 1;

    
//     RelationalOp** forest = new RelationalOp*[nTbl];
//     Schema* forestSchema = new Schema[nTbl];


    
//     int idx = 0;
//     for (TableList* node = _tables; node != NULL; node = node->next) {
//             string s = node->tableName;
//             SString ss = s;
//             bool b = catalog->GetSchema(ss, forestSchema[idx]);
//             if (false == b) {
//                     cout << "Semantic error: table " << s << " does not exist in the database!" << endl;
//                     exit(1);
//             }
    
//             SString file_path;
//             if(!catalog->GetDataFile(ss, file_path)){
//                 cerr << "No filepath found from catalog" << endl;

//             }
//             DBFile dbFile;

//             string heap_file =  s  + ".heap";
//             string text_file = "/home/sachin/database/2025-spring-cse177-eecs277/data/"+ s +".tbl";
//             //string fPath = file_path;
//         //    string heap_file = swapextension(file_path,".heap");
//         //     string text_file = swapextension(file_path,".tbl");
//             //string text_file = "/home/sachin/database/2025-spring-cse177-eecs277/code/nation.tbl.heap";

//             //text_file = "/home/sachin/database/2025-spring-cse177-eecs277/data/"+text_file; 
//             cout <<"FIle path --->  " << file_path << endl;
//             cout <<"Heap file --->  " << heap_file << endl;
//             cout <<"text file --->  " << text_file << endl;
//             dbFile.Create(const_cast<char*>(heap_file.c_str()), Heap);
//             dbFile.Open(const_cast<char*>(heap_file.c_str()));
//             dbFile.Load(forestSchema[idx], const_cast<char*>(text_file.c_str()));
      
//             forest[idx] = new Scan(forestSchema[idx], dbFile, s);
//             cout << forest[idx] << endl;
//             //dbFile.Close();
//             idx += 1;
//     }
    
//     cout << "SCANS" << endl;
    
//     cout << "+++++++++++++++++++++++" << endl;
//     for (int i = 0; i < nTbl; i++) cout << *forest[i] << endl;

	

// // push-down selections: create a SELECT operator wherever necessary
//     for (int i = 0; i < nTbl; i++) {
//         Record literal;
//         CNF cnf;
//         int ret = cnf.ExtractCNF (*_predicate, forestSchema[i], literal);
//         if (0 > ret) exit(1);

//         if (ret > 0) {
//             // Begin estimating number of tuples after applying the predicate
//             int originalTuples = forestSchema[i].GetNoTuples();
//             int estimatedTuples = originalTuples;
    
//             for (int j = 0; j < ret; ++j) {
//                 Comparison& cmp = cnf.andList[j];
//                 int attrIdx = cmp.whichAtt1;
    
//                 if (cmp.op == Equals) {
//                     Attribute attr;
//                     forestSchema[i].GetAtts().AtIndex(attrIdx, attr);
//                     int distinctVals = forestSchema[i].GetDistincts(attr.name);
//                     if (distinctVals > 0) {
//                         estimatedTuples /= distinctVals;
//                     }
//                 } else {
//                     estimatedTuples /= 3;
//                 }
//             }
//             SInt estimatedtuplesSint = estimatedTuples;
//             forestSchema[i].SetNoTuples(estimatedtuplesSint);
    
//             // Determine if any literals are involved in the predicate
//             bool usesLiterals = false;
//             for (int j = 0; j < ret; ++j) {
//                 if (cnf.andList[j].operand1 == Literal || cnf.andList[j].operand2 == Literal) {
//                     usesLiterals = true;
//                     break;
//                 }
//             }
    
//             // Provide constants to the Select operator
//             Record selectConstants;
//             if (usesLiterals) {
//                 selectConstants = literal;
//             } else {
//                 // Use dummy record if predicate contains no constants
//                 const int dummyLen = 2 * sizeof(int);
//                 char* dummyBits = new char[dummyLen];
//                 ((int*)dummyBits)[0] = dummyLen;
//                 ((int*)dummyBits)[1] = dummyLen;
//                 selectConstants.Consume(dummyBits);
//             }
    
//             // Wrap the Scan operator with a Select operator
//             RelationalOp* op = new Select(forestSchema[i], cnf, selectConstants, forest[i]);
//             forest[i] = op;
    
//             cout << "Selection applied on table index " << i
//                  << ": estimated tuples reduced from " << originalTuples
//                  << " to " << estimatedTuples << "." << endl;
//         }
// }


//         cout << endl << "PUSH DOWN SELECTION" << endl;
//         cout << "+++++++++++++++++++++++" << endl;
//         for (int i = 0; i < nTbl; i++) cout << *forest[i] << endl;


void QueryCompiler::Compile(TableList* _tables, NameList* _attsToSelect,
    FuncOperator* _finalFunction, AndList* _predicate,
    NameList* _groupingAtts, int& _distinctAtts,
    QueryExecutionTree& _queryTree) {

    int nTbl = 0;
    for (TableList* node = _tables; node != NULL; node = node->next) nTbl++;
    RelationalOp** forest = new RelationalOp*[nTbl];
    Schema* forestSchema = new Schema[nTbl];

    int idx = 0;
    for (TableList* node = _tables; node != NULL; node = node->next) {
        string s = node->tableName;
        SString ss = s;
        if (!catalog->GetSchema(ss, forestSchema[idx])) {
            cerr << "Error: Table " << s << " not found in catalog." << endl;
            exit(1);
        }

        // DBFile dbFile;
        // string heap_file = s + ".heap";
        // string text_file = "../data/" + s + ".tbl";
        // dbFile.Create(const_cast<char*>(heap_file.c_str()), Heap);
        // dbFile.Open(const_cast<char*>(heap_file.c_str()));
        // dbFile.Load(forestSchema[idx], const_cast<char*>(text_file.c_str()));
        // forest[idx] = new Scan(forestSchema[idx], dbFile, s);
        // idx++;

        // Allocate DBFile on the heap so it outlives this function
        DBFile* dbFile = new DBFile();
        string  heap_file = s + ".heap";
        string  text_file = "../data/" + s + ".tbl";

        dbFile->Create(const_cast<char*>(heap_file.c_str()), Heap);
        dbFile->Open  (const_cast<char*>(heap_file.c_str()));
        dbFile->Load  (forestSchema[idx],
                       const_cast<char*>(text_file.c_str()));

        // Pass the heap‐allocated DBFile by reference into Scan
        forest[idx] = new Scan(forestSchema[idx], *dbFile, s);
        idx++;
    }

    for (int i = 0; i < nTbl; i++) {
        Record literal;
        CNF cnf;
        int ret = cnf.ExtractCNF(*_predicate, forestSchema[i], literal);
        if (ret < 0) exit(1);

        bool useIndex = false;
        string attrWithIndex;
        TableList* t = _tables;
        for (int skip = 0; skip < i && t != NULL; skip++) t = t->next;
        SString tableName = std::string(t->tableName);

        for (int j = 0; j < ret; ++j) {
            Comparison& cmp = cnf.andList[j];
            if ((cmp.operand1 == Left && cmp.operand2 == Literal) ||
                (cmp.operand2 == Left && cmp.operand1 == Literal)) {
                int attIdx = (cmp.operand1 == Left) ? cmp.whichAtt1 : cmp.whichAtt2;
                SString attName = forestSchema[i].GetAtts()[attIdx].name;
                SString idxFile;
                string defaultPath = (string)tableName + "_" + (string)attName + ".bin";
                ifstream f("../data/" + defaultPath);
                if (f.good()) {
                    idxFile = defaultPath;
                    attrWithIndex = attName;
                    useIndex = true;
                    break;
                }
            }
        }

        if (useIndex) {
            
            string idxPath = "../data/" + std::string(tableName) + "_" + std::string(attrWithIndex) + ".bin";
            forest[i] = new ScanIndex(forestSchema[i], cnf, literal, tableName, idxPath, catalog);
            // cout << "[INFO] Using ScanIndex on table " << (const char*)tableName << " for attribute " << attrWithIndex << endl;
            cout << "[INFO] Using ScanIndex on table " << static_cast<std::string>(tableName) << " for attribute " << attrWithIndex << endl;

        } else if (ret > 0) {
            forest[i] = new Select(forestSchema[i], cnf, literal, forest[i]);
        }
    }

// // implementing deep left tree nested loop heuristic(greedy) join algorithm 




int remainingJoins = nTbl - 1;

while (remainingJoins > 0) {
    int selectedTable1 = -1, selectedTable2 = -1;
    float lowestCost = numeric_limits<float>::max();

   // cout << "Finding possible join pairs for iteration..." << endl;

    for (int i = 0; i < nTbl; i++) {
        for (int j = i + 1; j < nTbl; j++) {
            Schema leftSch = forestSchema[i];
            Schema rightSch = forestSchema[j];

           // cout << "Attempting ExtractCNF for pair (" << i << ", " << j << ")" << endl;

            CNF joinFilter;
            int numConditions = joinFilter.ExtractCNF(*_predicate, leftSch, rightSch);

            //cout << "ExtractCNF numConditions: " << numConditions << endl;

            if (numConditions > 0) {
                float joinCost = (float)(leftSch.GetNoTuples()) * rightSch.GetNoTuples();
                for (int k = 0; k < numConditions; k++) {
                    SString attNameLeft = leftSch.GetAtts()[joinFilter.andList[k].whichAtt1].name;
                    SString attNameRight = rightSch.GetAtts()[joinFilter.andList[k].whichAtt2].name;

                    SInt distinctValsLeft = leftSch.GetDistincts(attNameLeft);
                    SInt distinctValsRight = rightSch.GetDistincts(attNameRight);

                    joinCost /= max(int(distinctValsLeft), int(distinctValsRight));
                }

                //cout << "Cost between table " << i << " and table " << j << " is " << joinCost << endl;

                if (joinCost < lowestCost) {
                    lowestCost = joinCost;
                    selectedTable1 = i;
                    selectedTable2 = j;
                }
            }
        }
    }

    if (selectedTable1 == -1 || selectedTable2 == -1) {
        cerr << "No valid join pairs found, stopping join operations." << endl;
        break;
    }

    //cout << "Join picked between schemas:\n  -> " << forestSchema[selectedTable1]
     //    << "\n  -> " << forestSchema[selectedTable2]
      //   << "\nWith estimated cost: " << lowestCost << endl;

    //cout << "Selected join: Table " << selectedTable1 << " and Table " << selectedTable2
     //    << " with cost " << lowestCost << endl;

   // cout << "Actual Left Schema:" << forestSchema[selectedTable1] << endl;
   // cout << "Actual Right Schema:" << forestSchema[selectedTable2] << endl;

    Schema combinedSchema(forestSchema[selectedTable1]);
    combinedSchema.Append(forestSchema[selectedTable2]);

    //cout << "Combined schema is:" << combinedSchema << endl;

    SInt tempNotuple = SInt(lowestCost);
    combinedSchema.SetNoTuples(tempNotuple);

   // cout << "Combined Schema after setting number of tuples: " << combinedSchema << endl;

    CNF joinCond;
    joinCond.ExtractCNF(*_predicate, forestSchema[selectedTable1], forestSchema[selectedTable2]);

   removeMatchedPredicates(_predicate, joinCond,
                           forestSchema[selectedTable1], forestSchema[selectedTable2]);

     // make ordermaker
     // 3) build your OrderMakers for HJ/SHJ
    OrderMaker leftOrder, rightOrder;
    joinCond.GetSortOrders(leftOrder, rightOrder);
    
    if (leftOrder.numAtts <= 0 || rightOrder.numAtts <= 0) {
        // This really shouldn’t happen if you’re in the hash‐join branch—
        // fall back or error out.
        cerr << "ERROR: no equality columns for HashJoin\n";
        // e.g. switch to a nested‐loop join here instead
        
    }
    
    // 4) pick the algorithm:
    bool hasIneq = false;
    for (int k = 0; k < joinCond.numAnds; ++k)
    if (joinCond.andList[k].op != Equals)
        hasIneq = true;

    long L = forestSchema[selectedTable1].GetNoTuples();
    long R = forestSchema[selectedTable2].GetNoTuples();

   RelationalOp* root;

    if (leftOrder.numAtts <= 0 || rightOrder.numAtts <= 0) {
        // cross-product or bozo case: force a nested loop
        cout << "no equi-columns → falling back to NestedLoopJoin\n";
        root = new NestedLoopJoin(
            forestSchema[selectedTable1], forestSchema[selectedTable2], combinedSchema,
            joinCond, forest[selectedTable1], forest[selectedTable2]);
    }

    if (hasIneq) {
        cout << "using NestedLoopJoin for tables " << endl;
        root = new NestedLoopJoin(
            forestSchema[selectedTable1], forestSchema[selectedTable2], combinedSchema,
            joinCond, forest[selectedTable1], forest[selectedTable2]);
      }
      else if (L > 1000 && R > 1000) {
        cout << "using Symmetric Hash join for tables " << endl;
        root = new SymmetricHashJoin(forestSchema[selectedTable1],
                                     forestSchema[selectedTable2],
                                     combinedSchema,
                                     joinCond,
                                     forest[selectedTable1],
                                     forest[selectedTable2],
                                     leftOrder,
                                     rightOrder);
      }
      else {
        cout << "using Hash Join for tables " << endl;
        root = new HashJoin(forestSchema[selectedTable1],
                            forestSchema[selectedTable2],
                            combinedSchema,
                            joinCond,
                            forest[selectedTable1],
                            forest[selectedTable2],
                            leftOrder,
                            rightOrder);
      }
                       

    // RelationalOp* root = new NestedLoopJoin(
    //     forestSchema[selectedTable1], forestSchema[selectedTable2], combinedSchema,
    //     joinCond, forest[selectedTable1], forest[selectedTable2]);

        //   RelationalOp*  root = new HashJoin(forestSchema[selectedTable1],
        //                              forestSchema[selectedTable2],
        //                              combinedSchema,
        //                              joinCond,
        //                              forest[selectedTable1],
        //                              forest[selectedTable2],
        //                              leftOrder,
        //                              rightOrder);

    int first = min(selectedTable1, selectedTable2);
    int second = max(selectedTable1, selectedTable2);

//  this removes the right one (always higher index)
for (int z = second; z < nTbl - 1; z++) {
    forest[z] = forest[z + 1];
    forestSchema[z] = forestSchema[z + 1];
}
--nTbl;

//cout << "Forest schema after shift and assignment:" << endl;
// for (int i = 0; i < nTbl; i++) {
//     cout << i << ": " << forestSchema[i] << endl;
// }

// now safely update the remaining (smaller index) entry
int keep = min(selectedTable1, selectedTable2);
forest[keep] = root;
forestSchema[keep] = combinedSchema;


    //cout << "Forest schema after shift and assignment:" << endl;
    // for (int i = 0; i < nTbl; i++) {
    //     cout << i << ": " << forestSchema[i] << endl;
    // }

    // //cout << "\nForest schema after join and shift:" << endl;
    // for (int i = 0; i < nTbl; i++) {
    //     cout << i << ": " << forestSchema[i] << endl;
    // }

    --remainingJoins;

    //cout << "Final schema:" << forestSchema[0] << endl;
    //cout << "Made a join, remaining joins: " << remainingJoins << endl;
    cout << "Joining: " << selectedTable1 << " with " << selectedTable2 << endl;
    
}

Schema finalSchema = forestSchema[0];





// Apply the SUM aggregation only if a final function exists and there are no grouping attributes.
if (_finalFunction != 0 && _groupingAtts == 0) {
    // Use the existing schema as input for the SUM operation.
    Schema sumSchemaIn = forestSchema[0];

    // Build the output schema for the SUM operator.
    // This schema is defined to have exactly one attribute, "sum", whose type is FLOAT.
    StringVector sum_attributes;
    SString sum_attribute("sum");   // Aggregated field name; could be renamed to 'agg' if needed.
    sum_attributes.Append(sum_attribute);

    StringVector sum_attribute_types;
    SString sum_attribute_type("FLOAT");   // The aggregation result is always of type FLOAT.
    sum_attribute_types.Append(sum_attribute_type);

    IntVector sum_distincts;
    SInt sum_distinct(1);   // The SUM operator produces exactly one tuple.
    sum_distincts.Append(sum_distinct);

    Schema sumSchemaOut(sum_attributes, sum_attribute_types, sum_distincts);
    SInt n_tuple = 1;
    sumSchemaOut.SetNoTuples(n_tuple);  // Ensure the output schema reflects a single-tuple result.

    // Construct the computation function by building it from the parsed _finalFunction tree.
    Function compute;
    compute.GrowFromParseTree(_finalFunction, forestSchema[0]);

    // Create a new SUM operator. It takes the current forest[0] as its input (producer).
    RelationalOp* op = new Sum(sumSchemaIn, sumSchemaOut, compute, forest[0]);
    forest[0] = op;   // Update the forest's root to be the new SUM operator.

    // Update finalSchema so that it reflects the output schema of the SUM operation.
    finalSchema.Swap(sumSchemaOut);
}


// GROUP BY is activated when _groupingAtts is not null.
else if (_groupingAtts != 0) {
        // Use the first schema from the forest as input.
       Schema schemaIn = forestSchema[0];
        //  Schema schemaIn = finalSchema;
        
        //cout << schemaIn <<endl;
       // cout << " Groupby schema above" << endl;
        // Build the aggregation function from the parse tree.
        Function compute;
        compute.GrowFromParseTree(_finalFunction, schemaIn);
    
        // Prepare the initial aggregated schema with a single attribute "sum" of type FLOAT.
        StringVector aggAttrs;
        SString aggAttr("sum");
        aggAttrs.Append(aggAttr);
    
        StringVector aggAttrTypes;
        SString aggType("FLOAT");
        aggAttrTypes.Append(aggType);
    
        IntVector aggDistincts;
        SInt one(1);
        aggDistincts.Append(one);
        Schema schemaOut(aggAttrs, aggAttrTypes, aggDistincts);
    
        // Determine how many grouping attributes are specified.
        int numGroupAttrs = 0;
        for (NameList* cur = _groupingAtts; cur != nullptr; cur = cur->next) {
            numGroupAttrs++;
        }
        //cout << "GROUP BY - Number of grouping attributes: " << numGroupAttrs << endl;
    
        // Build an array to hold the indices of grouping attributes in schemaIn.
        int groupIndicesArr[numGroupAttrs];
        int pos = 0;
        for (NameList* cur = _groupingAtts; cur != nullptr; cur = cur->next) {
            SString attrName(cur->name);
            groupIndicesArr[pos++] = schemaIn.Index(attrName);
        }
    
        // Transfer the indices into an IntVector.
        IntVector groupIndices;
        for (int i = 0; i < numGroupAttrs; i++) {
            SInt idxVal = groupIndicesArr[i];
            groupIndices.Append(idxVal);
        }
    
        // Create a new schema for grouping by projecting the desired attributes from schemaIn.
        Schema groupingSchema(schemaIn);
        groupingSchema.Project(groupIndices);
    
        // Append the grouping attributes to the aggregation schema.
        schemaOut.Append(groupingSchema);
    
        // Use the distinct count of the first grouping attribute as the group count.
        SString groupAttrName = groupingSchema.GetAtts()[0].name;
        SInt numGroups = groupingSchema.GetDistincts(groupAttrName);
       // cout << "GROUP BY - Grouping attribute: " << groupAttrName
         //    << ", Distinct groups: " << numGroups << endl;
        SString sum_attr("sum");

        schemaOut.SetDistincts(sum_attr, numGroups);
        schemaOut.SetNoTuples(numGroups);
    
        // Create an OrderMaker to order the groups based on the grouping attributes.
        OrderMaker groupOrder(schemaIn, groupIndicesArr, numGroupAttrs);
    
        // Instantiate a new GroupBy operator and set it as the producer.
        RelationalOp* groupByOp = new GroupBy(schemaIn, schemaOut, groupOrder, compute, forest[0]);
        forest[0] = groupByOp;
    
        // Update finalSchema to reflect the output of the GROUP BY operation.
        finalSchema.Swap(schemaOut);
    }
    


        //create project with distinct condition
        else if (_finalFunction == 0 && _groupingAtts == 0){
                cout << "Starting Projection Operator" << endl;
    
                // Obtain the initial schema from the first operator in the forest.
                Schema schemaIn = forestSchema[0];
                
                // Determine the total number of attributes in the input schema.
                int numAttsInput = schemaIn.GetNumAtts();
                
                // Count how many attributes we need to select.
                NameList* selectPtr = _attsToSelect;
                int numAttsOutput = 0;
                while (selectPtr != nullptr) {
                    numAttsOutput++;
                    selectPtr = selectPtr->next;
                }
                
                // Reset the pointer to the beginning of the selection list.
                selectPtr = _attsToSelect;
                
                // Build an array that stores the indices of the attributes to retain.
                int* indicesToKeep = new int[numAttsOutput];

                for (int i = 0; i < numAttsOutput; i++) {
                    SString att_name(selectPtr->name);
                    indicesToKeep[i] = schemaIn.Index(att_name);
                    selectPtr = selectPtr->next;
                }

     

                
                // Convert the indices array into an IntVector needed for the projection.
                IntVector projectionList;
                for (int i = 0; i < numAttsOutput; i++) {
                       SInt idx = indicesToKeep[i];
                    projectionList.Append(idx); 
                }

               
                
                // Create a duplicate of the input schema to form the output schema.
                Schema schemaOut(schemaIn);
                // Apply the projection to update the output schema.
                schemaOut.Project(projectionList);
               // cout << "Projected numAtts = " << schemaOut.GetNumAtts() << endl;
               // cout << "Schema Out:" << endl;
                //cout << schemaOut  << endl;
             

               // cout << "Schema In:" << endl;
               // cout << schemaIn  << endl;

              //  cout << "Num Input Atts:" << endl;
               // cout << numAttsInput  << endl;


               // cout << "Num Output Atts:" << endl;
              //  cout << numAttsOutput  << endl;


               // cout << "Indices to keep: " << endl;
              //  cout << indicesToKeep << endl;



                // Instantiate a new Projection operator using the input and projected output schemas.
                RelationalOp* projOp = new Project(schemaIn, schemaOut,numAttsInput,numAttsOutput, indicesToKeep, forest[0]);
                forest[0] = projOp;  // Update the forest with the new operator.
             
                
                
                // If duplicate elimination is needed, add a Duplicate Removal operator.
                if (_distinctAtts != 0) {
                    RelationalOp* dupRemoval = new DuplicateRemoval(schemaOut, forest[0]);
                    forest[0] = dupRemoval;
                }
                finalSchema.Swap(schemaOut);
               
                cout << "Projection Operator completed" << endl;
                
        }
        
        // Write the output to a file
    cout << "Final estimated tuples before write out: " << finalSchema.GetNoTuples()  << endl;
   
    // Record record_temp;
    // record_temp.print(std::cout, finalSchema);
    // std::cout << std::endl;

	string outFile = "output.txt";
	RelationalOp* write_out_op = new WriteOut(finalSchema, outFile, forest[0]);
	forest[0] = write_out_op;

	// Set the final schema for the output operator.
	QueryExecutionTree qet;
	qet.SetRoot(*forest[0]);
	_queryTree = qet;

        //code for clean up 


    
    /* --- Inline cleanup code --- */
    
    // // Free the TableList (_tables)
    // freeTableList(_tables);
    // _tables = NULL;
    
    // // Free the NameList for selected attributes (_attsToSelect)
    // freeNameList(_attsToSelect);
    // _attsToSelect = NULL;
    
    // // Free the NameList for grouping attributes (_groupingAtts)
    // freeNameList(_groupingAtts);
    // _groupingAtts = NULL;
    
    // // Free the predicate list (_predicate)
    // freeAndList(_predicate);
    // _predicate = NULL;
    
    // // Finally, release the function operator tree.
    // releaseFuncOperator(_finalFunction);
    
    // delete [] forest;
    // delete [] forestSchema;

}

