SELECT SUM(l_extendedprice * l_discount) FROM lineitem WHERE l_discount < 0.07 AND l_quantity < 24.0

SELECT SUM(ps_supplycost), suppkey FROM part, supplier, partsupp WHERE p_partkey = ps_partkey AND s_suppkey = ps_suppkey AND s_acctbal > 2500.0 GROUP BY s_suppkey

SELECT SUM(c_acctbal), name FROM customer, orders WHERE c_custkey = o_custkey AND o_totalprice < 10000.0 GROUP BY c_name

SELECT l_orderkey, l_partkey, l_suppkey FROM lineitem WHERE l_returnflag = 'R' AND l_discount < 0.04 AND l_shipmode = 'MAIL'

SELECT DISTINCT c_name, c_address, c_acctbal FROM customer WHERE c_name = 'Customer#000070919'

SELECT SUM(l_discount), n_name FROM customer, orders, lineitem, nation WHERE c_custkey = o_custkey AND o_orderkey = l_orderkey AND c_nationkey = n_nationkey AND c_acctbal < 1000.0 AND l_quantity > 30.0 AND l_discount < 0.03 GROUP BY n_name

SELECT l_orderkey FROM lineitem WHERE l_quantity > 30.0

SELECT DISTINCT c_name FROM lineitem, orders, customer, nation, region WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND c_nationkey = n_nationkey AND n_regionkey = r_regionkey

SELECT l_discount FROM lineitem, orders, customer, nation, region WHERE l_orderkey = o_orderkey AND o_custkey = c_custkey AND c_nationkey = n_nationkey AND n_regionkey = r_regionkey AND r_regionkey = 1 AND o_orderkey < 10000


--join predicates--

-- in selection predicate - if inequality then divide by 3, if equality then divide by number of distinct values
-- first write the table names
-- second create a join predicate, 
-- third create a selection predicate(with equalities and inequalities)

region --------------------------- nation  ---------------------------- customer--------------------------- orders -------------------------- lineitem
     r_regionkey = n_regionkey              n_nationkey = c_nationkey              c_custkey = o_custkey              o_orderkey = l_orderkey

--selection predicates- total tuples for each table
customer -> 1500
lineitem -> 60175
nation -> 25
orders -> 15000 -- sincee this has selection predicate with inequality, divide by 3 -- o_orderkey < 10000 --> 15000/3 = 5000
region -> 5 -- r_regionkey = 1 --> 5/5 = 1, since equality divide by number of distinct values


-- calculate cost for each join predicates
(region,nation) : n_nationkey = r_regionkey --> 25 * 1 / 5 = 5 -- formula--> (tuples_tab_1 * tuples_tab_2 / max(distinct(tab1. join_attributes),distinct(tab_2.join_attributes)))
(nation,customer) : n_nationkey = c_nationkey --> 25 * 1500 / 25 = 1500
(customer,orders) : c_custkey = o_custkey --> 1500 * 5000 / 1500 = 5000
(orders,lineitem) : o_orderkey = l_orderkey --> 5000 * 60000 / 15000 = 20000

-- choose on with minimum cost - (region,nation) : n_nationkey = r_regionkey --> 5
--graph becomes

(region,nation) -------------------------------------customer--------------------------- orders -------------------------- lineitem
	            n_nationkey = c_nationkey			            c_custkey = o_custkey               o_orderkey = l_orderkey

-- calculate cost for each join predicates
(region,nation,customer) : n_nationkey = c_nationkey --> 5 * 1500 / 25 = 300
(customer,orders) : c_custkey = o_custkey --> 1500 * 5000 / 1500 = 5000
(orders,lineitem) : o_orderkey = l_orderkey --> 5000 * 60000 / 15000 = 20000


-- minimum cost -- (region,nation,customer) : n_nationkey = c_nationkey --> 1500
--graph becomes

((region,nation),customer) --------------------------- orders -------------------------- lineitem
	                           c_custkey  = o_custkey               o_orderkey = l_orderkey


-- calculate cost for each join predicates
(((region,nation),customer),orders) : c_custkey = o_custkey --> 300 * 5000/1500 = 1000
(orders,lineitem) : o_orderkey = l_orderkey --> 5000 * 60000 / 15000 = 20000

-- minimum cost -- (((region,nation),customer),orders) : c_custkey = o_custkey --> 1000

--graph becomes

((((region,nation),customer),orders)---------------------------------- lineitem
	                                      o_orderkey = l_orderkey

-- calculate cost for each join predicates
((((region,nation),customer),orders),lineitem) : o_orderkey = l_orderkey --> 1000 * 60000 / 15000 = 4000



---final join graph looks like


 region------
			| -----------
 nation------			|
						|----------------
						|               |
customer----------------                |
								        |---------------------------
										|							|
orders ----------------------------------							|
																    |
																    |	
																	|
lineitem-------------------------------------------------------------



