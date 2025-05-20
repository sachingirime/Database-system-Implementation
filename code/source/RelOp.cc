#include <iostream>
#include <cstring>
#include <sstream>
#include "RelOp.h"
#include<fstream>
#include <map>     // for std::map
#include <vector>   // For std::vector in Group by, for string and int types
#include"BTreeIndex.h"
#include <deque>
#include <list>  // include the standard list header
#include <unordered_map>
#include "Map.h"      // for class Map<…> and KeyInt

#include <unordered_set>
#include <algorithm>
#include <limits>






#include "Map.cc"


using namespace std;

// ─── escape helper for Graphviz labels ──────────────────────────────────────
static std::string esc(const std::string &s) {
    std::string r;
    for (char c : s) {
        if (c == '"')   r += "\\\"";
        else if (c == '\n') r += "\\n";
        else            r += c;
    }
    return r;
}

// Deep copy a Record: allocates new bits and assigns to dst
// Attempts to deep copy a Record. Returns false if src is null or uninitialized.
bool DeepCopyRecordSafe(Record& src, Record& dst) {
    char* bits = src.GetBits();
    if (!bits) {
        // Defensive warning: record is uninitialized
        cerr << "[WARN] DeepCopyRecordSafe: Tried to copy from null Record. Skipping." << endl;
        return false;
    }

    int len = ((int*)bits)[0];
    char* copy = new char[len];
    memcpy(copy, bits, len);
    dst.Consume(copy);
    return true;
}

// bool DeepCopyRecordSafe(Record& src, Record& dst) {
//     if (src.GetBits() == nullptr) return false;
//     dst.CopyBits(src.GetBits(), src.GetSize());
//     return true;
// }




// Utility function to replace the first occurrence of a substring with another string.
// Returns true if a replacement occurred, false otherwise.
bool modifyString(std::string& inputStr, const std::string& searchStr, const std::string& replaceStr) {
    // Avoid processing an empty search string.
    if (searchStr.empty()) {
        return false;
    }

    // Locate the position of the first occurrence of the search string.
    size_t foundPos = inputStr.find(searchStr);
    if (foundPos == std::string::npos) {
        return false;
    }

    // Build a new string by concatenating the parts before and after the found substring with the replacement.
    std::string newStr = inputStr.substr(0, foundPos) + replaceStr + inputStr.substr(foundPos + searchStr.length());

    // Update the original string.
    inputStr = newStr;

    return true;
}


ostream& operator<<(ostream& _os, RelationalOp& _op) {
	return _op.print(_os);
}


Scan::Scan(Schema& _schema, DBFile& _file, string _tblName) {
	// implement the scan
	schema = _schema;
	file = _file;
	tblName = _tblName;
	file.MoveFirst();

}

Scan::~Scan() {

}

bool Scan::GetNext(Record& _record) {
	//file.MoveFirst();
	return file.GetNext(_record);
		//cout << "INSIDE SCAN FILE.GetNext() --> " << file.GetNext(_record) <<endl;




}

ostream& Scan::print(ostream& _os) {
	_os << "SCAN " << tblName << " {" << schema << "}";
	return _os;
}

// ─── SCAN ────────────────────────────────────────────────────────────────────
int Scan::toDot(std::ostream &dot, int parent, int &counter) const {
    int id = counter++;
    dot << "  node" << id
        << " [label=\"SCAN\\n" << esc(tblName) << "\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";
    return id;
}



Select::Select(Schema& _schema, CNF& _predicate, Record& _constants,
	RelationalOp* _producer) {
		//implement the select
		schema = _schema;
		predicate = _predicate;
		constants = _constants;
		producer = _producer;
}

Select::~Select() {
	//delete producer;


}

bool Select::GetNext(Record& _record) {
	while (true) {
		bool ret = producer->GetNext(_record);
		if (false == ret) return false;

    // check the condition or evaluate the predicate
		if (true == predicate.Run(_record, constants)) return true;
	}
	return false;
	// if(producer->GetNext(_record)){return true;}
	// return false;
}

int Select::toDot(std::ostream &dot, int parent, int &counter) const {
    auto &atts = const_cast<Schema&>(schema).GetAtts();

    // build predicate string, newline‑escaped for Graphviz
    std::string p;
    for (int i = 0; i < predicate.numAnds; ++i) {
        if (i) p += "\\n";
        auto &c = predicate.andList[i];

        // left side
        if (c.operand1 == Literal) {
            // literal on the left
            Type t = atts[c.whichAtt2].type;
            char *val = const_cast<Record&>(constants).GetColumn(c.whichAtt1);
            switch (t) {
                case Integer:
                    p += std::to_string(*reinterpret_cast<int*>(val));
                    break;
                case Float:
                    p += std::to_string(*reinterpret_cast<double*>(val));
                    break;
                case String:
                    p += "'" + esc(std::string(val)) + "'";
                    break;
            }
        }
        else {
            p += esc(atts[c.whichAtt1].name);
        }

        // operator
        p += (c.op == Equals    ? "="
            : (c.op == LessThan ? "<" : ">"));

        // right side
        if (c.operand2 == Literal) {
            // literal on the right
            Type t = atts[c.whichAtt1].type;
            char *val = const_cast<Record&>(constants).GetColumn(c.whichAtt2);
            switch (t) {
                case Integer:
                    p += std::to_string(*reinterpret_cast<int*>(val));
                    break;
                case Float:
                    p += std::to_string(*reinterpret_cast<double*>(val));
                    break;
                case String:
                    p += "'" + esc(std::string(val)) + "'";
                    break;
            }
        }
        else {
            p += esc(atts[c.whichAtt2].name);
        }
    }

    int id = counter++;
    dot << "  node" << id
        << " [label=\"SELECT\\n" << p << "\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";
    producer->toDot(dot, id, counter);
    return id;
}



