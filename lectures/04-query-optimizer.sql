SELECT SUM(l_extendedprice * l_discount), n_name
FROM customer, orders, lineitem, nation
WHERE c_nationkey = n_nationkey AND o_orderkey = l_orderkey AND c_custkey = o_custkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03
GROUP BY n_name


nation -----------------------------------
										  | -- join(c_nationkey = n_nationkey) ---------------------
customer -- selection(c_acctbal < 1000) --															|
																									| -- join(c_custkey = o_custkey) -- groupby(n_name;SUM(l_extendedprice * l_discount))
orders --------------------------------------------------------										|
															   | -- join(o_orderkey = l_orderkey) --
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) --


selection(30 = l_quantity) -- selection(l_discount < 0.03)
selection(l_discount < 0.03) -- selection(30 = l_quantity)

schema of GROUP BY [n_name, SUM(l_extendedprice * l_discount)]
 (agg, type : Function --> Float) : synthesized attribute [SUM(l_extendedprice * l_discount)]
 n_name : from input schema

SELECT SUM(l_extendedprice * l_discount)
FROM customer, orders, lineitem, nation
WHERE c_nationkey = n_nationkey AND o_orderkey = l_orderkey AND c_custkey = o_custkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03

schema of SUM [SUM(l_extendedprice * l_discount)]
 (agg, type : Function --> Float) : synthesized attribute [SUM(l_extendedprice * l_discount)]


nation -----------------------------------
										  | -- join(c_nationkey = n_nationkey) ---------------------
customer -- selection(c_acctbal < 1000) --															|
																									| -- join(c_custkey = o_custkey) -- SUM(l_extendedprice * l_discount)
orders --------------------------------------------------------										|
															   | -- join(o_orderkey = l_orderkey) --
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) --



SELECT l_extendedprice, l_discount
FROM customer, orders, lineitem, nation
WHERE c_nationkey = n_nationkey AND o_orderkey = l_orderkey AND c_custkey = o_custkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03

schema of Project [l_extendedprice, l_discount]
 l_extendedprice
 l_discount


nation -----------------------------------
										  | -- join(c_nationkey = n_nationkey) ---------------------
customer -- selection(c_acctbal < 1000) --															|
																									| -- join(c_custkey = o_custkey) -- projection(l_extendedprice, l_discount)
orders --------------------------------------------------------										|
															   | -- join(o_orderkey = l_orderkey) --
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) --



SELECT DISTINCT l_discount
FROM customer, orders, lineitem, nation
WHERE c_nationkey = n_nationkey AND o_orderkey = l_orderkey AND c_custkey = o_custkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03

schema of Project [l_discount]
 l_discount

schema of DuplicateRemoval(DISTINCT) [l_discount]
 l_discount


nation -----------------------------------
										  | -- join(c_nationkey = n_nationkey) ---------------------
customer -- selection(c_acctbal < 1000) --															|
																									| -- join(c_custkey = o_custkey) -- distinct(l_discount)
orders --------------------------------------------------------										|
															   | -- join(o_orderkey = l_orderkey) --
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) --



SELECT DISTINCT l_discount
FROM customer, orders, lineitem, nation
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND c_nationkey = n_nationkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03


nation -----------------------------------
										  | -- join(c_nationkey = n_nationkey) ---------------------
customer -- selection(c_acctbal < 1000) --															|
																									| -- join(c_custkey = o_custkey) -- distinct(l_discount)
orders --------------------------------------------------------										|
															   | -- join(o_orderkey = l_orderkey) --
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) --


SELECT DISTINCT l_discount
FROM nation, customer, orders, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND c_nationkey = n_nationkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03

nation -----------------------------------
										  |-- join -----------------------  (25*1500)
customer -- selection(c_acctbal < 1000) --								  |							
																		  | -- join	--  (1500*15000)					
orders -------------------------------------------------------------------			  |
															   						  | -- join (15000*60175)
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) -------------------------

25*1500 + 1500*15000 + 15000*60175

nation -----------------------------------
										  |-- join -----------------------  (25*500)
customer -- selection(c_acctbal < 1000) --  (500)						  |							
																		  | -- join	--  (500*15000)					
