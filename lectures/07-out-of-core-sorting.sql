input:
  S = {1, 4, 3, 1, 5, 6, 3, 2, 2}

Problem: sort S under the constraint that
  you can only allocate an array with size k (let's say 3) in memory at a time

output:
  R = {1, 1, 2, 2, 3, 3, 4, 5, 6}

- TeraSort competition for sorting large datasets

Two-Phase Multiway Merge Sort (TPMMS)
1. Split S into partitions of size 3

2. Sorting phase
  - read every partition in memory, sort it, and write it back to storage
  - sorting in memory is done with any sorting algorithm (quicksort, mergesort)
  P_1: {1, 4, 3} --> sort as {1, 3, 4}
  P_2: {1, 5, 6} --> sort as {1, 5, 6}
  P_3: {3, 2, 2} --> sort as {2, 2, 3}

3. Merge the sorted partitions
  P_1: {1, 3, 4}
  P_2: {1, 5, 6}
  P_3: {2, 2, 3}

  R = {}

  - read first element from every partition in memory
  P_1: 1 *
  P_2: 1
  P_3: 2

  - extract the minimum and append it to the sorted output: R = {1}

  - read the next element from the partition where the minimum is taken from
  P_1: 3
  P_2: 1
  P_3: 2

  - repeat the process until all partitions are exhausted

  P_1: 3
  P_2: 1 *
  P_3: 2
  R = {1, 1}

  P_1: 3
  P_2: 5
  P_3: 2 *
  R = {1, 1, 2}

  P_1: 3
  P_2: 5
  P_3: 2 *
  R = {1, 1, 2, 2}

  P_1: 3 *
  P_2: 5
  P_3: 3
  R = {1, 1, 2, 2, 3}

  P_1: 4
  P_2: 5
  P_3: 3 *
  R = {1, 1, 2, 2, 3, 3}

  P_1: 4 *
  P_2: 5
  P_3: 
  R = {1, 1, 2, 2, 3, 3, 4}

  P_1: 
  P_2: 5 *
  P_3: 
  R = {1, 1, 2, 2, 3, 3, 4, 5}

  P_1: 
  P_2: 6 *
  P_3: 
  R = {1, 1, 2, 2, 3, 3, 4, 5, 6}

  P_1: 
  P_2:
  P_3: 
  R = {1, 1, 2, 2, 3, 3, 4, 5, 6}




input:
  S = {1, 1, 3, 5, 9, 2, 2, 4, 6, 5}

Sort S with vectors/containers with at most 4 elements.

output:
  R = {1, 1, 2, 2, 3, 4, 5, 5, 6, 9}

1. Split S into partitions of size 4

2. Sorting phase
  - read every partition in memory, sort it, and write it back to storage
  - sorting in memory is done with any sorting algorithm (quicksort, mergesort)
  P_1: {1, 1, 3, 5} --> sort as {1, 1, 3, 5}
  P_2: {9, 2, 2, 4} --> sort as {2, 2, 4, 9}
  P_3: {6, 5} --> sort as {5, 6}

3. Merge the sorted partitions
  P_1: {1, 1, 3, 5}
  P_2: {2, 2, 4, 9}
  P_3: {5, 6}

  R = {}

  - read first element from every partition in memory
  P_1: 1 *
  P_2: 2
  P_3: 5

  - extract the minimum and append it to the sorted output: R = {1}

  - read the next element from the partition where the minimum is taken from
  P_1: 1
  P_2: 2
  P_3: 5

  - repeat the process until all partitions are exhausted

  P_1: 1 *
  P_2: 2
  P_3: 5

  R = {1, 1}

  P_1: 3
  P_2: 2 *
  P_3: 5

  R = {1, 1, 2}

  P_1: 3
  P_2: 2 *
  P_3: 5

  R = {1, 1, 2, 2}

  P_1: 3 *
  P_2: 4
  P_3: 5

  R = {1, 1, 2, 2, 3}

  P_1: 5
  P_2: 4 *
  P_3: 5

  R = {1, 1, 2, 2, 3, 4}

  P_1: 5
  P_2: 9
  P_3: 5 *

  R = {1, 1, 2, 2, 3, 4, 5}

  P_1: 5 *
  P_2: 9
  P_3: 6

  R = {1, 1, 2, 2, 3, 4, 5, 5}

  P_1: 
  P_2: 9
  P_3: 6 *

  R = {1, 1, 2, 2, 3, 4, 5, 5, 6}

  P_1: 
  P_2: 9
  P_3:

  R = {1, 1, 2, 2, 3, 4, 5, 5, 6, 9}


input:
  S = {1, 1, 3, 5, 9, 2, 2, 4, 6, 5}

Sort S with vectors/containers with at most 3 elements.

output:
  R = {1, 1, 2, 2, 3, 4, 5, 5, 6, 9}

1. Split S into partitions of size 3

2. Sorting phase
  - read every partition in memory, sort it, and write it back to storage
  - sorting in memory is done with any sorting algorithm (quicksort, mergesort)
  P_1: {1, 1, 3} --> sort as {1, 1, 3}
  P_2: {5, 9, 2} --> sort as {2, 5, 9}
  P_3: {2, 4, 6} --> sort as {2, 4, 6}
  P_4: {5} --> sort as {5}

3. Merge the first 3 sorted partitions
  P_1: {1, 1, 3}
  P_2: {2, 5, 9}
  P_3: {2, 4, 6}

  P_21 = {}

  - read first element from every partition in memory
  P_1: 1 *
  P_2: 2
  P_3: 2

  - extract the minimum and append it to the sorted output: P_21 = {1}

  - read the next element from the partition where the minimum is taken from
  P_1: 1
  P_2: 2
  P_3: 2

  - repeat the process until all partitions are exhausted

  P_1: 1 *
  P_2: 2
  P_3: 2

  P_21 = {1, 1, 2, 2, 3, 4, 5, 6, 9}

3'. Merge the second-level partition with the remaining one from level one
  P_21 = {1, 1, 2, 2, 3, 4, 5, 6, 9}
  P_4: {5}

  R = {1, 1, 2, 2, 3, 4, 5, 5, 6, 9}


DISTINCT implementation with TPMMS
----------------------------------
S = {1, 2, 3, 1, 2, 3, 4, 5, 6}
R = {1, 2, 3, 4, 5, 6}

sort S into S' with TPMMS
S' = {1, 1, 2, 2, 3, 3, 4, 5, 6}

R = {}
S'.MoveToStart()
while not S'.AtEnd()
  tmp = S'.current()
  R.append(S'.current())

  // ignore all the values equal with the one added to the result
  S'.MoveToNext()
  while not S'.AtEnd() AND S'.current() == tmp
    S'.MoveToNext()
  end while
end while
R = {1, 2, 3, 4, 5, 6}


input:
  S = {1, 1, 3, 5, 9, 2, 2, 4, 6, 5}

DISTINCT with vectors/containers with at most 4 elements.

output:
  R = {1, 2, 3, 4, 5, 6, 9}

1. Split S into partitions of size 4

2. Sorting phase
  - read every partition in memory, sort it, and write it back to storage
  - sorting in memory is done with any sorting algorithm (quicksort, mergesort)
  P_1: {1, 1, 3, 5} --> sort as {1, 1, 3, 5} --> DISTINCT {1, 3, 5}
  P_2: {9, 2, 2, 4} --> sort as {2, 2, 4, 9} --> DISTINCT {2, 4, 9}
  P_3: {6, 5} --> sort as {5, 6} --> DISTINCT {5, 6}

3. Merge the sorted partitions
  P_1: {1, 3, 5}
  P_2: {2, 4, 9}
  P_3: {5, 6}

  R = {}

  - read first element from every partition in memory
  P_1: 1 *
  P_2: 2
  P_3: 5

  - tmp <- minimum
  DISTINCT:- append tmp to the sorted output: R = {1}
  GROUPBY:- tmp

  - read the next element from the partition where the minimum is taken from
  P_1: 3
  P_2: 2 *
  P_3: 5

  DISTINCT:- extract minimum and append it to the output only when different from tmp: R = {1, 2}
  GROUPBY:-
    if min == tmp then update the aggregate (count = count + 1)
    else append (tmp, aggreagte) to the output

  - tmp <- minimum

  - repeat the process until all partitions are exhausted

  P_1: 3 *
  P_2: 4
  P_3: 5

  R = {1, 2, 3}
  tmp <- {3}

  P_1: 5
  P_2: 4 *
  P_3: 5

  R = {1, 2, 3, 4}
  tmp <- {4}

  P_1: 5 *
  P_2: 9
  P_3: 5

  R = {1, 2, 3, 4, 5}
  tmp <- {5}

  P_1: 
  P_2: 9
  P_3: 5 *

  // do not add to R because it is equal with tmp
  R = {1, 2, 3, 4, 5}
  tmp <- {5}

  P_1:
  P_2: 9
  P_3: 6 *

  R = {1, 2, 3, 4, 5, 6}
  tmp <- {6}

  P_1:
  P_2: 9
  P_3:

  R = {1, 2, 3, 4, 5, 6, 9}
  tmp <- {9}

