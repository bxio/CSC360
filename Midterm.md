##To Study for on the Midterm##

	OS Architecture, DMA (Direct memory access) and programmed IO
	multiprogramming, mltithreading, multitasking
	CPU scheduling, tasks and dhread states, contexts
	Memory model in Unix and Linux of a task,
	User level (green therads) vs Kernal level (red threads)
	Premptive vs Cooperative scheduling 
		Threads can preempt each other vs threads yielding voluntarily
			Java's Sun VM is cooperative. Rose is Preemptive.
		Multithreading 
	Race conditions, SpinLocks 
	The Critical section challenge (two threads want same resource. How to guarentee determinant solution?)
		Hardware solutions to the critical section problem
			(Test and set, atomic swap)
		Software solutions
			Peterson's Ladies first algorithm (Give away your turn as soon as you are interested)
	Synchronization concepts
		mutex
		semaphores
		barriers
		conditions
		monitors
	Classic problems of synchronization concepts
		Bounded Buffer
			1 Consumer, 1 Producer
			m Consumer, n Producer
		Reader/Writer Problem
			Reader-preference and Writer-preference
		RW Locks and Reader/Writer Semaphores
			allowing multiple readers to proceed without locking each other out
		Concept of Fairness and Barging
		Sequential Lock in Linux (Non-blocking, lock-free synchronization)
			Allowing multiple readers and one writer overlap.
			* Multiple readers is nothing new. RW semaphore already does this. But what happens when you read something that is being modified?
			* The key is to keep a sequential version number in the lock. If the lock number is even, the state is stable and you can proceed to read. If it's odd, the data is currently being modified and readers should ditch what they have read and retry until they exit with a 0 state.
			* If readers exit with odd lock, they need to ditch their data and retry. Readers can only start on an even lock number. If they try to start on an odd number, they are stopped.
			* The writers have to have a mutex so that only 1 writer writes at one time.
	P-thread AP
	Finite State Model of thread and other concepts

	Dr. Cheng's Questions to Ponder

##Assignments##
	Scheduler and the API
		Thread states (reproduce in Finite State Diagram)
		Scheduling Policy
			Round robin, FIFO, Multi-level
	Native methods and how it relates to System call
		Sort of like supervisor call
		* There is two modes, user and kernel. While in user mode is not allowed to do certain things
		* in rose, native methods are stored in a table. When you compile, it looks up the 
	Why race conditions are bad?
		Locking order and the horror that is deadlock
	Bounded buffer and synchronization
		Reader/Writer problem, monitor, mutex, condition
	Difference between general monitor and Java's Synchronize method, and how Java's monitor is limited.

##Future##
Assignment 3 - Disk Scheduling
Assignment 4 - Disk Caching

m, n 
