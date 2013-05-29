CSC 360 Assignment 1			{#mainpage}
------------

Completed by:

### Bill Xiong - V00737042 ###
### Aahan Sureja - V00741138 ###

 ---


## Our Solutions ##

This is our first assignment. Please don't murder our marks.

## Question 1 ##

A) There are three threads racing to write to the same resource. The result *should* be 60,000; however, it isn't because one thread will always get to the resource before the other two. Like the poor Victorians counting flowers, the outcome of this program is completely nondeterminant.

B) When you set the main thread to Daemon level before you start the three threads, the program halts with the correct value because the threads that are created are created at Real time, which is now a higher priority than the main thread. Thus, each thread preempts the main thread as it starts, runs until completion, then exits. In effect, this serializes the three threads and makes them run one at a time, thus giving the correct value.

When you set the main thread to daemon after the threads are already started, there is nothing to preempt. The three threads run in round robin fashion, and once again the result is nondeterminant.

*What we did*

An instantiator was added to Race.java, we instantiated three instances of Race, set them to run 
at Real-Time priority (1), started the three threads, then set the main thread to Daemon priority (3).

You may uncomment line 26 and comment line 31 to see the program halt on the correct value. 

## Question 2 ##

A) When we run the program with LockRace1 instead of LockRace, it results in a deadlock. LockRace is waiting for l2 and LockRace1 is waiting for l1. Each thread has acquired one lock, and each thread is spinning, waiting for the other lock to become free, resulting in no progress whatsoever.

*What we did*

We declared the necessary variables and spinlocks. When you use two LockRace instances, the program halts with correct values. 

When you use both LockRace and LockRace1 instances, the program enters deadlock. To see this in action, please feel free to comment line 31 and uncomment 32.

## Question 3 ##

*What we did*

Methods we completed in j\_thread.c:
###AddReady###
AddReady adds the thread you pass it into ready_q, according to priority. It does this via two things
###VMTick###
VMTick is called every time a tick is up. It decrements the active thread's ticks if the thread is at Real Time or at User level. VMTick simply returns if the thread is at System or Daemon level.
###Dispatch###
Dispatch pushes the next thread in ready_q into the running state. We also include some calls to keep track of the time that the thread started running and the cumulative time that the thread has been run.
###Reschedule###
Reschedule takes the current running thread, puts it back into ready mode, and calls AddReady to put it back into the ready queue.
###PreemptIfNecessary###
PreemptIfNecessary checks if there is a thread that is of higher priority at the beginning of the ready_q. If there is, we push the current thread out of the running state into the ready state, call AddReady to add that thread to the ready queue, and then call dispatch to get a new thread to run.
## Question 4 ##

A native method was added to thread.java, its stub was added to j\_lang.c and cputime() method was added to j\_thread.c.

Two additional int32's (startTime and deltaTime) were added to the struct thread_t in jvm.h to keep track of the start time of the thread and the deltaTime (the total elapsed time) of the thread.

Three additional methods were added to j_thread.c in order to keep track of thread's times. We also Changed Rose so that it would call on these functions to determine the running time of the threads that we create.

Native methods are so difficult to get to work right. 
