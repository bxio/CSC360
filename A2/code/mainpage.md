CSC360 Assignment 2			{#mainpage}
------------

Completed by:

### Bill Xiong - V00737042 ###
### Aahan Sureja - V00741138 ###

 ---

**A brief note about the Quantum**
Please run all tests on Quantum 10. We've found that in certain cases, running Rose.exe on Quantum 1 fails.
Dr. Cheng has said that we don't need to fix this, as the size of the slice should not affect the correctness of the codes.

## Question 1 - The Readers/Writers Problem##
We were supposed to implement writer's priority. 

## Question 2 - The Synchronous Readers-Writer Problem##
Discussion of Question 2. Talk about BoundedBuffer and TestP1C1.java.

## Question 3 - POSIX Condition Variables##
For this part, we coded 3 methods.
Please see the code under `rose/j_thread.c` for more information.

For CondWait, we store the mutex that we release under the thread's relock field. This allows us to relock it when we get to the Signal method.

For CondSignal, we first check if thr_active has a relock field. If it does, we lock it and set the next thread in the condition's block queue to the mutex's block queue, so that the next time the active thread calls MutexUnlock, the mutex will pass to the next thread instead of getting unlocked.

For CondBroadcast
## Test Cases##
Part A
- To test the basic read/write case, run `a/TestBasicRW.bin` with Rose.
- To test the priority, run `a/TestPreference.bin`

Part B
- To test 1 producer and 1 consumers, run `b/TestP1C1.bin`
- To test 1 producer and 3 consumers, run `b/TestP1C3.bin`

Part C
- Run `TestCases/UnitTest.bin` on quantum 10. The expected output is "[bb],[a[cc],a],"
