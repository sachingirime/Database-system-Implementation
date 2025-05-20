B+-tree indexes
---------------

Heap scan
- linear scan over all the tuples

Indexed scan
- table + predicate (condition) on table

SELECT DISTINCT o_orderdate
FROM orders
WHERE o_orderdate>'1994-08-01' AND o_orderdate<'1997-08-01' --> non-selective predicate
WHERE o_orderdate>'1994-08-01' AND o_orderdate<'1994-08-02' --> selective predicate

Scan(orders) -- Select(o_orderdate>'1994-08-01' AND o_orderdate<'1995-08-01') -- Project(o_orderdate) -- DuplicateRemoval

- combine Scan and Select into a single operator that returns only the tuples satisfying the predicate
- make sure that we access only the tuples that satisfy the predicate

- indexes require a tradeoff in space --> have another file with all or a part of the data such that predicates can be evaluated

SELECT DISTINCT o_orderkey
FROM orders
WHERE o_orderkey>1000 AND o_orderkey<2000

- do not access every single page --> call Page.GetPage() fewer times than for every page in the file

- binary search on pages
- vector[Page], leftBound = 0, rightBound = numPages in file
- center = leftBound + abs[(rightBound - leftBound) / 2]

    GetPage(page, center)
    Apply predicate to page.firstRecord and page.lastRecord
    Update the center based on return values for the predicate evaluation
    When page that satisfies predicate is found evaluate predicate on records
    Check neighbor pages


SELECT DISTINCT o_orderdate
FROM orders
WHERE o_orderdate>'1994-08-01' AND o_orderdate<'1995-08-01'

- create another copy of the file sorted on o_orderdate --> double the size
- query compiler/executor selects the best file to use for every query

- original B+-tree

         2
         3
         5
    7
         7
        11
13 


        13
        17
        19
    23
        23
        29

    31
        31
        37
        41

    43
        43
        47


- insert 28

         2
         3
         5
    7
         7
        11
13 


        13
        17
        19
    23
        23
        28
        29

    31
        31
        37
        41

    43
        43
        47

- insert 9


         2
         3
         5
    (unchanged)
    7
         7
         9
        11
13 
        (unchanged)
        13
        17
        19

    (unchanged)
    23
        (unchanged)
        23
        25

28
        (set)
        28
        29

    (set)
    31
        (unchanged)
        31
        37
        41
    43
        (unchanged)
        43
        47

- insert 25

- 4 keys in a leaf with only 3 places
23
25
28
29

- split in the middle
23
25

28
29

- insert 28 in the parent node (intermediate)
23
28
31
43

- split in the middle
23
28

31
43

- insert 28 in the parent node (root)
13
28


         2
         3
         5
    7
         7
         9
        11
13 

        13
        17
        19
    23
        23
        25

    28
        28
        29  

31    
        31
        37
        41
    43
        43
        47



         2
         3
         5
    7
         7
         9
        11
13 

        13
        15

    17

        17
        19
    23
        23
        25

28

        28
        29
    31
        31
        37
        41

    43
        43
        47

- insert 15

         2
         3
         5
    7
         7
         8

    9
         9
        11
13 

        13
        15
    17

        17
        19
    23
        23
        25

28

        28
        29
    31
        31
        37
        41

    43
        43
        47

- insert 8

         2
         3
         5
    7
         7
         8

    9
         9
        11
13 

        13
        15
    17

        17
        19
    23
        23
        25

28

        28
        29
    31
        31
        36

    37
        37
        41

    43
        43
        47

- insert 36

         1
         2

    3     
         3
         5
    7
         7
         8

    9
         9
        11
13 

        13
        15
    17

        17
        19
    23
        23
        25

28

        28
        29
    31
        31
        36

    37
        37
        41

    43
        43
        47

- insert 1

         1
         2

    3    
         3
         5
    7
         7
         8

    9
         9
        10
        11
13 

        13
        15
    17

        17
        19
    23
        23
        25

28

        28
        29
    31
        31
        36

    37
        37
        41

    43
        43
        47

