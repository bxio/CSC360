Assignment 3			{#mainpage}
------------

Completed by:

### Bill Xiong - V00737042 ###
### Aahan Sureja - V00741138 ###

 ---


## Question 1 ##

We implement the shortest-seek-time-first as well as the circular scan algorithms for disk scheduling. 

In SSTF.java, we insert requests in sorted ascending order into a doubly linked list. Once a request has been serviced, we see if the request previous to the current one (current.prev) is closer to the current than the one proceeding it (current.next). We update our current head position accordingly.

In C-SCAN.java, we maintain one direction for servicing requests, and loop back to the first block once we reach the last block of the disk.

We found SSTF to be significantly faster than the provided FCFS algorithm. C-SCAN is the most fair algorithm.

## Question 2 ##

In FileSystem.java, we implemented a simple filesystem with 8 inodes, 192 data blocks, and a corresponding 192-bit bitmap. 

_free\_blocks()_ accesses the bitmap and update values when deleting a file. We generate the bitmask and use the bitwise AND operation to update the bitmap.

For writing, we have written a helper method _fill\_blocks()_ which is called by _get\_free\_blocks()_ to update the bitmap for writing. We generate the bitmask and use the bitwise INCLUSIVE OR operation to update the bitmap.

A major chunk of the work in reading, writing and deleting is calculating the address on disk using the different offsets. See the code for more details.

## Testing and Test Cases ##

Please find the following under the folder `/testCases`. We ran our tests with a quantum of 10, but this should not affect your results.

### Part A ###
We have two test cases each for SSTF and C-SCAN. One test case with 3 requests per thread, one with 5 requests per thread. The threads are delibrately added in unsorted order to test our _insert_ method.
### Part B ###
For ** TestRead **, You may run `/b/TestRead.bin` to test reading from the first inode. You may also run `/testCases/TestReadINODE1.bin` to read the same file, shifted down 1 inode. (In this test case, the first inode is empty.)

