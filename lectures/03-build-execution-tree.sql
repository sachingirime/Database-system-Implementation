{       TableList* _tables,
        NameList* _attsToSelect,
	FuncOperator* _finalFunction,
        AndList* _predicate,
	NameList* _groupingAtts,
        int& _distinctAtts
}
        |
        |
        |
{
        QueryExecutionTree& _queryTree
}

- we build the query execution tree bottom-up
- we start from the leaves and move up to the root by creating join operators

SCAN
----
int nTbl = 0;
for (TableList* node = _tables; node != NULL; node = node->next) nTbl += 1;
RelationalOp** forest = new RelationalOp*[nTbl];
Schema* forestSchema = new Schema[nTbl];

int idx = 0;
for (TableList* node = _tables; node != NULL; node = node->next) {
        string s = node->tableName;
        SString ss = s;
        bool b = catalog->GetSchema(ss, forestSchema[idx]);
        if (false == b) {
                cout << "Semantic error: table " << s << " does not exist in the database!" << endl;
                exit(1);
        }

        DBFile dbFile;
        forest[idx] = new Scan(forestSchema[idx], dbFile, s);
        idx += 1;
}

cout << "SCANS" << endl;
cout << "+++++++++++++++++++++++" << endl;
for (int i = 0; i < nTbl; i++) cout << *forest[i] << endl;


SELECT
------
Select(Schema& _schema, CNF& _predicate, Record& _constants,
        RelationalOp* _producer)

SELECT SUM(ps_supplycost), suppkey 
FROM part, supplier, partsupp 
WHERE p_partkey = ps_partkey AND s_suppkey = ps_suppkey AND s_acctbal > 2500.00
GROUP BY s_suppkey
--> create SELECT only for supplier (s_acctbal > 2500.00)
--> no SELECT for part or partsupp

SELECT l_orderkey
FROM lineitem
WHERE l_shipdate > l_receiptdate AND l_shipdate < l_commitdate
--> create SELECT only for lineitem (l_shipdate > l_receiptdate AND l_shipdate < l_commitdate)

for (int i = 0; i < nTbl; i++) {
        Record literal;
        CNF cnf;
        int ret = cnf.ExtractCNF (*_predicate, forestSchema[i], literal);
        if (0 > ret) exit(1);

        if (0 < ret) {
                RelationalOp* op = new Select(forestSchema[i], cnf, literal, forest[i]);
                forest[i] = op;
        }
}

cout << endl << "PUSH DOWN SELECTION" << endl;
cout << "+++++++++++++++++++++++" << endl;
for (int i = 0; i < nTbl; i++) cout << *forest[i] << endl;


SELECT SUM(ps_supplycost), suppkey 
FROM part, supplier, partsupp 
WHERE p_partkey = ps_partkey AND s_suppkey = ps_suppkey
GROUP BY s_suppkey
--> this query does not have any SELECT

SELECT SUM(ps_supplycost)
FROM partsupp
--> this query does not have any SELECT or JOIN
--> this query is not part of our SQL subset


+++++++++++++++++++
RULES FOR QUERY EXECUTION TREES

R1: OUR SQL SUBSET REQUIRES EITHER A SELECT OR A JOIN OPERATOR FOR EVERY QUERY
R2: OUR SQL SUBSET HAS EXACTLY ONE OF PROJECT, SUM, OR GROUP BY FOR EVERY QUERY
R3: DISTINCT IS PART OF THE QUEYR TREE ONLY WHEN PROJECT IS ALSO IN THE QUERY TREE
+++++++++++++++++++

PROJECT
-------
Project(Schema& _schemaIn, Schema& _schemaOut, int _numAttsInput,
        int _numAttsOutput, int* _keepMe, RelationalOp* _producer);

+++++++++++++++++++
RULE FOR PROJECT

PROJECT IS REQUIRED ONLY WHEN THERE IS NO SUM OR GROUP BY
if (_finalFunction == 0 && _groupingAtts == 0)
+++++++++++++++++++

_schemaIn is the schema of the _producer operator
_schemaOut is a schema that contains only the attributes in _attsToSelect

SELECT l_orderkey
FROM lineitem
WHERE l_shipdate > l_receiptdate AND l_shipdate < l_commitdate
--> this query requires a PROJECT

SCAN(lineitem) : schema is lineitem
SELECT(l_shipdate > l_receiptdate AND l_shipdate < l_commitdate) : schema is lineitem
PROJECT(l_orderkey) : _schemaIn is lineitem; _schemaOut is {l_orderkey}