orders -------------------------------------------------------------------			  |
															   						  | -- join (5000*400)
lineitem -- selection(30 = l_quantity AND l_discount < 0.03)  (400)-------------------


nation -----------------------------------
										  |-- join -----------------------  (25*1500)
customer -- selection(c_acctbal < 1000) --								  |							
																		  | -- join	 (1500*1)
orders --------------------------------------------------------			  |						
															   |-- join --  (15000*1)
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) --  (1)



SELECT DISTINCT l_discount
FROM customer, orders, lineitem, nation
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND c_nationkey = n_nationkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03

customer -- selection(c_acctbal < 1000) --
										  | -- join -----------  (1500*15000)
orders -----------------------------------					   |
															   | -- join --  (15000*60175)
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) --			   |
																		   | -- join  (60175*25)
nation --------------------------------------------------------------------

1500*15000 + 15000*60175 + 60175*25


SELECT DISTINCT l_discount
FROM customer, orders, nation, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND c_nationkey = n_nationkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03

customer -- selection(c_acctbal < 1000) --
										  | -- join -----------  (1500*15000)
orders -----------------------------------					   |
															   | -- join --  (15000*25)
nation --------------------------------------------------------			   |
																		   | -- join  (15000*60175)
lineitem -- selection(30 = l_quantity AND l_discount < 0.03) --------------

1500*15000 + 15000*25 + 60175*15000


1500*15000 +  25*1500 + 15000*60175
1500*15000 + 60175*25 + 15000*60175
1500*15000 + 15000*25 + 15000*60175




customer: 1500
lineitem: 60175
nation: 25
orders: 15000

lineitem: 30 = l_quantity --> 60175/50 =~ 1200
lineitem: l_discount < 0.03 --> 60175/3 =~ 20000
lineitem: (30 = l_quantity AND l_discount < 0.03) --> 60175/(50*3) =~ 400

lineitem: l_discount > 0.01 --> 60175/3 =~ 20000
lineitem: (30 = l_quantity AND l_discount < 0.03 AND l_discount > 0.01) --> 60175/(50*3*3) =~ 130

customer: c_acctbal < 1000 --> 1500/3 = 500


nation (25) -----------------------------------
										  
customer (1500) -- selection(c_acctbal < 1000) (500)						  							
																		  
orders (15000) -------------------------------------------------------------------			  
															   					 
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) -------

c_nationkey = n_nationkey AND o_orderkey = l_orderkey AND c_custkey = o_custkey


Greedy algorithm for left-deep trees

1. Identify the 2-table join with the smallest cardinality (number of joining tuples)

Join estimator: [tuples(T) * tuples(R)] / [max(distinct(T.join_att), distinct(R.join_att))]

customer -- nation (c_nationkey = n_nationkey) --> (500 * 25) / max(25, 25) = 500
orders -- lineitem (o_orderkey = l_orderkey) --> (15000 * 400) / max(15000, 15000) = 400
customer -- orders (c_custkey = o_custkey) --> (15000 * 500) / max(1000, 1500) = 5000

Choose: orders -- lineitem (o_orderkey = l_orderkey)


nation (25) -----------------------------------
										  
customer (1500) -- selection(c_acctbal < 1000) (500)						  							
																		  
orders (15000) -------------------------------------------------------------------			  
															   					 | -- join(o_orderkey = l_orderkey) (400)
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) -------

output schema of join(o_orderkey = l_orderkey)
  o_orderkey: 15000
  o_custkey: 1000
  o_orderstatus: 3
  o_totalprice: 14996
  o_orderdate: 2401
  o_orderpriority: 5
  o_clerk: 1000
  o_shippriority: 1
  o_comment: 15000
  l_orderkey: 15000
  l_partkey: 2000
  l_suppkey: 100
  l_linenumber: 7
  l_quantity: 50
  l_extendedprice: 35921
  l_discount: 11
  l_tax: 9
  l_returnflag: 3
  l_linestatus: 2
  l_shipdate: 2518
  l_commitdate: 2460
  l_receiptdate: 2529
  l_shipinstruct: 4
  l_shipmode: 7
  l_comment: 60175


