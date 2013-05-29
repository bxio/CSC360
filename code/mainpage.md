Assignment 1			{#mainpage}
------------

Completed by:

### Bill Xiong - V00737042 ###
### Aahan Sureja - V00741138 ###

 ---


## Our Solutions ##

This is our first assignment. Please don't murder our marks.

## Question 1 ##

An instantiator was added to Race.java, we instantiated three instances of Race, set them to run at Real-Time priority (1), then set the main thread to Daemon priority (3).

You may uncomment line 29 and comment line 35 to see the program halt on the correct value. This is because the main thread is set to daemon and each other thread is run in series, which gets rid of the race condition entirely, allowing the program to output the correct value.

## Question 2 ##

We declared the necessary variables and spinlocks. When you use two LockRace instances, the program halts with correct values. 

When you use both LockRace and LockRace1 instances, the program enters deadlock. To see this in action, please feel free to comment line 31 and uncomment 32.

## Question 3 ##

Methods we completed in j\_thread.c:
-	addReady
-	VMTick
-	Dispatch
-	Reschedule
-	PreemptIfNecessary

## Question 4 ##

A native method was added to thread.java, its stub was added to j\_lang.c and cputime() method was added to j\_thread.c.

We also Changed Rose so that it would call on these functions to determine the running time of the threads that we create.

### Subheading ###

You can create subheadings as well if you need that extra level of organization.
