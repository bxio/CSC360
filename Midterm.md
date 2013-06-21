##To Study for on the Midterm##

	OS Architecture, DMA and programmed IO
	multiprogramming, mltithreading, multitasking
	CPU scheduling, tasks and dhread states, contexts
	Memory model in Unix and Linux of a task,
	User level (green therads) vs Kernal level (red threads)
	Premptive vs Cooperative scheduling 
		Threads can preempt each other vs threads yielding voluntarily
		Java's Sun VM is cooperative. Rose is Preemptive.
		multithreading 
	Race conditions, SpinLocks 
	The Critical section challenge (two threads want same resource. How to guarentee determinant solution?)
		Hardware solutions to the critical section problem
			(Test and set, atomic swap)
		Software solutions
			Peterson's Ladies first algorithm
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
	P-thread AP
	Finite State Model of thread and other concepts

##Assignments##
	Scheduler and the API
		Thread states (reproduce in Finite State Diagram)
		Scheduling Policy
			Round robin, FIFO, Multi-level
	Native methods and how it relates to System call
		Sort of like supervisor call
	Why race conditions are bad?
	