2. Iteratively add a new table to the existing join until all the tables are added

2.1
Current join tree: orders -- lineitem
c_nationkey = n_nationkey AND [o_orderkey = l_orderkey] AND c_custkey = o_custkey

Consider tables not part of the join tree that join with at least one table from the join tree

nation is not considered because it does not join with orders or lineitem

Choose: customer

nation (25) -----------------------------------
										  
customer (1500) -- selection(c_acctbal < 1000) (500) -----------------------------------------------------------------					  							
																		   											 | -- join(c_custkey = o_custkey)
orders (15000) -------------------------------------------------------------------									 |		   
															   					 | -- join(o_orderkey = l_orderkey) --
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) -------


2.2
Current join tree: orders -- lineitem -- customer
c_nationkey = n_nationkey AND [o_orderkey = l_orderkey] AND [c_custkey = o_custkey]

Choose: nation

nation (25) --------------------------------------------------------------------------------------------------------------------------------------------
										  																											   | -- join(c_nationkey = n_nationkey)
customer (1500) -- selection(c_acctbal < 1000) (500) -----------------------------------------------------------------					  			   |			
																		   											 | -- join(c_custkey = o_custkey) --
orders (15000) -------------------------------------------------------------------									 |		   
															   					 | -- join(o_orderkey = l_orderkey) --
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) -------




Example 2
SELECT DISTINCT c_name 
FROM lineitem, orders, customer, nation, region
WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND 
	c_nationkey = n_nationkey AND n_regionkey = r_regionkey


lineitem (60175) -- Project[l_orderkey]

orders (15000) -- Project[o_orderkey, o_custkey]

customer (1500) -- Project[c_custkey, c_nationkey, c_name]

nation (25) -- Project[n_nationkey, n_regionkey]

region (5) -- Project[r_regionkey]

l_orderkey = o_orderkey AND o_custkey = c_custkey AND c_nationkey = n_nationkey AND n_regionkey = r_regionkey


1. Identify the 2-table join with the smallest cardinality (number of joining tuples)

Join estimator: [tuples(T) * tuples(R)] / [max(distinct(T.join_att), distinct(R.join_att))]

customer -- nation (c_nationkey = n_nationkey) --> (1500 * 25) / max(25, 25) = 1500
orders -- lineitem (o_orderkey = l_orderkey) --> (15000 * 60175) / max(15000, 15000) = 60175
customer -- orders (c_custkey = o_custkey) --> (15000 * 1500) / max(1000, 1500) = 15000
region -- nation (n_regionkey = r_regionkey) --> (5 * 25) / max(5, 5) = 25

Choose: region -- nation (n_regionkey = r_regionkey)

lineitem (60175)

orders (15000)

customer (1500)

nation (25) --
			 | -- join(n_regionkey = r_regionkey) -- (n_nationkey, n_regionkey, r_regionkey) Project[n_nationkey] (25)
region (5) ---


2. Iteratively add a new table to the existing join until all the tables are added

2.1
Current join tree: nation -- region
l_orderkey = o_orderkey AND o_custkey = c_custkey AND c_nationkey = n_nationkey AND [n_regionkey = r_regionkey]

Consider tables not part of the join tree that join with at least one table from the join tree

lineitem and orders are not considered because they do not join with nation or region

Choose: customer

lineitem (60175)

orders (15000)

customer (1500) ------------------------------------
												   | -- join(c_nationkey = n_nationkey)
nation (25) --									   |
			 | -- join(n_regionkey = r_regionkey) --
region (5) ---


2.2
Current join tree: nation -- region -- customer
l_orderkey = o_orderkey AND o_custkey = c_custkey AND [c_nationkey = n_nationkey] AND [n_regionkey = r_regionkey]

Consider tables not part of the join tree that join with at least one table from the join tree

lineitem is not considered because it does not join with nation, region, or customer

Choose: orders

lineitem (60175)

orders (15000) ---------------------------------------------------------------------------
																						  |
customer (1500) ------------------------------------									  |
												   | -- join(c_nationkey = n_nationkey) --
nation (25) --									   |
			 | -- join(n_regionkey = r_regionkey) --
region (5) ---