ScanIndex::ScanIndex(Schema& _schema,
    CNF& _predicate,
    Record& _constants,
    const string& _tableName,
    const string& indexFileName,
    Catalog* catalog)
: schema(_schema),
  predicate(_predicate),
  tableName(_tableName),
  index(catalog),
  initialized(false),
  currentIndex(0)
{
    constants.CopyBits(_constants.GetBits(), _constants.GetSize());
    index.Read(indexFileName);
    // FIX: Validate tree after reading
    index.ValidateTree();
    string dataPath = "../data/" + tableName + ".heap";
    dataFile.Open(1, const_cast<char*>(dataPath.c_str()));
}

ostream& ScanIndex::print(ostream& _os) {
    _os << "SCAN-INDEX " << tableName;
    return _os;
}

// For equality queries (e.g., c_custkey = 221), ScanIndex::GetNext uses BTreeIndex::Find to locate all RIDs for the exact key, leveraging the B+ tree’s efficient traversal and sibling scans for duplicates. 
// For range queries (e.g., c_custkey < 221), it accesses BTreeIndex::GetIndexMap to scan leaf nodes, collecting RIDs for keys within the range (e.g., min_int to 220). 
// The GetNext method initializes by identifying the query type, populates matchingRecordIds with RIDs, and iterates them to fetch records from the heap file. 
// It applies the predicate to ensure correctness, optimizing disk I/O over a full table scan. This enables fast processing for the query’s joins and aggregation.

bool ScanIndex::GetNext(Record& _record) {
    if (!initialized) {
        initialized = true;
        matchingRecordIds.clear();

        int indexAtt = -1;
        for (int i = 0; i < predicate.numAnds; ++i) {
            const Comparison& c = predicate.andList[i];
            if (c.operand1 == Left && c.operand2 == Literal) {
                indexAtt = c.whichAtt1;
                break;
            }
            if (c.operand1 == Literal && c.operand2 == Left) {
                indexAtt = c.whichAtt2;
                break;
            }
        }

        if (indexAtt >= 0) {
            int32_t lower = std::numeric_limits<int32_t>::min();
            int32_t upper = std::numeric_limits<int32_t>::max();
            bool hasEq = false;
            int32_t eqKey = 0;

            for (int i = 0; i < predicate.numAnds; ++i) {
                const Comparison& c = predicate.andList[i];
                int32_t k;
                if (c.operand1 == Left && c.operand2 == Literal && c.whichAtt1 == indexAtt) {
                    k = *reinterpret_cast<int32_t*>(constants.GetColumn(c.whichAtt2));
                    switch (c.op) {
                        case LessThan:    upper = std::min(upper, k - 1); break;
                        case GreaterThan: lower = std::max(lower, k + 1); break;
                        case Equals:      hasEq = true; eqKey = k;        break;
                        default:          break;
                    }
                } else if (c.operand1 == Literal && c.operand2 == Left && c.whichAtt2 == indexAtt) {
                    k = *reinterpret_cast<int32_t*>(constants.GetColumn(c.whichAtt1));
                    switch (c.op) {
                        case LessThan:    upper = std::min(upper, k - 1); break;
                        case GreaterThan: lower = std::max(lower, k + 1); break;
                        case Equals:      hasEq = true; eqKey = k;        break;
                        default:          break;
                    }
                }
            }

            if (hasEq) {
                vector<int> rids;
                if (index.Find(eqKey, rids)) {
                    matchingRecordIds.insert(matchingRecordIds.end(), rids.begin(), rids.end());
                    cout << "[ScanIndex] Found " << rids.size() << " RIDs for key=" << eqKey << endl;
                } else {
                    cout << "[ScanIndex] Key=" << eqKey << " not found in index" << endl;
                }
            } else {
                std::unordered_set<int> seen;
                for (auto& entry : index.GetIndexMap()) {
                    const auto& page = entry.second;
                    if (page.pageType != IndexPage::LEAF) continue;
                    for (int j = 0; j < page.numKeys; ++j) {
                        int key = page.keys[j];
                        int rid = page.pointers[j];
                        if (key >= lower && key <= upper) {
                            seen.insert(rid);
                        }
                    }
                }
                matchingRecordIds.assign(seen.begin(), seen.end());
                std::sort(matchingRecordIds.begin(), matchingRecordIds.end());
            }
        }

        std::cerr << "[ScanIndex] found " << matchingRecordIds.size()
                  << " matching record IDs for predicate.\n";
    }

    while (currentIndex < matchingRecordIds.size()) {
        int targetId = matchingRecordIds[currentIndex++];
        int rid = 0;
        int numPages = dataFile.GetLength();

        for (int p = 0; p < numPages; ++p) {
            Page pg;
            dataFile.GetPage(pg, p);
            Record tmp;
            while (pg.GetFirst(tmp)) {
                if (rid++ == targetId) {
                    if (predicate.Run(tmp, constants)) {
                        _record.CopyBits(tmp.GetBits(), tmp.GetSize());
                        return true;
                    }
                    goto nextRID;
                }
            }
        }
    nextRID:
        ;
    }

    return false;
}



int ScanIndex::toDot(std::ostream &dot, int parent, int &counter) const {
    int id = counter++;
    dot << "  node" << id
        << " [label=\"SCAN-INDEX\\n" << esc(tableName) << "\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";
    return id;
}


// Helper: Convert a constant from a record into its string representation.
std::string convertConstantToStr(Record& constants, Type attType, int index) {
    std::string result;
    if (attType == Integer) {
        // Retrieve the integer value and convert it to string.
        int value = *reinterpret_cast<int*>(constants.GetColumn(index));
        result = std::to_string(value);
    } else if (attType == String) {
        // Wrap the string value in single quotes.
        result = std::string("'") + constants.GetColumn(index) + "'";
    } else if (attType == Float) {
        // Retrieve the floating-point value and convert it to string.
        double value = *reinterpret_cast<double*>(constants.GetColumn(index));
        result = std::to_string(value);
    }
    return result;
}

