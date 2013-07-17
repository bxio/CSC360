Assignment 3			{#mainpage}
------------

Completed by:

### Bill Xiong - V00737042 ###
### Aahan Sureja - V00741138 ###

 ---

**Delete Me** 

Put a brief overview of what was done in the assignment here. This file can be found and edited at code/mainpage.md.
For more information on markdown, [see doxygen markdown documentation](http://www.stack.nl/~dimitri/doxygen/manual/markdown.html). 

To help organize your documents you can use headings like:

## Question 1 ##

We implement the shortest-seek-time-first as well as the circular scan algorithms for disk scheduling. 

In SSTF.java, we insert requests in sorted ascending order into a doubly linked list. Once a request has been serviced, we see if the request previous to the current one (current.prev) is closer to the current than the one proceeding it (current.next). We update our current head position accordingly.

In C-SCAN.java, we maintain one direction for servicing requests, and loop back to the first block once we reach the last block of the disk.

We found SSTF to be significantly faster than the provided FCFS algorithm. C-SCAN is the most fair algorithm.

## Question 2 ##

Discussion of Question 2. FileSystem.

## Testing and Test Cases ##

Please find the following under the folder `/tests`

### Part A ###

### Part B ###
For ** TestRead **, You may run `/b/TestRead.bin` to test reading from the first inode. You may also run `/testCases/TestReadINODE1.bin` to read the same file, shifted down 1 inode. (In this test case, the first inode is empty.)