Example 3
SELECT DISTINCT l_discount
FROM customer, orders, lineitem, nation, partsupp, part, supplier
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND c_nationkey = n_nationkey AND
	l_partkey = ps_partkey AND l_suppkey = ps_suppkey AND
	ps_partkey = p_partkey AND ps_suppkey = s_suppkey AND
	c_acctbal < 1000 AND 30 = l_quantity AND l_discount < 0.03


nation (25) -----------------------------------
										  
customer (1500) -- selection(c_acctbal < 1000) (500)						  							
																		  
orders (15000) -------------------------------------------------------------------			  
															   					 
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) -------

partsupp (8000)

part (2000)

supplier (100)

c_nationkey = n_nationkey AND o_orderkey = l_orderkey AND c_custkey = o_custkey AND
l_partkey = ps_partkey AND l_suppkey = ps_suppkey AND
ps_partkey = p_partkey AND ps_suppkey = s_suppkey


Greedy algorithm for left-deep trees

1. Identify the 2-table join with the smallest cardinality (number of joining tuples)

Join estimator: [tuples(T) * tuples(R)] / [max(distinct(T.join_att), distinct(R.join_att))]

customer -- nation (c_nationkey = n_nationkey) --> (500 * 25) / max(25, 25) = 500
orders -- lineitem (o_orderkey = l_orderkey) --> (15000 * 400) / max(15000, 15000) = 400
customer -- orders (c_custkey = o_custkey) --> (15000 * 500) / max(1000, 1500) = 5000
lineitem -- partsupp (l_partkey = ps_partkey AND l_suppkey = ps_suppkey) --> (400 * 8000) / (max(2000, 2000) * max(100, 100)) = 16
partsupp -- part (ps_partkey = p_partkey) --> (8000 * 2000) / max(2000, 2000) = 8000
partsupp -- supplier (ps_suppkey = s_suppkey) --> (8000 * 100) / max(100, 100) = 8000

Choose: lineitem -- partsupp (l_partkey = ps_partkey AND l_suppkey = ps_suppkey)


nation (25) -----------------------------------
										  
customer (1500) -- selection(c_acctbal < 1000) (500)						  							
																		  
orders (15000) -------------------------------------------------------------------			  
															   					 
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) --
																			| -- join (l_partkey = ps_partkey AND l_suppkey = ps_suppkey) (16)
partsupp (8000) -------------------------------------------------------------

part (2000)

supplier (100)


2. Iteratively add a new table to the existing join until all the tables are added

2.1
Current join tree: lineitem -- partsupp
c_nationkey = n_nationkey AND o_orderkey = l_orderkey AND c_custkey = o_custkey AND
ps_partkey = p_partkey AND ps_suppkey = s_suppkey

Consider tables not part of the join tree that join with at least one table from the join tree

customer and nation are not considered because they do not join with lineitem or partsupp


(lineitem -- partsupp) -- orders (o_orderkey = l_orderkey) --> (16 * 15000) / max(15000, 15000) = 16
(lineitem -- partsupp) -- part (ps_partkey = p_partkey) --> (16 * 2000) / max(2000, 2000) = 16
(lineitem -- partsupp) -- supplier (ps_suppkey = s_suppkey) --> (16 * 100) / max(100, 100) = 16

Choose: orders

nation (25) -----------------------------------
										  
customer (1500) -- selection(c_acctbal < 1000) (500)						  							
																		  
orders (15000) ----------------------------------------------------------------------------------------------------------------------------------
															   					 																| -- join (o_orderkey = l_orderkey) (16)
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) --																	|
																			| -- join (l_partkey = ps_partkey AND l_suppkey = ps_suppkey) (16) --
partsupp (8000) -------------------------------------------------------------

part (2000)

supplier (100)


2.2
Current join tree: lineitem -- partsupp -- orders
c_nationkey = n_nationkey AND c_custkey = o_custkey AND
ps_partkey = p_partkey AND ps_suppkey = s_suppkey

Consider tables not part of the join tree that join with at least one table from the join tree

nation are not considered because they do not join with lineitem, partsupp, or customer