- insert 10

         1
         2

    3    
         3
         5
    7
         7
         8

    9
         9
        10
        11
13 

        13
        15
    17

        17
        19
    23
        23
        25

28

        28
        29
        30
    31
        31
        36

    37
        37
        41

    43
        43
        47

- insert 30

         1
         2

    3    
         3
         5
7


         7
         8
    9
         9
        10

    11
        11
        12
13 

        13
        15
    17

        17
        19
    23
        23
        25

28

        28
        29
        30
    31
        31
        36

    37
        37
        41

    43
        43
        47

- insert 12

         1
         2
    (set)
    3    
         3
         5
    7
         7
         8

9
        (set)
         9
        10

    11
        (set)
        11
        12
13 

        13
        15
    17

        17
        19
    23
        23
        25

28

        28
        29
        30
    31
        31
        36

    37
        37
        41

    43
        43
        47



         1
         2

     3    
         3
         5
     7
         7
         8

9    
         9
        10
    11    
        11
        12

13 

        13
        15
    17

        17
        19
    23
        23
        25

28

        28
        29
        30
    31
        31
        36

    37
        37
        41

    43
        43
        47


- build a B+-tree starting from an empty tree

- insert 36
36

- insert 12
12
36

- insert 47
12
36
47

- insert 8

     8
    12
36
    36
    47


     8
    12

36

    36
    47

- insert 10
     
     8
    10
    12

36

    36
    47

- insert 37
     
     8
    10
    12

36

    36
    37
    47

     8
    10
    12
36
    36
    37
40    
    40
    47


- insert 40
     
     8
    10
    12

36

    36
    37

40
    40
    47


     1
     8
10     
    10
    12
36
    36
    37
40
    40
    47


- insert 1
     
     1
     8

10
    10
    12

36

    36
    37

40
    40
    47

- insert 15
     
     1
     8

10
    10
    12
    15

36

    36
    37

40
    40
    47


        1
        8

    10

        10
        12

    15    


        15
        20

    36

        36
        37

    40
        40
        47


        1
        8

    10

        10
        12

    15    
        (set)
        15
        20

36    

        36
        37

    40
        40
        47


        1
        8

    10

        10
        12

15    

        (set)
        15
        20

    36

        36
        37

    40
        40
        47


- insert 20
     
        1
        8

    10
        10
        12

    15
        15
        20

36
    
        36
        37
    40
        40
        47

- insert 17
     
         1
         8

    10
        10
        12

    15
        15
        17
        20

36
    
        36
        37

    40
        40
        47

- insert 5
     
         1
         5
         8

    10
        10
        12

    15
        15
        17
        20

36
    
        36
        37

    40
        40
        47

- insert 32
     
         1
         5
         8

    10
        10
        12

    15
        15
        17

    20    
        20
        32

36
    
        36
        37

    40
        40
        47


         1
         5
    8
         8
         9

10
        (set)
        10
        12

    15
        15
        17

    20    
        20
        32

36
    
        36
        37

    40
        40
        47


- insert 9
     
         1
         5

     8    
         8
         9

    10
        10
        12

15  
        15
        17
    20    
        20
        32

36
    
        36
        37

    40
        40
        47

- insert 3
     
         1
         3
         5

     8    
         8
         9

    10
        10
        12

15  
        15
        17
    20    
        20
        32

36
    
        36
        37

    40
        40
        47

- insert 55
     
         1
         3
     5
         5
         7

     8    
         8
         9

    10
        10
        12

15  
        15
        17
    20    
        20
        32

36
    
        36
        37

    40
        40
        47
        55

- insert 7
     
         1
         3

     5
         5
         7

     8    
         8
         9

    10
        10
        12

15  
        15
        17
    20    
        20
        32

36
    
        36
        37

    40
        40
        47
        55

- insert 2
     
         1
         2
     3
         3
         4

     5
         5
         7

8
         8
         9

    10
        10
        12

15  
        15
        17
    20    
        20
        32

36
    
        36
        37

    40
        40
        47
        55

- insert 4
     
         1
         2

     3
         3
         4

     5
         5
         7

8    
         8
         9

    10
        10
        12

