SELECT SUM(l_extendedprice * l_discount * (1.0-l_tax))
FROM lineitem

SELECT SUM(ps_supplycost), s_suppkey
FROM part, supplier, partsupp 
WHERE p_partkey = ps_partkey AND s_suppkey = ps_suppkey AND s_acctbal < 1000.0 AND p_size < 5
GROUP BY s_suppkey

SELECT SUM(c_acctbal), c_name 
FROM customer, orders 
WHERE c_custkey = o_custkey AND o_totalprice < 100.0
GROUP BY c_name

SELECT l_receiptdate
FROM lineitem, orders, customer
WHERE	l_orderkey = o_orderkey AND o_custkey = c_custkey AND	c_name = 'Customer#000000116' AND
	o_orderdate>'1995-03-10' AND o_orderdate<'1995-03-20'

SELECT DISTINCT c_address
FROM customer, nation, region
WHERE c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND r_name = 'AFRICA'

SELECT SUM(l_discount) 
FROM customer, orders, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND	c_name = 'Customer#000070919'

SELECT DISTINCT n_name
FROM nation, customer, orders
WHERE o_orderdate>'1994-08-01' AND o_orderdate<='1994-08-11' AND o_custkey=c_custkey AND
	n_nationkey=c_nationkey

SELECT DISTINCT c_name 
FROM lineitem, orders, customer, nation, region
WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND 
	c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND r_name = 'EUROPE' AND o_orderstatus = 'P' AND
	o_orderpriority = '1-URGENT' AND l_shipdate > '1995-10-10' AND l_shipdate < '1995-10-15'

SELECT l_discount 
FROM lineitem, orders, customer, nation, region
WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND 
	c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND 
	r_name = 'ASIA' AND c_mktsegment = 'BUILDING' AND c_acctbal < 200.0 AND l_returnflag = 'N' AND l_linestatus = 'F'

SELECT SUM(l_discount) 
FROM customer, orders, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND
	c_name = 'Customer#000070919' AND l_quantity > 25 AND l_discount < 0.03

SELECT SUM(ps_supplycost), s_suppkey
FROM part, supplier, partsupp 
WHERE p_partkey = ps_partkey AND s_suppkey = ps_suppkey
GROUP BY s_suppkey

SELECT SUM(l_extendedprice * l_discount * (1.0-l_tax))
FROM lineitem
WHERE l_discount < 0.07 AND l_quantity < 12.0

SELECT SUM(c_acctbal), c_name 
FROM customer, orders 
WHERE c_custkey = o_custkey
GROUP BY c_name

SELECT l_receiptdate
FROM lineitem, orders, customer
WHERE	l_orderkey = o_orderkey AND o_custkey = c_custkey AND
	o_orderdate>'1995-03-01' AND o_orderdate<'1996-03-01'

SELECT DISTINCT c_address
FROM customer, nation, region
WHERE c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND r_name = 'AFRICA'

SELECT SUM(l_discount) 
FROM customer, orders, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey

SELECT DISTINCT n_name
FROM nation, customer, orders
WHERE o_orderdate>'1994-08-01' AND o_orderdate<'1995-08-01' AND o_custkey=c_custkey AND
	n_nationkey=c_nationkey

SELECT DISTINCT c_name 
FROM lineitem, orders, customer, nation, region
WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND 
	c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND r_name = 'EUROPE'

SELECT l_discount 
FROM lineitem, orders, customer, nation, region
WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND 
	c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND 
	r_name = 'ASIA'

SELECT SUM(l_discount) 
FROM customer, orders, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND l_discount < 0.03

