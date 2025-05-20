Bitmap indexes
--------------

- storage space in bits is supposed to be limited --> not a lot of space usage
- operations on bits --> which are supposed to be faster to execute
- do all the query operations on the index

- built on a column of a table

- p_mfgr
Manufacturer#1
Manufacturer#1
Manufacturer#4
Manufacturer#3
Manufacturer#3
Manufacturer#2
Manufacturer#1
Manufacturer#4
Manufacturer#4
Manufacturer#5
Manufacturer#2
Manufacturer#3
Manufacturer#5
Manufacturer#1
Manufacturer#1
Manufacturer#3
Manufacturer#4
Manufacturer#1
Manufacturer#2
Manufacturer#1

Manufacturer#1 : 11000010000001100101 (3 bytes)
Manufacturer#2 : 00000100001000000010 
Manufacturer#3 : 00011000000100010000
Manufacturer#4 : 00100001100000001000
Manufacturer#5 : 00000000010010000000

- total space: N (number of tuples) * D (number of distinct values)

- p_mfgr : CHAR(14) --> 20 * 14 (bytes)
- bitmap index : 20 * 5 (bits) --> (20 * 5) / 8 --> 5 * 3 (bytes)

- full part table
--> p_mfgr : CHAR(14) --> 2000 * 14 (bytes)
--> bitmap index : 2000 * 5 (bits) --> (2000 * 5) / 8 --> 5 * 250 (bytes)

- size of index depends on the number of distinct values D
--> D = 1 : size = N (minimum)
--> D = N : size = N^2 (maximum) --> reduce the size drammatically through compression

The data structure of a bitmap index
- map : distinct values --> corresponding bitmap


Queries over bitmap indexes
---------------------------
SELECT *
FROM part
WHERE p_mfgr = 'Manufacturer#1'

map['Manufacturer#1'] --> bitmap(11000010000001100101)

SELECT *
FROM part
WHERE p_mfgr = 'Manufacturer#1' OR p_mfgr = 'Manufacturer#5'

map['Manufacturer#1'] --> bitmap(11000010000001100101)
map['Manufacturer#5'] --> bitmap(00000000010010000000)

11000010000001100101 OR 00000000010010000000 --> 11000010010011100101


Brand#13
Brand#13
Brand#42
Brand#34
Brand#32
Brand#24
Brand#11
Brand#44
Brand#43
Brand#54
Brand#25
Brand#33
Brand#55
Brand#13
Brand#15
Brand#32
Brand#43
Brand#11
Brand#23
Brand#12

Brand#13 : 11000000000001000000 (3 bytes)

SELECT *
FROM part
WHERE p_mfgr = 'Manufacturer#1' AND p_brand = 'Brand#13'

map_p_mfgr['Manufacturer#1'] --> bitmap(11000010000001100101)
map_p_brand['Brand#13'] -->      bitmap(11000000000001000000)

11000010000001100101 AND 11000000000001000000 --> 11000000000001000000


Run-Length Encoding (RLE)
- lossless compression method --> allow for complete recovery of the original data
- compress a bitmap index by reducing the size of the contiguous sequences of zeros

11000000000001000000

00000000000 (11 zeros) --> number 11 in binary : 1011 (11 in binary)
11 bits vs 4 bits --> compression ratio of 4/11

000000 (6 zeros) --> number 6 in binary : 110 (6 in binary)
6 bits vs 3 bits --> compression ratio of 3/6


1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20

1 : 10000000000000000000
0000000000000000000 (19 zeros) --> number 19 in binary : 10011 (19 in binary)
19 bits vs 5 bits --> compression ratio of 5/19

15 : 00000000000000100000
0000000000000100000
14 zeros --> number 14 in binary : 1110 (14 in binary)
5 zeros  --> number  5 in binary :  101 (5 in binary)
19 bits vs 7 bits --> compression ratio of 7/19


How do we represent the ones in the bitmap?
11000000000001000000

00000000000 (11 zeros) --> number 11 in binary : 1011 (11 in binary)
11 bits vs 4 bits --> compression ratio of 4/11

000000 (6 zeros) --> number 6 in binary : 110 (6 in binary)
6 bits vs 3 bits --> compression ratio of 3/6

- for every one (1) in the bitmap, encode how many zeros (0) are preceding it
1 1 000000000001 000000
0 0           11          --> number of zeros preceding every one (1)
 0 --> 1 bit in binary; value is 0 ---> 0 
 0 --> 1 bit in binary; value is 0 ---> 0 
11 --> 4 bits in binary; value is 1011 ---> 1011

11000000000001000000 ---> 0 0 1011 ---> 001011

Can we get back to 11000000000001000000 deterministically and uniquely from 001011?
0 --> 0 
0 --> 0
1 --> 1 (0)
0 --> 0

How can we interpret 11?
11 --> 3 (000)                  ----> 110110001
1 --> 1 (0) and 1 --> 1 (0)     ----> 110110101

- RLE compressed
  1     1     000000000001    000000 ----> (00) (00) (1110 1011)
(00)  (00)      (1110 1011)
11000000000001000000 ---> 000011101011



Manufacturer#1 : 11000010000001100101 (3 bytes)
Manufacturer#2 : 00000100001000000010 
Manufacturer#3 : 00011000000100010000
Manufacturer#4 : 00100001100000001000
Manufacturer#5 : 00000000010010000000


11000010000001100101 --> 000011010011011000101001 (20 bits --> 24 bits)
  1   1   00001 0000001   1   001  01
0 0 0 0 110 100 110 110 0 0 10 10 0 1

00000100001000000010 --> 110101110100110111 (20 bits --> 18 bits)
 000001   00001 00000001 0
110 101 110 100  110 111

00011000000100010000 --> 1011001101101011 (20 bits --> 16 bits)
 0001   1 0000001  0001 0000
10 11 0 0 110 110 10 11

00100001100000001000 --> 101011010000110111 (20 bits --> 18 bits)
  001   00001   1 00000001 000
10 10 110 100 0 0  110 111

00000000010010000000 --> 111010011010 (20 bits --> 12 bits)
0000000001   001 0000000 
 1110 1001 10 10

Overall: 100 bits --> 88 bits (88/100)


10000000000000000000 --> 00
01000000000000000000 --> 01
00100000000000000000 --> 10 10
00010000000000000000 --> 10 11
00001000000000000000 --> 110 100

Overall: 100 bits --> 18 bits (18/100)