15  
        15
        17
    20    
        20
        32

36
    
        36
        37

    40
        40
        47
        55


- find keys 10 <= k <= 30
     
         1
         2

     3
         3
         4

     5
         5
         7

8    
         8
         9

    10
        10
        12

15  
        15
        17
    20    
        20
        32

36
    
        36
        37

    40
        40
        47
        55


SELECT DISTINCT n_name
FROM nation, customer, orders
WHERE o_orderkey > 10 AND o_orderkey < 10000 AND o_custkey=c_custkey AND
	n_nationkey=c_nationkey

CREATE INDEX ind_o_orderkey TABLE orders ON o_orderkey

- save index into a file
--> every node in the tree goes to its separate page
--> page_size: 128 KB
--> build indexes only for INTEGER attributes (4 bytes)
--> pointer (or position) of two values: data-page-number in heap file, record-number in page (off_t : 8 bytes)
--> how many keys can we have on a page?
    --> (page_size - header) / (n * 4 + (n+1) * 16) >= 0
    --> find maximum n for which the inequality holds
    --> header is 8 KB
    --> 120 KB / (n * 20 B + 16 B) >= 0
    --> 120 KB - 16 B >= n * 20 B 
    --> n <= (120 KB - 16 B) / 20 B
    --> n = 6000 (number of keys in a node); 6001 pointers

- simplification: use only the page number as the pointer or address in an index node
--> this provides consistency between internal nodes and leaf nodes
--> you can still use n = 6000 as the number of keys in a node

IndexPage format
- page (node) type : internal or leaf
- page (node) number : the address or pointer of the page is used to identify the page
- last pointer address : the page number for the last pointer in the node (there are n keys and (n+1) pointers in a node)
- number of keys K : the number of keys can be smaller than n=6000
- K pairs (key, address = page number) of integers
- the pairs are sorted by the keys in increasing order
- the format has fixed size, so there is no need to store starting addresses for every (key, address) pair

class IndexPage {
	int pageType --> initially, the root is a leaf
    int pageId
	int lastPointer
	int numKeys
	int keys[numKeys]
	int pointers[numKeys]

    ToBinary()
    FromBinary()
}

- add methods to File to handle IndexPage
    --> int GetPage(IndexPage& putItHere, off_t whichPage)
    --> void AddPage(IndexPage& addMe, off_t whichPage);

- the root of the B+-tree is always stored as the first page in the File
- follow the pointers from there to identify the other pages (corresponding to the nodes)

class BTreeIndex {
    Map index : pageId --> IndexPage (SInt --> IndexPage) --> index[0] = root

    Write() --> save index to File
    Read() --> read index from File

    Build(indexName, tableName, attributeName)

    Find(key, &pageNumber)
}

BTreeIndex::Build(indexName, tableName, attributeName)

BTreeIndex::Find(key, &pageNumber)


class IndexScan {
    BTreeIndex index
    File dataFile

	CNF predicate
	Record constants

    TwoWayList<Record> buffer

	bool GetNext(Record& _record)
}

IndexScan::GetNext(Record& _record)


SELECT o_orderdate
FROM orders
WHERE o_orderkey = 1346

CREATE INDEX ind_o_orderkey TABLE orders ON o_orderkey

- page size = 131072 (128 KB)
- store a B+-tree in a page
- node in B+-tree has n keys and (n+1) pointers or addresses
- what is a pointer
    --> page number in data file (4 or 8 bytes)
    --> (page number, record position) in data file (8 or 16 bytes)

- space: n * 4 + (n+1) * 4 <= page size - page header (131072 - 16)
n <= (131072 - 20) / 8
n <= 16381

- index on o_orderkey will have a single node (root and leaf)


SELECT o_orderdate FROM orders WHERE o_orderkey > 1346 AND o_orderdate > '1995-01-01';

CREATE INDEX ind_o_orderkey TABLE orders ON o_orderkey;

SELECT o_orderdate FROM orders WHERE o_orderkey > 1346;
create index on customer.c_custkey
CREATE INDEX ind_c_custkey TABLE customer ON c_custkey;