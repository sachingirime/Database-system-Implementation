- equi-join: equality predicates (R.A = S.B)
- theta-join: inequality predicates (R.A > S.B)

Nested-loop Join
----------------
for all tuples r in R do
  for all tuples s in S do
    if predicate(r, s) == true
      return tuple r#s (concatenated tuple)
    end if
  end for
end for

- we have to iterate multiple times over table S, one iteration for every tuple in R


NestedLoopJoin::GetNext(Record& outputRec)
++++++++++++++++++++++++++++++++++++++++++
- state (class) variables
  List :: Record --> list (inner loop records)
  Record --> lRec (current record in the outer loop)

  if firstTime is true

    // make sure this is performed only once
    // read complete S and store it in a list
    while (rightChild->GetNext(Record& rec))
      list.Append(rec)

    firstTime = false

    // --> for all tuples r in R do
    if (leftChild->GetNext(Record& lRec) == false)
      return false

    list.MoveToStart()

  end if

  while (true)

    // --> for all tuples s in S do
    while list.AtEnd() == false
      rRec = list.Current()

      if predicate.Run(lRec, rRec) == true
        list.Advance()
        outputRec.Append(lRec, rRec)
        return true
      end if

      list.Advance()
    end while

    // --> for all tuples r in R do
    if (leftChild->GetNext(Record& lRec) == false)
      return false

    list.MoveToStart()

  end while


R(A) (leftChild): {1, 3, 4, 2, 1}
S(B) (rightChild): {1, 3, 1}
R equi-join S (R.A = S.B) --> {(1, 1), (1, 1), (3, 3), (1, 1), (1, 1)} 

IndexNestedLoopJoin::GetNext(Record& outputRec)
HashJoin::GetNext(Record& outputRec)
+++++++++++++++++++++++++++++++++++++++++++++++
- state variables
  Map :: Record -> KeyInt --> map (inner loop records)
  Record --> lRec (current record in the outer loop)
- find matching tuples faster without comparing every pair of records
- works only for equi-join
- this is exactly the HashJoin algorithm you have to implement in Project 5

  if firstTime is true

    // make sure this is performed only once
    // read complete S and store it in a map
    while (rightChild->GetNext(Record& rec)) {
      rec.SetOrderMaker(rightOrder)
      KeyInt v(0)
      map.Insert(rec, v)
    }

    firstTime = false

    if (leftChild->GetNext(Record& lRec) == false)
      return false

    lRec.SetOrderMaker(leftOrder)
    if map.IsThere(lRec) == true
      rRec = map.CurrentKey()
      outputRec.Append(lRec, rRec)
      map.Advance()

      return true
    end if      

  end if

  while (true)

    rRec = map.CurrentKey()
    if lRec.IsEqual(rRec) == true
      map.Advance()
      outputRec.Append(lRec, rRec)
      return true
    end if

    if (leftChild->GetNext(Record& lRec) == false)
      return false

    lRec.SetOrderMaker(leftOrder)
    if map.IsThere(lRec) == true
      rRec = map.CurrentKey()
      outputRec.Append(lRec, rRec)
      map.Advance()
      
      return true
    end if      

  end while



SymmetricHashJoin::GetNext(Record& outputRec)
+++++++++++++++++++++++++++++++++++++++++++++
- state variables
  Map :: Record -> KeyInt --> lMap
  Map :: Record -> KeyInt --> rMap
  lDone :: boolean
  rDone :: boolean
  whichSide :: {left, right}
  List :: Record --> buffer

- works only for equi-join
- non-blocking: joining tuples are produced as early as it is possible
- no complete processing on one side before the other side



  if (lDone == true) && (rDone == true) && (buffer.AtEnd() == true)
    return false
  end if

  if buffer.AtEnd() == false
    outputRec = buffer.Current()
    buffer.Advance()
    return true
  end if

  buffer = {}

  while (true)

    if whichSide == right
      whichSide = left

      if (rDone == false) && (rightChild->GetNext(Record& rRec) == true)
        rRec.SetOrderMaker(rightOrder)
        rMap.Insert(rRec, KeyInt(0))

        if lMap.IsThere(rRec) == true

          lRec = lMap.CurrentKey()
          while (lRec.IsEqual(rRec) == true)
            oRec = Append(lRec, rRec)
            buffer.Append(oRec)
            lMap.Advance()
            lRec = lMap.CurrentKey()
          end while

          buffer.MoveToStart()
          
        end if

      else
        rDone = true
      end if

    else if whichSide == left
      whichSide = right

      if (lDone == false) && (leftChild->GetNext(Record& lRec) == true)
        lRec.SetOrderMaker(leftOrder)
        lMap.Insert(lRec, KeyInt(0))

        if rMap.IsThere(lRec) == true

          rRec = rMap.CurrentKey()
          while (lRec.IsEqual(rRec) == true)
            oRec = Append(lRec, rRec)
            buffer.Append(oRec)
            rMap.Advance()
            rRec = rMap.CurrentKey()
          end while

          buffer.MoveToStart()
          
        end if

      else
        lDone = true
      end if

    end if

    if (lDone == true) && (rDone == true) && (buffer.AtEnd() == true)
      return false
    end if

    if (buffer.AtEnd() == false)
      outputRec = buffer.Current()
      buffer.Advance()
      return true
    end if

  end while