_attsToSelect = {l_orderkey}
_schemaIn = {lineitem}

--> these data structures are initialized in QueryCompiler.Compile()
_numAttsInput = _schemaIn.GetNumAtts() // 16
_numAttsOutput = _attsToSelect.Length() // 1
_keepMe --> index of atts in _attsToSelect in _schemaIn // {0}
        --> _schemaIn.Index(_attsToSelect.Current)

{int _numAttsOutput, int* _keepMe} --> IntVector projVec
_schemaOut = _schemaIn
_schemaOut.Project(projVec)

--> create a Project operator instance by calling its constructor


SUM
---
Sum(Schema& _schemaIn, Schema& _schemaOut, Function& _compute,
        RelationalOp* _producer);

SELECT SUM(ps_supplycost)
FROM partsupp
WHERE ps_availqty > 3

+++++++++++++++++++
RULE FOR SUM

SUM IS REQUIRED ONLY WHEN THERE IS SUM BUT THERE IS NO GROUP BY
if (_finalFunction != 0 && _groupingAtts == 0)
+++++++++++++++++++

Schema _schemaOut --> create a schema with a single attribute

Schema(StringVector& _attributes, StringVector& _attributeTypes, IntVector& _distincts);
_attributes = {"agg"}
_attributeTypes = {"FLOAT"} // update the Function interface to get the type of the aggregate
_distincts = {1}

FuncOperator* _finalFunction --> Function _compute
Function compute;
compute.GrowFromParseTree (_finalFunction, _schemaIn);


GROUP BY
--------
GroupBy(Schema& _schemaIn, Schema& _schemaOut, OrderMaker& _groupingAtts,
        Function& _compute, RelationalOp* _producer);

SELECT SUM(ps_supplycost), s_suppkey
FROM part, supplier, partsupp
WHERE p_partkey = ps_partkey AND s_suppkey = ps_suppkey
GROUP BY s_suppkey

+++++++++++++++++++
RULE FOR GROUP BY

GROUP BY IS REQUIRED WHEN THERE IS GROUP BY
if (_groupingAtts != 0)
+++++++++++++++++++

Function& _compute
FuncOperator* _finalFunction --> Function _compute
Function compute;
compute.GrowFromParseTree (_finalFunction, _schemaIn);

Schema& _schemaOut
Schema(StringVector& _attributes, StringVector& _attributeTypes, IntVector& _distincts);
_attributes = {"agg"}
_attributeTypes = {"FLOAT"} // update the Function interface to get the type of the aggregate
_distincts = {1}

_groupingAtts = _attsToSelect = {s_suppkey}
_schemaIn = {part \union supplier \union partsupp} // 21 attributes

--> these data structures are initialized in QueryCompiler.Compile()
_numAttsInput = _schemaIn.GetNumAtts() // 21
_numAttsOutput = _attsToSelect.Length() // 1
_keepMe --> index of atts in _attsToSelect in _schemaIn // {9}
        --> _schemaIn.Index(_attsToSelect.Current)

{int _numAttsOutput, int* _keepMe} --> IntVector projVec
_schemaGroup = _schemaIn
_schemaGroup.Project(projVec) // {s_suppkey}

_schemaOut.Append(_schemaGroup) // {"agg", s_suppkey}

OrderMaker& _groupingAtts
OrderMaker _groupingAtts(_schemaIn, _keepMe, _numAttsOutput);


DISTINCT
--------
DuplicateRemoval(Schema& _schema, RelationalOp* _producer);

+++++++++++++++++++
RULE FOR DISTINCT

DISTINCT IS REQUIRED WHEN THERE IS DISTINCT
if (_distinctAtts != 0)

DISTINCT ALWAYS HAS A PROJECT AS ITS PRODUCER
IF THERE IS NO PROJECT, THERE IS NO DISTINCT
+++++++++++++++++++


WRITE OUT
---------
WriteOut(Schema& _schema, string& _outFile, RelationalOp* _producer);

--> the root of all the query execution trees is a WriteOut operator

_outFile --> path to text file where query results are printed


JOIN
----
Join(Schema& _schemaLeft, Schema& _schemaRight, Schema& _schemaOut,
        CNF& _predicate, RelationalOp* _left, RelationalOp* _right);

--> if we have N tables in the query, we have to create (N-1) joins

+++++++++++++++++++
 - if N = 1 --> no join is needed
SELECT l_orderkey, l_partkey, l_suppkey
FROM lineitem
WHERE l_returnflag = 'R' AND l_discount < 0.04 AND l_shipmode = 'MAIL'

