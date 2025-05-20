SQL ::= SELECT SelectAtts FROM Tables WHERE AndList |
        SELECT SelectAtts FROM Tables WHERE AndList GROUP BY Atts |
        SELECT SelectAtts FROM Tables WHERE AndList ORDER BY Atts |
        SELECT SelectAtts FROM Tables WHERE AndList GROUP BY Atts ORDER BY Atts

Tables ::= YY_NAME | Tables ',' YY_NAME

SelectAtts ::= Function ',' Atts | Function | Atts | DISTINCT Atts 
Atts ::= YY_NAME | Atts ',' YY_NAME 

Function ::= SUM '(' CompoundExp ')'

++++++++++++++++++++++++++++++++++++++++++++++++++++
Function ::= SUM '(' CompoundExp ')' | COUNT '(' CompoundExp ')' | AVERAGE '(' CompoundExp ')'

Function ::= FuncType '(' CompoundExp ')'
FuncType ::= SUM | COUNT | AVERAGE | MIN | MAX 
++++++++++++++++++++++++++++++++++++++++++++++++++++

CompoundExp ::= SimpleExp Op CompoundExp | '(' CompoundExp ')' Op CompoundExp |
 		       '(' CompoundExp ')' | SimpleExp | '-' CompoundExp 

Op ::= '-' | '+' | '*' | '/' 
BoolComp ::= '<' | '>' | '=' 

Literal ::= YY_STRING | YY_FLOAT | YY_INTEGER | YY_NAME 
SimpleExp ::= YY_FLOAT | YY_INTEGER | YY_NAME

AndList ::= Condition | Condition AND AndList 
Condition ::= Literal BoolComp Literal 



TableList* tables = {}; // the list of tables in the query
AndList* predicate = {}; // the predicate in WHERE
NameList* attsToSelect = {}; // the attributes in SELECT


SELECT c_address, c_phone, c_acctbal FROM customer WHERE c_name = 'Customer#000000010'
SELECT SelectAtts                    FROM Tables   WHERE AndList

c_address, c_phone, c_acctbal
Atts
SelectAtts
Atts                 ','    YY_NAME
c_address, c_phone    ,     c_acctbal
attsToSelect = {c_acctbal}

c_address, c_phone
Atts
Atts                 ','    YY_NAME
c_address             ,     c_phone
attsToSelect = {c_phone, c_acctbal}

c_address
Atts
YY_NAME
attsToSelect = {c_address, c_phone, c_acctbal}

customer
Tables
YY_NAME
tables = {customer}

c_name = 'Customer#000000010'
AndList
Condition
Literal     BoolComp    Literal
c_name      =           'Customer#000000010'
predicate = {ComparisonOp {left, code, right}}

c_name
Literal
YY_NAME
left = {code = NAME, value = c_name}

=
BoolComp
=
code = EQUALS

'Customer#000000010'
Literal
YY_STRING
right = {code = STRING, value = 'Customer#000000010'}


++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
struct FuncOperator* finalFunction = {}; // the aggregate function
struct TableList* tables = {}; // the list of tables in the query
struct AndList* predicate = {}; // the predicate in WHERE
struct NameList* groupingAtts = {}; // grouping attributes
struct NameList* attsToSelect = {}; // the attributes in SELECT
int distinctAtts = 0; // 1 if there is a DISTINCT in a non-aggregate query 


SELECT SUM(l_extendedprice * l_discount * (1.0-l_tax)) FROM lineitem WHERE l_orderkey > -1
SELECT SelectAtts                                      FROM Tables   WHERE AndList
---------------------
groupingAtts = {}

SUM  ( l_extendedprice * l_discount * (1.0-l_tax) )
SelectAtts
Function
SUM '(' CompoundExp ')'
---------------------
attsToSelect = {}

l_extendedprice *       l_discount * (1.0-l_tax)
CompoundExp
SimpleExp       Op      CompoundExp
YY_NAME         '*'

l_discount      *       (1.0-l_tax)
CompoundExp
SimpleExp       Op      CompoundExp
YY_NAME         '*'

(       1.0-l_tax             )
CompoundExp
'('     CompoundExp         ')'

1.0             -               l_tax
CompoundExp
SimpleExp       Op              CompoundExp
YY_FLOAT        '-'             CompoundExp

l_tax
CompoundExp
SimpleExp
YY_NAME

lineitem
Tables
YY_NAME

l_orderkey      >               -1
AndList
Condition
Literal         BoolComp        Literal
YY_NAME         '>'             YY_INTEGER



l_extendedprice * l_discount * (1.0-l_tax)

        l_extendedprice
*
                l_discount
        *
                        1.0
                -
                        l_tax
* l_extendedprice * l_discount - 1.0 l_tax

l_extendedprice
l_discount
1.0
l_tax
-
+++++++++++++++++++++
1.0-l_tax --> r1

l_extendedprice
l_discount
r1
*
+++++++++++++++++++++
l_discount*r1 --> r2

l_extendedprice
r2
*
+++++++++++++++++++++
l_extendedprice*r2 --> result



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