// Helper: Convert a comparison operator into its corresponding string.
std::string convertOperatorToStr(CompOperator op) {
    std::string opStr;
    if (op == Equals) {
        opStr = " = ";
    } else if (op == LessThan) {
        opStr = " &lt; ";
    } else if (op == GreaterThan) {
        opStr = " &gt; ";
    }
    return opStr;
}


ostream& Select::print(ostream& _os) {
	// Generate the node's identifier using the Greek letter sigma and embed the predicate.
stringstream node_name;
node_name << "\"\u03C3_{" << predicate << "}\"";

// Lambda to convert a single comparison into a string representation.
auto convertComparison = [&] (const Comparison &comp, int idx) -> std::string {
    std::string result;
    // For the first operand, ensure both outcomes are std::string.
    result += (comp.operand1 != Literal)
                ? std::string(schema.GetAtts()[comp.whichAtt1].name)
                : convertConstantToStr(constants, comp.attType, idx);
    // Append the operator string.
    result += convertOperatorToStr(comp.op);
    // For the second operand, similarly convert attribute name to std::string if needed.
    result += (comp.operand2 != Literal)
                ? std::string(schema.GetAtts()[comp.whichAtt2].name)
                : convertConstantToStr(constants, comp.attType, idx);
    return result;
};


// Build a human-readable predicate string from the CNF structure.
string predicates;
if (predicate.numAnds > 0) {
    // Process the first comparison.
    predicates = convertComparison(predicate.andList[0], 0);
    // Process any additional comparisons, concatenating them with " AND ".
    for (int i = 1; i < predicate.numAnds; i++) {
        predicates += " AND " + convertComparison(predicate.andList[i], i);
    }
}

// Form the label for the Graphviz node using subscript formatting.
string label = "<\u03C3<SUB>" + predicates + "</SUB>>";
// Output the node definition to the Graphviz dot file with plaintext shape.
//queryGraphDotFile << node_name.str() << "[label=" << label << ", shape=plaintext]" << endl;

// Replace HTML-escaped tokens with the actual characters for terminal display.
modifyString(predicates, "&lt;", "<");
modifyString(predicates, "&gt;", ">");

// Print the SELECT operation details to the terminal.
_os << "SELECT (" << predicates << " ) {" << schema << "} " << *producer << endl;

	return _os;
}


Project::Project(Schema& _schemaIn, Schema& _schemaOut, int _numAttsInput,
	int _numAttsOutput, int* _keepMe, RelationalOp* _producer) {
		schemaIn = _schemaIn;
		schemaOut = _schemaOut;
		numAttsInput = _numAttsInput;
		numAttsOutput = _numAttsOutput;
		keepMe = _keepMe;
		producer = _producer;


}

Project::~Project() {
 delete[] keepMe;
    //delete producer;
    delete producer;
}


bool Project::GetNext(Record& _record) {
	if (producer->GetNext(_record)){
		_record.Project(keepMe,numAttsOutput,numAttsInput);

		return true;

	}
	return false;
}

ostream& Project::print(ostream& _os) {

	// Create a comma-separated string of attribute names from schemaOut.
		string atts;
		int totalAtts = schemaOut.GetAtts().Length();
		if (totalAtts > 0) {
			// Start with the first attribute to avoid a leading comma.
			atts = schemaOut.GetAtts()[0].name;
			// Append the remaining attribute names, each preceded by a comma and space.
			for (int i = 1; i < totalAtts; ++i) {
				atts += ", " + std::string(schemaOut.GetAtts()[i].name);
			}
		}

		// Assemble the node identifier using the Unicode character for π.
		stringstream node_name;
		node_name << "\"\u03C0_{" << atts << "}\"";

		// Construct the label using subscript formatting to highlight the attribute list.
		string label = "<\u03C0<SUB>" + atts + "</SUB>>";

		// Write the node information to the Graphviz dot file with a plaintext shape.
		//queryGraphDotFile << node_name.str() << "[label=" << label << ", shape=plaintext]" << endl;

		// Display the PROJECT operation along with its schema details on the terminal.
		_os << "PROJECT {" << schemaOut << "} " << *producer << endl;



		return _os;
}

// ─── PROJECT ─────────────────────────────────────────────────────────────────
int Project::toDot(std::ostream &dot, int parent, int &counter) const {
    auto &atts = const_cast<Schema&>(schemaOut).GetAtts();
    std::string a;
    for (unsigned i = 0; i < atts.Length(); ++i) {
        if (i) a += ", ";
        a += esc(atts[i].name);
    }

    int id = counter++;
    dot << "  node" << id
        << " [label=\"PROJECT\\n" << a << "\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";
    producer->toDot(dot, id, counter);
    return id;
}

Join::Join(Schema& _schemaLeft, Schema& _schemaRight, Schema& _schemaOut,
	CNF& _predicate, RelationalOp* _left, RelationalOp* _right) {
		schemaLeft = _schemaLeft;
		schemaRight = _schemaRight;
		schemaOut = _schemaOut;
		predicate = _predicate;
		left = _left;
		right = _right;

}

Join::~Join() {
    delete left;
    delete right;



}

