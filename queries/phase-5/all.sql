SELECT SUM(ps_supplycost), s_suppkey
FROM part, supplier, partsupp 
WHERE p_partkey = 10 AND p_partkey = ps_partkey AND s_suppkey = ps_suppkey
GROUP BY s_suppkey

SELECT SUM(c_acctbal), c_name 
FROM customer, orders 
WHERE c_custkey = 121 AND c_custkey = o_custkey

SELECT l_receiptdate
FROM lineitem, orders, customer
WHERE	l_orderkey = o_orderkey AND o_custkey = c_custkey AND
	o_orderkey > 10 AND o_orderkey < 21

SELECT DISTINCT c_address
FROM customer, nation, region
WHERE c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND r_regionkey = 2

SELECT SUM(l_discount) 
FROM customer, orders, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND c_custkey = 222

SELECT DISTINCT n_name
FROM nation, customer, orders
WHERE o_orderkey > 10 AND o_orderkey < 10000 AND o_custkey=c_custkey AND
	n_nationkey=c_nationkey

SELECT DISTINCT c_name 
FROM lineitem, orders, customer, nation, region
WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND 
	c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND r_regionkey = 3

SELECT l_discount 
FROM lineitem, orders, customer, nation, region
WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND 
	c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND 
	r_regionkey = 1

SELECT SUM(l_discount) 
FROM customer, orders, lineitem
WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND c_custkey > 100 AND c_custkey < 201

SELECT SUM(l_extendedprice * l_discount * (1.0-l_tax))
FROM lineitem
WHERE l_orderkey > 10 AND l_orderkey < 51

