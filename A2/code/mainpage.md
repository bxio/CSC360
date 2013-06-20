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
Our task was to implement the Readers/Writers problem with priority given to writers.

We divided the readers into two categories: activeReaders and waitingReaders. Since there is only one active writer at any given time, we used a boolean to signal whether there is an active writer or not. 

For StartRead, we first check whether the database is already in the WRITING state. If it is, we add it to the readQ. Once its done waiting and there is 1 active reader, we change the state to READING.

For EndRead, we check if there are no more readers and there are one or more writers waiting to write. If so, we pass dbState directly to WRITING as opposed to NOTUSED, and signal the writeQ.

For StartWrite, we check for a reader or writer already reading or writing. In that case, we add it to the writeQ. Once its done waiting, it becomes active, and we turn the flag to true and the state of the database to WRITING.

In EndWrite we turn the activeWriter flag to false, change the database state to NOTUSED. If there are writers waiting, we signal the writeQ. Else, we broadcast the readQ.

## Question 2 - The Synchronous Readers-Writer Problem##
We implemented this similar to Question 1, with a few modifications to accommodate the synchronization.

In StartRead, we prevent a reader from reading ahead of the other readers by making it wait on a condition called waitingReaders. We count the number of readers coming in.

In Get, we increment the out position only when there are no more readers left. 

In EndRead, the last reader broadcasts the waiting readers.

## Question 3 - POSIX Condition Variables##
For this part, we coded 3 methods.
Please see the code under `rose/j_thread.c` for more information.

For CondWait, we store the mutex that we release under the thread's relock field. This allows us to grab it in order to  relock it when we get to the Signal method.

For CondSignal, we first check if thr_active has a relock field. If it does, we lock it and set the next thread in the condition's block queue to the mutex's block queue, so that the next time the active thread calls MutexUnlock, the mutex will pass to the next thread instead of getting unlocked.

For CondBroadcast, we do the same thing as CondSignal, except we DeQ everything from the condition block queue and EnQ it into the mutex's block queue 

## Test Cases##
Part A
- To test the basic read/write case, run `a/TestBasicRW.bin` with Rose.
- To test the priority, run `a/TestPreference.bin`

Part B
- To test 1 producer and 1 consumers, run `b/TestP1C1.bin`
- To test 1 producer and 3 consumers, run `b/TestP1C3.bin`

Part C
- Run `TestCases/UnitTest.bin` on quantum 10. The expected output is "[bb],[a[cc],a],"