((lineitem -- partsupp) -- orders) -- part (ps_partkey = p_partkey) --> (16 * 2000) / max(2000, 2000) = 16
((lineitem -- partsupp) -- orders) -- supplier (ps_suppkey = s_suppkey) --> (16 * 100) / max(100, 100) = 16
((lineitem -- partsupp) -- orders) -- customer (c_custkey = o_custkey) --> (16 * 500) / max(1500, 1000) = 5.33

Choose: customer

nation (25) -----------------------------------
										  
customer (1500) -- selection(c_acctbal < 1000) (500) --------------------------------------------------------------------------------------------------------------------------------------						  							
																		  																												  | -- join (c_custkey = o_custkey) (6)
orders (15000) ----------------------------------------------------------------------------------------------------------------------------------										  |
															   					 																| -- join (o_orderkey = l_orderkey) (16) --
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) --																	|
																			| -- join (l_partkey = ps_partkey AND l_suppkey = ps_suppkey) (16) --
partsupp (8000) -------------------------------------------------------------

part (2000)

supplier (100)


2.3
Current join tree: lineitem -- partsupp -- orders -- customer
c_nationkey = n_nationkey AND
ps_partkey = p_partkey AND ps_suppkey = s_suppkey

Consider tables not part of the join tree that join with at least one table from the join tree

(((lineitem -- partsupp) -- orders) -- customer) -- part (ps_partkey = p_partkey) --> (6 * 2000) / max(2000, 2000) = 6
(((lineitem -- partsupp) -- orders) -- customer) -- supplier (ps_suppkey = s_suppkey) --> (6 * 100) / max(100, 100) = 6
(((lineitem -- partsupp) -- orders) -- customer) -- nation (c_nationkey = n_nationkey) --> (6 * 25) / max(25, 25) = 6

Choose: nation

nation (25) ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
										  																																														 |
customer (1500) -- selection(c_acctbal < 1000) (500) --------------------------------------------------------------------------------------------------------------------------------------						  				 | -- join (c_nationkey = n_nationkey) (6)			
																		  																												  | -- join (c_custkey = o_custkey) (6) --
orders (15000) ----------------------------------------------------------------------------------------------------------------------------------										  |
															   					 																| -- join (o_orderkey = l_orderkey) (16) --
lineitem (60175) -- selection(30 = l_quantity AND l_discount < 0.03) (400) --																	|
																			| -- join (l_partkey = ps_partkey AND l_suppkey = ps_suppkey) (16) --
partsupp (8000) -------------------------------------------------------------

part (2000)

supplier (100)



SELECT DISTINCT c_name 
FROM lineitem, orders, customer, nation, region
WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND 
	c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND r_name = 'EUROPE' AND o_orderstatus = 'O' AND
	o_orderpriority = '1-URGENT' AND l_shipdate > '1995-10-10' AND l_shipdate < '1995-10-15'

insert into attributes values('lineitem', 0, 'l_orderkey', 'INTEGER', 15000);
insert into attributes values('lineitem', 10, 'l_shipdate', 'STRING', 2518);

insert into attributes values('orders', 0, 'o_orderkey', 'INTEGER', 15000);
insert into attributes values('orders', 1, 'o_custkey', 'INTEGER', 1000);
insert into attributes values('orders', 2, 'o_orderstatus', 'STRING', 3);
insert into attributes values('orders', 5, 'o_orderpriority', 'STRING', 5);

insert into attributes values('customer', 0, 'c_custkey', 'INTEGER', 1500);
insert into attributes values('customer', 3, 'c_nationkey', 'INTEGER', 25);

insert into attributes values('nation', 0, 'n_nationkey', 'INTEGER', 25);
insert into attributes values('nation', 2, 'n_regionkey', 'INTEGER', 5);

insert into attributes values('region', 0, 'r_regionkey', 'INTEGER', 5);
insert into attributes values('region', 1, 'r_name', 'STRING', 5);


lineitem[60175] -- select(l_shipdate > '1995-10-10' AND l_shipdate < '1995-10-15')[60175 / (3 * 3) = 6687]

orders[15000] -- select(o_orderstatus = 'O' AND o_orderpriority = '1-URGENT')[15000 / (3 * 5) = 1000]

customer[1500]

nation[25]