ostream& Join::print(ostream& _os) {

		// Establish the node identifier for the join operation using the bowtie symbol.
	stringstream node_name;

	// Build the join predicate string by iterating through each comparison in the predicate.
	string predicates;
	for (int i = 0; i < predicate.numAnds; i++) {
		int left_att_position, right_att_position;
		// Determine which operand comes from the right relation.
		if (predicate.andList[i].operand1 == Right) {
			right_att_position = predicate.andList[i].whichAtt1;
			left_att_position = predicate.andList[i].whichAtt2;
		} else {
			right_att_position = predicate.andList[i].whichAtt2;
			left_att_position = predicate.andList[i].whichAtt1;
		}

		// Retrieve attribute names from the corresponding schemas.
		string left_name = schemaLeft.GetAtts()[left_att_position].name;
		string right_name = schemaRight.GetAtts()[right_att_position].name;

		// Formulate the condition string for this comparison.
		string condition = left_name + " = " + right_name;

		// Append conditions together, separated by " AND " if not the first.
		if (i > 0) {
			predicates += " AND ";
		}
		predicates += condition;
	}

	// Set up the node's identifier for the dot file using the bowtie symbol (Unicode U+22C8).
	node_name << "\"\u22C8_{" << predicates << "}\"";

	// Create a label for the node with subscript formatting displaying the predicate.
	string label = "<\u22C8<SUB>" + predicates + "</SUB>>";

	// Output the node to the Graphviz dot file.
	//queryGraphDotFile << node_name.str() << "[label=" << label << ", shape=plaintext]" << endl;

	// Replace HTML escape sequences with their literal characters for terminal output.
	modifyString(predicates, "&lt;", "<");
	modifyString(predicates, "&gt;", ">");

	// Print out the join operator details to the terminal.
	_os << "JOIN (" << predicates << ") " << schemaOut << " { Left: " << *left
		<< " Right: " << *right << " }";


    return _os;
}


// ─── GENERIC JOIN (used by NestedLoopJoin) ──────────────────────────────────
int Join::toDot(std::ostream &dot, int parent, int &counter) const {
    // Pull attribute lists from both sides
    auto &L = const_cast<Schema&>(schemaLeft).GetAtts();
    auto &R = const_cast<Schema&>(schemaRight).GetAtts();

    // Build the join‑predicate string
    std::string p;
    for (int i = 0; i < predicate.numAnds; ++i) {
        if (i) p += "\\n";
        auto &c = predicate.andList[i];

        // Determine which side is “left” and “right” in the comparison
        std::string leftName, rightName;
        if (c.operand1 == Left  && c.operand2 == Right) {
            leftName  = L[c.whichAtt1].name;
            rightName = R[c.whichAtt2].name;
        }
        else if (c.operand1 == Right && c.operand2 == Left) {
            leftName  = L[c.whichAtt2].name;
            rightName = R[c.whichAtt1].name;
        }
        else {
            // fallback (non‑equi or mis‑ordered): assume both from same schema
            leftName  = L[c.whichAtt1].name;
            rightName = R[c.whichAtt2].name;
        }

        p += esc(leftName) + "=" + esc(rightName);
    }

    // Emit this node
    int id = counter++;
    dot << "  node" << id
        << " [label=\"JOIN\\n" << p << "\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";

    // Recurse into children
    left ->toDot(dot, id, counter);
    right->toDot(dot, id, counter);
    return id;
}


NestedLoopJoin::NestedLoopJoin(Schema& _schemaLeft, Schema& _schemaRight, Schema& _schemaOut,
	CNF& _predicate, RelationalOp* _left, RelationalOp* _right)
	: Join(_schemaLeft, _schemaRight, _schemaOut, _predicate, _left, _right) {

}

NestedLoopJoin::~NestedLoopJoin() {
    delete left;
    delete right;

}

bool NestedLoopJoin::GetNext(Record& _record) {
    // per‑instance storage keyed by this pointer
    static std::unordered_map<const NestedLoopJoin*, RecordList> listMap;
    static std::unordered_map<const NestedLoopJoin*, bool>       doneMap;
    static std::unordered_map<const NestedLoopJoin*, Record>     leftMap;

    RecordList &list    = listMap[this];
    bool        firstTime = !doneMap[this];
    Record     &lRec     = leftMap[this];
    //     static RecordList list;
    // static bool firstTime = true;
    // static Record lRec;

    // BUILD PHASE: load entire right side once
    if (firstTime) {
        std::cout << "[NestedLoopJoin] Entering build phase." << std::endl;
        Record temp;
        while (right->GetNext(temp)) {
            list.Append(temp);
        }
        doneMap[this] = true;
        if (!left->GetNext(lRec)) {
            return false;
        }
        list.MoveToStart();
    }

    // PROBE PHASE
    while (true) {
        // scan inner list for this outer tuple
        while (!list.AtEnd()) {
            Record &rRec = list.Current();

            if (predicate.Run(lRec, rRec)) {
                // we found the first match for this outer tuple
                list.Advance();
               // cout << "match found" << endl;
                int nL = schemaLeft .GetNumAtts();
                int nR = schemaRight.GetNumAtts();
              //  std::cout << "num attributes left --> " << nL << std::endl;
//                 int numrightAtts = schemaRight.GetNumAtts();
              //  std::cout << "num attributes right --> " << nR << std::endl;
               // std::cout << schemaLeft <<endl;
                //std::cout << schemaRight <<endl;
               // std::cout << "Schemas printed above -- left and right record below" << endl;
                lRec.print(std::cout, schemaLeft);
               // std::cout << "Record  left above" << endl;
                rRec.print(std::cout, schemaRight);
              //  std::cout << "Record  right above" << endl;

                _record.AppendRecords(lRec, rRec, nL, nR);
                _record.print(std::cout, schemaOut);


                return true;
            }
            list.Advance();
        }

        // no match for this outer tuple, advance to next
        if (!left->GetNext(lRec)) {
            return false;
        }
        list.MoveToStart();
    }
}

// ─── NESTED‐LOOP‐JOIN ────────────────────────────────────────────────────────
// Just delegates to the generic Join above
int NestedLoopJoin::toDot(std::ostream &dot, int parent, int &counter) const {
    return Join::toDot(dot, parent, counter);
}
  //------------------------------------------------------------------------------