SCAN(lineitem) / schema = {lineitem}
SELECT(l_returnflag = 'R' AND l_discount < 0.04 AND l_shipmode = 'MAIL') / schema = {lineitem}
PROJECT(l_orderkey, l_partkey, l_suppkey) / schemaIn = {lineitem}; schemaOut = {l_orderkey, l_partkey, l_suppkey}
WRITEOUT() / schema = {l_orderkey, l_partkey, l_suppkey}

SELECT DISTINCT c_name, c_address, c_acctbal 
FROM customer 
WHERE c_name = 'Customer#000070919'

SCAN(customer) / schema = {customer}
SELECT(c_name = 'Customer#000070919') / schema = {customer}
PROJECT(c_name, c_address, c_acctbal) / schemaIn = {customer}; schemaOut = {c_name, c_address, c_acctbal}
DUPLICATEREMOVAL() / schema = {c_name, c_address, c_acctbal}
WRITEOUT() / schema = {c_name, c_address, c_acctbal}


+++++++++++++++++++
 - if N = 2 --> only one join is needed
SELECT SUM(c_acctbal), c_name 
FROM customer, orders 
WHERE c_custkey = o_custkey AND o_totalprice < 10000.00
GROUP BY c_name

SCAN(customer) / schema = {customer}-----------------------
                                                          | -- JOIN(c_custkey = o_custkey) / schemaOut = {customer \UNION orders}
SCAN(orders) / schema = {orders}                          |
SELECT(o_totalprice < 10000.00) / schema = {orders}--------

JOIN
----
CNF predicate;
predicate.ExtractCNF (AndList& parseTree, Schema& leftSchema = {customer}, Schema& rightSchema = {orders});
--> check this returns value > 0 and only then create a JOIN

GROUPBY(SUM(c_acctbal), {c_name}) / schemaIn = {customer \UNION orders}; schemaOut = {aggregate, c_name}
WRITEOUT() / schema = {aggregate, c_name}

RelationalOp** forest = new RelationalOp*[nTbl];
Schema* forestSchema = new Schema[nTbl];

joinOp = new NestedLoopJoin(forestSchema[0], forestSchema[1], schemaOut, predicate, forest[0], forest[1])
nTbl = 1
forest[0] = joinOp
forestSchema[0] = schemaOut

groupOp = new GroupBy(..., forestSchema[0], forest[0])
forest[0] = groupOp
forestSchema[0] = schemaOut

writeOp = new WriteOut(..., forestSchema[0], forest[0])
forest[0] = writeOp


+++++++++++++++++++
 - if N > 2 --> create N-1 joins

SELECT SUM(ps_supplycost), s_suppkey
FROM part, supplier, partsupp
WHERE p_partkey = ps_partkey AND s_suppkey = ps_suppkey
GROUP BY s_suppkey

SCAN(part)
SCAN(supplier)
SCAN(partsupp)
+++++++++++++++++++++++++++
JOINS
--> create 2 JOINS
+++++++++++++++++++++++++++
GROUPBY(SUM(ps_supplycost), {s_suppkey})
WRITEOUT()

- create one join at a time

- for the first table in the forest, find a table it joins with and create a join operator
SCAN(part)

CNF predicate;
predicate.ExtractCNF (AndList& parseTree, forestSchema[0], forestSchema[i]) / i = {1, nTbl-1}
--> check this returns value > 0 and only then create a JOIN
--> there is a join predicate between forest[0] and forest[i]

i = 1
- it does not work for SCAN(supplier)

i = 2
- there is a join condition with SCAN(partsupp)
- we create a join between SCAN(part) and SCAN(partsupp)
joinOp = new NestedLoopJoin(forestSchema[0], forestSchema[i], schemaOut, predicate, forest[0], forest[i])
nTbl = nTbl-1
forest[0] = joinOp
forestSchema[0] = schemaOut

- translate forest and forestSchema to the left starting at index i
for j = i to nTbl-1
        forest[j] = forest[j+1]
        forestSchema[j] = forestSchema[j+1]

- for the first operator in the forest, find an operator it joins with and create a join operator
JOIN(part, partsupp)

i = 1
- there is a join condition with SCAN(supplier)

while (nTbl > 1) {

        for i = 1 to nTbl-1 {
                try join between forest[0] and forest[i]
                break
        }
        
        create join between forest[0] and forest[i]
        translate forest and forestSchema to the left starting at index i

        nTbl = nTbl - 1
}