region[5] -- select(r_name = 'EUROPE')[5 / 5 = 1]


MapD(GPU database)
- sort tables in decreasing order of their size and build joins in that order
- left-deep tree

lineitem -------
			   | -- join(l_orderkey = o_orderkey) ------
orders ---------									   |
													   | -- join(o_custkey = c_custkey) ------
													   |									 |
customer -----------------------------------------------									 | -- join(c_nationkey = n_nationkey) -----
																							 |										  |
nation ---------------------------------------------------------------------------------------										  |
																																	  |
region --------------------------------------------------------------------------------------------------------------------------------


Greedy left-deep tree

region -- nation -- customer -- orders -- lineitem [chain query]

Step 0
------
Joins considered
customer -- nation (c_nationkey = n_nationkey) --> (1500 * 25) / max(25, 25) = 1500
orders -- lineitem (o_orderkey = l_orderkey) --> (1000 * 6687) / max(15000, 15000) = 446
customer -- orders (c_custkey = o_custkey) --> (1500 * 1000) / max(1500, 1000) = 1000
nation -- region (n_regionkey = r_regionkey) --> (25 * 1) / max(5, 5) = 5

Choose: nation join region on (n_regionkey = r_regionkey)


Step 1
------
Joins considered
(nation -- region) -- customer

Choose: (nation join region on (n_regionkey = r_regionkey)) join customer on (c_nationkey = n_nationkey)


Step 2
------
Joins considered
((nation -- region) -- customer) -- orders

Choose: (nation join region on (n_regionkey = r_regionkey)) join customer on (c_nationkey = n_nationkey)
			join orders on (c_custkey = o_custkey)


Step 3
------
Joins considered
(((nation -- region) -- customer) -- orders) -- lineitem

Choose: (nation join region on (n_regionkey = r_regionkey)) join customer on (c_nationkey = n_nationkey)
			join orders on (c_custkey = o_custkey) join lineitem on (o_orderkey = l_orderkey)


Greedy bushy tree

((nation join region on (n_regionkey = r_regionkey)) join
(customer join orders on (c_custkey = o_custkey)) on (c_nationkey = n_nationkey)) join
lineitem on (o_orderkey = l_orderkey)


region
		-- join -----
nation
					-- join ---
customer 
		-- join ----			-- join
orders 
		-----------------------
lineitem

Step 0
------
Joins considered
customer -- nation (c_nationkey = n_nationkey) --> (1500 * 25) / max(25, 25) = 1500
orders -- lineitem (o_orderkey = l_orderkey) --> (1000 * 6687) / max(15000, 15000) = 446
customer -- orders (c_custkey = o_custkey) --> (1500 * 1000) / max(1500, 1000) = 1000
nation -- region (n_regionkey = r_regionkey) --> (25 * 1) / max(5, 5) = 5

Choose: nation join region on (n_regionkey = r_regionkey)

Step 1
------
Joins considered
(nation -- region) -- customer (c_nationkey = n_nationkey) --> (5 * 1500) / max(25, 25) = 300
orders -- lineitem (o_orderkey = l_orderkey) --> (1000 * 6687) / max(15000, 15000) = 446
customer -- orders (c_custkey = o_custkey) --> (1500 * 1000) / max(1500, 1000) = 1000

Choose: (nation join region on (n_regionkey = r_regionkey)) join customer on (c_nationkey = n_nationkey)

Step 2
------
Joins considered
((nation -- region) -- customer) -- orders (c_custkey = o_custkey) --> (300 * 1500) / max(1500, 1000) = 300
orders -- lineitem (o_orderkey = l_orderkey) --> (1000 * 6687) / max(15000, 15000) = 446

Choose: (nation join region on (n_regionkey = r_regionkey)) join customer on (c_nationkey = n_nationkey)
			join orders on (c_custkey = o_custkey)

Step 3
------
Joins considered
(((nation -- region) -- customer) -- orders) -- lineitem (o_orderkey = l_orderkey)

Choose: (nation join region on (n_regionkey = r_regionkey)) join customer on (c_nationkey = n_nationkey)
			join orders on (c_custkey = o_custkey) join lineitem on (o_orderkey = l_orderkey)