// --------- HashJoin -----------------------------------------------------

HashJoin::HashJoin(Schema& _schemaLeft,
    Schema& _schemaRight,
    Schema& _schemaOut,
    CNF& _predicate,
    RelationalOp* _left,
    RelationalOp* _right,
    const OrderMaker& _leftOrder,
    const OrderMaker& _rightOrder)
: Join(_schemaLeft, _schemaRight, _schemaOut, _predicate, _left, _right),
leftOrder(_leftOrder),
rightOrder(_rightOrder)
{}

HashJoin::~HashJoin() {
// The base Join destructor already deletes left & right
delete left;
delete right;
}

bool HashJoin::GetNext(Record& outputRec) {
    // Per-instance state
    static std::unordered_map<const HashJoin*, bool> builtMap;
    static std::unordered_map<const HashJoin*, Map<Record, List<Record>>> hashMap;
    static std::unordered_map<const HashJoin*, Record> probeMap;
    static std::unordered_map<const HashJoin*, List<Record>*> matchListMap;
    static std::unordered_map<const HashJoin*, bool> inMatchingMap;

    bool& built = builtMap[this];
    Map<Record, List<Record>>& map = hashMap[this];
    Record& leftRec = probeMap[this];
    List<Record>*& matchList = matchListMap[this];
    bool& inMatching = inMatchingMap[this];

    // Emit buffered matches from previous GetNext call
    if (inMatching && matchList && !matchList->AtEnd()) {
        Record& rightRec = matchList->Current();
        matchList->Advance();
        int nl = schemaLeft.GetNumAtts();
        int nr = schemaRight.GetNumAtts();
        outputRec.AppendRecords(leftRec, rightRec, nl, nr);
        return true;
    }

    inMatching = false;

    // Build hash table from right side (only once)
    if (!built) {
        Record temp;
        while (right->GetNext(temp)) {
            temp.SetOrderMaker(&rightOrder);

            Record key;
            key.CopyBits(temp.GetBits(), temp.GetSize());
            key.SetOrderMaker(&rightOrder);

            if (map.IsThere(key)) {
                map.CurrentData().Append(temp);
            } else {
                List<Record> lst;
                lst.Append(temp);
                map.Insert(key, lst);
            }
        }
        built = true;
    }

    // Probe left side
    while (left->GetNext(leftRec)) {
        leftRec.SetOrderMaker(&leftOrder);

        if (map.IsThere(leftRec)) {
            matchList = &map.CurrentData();
            matchList->MoveToStart();
            inMatching = true;

            // Emit first match now
            Record& rightRec = matchList->Current();
            matchList->Advance();

            int nl = schemaLeft.GetNumAtts();
            int nr = schemaRight.GetNumAtts();
            outputRec.AppendRecords(leftRec, rightRec, nl, nr);
            return true;
        }
    }

    return false;  // no more matches
}

// ─── HASH‐JOIN ──────────────────────────────────────────────────────────────
int HashJoin::toDot(std::ostream &dot, int parent, int &counter) const {
    // Exactly the same as generic, but label it “HASH-JOIN”
    auto &L = const_cast<Schema&>(schemaLeft).GetAtts();
    auto &R = const_cast<Schema&>(schemaRight).GetAtts();

    std::string p;
    for (int i = 0; i < predicate.numAnds; ++i) {
        if (i) p += "\\n";
        auto &c = predicate.andList[i];

        std::string leftName, rightName;
        if (c.operand1 == Left  && c.operand2 == Right) {
            leftName  = L[c.whichAtt1].name;
            rightName = R[c.whichAtt2].name;
        }
        else if (c.operand1 == Right && c.operand2 == Left) {
            leftName  = L[c.whichAtt2].name;
            rightName = R[c.whichAtt1].name;
        }
        else {
            leftName  = L[c.whichAtt1].name;
            rightName = R[c.whichAtt2].name;
        }

        p += esc(leftName) + "=" + esc(rightName);
    }

    int id = counter++;
    dot << "  node" << id
        << " [label=\"HASH-JOIN\\n" << p << "\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";

    left ->toDot(dot, id, counter);
    right->toDot(dot, id, counter);
    return id;
}


// --------- SymmetricHashJoin ---------------------------------------------

SymmetricHashJoin::SymmetricHashJoin(Schema& _schemaLeft,
                      Schema& _schemaRight,
                      Schema& _schemaOut,
                      CNF& _predicate,
                      RelationalOp* _left,
                      RelationalOp* _right,
                      const OrderMaker& _leftOrder,
                      const OrderMaker& _rightOrder)
: Join(_schemaLeft, _schemaRight, _schemaOut, _predicate, _left, _right),
leftOrder(_leftOrder),
rightOrder(_rightOrder)
{}

SymmetricHashJoin::~SymmetricHashJoin() {
    delete left;
    delete right;
// The base Join destructor handles deleting children
}