R(A) (leftChild): {1, 3, 4, 2, 1}
S(B) (rightChild): {1, 3, 1}

lMap = {}
rMap = {}

left
lRec = {1}
- rMap.IsThere(lRec) --> no match
- lMap.Insert(lRec) --> lMap = {1}

right
rRec = {1}
- lMap.IsThere(rRec) --> match (1, 1); return (1, 1) to parent operator
- rMap.Insert(rRec) --> rMap = {1}

left
lRec = {3}
- rMap.IsThere(lRec) --> no match
- lMap.Insert(lRec) --> lMap = {1, 3}

right
rRec = {3}
- lMap.IsThere(rRec) --> match (3, 3); return (3, 3) to parent operator
- rMap.Insert(rRec) --> rMap = {1, 3}

left
lRec = {4}
- rMap.IsThere(lRec) --> no match
- lMap.Insert(lRec) --> lMap = {1, 3, 4}

right
rRec = {1}
- lMap.IsThere(rRec) --> match (1, 1); return (1, 1) to parent operator
- rMap.Insert(rRec) --> rMap = {1, 1, 3}

left
lRec = {2}
- rMap.IsThere(lRec) --> no match
- lMap.Insert(lRec) --> lMap = {1, 2, 3, 4}

left
lRec = {1}
- rMap.IsThere(lRec) --> match (1, 1), (1, 1); return (1, 1), (1, 1) to parent operator
- lMap.Insert(lRec) --> lMap = {1, 1, 2, 3, 4}

- order with hash join --> {(1, 1), (1, 1), (3, 3), (1, 1), (1, 1)} 
- order with symmetric hash join --> {(1, 1), (3, 3), (1, 1), (1, 1), (1, 1)} 

- the above algorithms require that at least one side of the join fits in memory
- algorithm that does not have this requirement (none of the sides fits in memory)

R(A) (leftChild): {1, 5, 3, 1, 9}
S(B) (rightChild): {5, 3, 3, 1}
- available memory is 3

SortMergeJoin
-------------
1. Sort R using TPMMS
2. Sort S using TPMMS
3. Merge match sorted R and S

- sort Rp_1 : {1, 5, 3} --> {1, 3, 5}
- sort Rp_2 : {1, 9} --> {1, 9}
- merge Rp_1 and Rp_2 into Rs: {1, 1, 3, 5, 9}

- sort Sp_1 : {5, 3, 3} --> {3, 3, 5}
- sort Sp_2 : {1} --> {1}
- merge Sp_1 and Sp_2 into Ss : {1, 3, 3, 5}

- merge Rs and Ss using the same approach as in TPMMS
Rs : {1, 1}
Ss : {1}
--> join pairs : {(1, 1); (1, 1)}

Rs : {3}
Ss : {3, 3}
--> join pairs : {(3, 3); (3, 3)}

Rs : {5}
Ss : {5}
--> join pairs : {(5, 5)}

Rs : {9}
Ss : {}

- optimization : merge the partitions across tables together
- sort Rp_1 : {1, 5, 3} --> {1, 3, 5}
- sort Rp_2 : {1, 9} --> {1, 9}
- sort Sp_1 : {5, 3, 3} --> {3, 3, 5}
- sort Sp_2 : {1} --> {1}

Rp_1 : {1}
Rp_2 : {1} --> {1, 1}

Sp_1 : {3, 3}
Sp_2 : {1} --> {1}
--> join pairs : {(1, 1); (1, 1)}

Rp_1 : {3}
Rp_2 : {9} --> {3}

Sp_1 : {3, 3}
Sp_2 : {} --> {3, 3}
--> join pairs : {(3, 3); (3, 3)}

Rp_1 : {5}
Rp_2 : {9} --> {5}

Sp_1 : {5}
Sp_2 : {} --> {5}
--> join pairs : {(5, 5)}

Rp_1 : {}
Rp_2 : {9} --> {9}

Sp_1 : {}
Sp_2 : {} --> {}
