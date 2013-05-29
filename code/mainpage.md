Assignment 1			{#mainpage}
------------

Completed by:

### Bill Xiong - V00737042 ###
### Aahan Sureja - V00741138 ###

 ---


## Our Solutions ##

This is our first assignment. Please don't murder our marks.

## Question 1 ##

There are three threads racing to write to the same resource. The result *should* be 60,000; however, it isn't because one thread will always get to the resource before the other two. Like the poor Victorians counting flowers, the outcome of this program is completely nondeterminant.

When you create the three threads, and then set yourself to daemon, the program outputs the correct value. This is because the main thread is set to daemon and each other thread is run in series, which gets rid of the race condition entirely, allowing the program to output the correct value.

*What we did*
An instantiator was added to Race.java, we instantiated three instances of Race, set them to run 
at Real-Time priority (1), then set the main thread to Daemon priority (3).

You may uncomment line 31 and comment line 36 to see the program halt on the correct value. 

## Question 2 ##

When we run the program with LockRace1 instead of LockRace, it results in a deadlock. LockRace is waiting for l2 and LockRace1 is waiting for l1. Each thread has acquired one lock, and each thread is spinning, waiting for the other lock to become free, resulting in no progress whatsoever.

*What we did*
We declared the necessary variables and spinlocks. When you use two LockRace instances, the program halts with correct values. 

When you use both LockRace and LockRace1 instances, the program enters deadlock. To see this in action, please feel free to comment line 31 and uncomment 32.

## Question 3 ##

*What we did*
Methods we completed in j\_thread.c:
-	addReady
-	VMTick
-	Dispatch
-	Reschedule
-	PreemptIfNecessary

## Question 4 ##

A native method was added to thread.java, its stub was added to j\_lang.c and cputime() method was added to j\_thread.c.

Two additional int32's (startTime and deltaTime) were added to the struct thread_t in jvm.h to keep track of the start time of the thread and the deltaTime (the total elapsed time) of the thread.

We also Changed Rose so that it would call on these functions to determine the running time of the threads that we create.

The method we added, cputime(), is called in dispatch. We grab the current time (ie, the time that the thread ended execution) and we subtract from this the startTime of the thread, and then we add it to the deltaTime of the thread.