bool SymmetricHashJoin::GetNext(Record& outputRec) {
    const int BATCH_SIZE = 200;

    static unordered_map<const SymmetricHashJoin*, Map<Record, List<Record>>> lMap, rMap;
    static unordered_map<const SymmetricHashJoin*, bool> lDoneMap, rDoneMap;
    static unordered_map<const SymmetricHashJoin*, int> sideMap;
    static unordered_map<const SymmetricHashJoin*, bool> inMatchingMap;
    static unordered_map<const SymmetricHashJoin*, List<Record>*> matchListMap;
    static unordered_map<const SymmetricHashJoin*, Record> probeMap;
    static unordered_map<const SymmetricHashJoin*, bool> probingLeftMap;

    auto& lHash = lMap[this];
    auto& rHash = rMap[this];
    bool& lDone = lDoneMap[this];
    bool& rDone = rDoneMap[this];
    int& whichSide = sideMap[this]; // 0 = LEFT, 1 = RIGHT
    bool& inMatching = inMatchingMap[this];
    List<Record>*& matchList = matchListMap[this];
    Record& probe = probeMap[this];
    bool& probingLeft = probingLeftMap[this];

    // Emit any remaining matches
    if (inMatching && matchList && !matchList->AtEnd()) {
        Record& match = matchList->Current();
        matchList->Advance();

        int nLeft = schemaLeft.GetNumAtts();
        int nRight = schemaRight.GetNumAtts();
        outputRec.AppendRecords(probingLeft ? probe : match, probingLeft ? match : probe, nLeft, nRight);
        return true;
    }
    inMatching = false;

    // Pull from alternating side
    while (true) {
        whichSide = 1 - whichSide;
        int loaded = 0;

        while (loaded < BATCH_SIZE) {
            Record rec;
            if ((whichSide == 0 && (lDone || !left->GetNext(rec))) ||
                (whichSide == 1 && (rDone || !right->GetNext(rec)))) {
                if (whichSide == 0) lDone = true;
                else rDone = true;
                break;
            }

            rec.SetOrderMaker(whichSide == 0 ? &leftOrder : &rightOrder);
            Record key;
            key.CopyBits(rec.GetBits(), rec.GetSize());
            key.SetOrderMaker(whichSide == 0 ? &leftOrder : &rightOrder);

            Map<Record, List<Record>>& probeMap = (whichSide == 0 ? rHash : lHash);
            // if (probeMap.IsThere(key)) {
            //     matchList = &probeMap.CurrentData();
            //     matchList->MoveToStart();
            //     probe.CopyBits(rec.GetBits(), rec.GetSize());
            //     probe.SetOrderMaker(whichSide == 0 ? &leftOrder : &rightOrder);
            //     probingLeft = (whichSide == 0);
            //     inMatching = true;
            //     return GetNext(outputRec);
            // }

            if (probeMap.IsThere(key)) {
                matchList = &probeMap.CurrentData();
                matchList->MoveToStart();
                probe.CopyBits(rec.GetBits(), rec.GetSize());
                probe.SetOrderMaker(whichSide == 0 ? &leftOrder : &rightOrder);
                probingLeft = (whichSide == 0);
                inMatching = true;
            
                // Emit the first match now
                if (matchList && !matchList->AtEnd()) {
                    Record& match = matchList->Current();
                    matchList->Advance();
            
                    int nLeft = schemaLeft.GetNumAtts();
                    int nRight = schemaRight.GetNumAtts();
                    outputRec.AppendRecords(probingLeft ? probe : match,
                                            probingLeft ? match : probe,
                                            nLeft, nRight);
                    return true;
                }
            }
            

            Map<Record, List<Record>>& insertMap = (whichSide == 0 ? lHash : rHash);
            if (insertMap.IsThere(key)) {
                insertMap.CurrentData().Append(rec);
            } else {
                List<Record> lst;
                lst.Append(rec);
                insertMap.Insert(key, lst);
            }

            loaded++;
        }

        if (lDone && rDone) return false;
    }
}


// ─── SYMMETRIC‐HASH‐JOIN ────────────────────────────────────────────────────
int SymmetricHashJoin::toDot(std::ostream &dot, int parent, int &counter) const {
    // Label “SYM-HASH-JOIN” with the same predicate logic
    auto &L = const_cast<Schema&>(schemaLeft).GetAtts();
    auto &R = const_cast<Schema&>(schemaRight).GetAtts();

    std::string p;
    for (int i = 0; i < predicate.numAnds; ++i) {
        if (i) p += "\\n";
        auto &c = predicate.andList[i];

        std::string leftName, rightName;
        if (c.operand1 == Left  && c.operand2 == Right) {
            leftName  = L[c.whichAtt1].name;
            rightName = R[c.whichAtt2].name;
        }
        else if (c.operand1 == Right && c.operand2 == Left) {
            leftName  = L[c.whichAtt2].name;
            rightName = R[c.whichAtt1].name;
        }
        else {
            leftName  = L[c.whichAtt1].name;
            rightName = R[c.whichAtt2].name;
        }

        p += esc(leftName) + "=" + esc(rightName);
    }

    int id = counter++;
    dot << "  node" << id
        << " [label=\"SYM-HASH-JOIN\\n" << p << "\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";

    left ->toDot(dot, id, counter);
    right->toDot(dot, id, counter);
    return id;
}



DuplicateRemoval::DuplicateRemoval(Schema& _schema, RelationalOp* _producer) {
	schema = _schema;
	producer = _producer;


}

DuplicateRemoval::~DuplicateRemoval() {



}


