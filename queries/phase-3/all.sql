SELECT n_nationkey, n_name
FROM nation 
WHERE n_regionkey=1

SELECT l_orderkey, l_suppkey, l_partkey
FROM lineitem
WHERE l_orderkey<l_suppkey AND l_orderkey<l_partkey AND l_partkey<l_suppkey

SELECT l_orderkey, l_partkey, l_suppkey 
FROM lineitem
WHERE l_returnflag='R' AND l_discount<0.04 AND l_shipmode='MAIL'

SELECT c_name, c_address, c_acctbal 
FROM customer 
WHERE c_name='Customer#000070919'

SELECT o_orderdate, o_orderstatus, o_orderpriority, o_shippriority
FROM orders
WHERE o_totalprice>100.0

SELECT l_orderkey 
FROM lineitem
WHERE l_quantity>30.0

SELECT s_name, s_phone
FROM supplier
WHERE s_nationkey=10 AND s_acctbal<100.0

SELECT p_name, p_mfgr, p_brand, p_type 
FROM part
WHERE p_size>2 AND p_size<6 AND p_retailprice>10.0 AND p_retailprice<15.0

SELECT ps_partkey, ps_suppkey, ps_availqty, ps_supplycost
FROM partsupp
WHERE ps_partkey=123 AND ps_suppkey>10 AND ps_suppkey<100

SELECT l_extendedprice, l_discount
FROM lineitem
WHERE '1994-01-01'<l_shipdate AND '1994-02-01'>l_shipdate

SELECT SUM(l_extendedprice * l_discount * (1.0-l_tax)), l_suppkey
FROM lineitem
WHERE l_discount < 0.07 AND l_quantity < 12.0
GROUP BY l_suppkey

SELECT SUM(ps_supplycost), ps_suppkey
FROM partsupp 
GROUP BY ps_suppkey

SELECT SUM(c_acctbal), c_name 
FROM customer
GROUP BY c_name

SELECT DISTINCT o_orderdate
FROM orders
WHERE	o_orderdate>'1995-03-10' AND o_orderdate<'1995-03-20'

SELECT DISTINCT c_address
FROM customer
WHERE c_nationkey = 6

SELECT SUM(l_discount) 
FROM lineitem
WHERE l_orderkey>100 AND l_orderkey<400

SELECT DISTINCT c_nationkey
FROM customer
WHERE c_custkey > 100 AND c_custkey < 150

SELECT DISTINCT l_suppkey, l_shipdate, l_commitdate, l_receiptdate
FROM lineitem
WHERE l_shipdate > '1995-10-10' AND l_shipdate < '1995-10-15'

SELECT DISTINCT l_discount 
FROM lineitem
WHERE l_returnflag = 'N' AND l_linestatus = 'F'

SELECT SUM(l_extendedprice * l_discount * (1.0-l_tax))
FROM lineitem
WHERE l_discount < 0.07 AND l_quantity < 12.0