bool DuplicateRemoval::GetNext(Record& _record) {
    // Create a local alias for the vector to avoid fully qualified names.
    typedef std::vector<Record*> RecordVector;

    // The following static variables persist across calls within one query.
    static RecordVector uniqueRecords;
    static size_t currentIndex = 0;
    static bool accumulated = false;

    // ------------------------------
    // Phase 1: Accumulation Phase
    // ------------------------------
    if (!accumulated) {
        // Reset (clear any leftover state from previous queries).
        uniqueRecords.clear();
        currentIndex = 0;

        // Use an OrderMaker for comparing records
        OrderMaker order(schema);
        Record temp;
        // Loop to drain the producer; DBFile or prior operator will output a record each call.
        while (producer->GetNext(temp)) {
            // Set the order maker on the incoming record so that comparison works.
            temp.SetOrderMaker(&order);
            bool duplicate = false;
            // Compare with all previously accumulated records.
            for (size_t i = 0; i < uniqueRecords.size(); i++) {
                if (order.Run(*uniqueRecords[i], temp) == 0) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                // Deep-copy the record from temp.
                Record* copy = new Record();
                char* bits = temp.GetBits();
                int len = ((int*)bits)[0];
                char* newBits = new char[len];
                memcpy(newBits, bits, len);
                copy->Consume(newBits);
                // Set the order maker on the copied record so it can be used later for comparisons.
                copy->SetOrderMaker(&order);
                uniqueRecords.push_back(copy);
            }
        }
        accumulated = true;
    }

    // ------------------------------
    // Phase 2: Return Distinct Records
    // ------------------------------
    if (currentIndex < uniqueRecords.size()) {
        Record* distinctRecord = uniqueRecords[currentIndex];
        // Deep copy the stored record into _record.
        char* bits = distinctRecord->GetBits();
        int len = ((int*)bits)[0];
        char* newBits = new char[len];
        memcpy(newBits, bits, len);
        _record.Consume(newBits);
        currentIndex++;
        return true;
    } else {
        // At end, free all memory allocated in the accumulation phase.
        for (size_t i = 0; i < uniqueRecords.size(); i++) {
            delete uniqueRecords[i];
        }
        uniqueRecords.clear();
        // Reset static variables so that subsequent queries can accumulate fresh data.
        accumulated = false;
        currentIndex = 0;
        return false;
    }
}


ostream& DuplicateRemoval::print(ostream& _os) {
	_os << "DISTINCT {" << schema << "} " << *producer << endl;

	return _os;
}

// ─── DISTINCT ────────────────────────────────────────────────────────────────
int DuplicateRemoval::toDot(std::ostream &dot, int parent, int &counter) const {
    int id = counter++;
    dot << "  node" << id << " [label=\"DISTINCT\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";
    producer->toDot(dot, id, counter);
    return id;
}


// -----------------------------------------------------------------------------
// Constructor: initialize hasReturned to false
Sum::Sum(Schema& _schemaIn, Schema& _schemaOut, Function& _compute,
    RelationalOp* _producer)
: schemaIn(_schemaIn)
, schemaOut(_schemaOut)
, compute(_compute)
, producer(_producer)
, hasReturned(false)    // <-- initialize here
{}

// -----------------------------------------------------------------------------
Sum::~Sum() {
// if you delete producer here, leave that in place:
// delete producer;
}

// -----------------------------------------------------------------------------
bool Sum::GetNext(Record& _record) {
// If we've already returned our single result, stop.
if (hasReturned) return false;

double runningSum = 0.0;
int    iRes;
double dRes;
Record temp;

// Accumulate over all input tuples
while (producer->GetNext(temp)) {
   Type returnType = compute.Apply(temp, iRes, dRes);
   if (returnType == Integer) {
       runningSum += static_cast<double>(iRes);
   }
   else if (returnType == Float) {
       runningSum += dRes;
   }
   else {
       cerr << "[Sum] Unknown return type from compute.Apply\n";
       return false;
   }
}

// Build a one‑attribute record holding the double sum
const int recSize       = 16;    // two-int header + double
char*     recSpace      = new char[recSize];
int       totalSize     = recSize;
int       firstAttrOff  = 8;     // header is two ints = 8 bytes

// header: [ totalSize, offsetToFirstAttr ]
memcpy(recSpace,                  &totalSize,       sizeof(int));
memcpy(recSpace + sizeof(int),    &firstAttrOff,    sizeof(int));
// data: the double
memcpy(recSpace + firstAttrOff,   &runningSum,      sizeof(double));

_record.Consume(recSpace);

// mark that we've emitted our one row
hasReturned = true;
return true;
}

// -----------------------------------------------------------------------------
ostream& Sum::print(ostream& _os) {
_os << "SUM {" << schemaOut << "} " << *producer << endl;
return _os;
}

// ─── SUM ─────────────────────────────────────────────────────────────────────
int Sum::toDot(std::ostream &dot, int parent, int &counter) const {
    int id = counter++;
    dot << "  node" << id << " [label=\"SUM\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";
    producer->toDot(dot, id, counter);
    return id;
}

// Constructor: initialize all per‐instance flags
GroupBy::GroupBy(Schema& _schemaIn,
    Schema& _schemaOut,
    OrderMaker& _groupingAtts,
    Function& _compute,
    RelationalOp* _producer)
: schemaIn(_schemaIn),
schemaOut(_schemaOut),
groupingAtts(_groupingAtts),
compute(_compute),
producer(_producer),
gb_initialized(false),
gb_done(false)
{}

GroupBy::~GroupBy() {
delete producer;
}

bool GroupBy::GetNext(Record& _record) {
// 1) Figure out key type once
if (!gb_initialized) {
Attribute grpAtt = schemaIn.GetAtts()[ groupingAtts.whichAtts[0] ];
gb_keyIsString = (grpAtt.type == String);
gb_initialized = true;
}

// 2) Accumulate all groups on first invocation
if (!gb_done) {
Record rec;
while (producer->GetNext(rec)) {
int    iVal;
double dVal;
Type   t = compute.Apply(rec, iVal, dVal);
double value = (t == Integer ? iVal : dVal);

// extract grouping key
char* col = rec.GetColumn(groupingAtts.whichAtts[0]);
std::string key = gb_keyIsString
   ? std::string(col)
   : std::to_string(*reinterpret_cast<int*>(col));

gb_map[key] += value;
}
gb_it   = gb_map.begin();
gb_done = true;
}

// 3) If we've exhausted the map, we're done
if (gb_it == gb_map.end()) {
return false;
}

// 4) Build a single output record for the current group
const std::string keyStr = gb_it->first;
const double      sumVal = gb_it->second;
++gb_it;

// record layout: [ totalSize, offset0, offset1 ] + double + key
int sizeKey    = gb_keyIsString ? keyStr.size()+1 : sizeof(int);
int headerSize = 3 * sizeof(int);
int recSize    = headerSize + sizeof(double) + sizeKey;

char* buf = new char[recSize];
int* hdr  = reinterpret_cast<int*>(buf);
hdr[0] = recSize;
hdr[1] = headerSize;                 // offset of sumVal
hdr[2] = headerSize + sizeof(double);// offset of key

// copy the sum
memcpy(buf + hdr[1], &sumVal, sizeof(double));

// copy the key
if (gb_keyIsString) {
memcpy(buf + hdr[2], keyStr.c_str(), sizeKey);
} else {
int k = std::stoi(keyStr);
memcpy(buf + hdr[2], &k, sizeof(int));
}

_record.Consume(buf);
return true;
}

ostream& GroupBy::print(ostream& _os) {
_os << "GROUP BY {" << schemaOut
<< "} {grouping atts: " << groupingAtts
<< "}" << *producer << endl;
return _os;
}

// ─── GROUP‐BY ───────────────────────────────────────────────────────────────
int GroupBy::toDot(std::ostream &dot, int parent, int &counter) const {
    auto &atts = const_cast<Schema&>(schemaIn).GetAtts();
    std::string g;
    for (int i = 0; i < groupingAtts.numAtts; ++i) {
        if (i) g += "\\n";
        g += esc(atts[groupingAtts.whichAtts[i]].name);
    }

    int id = counter++;
    dot << "  node" << id << " [label=\"GROUP-BY\\n" << g << "\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";
    producer->toDot(dot, id, counter);
    return id;
}


WriteOut::WriteOut(const Schema& _schema,
    const string& _outFile,
    RelationalOp* _producer)
: schema(_schema)
, outFile(_outFile)
, producer(_producer)
, initialized(false)
, isDone(false)
{
// nothing else
}

WriteOut::~WriteOut() {
if (fileStream.is_open())
fileStream.close();
delete producer;
}

bool WriteOut::GetNext(Record& _record) {
// 1) if already finished, bail
if (isDone) return false;

// 2) first call → open/truncate the file
if (!initialized) {
fileStream.open(outFile, std::ios::trunc);
if (!fileStream) {
std::cerr << "[WriteOut] ERROR: cannot open "
 << outFile << " for writing\n";
isDone = true;
return false;
}
initialized = true;
std::cerr << "[WriteOut] opened file `" << outFile << "`\n";
}

// 3) pull next record from upstream
if (producer->GetNext(_record)) {
_record.print(fileStream, schema);
fileStream << "\n";
return true;
}

// 4) upstream is done → close & mark
isDone = true;
fileStream.close();
std::cerr << "[WriteOut] finished writing, closed file\n";
return false;
}

// bool WriteOut::GetNext(Record& _record) {
//     // 1) if already finished, bail
//     if (isDone) return false;

//     // 2) first call → open (and truncate) the file
//     if (!initialized) {
//         fileStream.open(outFile, std::ios::trunc);
//         if (!fileStream) {
//             std::cerr << "[WriteOut] ERROR: cannot open "
//                       << outFile << " for writing\n";
//             isDone = true;
//             return false;
//         }
//         initialized = true;
//         std::cerr << "[WriteOut] opened file `" << outFile << "`\n";
//     }

//     // 3) pull next record from upstream
//     if (producer->GetNext(_record)) {
//         // --- 3a) write to file exactly as before ---
//         _record.print(fileStream, schema);
//         fileStream << "\n";

//         // --- 3b) now echo to terminal in “column|column|...” style ---
//         int n = schema.GetNumAtts();
//         for (int i = 0; i < n; ++i) {
//             auto& att = schema.GetAtts()[i];
//             char*  col = _record.GetColumn(i);

//             switch (att.type) {
//                 case Integer:
//                     std::cout << *reinterpret_cast<int*>(col);
//                     break;
//                 case Float:
//                     std::cout << *reinterpret_cast<double*>(col);
//                     break;
//                 case String:
//                     std::cout << col;
//                     break;
//             }
//             if (i + 1 < n) std::cout << "|";
//         }
//         std::cout << "\n";

//         return true;
//     }

//     // 4) upstream is done → close & mark
//     isDone = true;
//     fileStream.close();
//     std::cerr << "[WriteOut] finished writing, closed file\n";
//     return false;
// }

ostream& WriteOut::print(ostream& _os) {
_os << "OUTPUT {" << schema << "} " << *producer;
return _os;
}


// ─── OUTPUT ──────────────────────────────────────────────────────────────────
int WriteOut::toDot(std::ostream &dot, int parent, int &counter) const {
    int id = counter++;
    dot << "  node" << id << " [label=\"OUTPUT\"];\n";
    if (parent >= 0)
        dot << "  node" << parent << " -> node" << id << ";\n";
    producer->toDot(dot, id, counter);
    return id;
}
void QueryExecutionTree::ExecuteQuery() {
	Record r;
	while (root->GetNext(r)) {
		// this loop ends when the query has processed all the records
		cout << "Record inside execution tree - ExecuteQuery -->  "<< root   <<endl;

	}
}


ostream& operator<<(ostream& _os, QueryExecutionTree& _op) {
	_os << "QUERY EXECUTION TREE: " << *_op.root << endl;

	return _os;
}

// ─── ROOT WRITER ─────────────────────────────────────────────────────────────
void QueryExecutionTree::ToDotFile(const std::string &path) const {
    std::ofstream dot(path);
    dot << "digraph QueryPlan {\n";
    dot << "  node [shape=box, style=rounded];\n";
    // optional if you want root-at-bottom:
    // dot << "  rankdir=BT;\n";
    int counter = 0;
    root->toDot(dot, -1, counter);
    dot << "}\n";
}