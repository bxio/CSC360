/**************************************************************************************
*  j_thread.c
*
*  This file contains thread support functions.
*
**************************************************************************************
*
*  The contents of this file are subject to the RTJ COMPUTING non-commercial
*  source code license agreement ("License"); you may not use this file except in
*  compliance with the License. A copy of the License (license.terms) can be
*  found in the root of installation folder or on the web at
*  http://www.rtjcom.com/Files/license.terms
*
*  Software distributed under the License is distributed on an "AS IS"
*  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
*  License for the specific language governing rights and limitations
*  under the License.
*
*  Copyright (c) 2000 RTJ Computing. All right reserved.
*
*  Developer(s): Peter Gasparik <peterg@rtjcom.com>
**************************************************************************************/

#define __timer_t_defined
#include <stdio.h>

#include "jvm.h"
#include "jcommon.h"

static method_t *findRunMethod(class_t *cl);
static bool releaseClassLock(void);
static thread_t *NewThread(void);
static void AddFront( thread_t **q, thread_t *p );
static void EnQ( thread_t **q, thread_t *p );
static void DeQ( thread_t **q, thread_t **p );
static void AssignQuantum( thread_t *p );

static thread_t *ready_q;	       /* a queue of READY threads */
static thread_t *dead_pool;	       /* a queue of DEAD threads */


/****************************************************************************
* Run <init>() method on the current active Thread object
*
* Input:        none
* Output:       none
* Return:       true    if successfull
*               false   if exception has been generated
****************************************************************************/
bool initMainThread(void)
{
	if (run_init(thr_active->objref, NULL))
	{
		thr_count++;
		return true;
	}
	return false;
}


/****************************************************************************
* Creates new entry in thr_table if there is enough room. If table is full
* EXCEP_ThreadExecFail is thrown.
* Then it checks if run() method exists in the class which address has been
* provided in X. If class doesn't contain run() EXCEP_ThreadStartupFail will
* be thrown.
* Once run() has been located the new thr_table entry is created and appropriate
* fields are setup and run() will begin execution.
*
* Input:        ref     reference to class object containing the run() method
* Output:       none
* Return:       true    if successfull
*               false   if exception has been generated
****************************************************************************/
bool createThread(ref_t *ref, int32 pri)
{
	method_t *run;
	thread_t *thread;

	run = findRunMethod(ref->class_ptr);
	if (run == NULL)
	{
		if (vm_excep == NO_EXCEP)
			vm_excep = INTERR_ThreadSetupFail;
		return false;
	}


	if ((thread = NewThread()) == NULL) {
		vm_excep = INTERR_ThreadLimit;
		return false;
	}

	thread->objref = ref;
	thread->curr_frame = NULL;
		/* By default, a thread is set to the USER_LEVE */
	if ((pri < 0) || (pri >= MAX_PRI_LEVELS )) pri = USER_LEVEL;
	thread->pri = pri;
	thread->curr_frame = createFrame(NULL,run);

	if (thread->curr_frame == NULL)
		return false;


	thread->curr_frame->locals[0].i = PUT_REF(ref);

	thr_count++;

	AssignQuantum( thread );
	AddReady(thread,false);

	PreemptIfNecessary();

	return true;
}


/****************************************************************************
* Searches for the run method in current class. I not found then it will
* traverse trough references to super classes.
*
* Input:        cl      pointer to current class
* Output:       none
* Return:       location of run() method if found, otherwise NULL
*               is returned
****************************************************************************/
static method_t *findRunMethod(class_t *cl)
{
	method_t *run;

	while (cl->run == NULL)
	{
		if (cl->super == NULL)
			return NULL;

		cl = CLASS_LOC(cl->super);
	}
	run = METHOD_LOC(cl->run);
	if (run->flags & ACC_ABSTRACT)
	{
		vm_excep = ERROR_AbstractMethod;
		return NULL;
	}

	return run;
}


/****************************************************************************
* Removes currently running thread from threads table.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
void deleteThread(thread_t *thread)
{

	if ((thr_count > 0) && (thread != NULL))
	{
		thr_count--;
		thread->state = DEAD;
		  /* put it back into the DEAL pool */
	AddFront( &(dead_pool), thread );

		  /* reschedule if the active thread is deleted! */
	if((thr_active == thread) && (thr_count > 0)) Dispatch();

		/* if event thread has been deleted then clear pointer to it */
		if (thread == ev_thread) ev_thread = NULL;
	}
}


/****************************************************************************
* Finds the thread in the thread table matches the specified class reference.
*
* Input:        ref      reference to class object
* Output:       none
* Return:       ptr. to thread for matching class
*               NULL if not found
****************************************************************************/
thread_t *findThread(ref_t *ref)
{
	uint16 i;

/*
	printf("findThread(%lX)",ref);
*/
	for (i=0; i < config->threads; i++)
	{
		if ((threads[i].state != DEAD) && (threads[i].objref == ref))
			return &threads[i];
	}

	return NULL;
}



/***************************************************************************
* Sets the threads position in the queue of threads waiting for lock release.
*
* Input:        thread  - pointer to thread for which to set the queue position
*               ref     - object reference that has this lock. If NULL specified
*                         then lock is class wide.
* Output:       none
* Return:       none
****************************************************************************/
void setWaitingPos(thread_t *thread, ref_t *ref)
{
	uint16 i, pos = 0;

	/* check if the queue position has been already allocated */
	if (thread->wait_pos > 0)
		return;

	/* place the thread at the end of the wait queue */
	for (i=0; i < config->threads; i++)
	{
		if ((threads[i].state == WAIT_LOCK) &&
			(threads[i].wait_obj == ref) && (pos < threads[i].wait_pos))
		{
			pos = threads[i].wait_pos;
		}
	}
	thread->wait_pos = (uint16)(pos + 1);
}

/***************************************************************************
* Clears the thread's position in the queue and decrements
* queue possition for all other threads waiting for the same lock.
*
* Input:        thread  - pointer to thread for which to clear the queue position
*               ref     - object reference that has this lock. If NULL specified
*                         then lock is class wide.
* Output:       none
* Return:       none
****************************************************************************/
void clearWaitingPos(thread_t *thread, ref_t *ref)
{
	uint16 i;

	/* decrement wait position of threads waiting for this object */
	for (i=0; i < config->threads; i++)
	{
		if ((threads[i].state == WAIT_LOCK) &&
			(threads[i].wait_obj == ref))
		{
			threads[i].wait_pos--;
		}
	}
	thread->wait_pos = 0;
	thread->state = READY;
}

/***************************************************************************
* Retrieves the next thread from the queue of threads waiting for lock to be
* released.
*
* Input:        ref     - object reference that has this lock. If NULL specified
*                         then lock is class wide.
* Output:       none
* Return:       Pointer to the next thread that was waiting for the lock release.
****************************************************************************/
thread_t *getWaitingThread(ref_t *ref)
{
	uint16 i, pos = 0xFFFF, index;

	/* find thread with lowest queue position */
	for (i=0; i < config->threads; i++)
	{
		if ((threads[i].state == WAIT_LOCK) && (threads[i].wait_obj == ref && pos >= threads[i].wait_pos))
		{
			pos = threads[i].wait_pos;
			index = i;
		}
	}

	if (pos == 0xFFFF)
		return NULL;

	return &threads[index];
}

/***************************************************************************
* Acquires lock for the object referenced by the input parameter. If the lock
* can't be acquired then the specified thread is placed into "wait for lock" state.
*
* Input:        thread   - pointer to thread that executes method trying to
*                         acquire object's lock
*               ref      - pointer to object's reference
*                          false if lock should apply on entire class
* Output:       none
* Return:       none
****************************************************************************/
void acquireObjectLock(thread_t *thread, ref_t *ref)
{
	if (thread->wait_locks > 0 && ref->ptr->locks == 0)
	{
		ref->ptr->locks = thread->wait_locks;
		ref->ptr->thread_id = thread->id;
		thread->wait_locks = 0;
		clearWaitingPos(thread, ref);
		return;
	}
	else if (ref->ptr->locks == 0 || ref->ptr->thread_id == thread->id)
	{
		ref->ptr->locks++;
		ref->ptr->thread_id = thread->id;
		clearWaitingPos(thread, ref);
		return;
	}

	/* suspend this thread and force another thread to run */
	setWaitingPos(thread, ref);
	thread->wait_obj = ref;
	thread->state = WAIT_LOCK;
}

/***************************************************************************
* Acquires class-wide lock that that prohibits all static methods from being
* executed by other threads.
* Note that class instance methods can still be executed by other threads.
* If the lock can't be acquired then the specified thread is placed into
* "wait for lock" state.
*
* Input:        thread   - pointer to thread that executes method trying to
*                         acquire lock on a class
* Output:       none
* Return:       none
****************************************************************************/
void acquireClassLock(thread_t *thread)
{
	uint16 cls_index;

	cls_index = thread->curr_frame->class_ptr->index;
	if (class_locks[cls_index].locks == 0 ||
		class_locks[cls_index].thread_id == (uint16)thread->id)
	{
		class_locks[cls_index].locks++;
		class_locks[cls_index].thread_id = thread->id;
		clearWaitingPos(thread, (ref_t *)(uint32)cls_index);
	}
	else
	{
		/* suspend this thread and force another thread to run */
		setWaitingPos(thread, (ref_t *)(uint32)cls_index);
		thread->wait_obj = (ref_t *)(uint32)cls_index;
		thread->state = WAIT_LOCK;
	}
}

/***************************************************************************
* Checks if the currently executed method acquired any locks that can be
* released so the other threads waiting on these locks can be resumed.
*
* Input:        none
* Output:       none
* Return:       true    - if locks has been releases or this thread doesn't
*                         own any locks
*               false   - if there was an error during release operation (shouldn't happen)
****************************************************************************/
bool releaseLocks(void)
{
	if ((thr_active->curr_frame->method->flags & (ACC_SYNCH | ACC_STATIC)) == ACC_SYNCH)
	{
		if (!releaseObjectLock(GET_REF(thr_active->curr_frame->locals[0].val)) && !handle_excep())
			return false;
	}
	else if ((thr_active->curr_frame->method->flags & (ACC_SYNCH | ACC_STATIC)) == (ACC_SYNCH | ACC_STATIC))
	{
		if (!releaseClassLock() && !handle_excep())
			return false;
	}
	return true;
}


/***************************************************************************
* Releases the lock for the object referenced by the input parameter and resumes
* execution of the first thread waiting for this lock to be released.
*
* Input:        ref     - pointer to object's reference
* Output:       none
* Return:       none
****************************************************************************/
bool releaseObjectLock(ref_t *ref)
{
	thread_t *next_thread;

	if (thr_active->id != ref->ptr->thread_id || ref->ptr->locks == 0)
	{
		vm_excep = EXCEP_IllegalMonitState;
		return false;
	}

	ref->ptr->locks--;
	if (ref->ptr->locks == 0)
	{
		ref->ptr->thread_id = 0;
		/* resume the next thread waiting for this lock to be released */
		next_thread = getWaitingThread(ref);
		if (next_thread != NULL)
		{
			next_thread->state = READY;
			next_thread->wait_obj = NULL;
			acquireObjectLock(next_thread, ref);
		}
	}

	return true;
}

/***************************************************************************
* Releases the lock for the object referenced by the input parameter and resumes
* execution of the first thread waiting for this lock to be released.
*
* Input:        ref     - pointer to object's reference
* Output:       none
* Return:       none
****************************************************************************/
static bool releaseClassLock(void)
{
	thread_t *next_thread;
	uint16 cls_index;

	cls_index = thr_active->curr_frame->class_ptr->index;
	if ((uint16)thr_active->id != class_locks[cls_index].thread_id ||
		class_locks[cls_index].locks == 0)
	{
		vm_excep = EXCEP_IllegalMonitState;
		return false;
	}

	class_locks[cls_index].locks--;
	if (class_locks[cls_index].locks == 0)
	{
		class_locks[cls_index].thread_id = 0;
		/* resume the next thread waiting for this class lock to be released */
		next_thread = getWaitingThread((ref_t *)(uint32)cls_index);
		if (next_thread != NULL)
		{
			next_thread->state = READY;
			acquireClassLock(next_thread);
		}
	}

	return true;
}



/*=========================
 * UVic ROSE Additions
 *=========================
 */

	/*===== General Queue Operations on Threads ===== */

  /* enqueue "p" at the end of "q" */
static void EnQ( thread_t **q, thread_t *p )
{
	thread_t  *tmp;

	p->next = NULL;
	if ((*q) == NULL)
		 (*q) = p;
	else {
		   /* "q" is not empty */
		tmp = *q;
		while ( tmp->next != NULL ) { tmp = tmp->next; }
		tmp->next = p;
	}

} /* end EnQ */


  /* remove the first element "p" from "q" */
static void DeQ( thread_t **q, thread_t **p )
{
	(*p) = (*q);
	if ( (*q) != NULL ) {
	   (*q) = (*q)->next;
	   (*p)->next = NULL;
	}

} /* end DeQ */


  /* add "p" at the front of "q" */
static void AddFront( thread_t **q, thread_t *p )
{
   p->next = (*q);
   (*q) = p;

} /* end AddFront */


   /* return a new thread descriptor from the DEAL pool */
   /* if DEAL pool is empty, then it returns NULL */
static thread_t *NewThread(void)
{
	thread_t  *th;

	DeQ( &dead_pool, &th );   /* tested ??? */

	return th;

} /* end NewThread */



  /* ===== ROSE Initialization ====== */

bool InitROSE(void)
{
	uint16 i;

	ev_thread = NULL;

	thr_count = 0;
	ready_q = NULL;

	/* initialize both mutexes and conditions tables */
	for (i=0; i < MAX_MUTEXES; i++)
	{
		mutexes[i].used = false;
		mutexes[i].owner = NULL;
		mutexes[i].blockQ = NULL;
	}

	for (i=0; i < MAX_CONDITIONS; i++)
	{
		conditions[i].used = false;
		conditions[i].blockQ = NULL;
	}

	for (i=0; i < MAX_SEMAPHORES; i++)
	{
		semaphores[i].used = false;
		semaphores[i].blockQ = NULL;
	}

	/* assign unique ID to each entry in thread table */
	for (i=0; i < config->threads; i++)
	{
		threads[i].id = (uint8)(i + 1);
	threads[i].state = DEAD;
	}

	/* initially all threads are DEAD */
	dead_pool = &(threads[0]);
	for (i=0; i < (config->threads-1); i++)
		threads[i].next = &(threads[i+1]);
	threads[(config->threads-1)].next = NULL;

	/* add new entry to thread table for main() method */
	thr_active = NewThread();
	thr_active->objref = createObject(CLASS_LOC(APP->thread));
	if ( thr_active->objref != NULL )
	{
		thr_active->state = RUNNING;
	/* set to highest priority level */
	thr_active->pri= SYSTEM_LEVEL;
		AssignQuantum( thr_active );
		return true;
	}
	return false;

} /* end InitROSE */


/*================================
 * UVic Scheduler Primitives
 * (UVic/CS MHMC 22/Aug/2002)
 *================================
 */

  /** Assign fresh ticks to a thread. **/
static void AssignQuantum( thread_t *p )
{
	if (p == NULL)
		return;

	switch (p->pri) {
		default:
		case DAEMON_LEVEL:
		case SYSTEM_LEVEL:
			p->ticks = 0x7FFFFFFF; // Largest 32 bit signed integer (http://msdn.microsoft.com/en-us/library/ee621251.aspx)
			break;
		case REAL_TIME_LEVEL:
			p->ticks = 1;
			break;
		case USER_LEVEL:
			p->ticks = 2;
			break;
	}

	return;

} /* end AssignQuantum */



  /** current active thread is not runnable;
   * select a new ready thread and switch their contexts
   */
void Dispatch()
{
	if (ready_q == NULL) {
	fprintf( stderr, "(Dispatch) ERROR: Empty runnable queue!\n" );
	exit( 0 );
	}

	  /* save current active's context */
	thr_active->curr_frame->pc = vm_pc;
	thr_active->curr_frame->sp = vm_sp;

	DeQ(&(ready_q), &thr_active);

	thr_active->state = RUNNING;
	thr_active->next = NULL;


	  /* restore our new active's context */
	vm_pc = thr_active->curr_frame->pc;
	vm_sp = thr_active->curr_frame->sp;

} /* end Dispatch */


  /**
   * Add a new READY "thread" into the ready_q.
   * if "front" is true, then "thread" is added to the front of its level
   */
void AddReady(thread_t* thread, bool front)
{
	if (thread == NULL)
		return;

	thread->state = READY;
	thread->next = NULL;

	if(ready_q == NULL)
	{
		ready_q = thread;
	}
	else
	{
		thread_t* elem =  ready_q;
		if(front)
		{
			/* check the head of the ready queue */
			if(ready_q->pri >= thread->pri)
			{
				thread->next = ready_q;
				ready_q = thread;
				return;
			}

			/* skip over everything of greater priority */
			while((elem->next != NULL) && (elem->next->pri < thread->pri))
				elem = elem->next;
		}
		else
		{
			/* check the head of the ready queue */
			if(ready_q->pri > thread->pri)
			{
				thread->next = ready_q;
				ready_q = thread;
				return;
			}

			/* skip over everything of equal or greater priority */
			while((elem->next != NULL) && (elem->next->pri <= thread->pri))
				elem = elem->next;
		}

		thread->next = elem->next;
		elem->next = thread;
	}
} /* end AddReady */



  /**
   * check whether "thr_active" is still the highest priority
   * runnable thread; if not, then pre-empt it.
   * N.B. If preemption occurs, "thr_active" is added back to the
   *      front of its level.
   */
void PreemptIfNecessary(void)
{
// If active thread's priority is lower than something
// then reschedule
	thread_t* p;

	if (ready_q == NULL) return;

	/* Is "thr_active" still the highest priority runnable thread? */
	if ((thr_active == NULL) || (ready_q->pri < thr_active->pri)) { /* No */
		p = thr_active;

		/* select a new active thread */
		Dispatch();

		/* insert the old "thr_active" back to the front where it belongs */
		AddReady(p,true);
	}

} /* end PreemptIfNecessary */



  /** "thr_active" is given up voluntarily its share of processor;
   * select a new READY thread to run (remember to assign quantum)
   */
void Reschedule()
{
	if(thr_active != NULL)
	{
		AssignQuantum( thr_active );
		AddReady( thr_active, false );
	}
	Dispatch();

} /* end Reschedule */



  /** "thr_active" requests to change its priority level;
   * as a result, pre-emption may occur
   */
void SetLevel( int32 level )
{
	if ((level < 0) || (level >= MAX_PRI_LEVELS) ||
		(level == thr_active->pri)) return;

	thr_active->pri = level;
	AssignQuantum( thr_active );
	PreemptIfNecessary();

} /* end SetLevel */


  /** A tick has elapsed; it may be time to reschedule */
void VMTick(void)
{
	if ((thr_active != NULL) &&
		(thr_active->pri != SYSTEM_LEVEL) &&
		(thr_active->pri != DAEMON_LEVEL) &&
		((--thr_active->ticks) == 0))
		Reschedule();
	return;

} /* end VMTick */



/*================================
 * UVic native extension
 * (UVic/CS MHMC 22/Aug/2002)
 *================================
 */


  /* allocate a new Mutex */
int32 MutexInit()
{
	int32 i, idx;

	idx = INVALID;
	for( i=0; i < MAX_MUTEXES; ++i ) {
	  if (!mutexes[i].used) {
		 mutexes[i].used = true;
		 idx = i; break;
	  }
	}
	if (idx == INVALID) {
		fprintf( stderr, "ERROR: exhaust Mutexes!\n" );
		exit( 0 );
	}
	return idx;

} /* end MutexInit */



static mutex_t *MutexOf( int32 id )
{
	if ((id < 0) || (id > MAX_MUTEXES) || (!mutexes[id].used)) {
	fprintf( stderr, "ERROR: invalid mutex %d!\n", id );
	exit( 0 );
	}
	return (&(mutexes[id]));

} /* end MutexOf */


void MutexLock( int32 id )
{
	mutex_t *m;

	m = MutexOf( id );
	if (m->owner == NULL) { /* this mutex is free */
		m->owner = thr_active;
	} else { /* it is locked  */
		thr_active->state = BLOCK_ON_MUTEX;
		EnQ( &(m->blockQ), thr_active );
		/* select another thread to run */
		Dispatch();
	}

} /* end MutexLock */



void MutexUnLock( int32 id )
{
	mutex_t *m;
	thread_t *p;

	m = MutexOf( id );
	if (m->owner == thr_active) { /* I'm the owner */
		/* Is anyone waiting? */
		if (m->blockQ != NULL) { /* yes */
			/* Let's grab the first queued locked thread,
			   lock the mutex for it and possibly let it run. */
			DeQ( &(m->blockQ), &p );
			m->owner = p;
			AssignQuantum( p );
			AddReady( p, false );
			PreemptIfNecessary();
		}
		else {
			m->owner = NULL;  /* it is free now! */
		}
	}
} /* end MutexUnLock */



static condition_t *ConditionOf( int32 id )
{
	if ((id < 0) || (id > MAX_CONDITIONS) || (!conditions[id].used)) {
	fprintf( stderr, "Error: invalid condition variable %d!\n", id );
	exit( 0 );
	}
	return (&(conditions[id]));

} /* end ConditionOf */



  /* allocate a new Condition */
int32 CondInit()
{
	int32 i, idx;

	idx = INVALID;
	for( i=0; i < MAX_CONDITIONS; ++i ) {
	  if (!conditions[i].used) {
		 conditions[i].used = true;
		 idx = i;
		 break;
	  }
	}
	if (idx == INVALID) {
		fprintf( stderr, "ERROR: exhaust Condition variables!\n" );
		exit( 0 );
	}
	return idx;

} /* end CondInit */



void CondWait( int32 cond_id, int32 mutex_id )
{
	mutex_t      *m;
	condition_t  *c;
	thread_t     *p;

	m = MutexOf( mutex_id );
	c = ConditionOf( cond_id );
	/* TO BE WRITTEN BY YOU! */
	p = thr_active;
	//Unlock the mutex
	MutexUnLock( mutex_id );
	//store the mutex so we can relock it
	//printf("Stored mutex:%p in thr_active's relock.\n",m);
	p->relock = m;
	//printf("Relock mutex is %p\n",m);
	//set active thread's state to blocked on condition
	p->state = BLOCK_ON_COND;
	//Enqueue the active thread
	EnQ(&(c->blockQ),p);
	//Dispatch a new thread.
	//printf("Last Mutex id: %u\n",mutex_id);
	Dispatch();

} /* end CondWait */

void CondSignal( int32 cond_id )
{
	condition_t  *c;
	mutex_t      *m;
	thread_t     *p;

	c = ConditionOf( cond_id );

	/* TO BE WRITTEN BY YOU! */

	if(c->blockQ == NULL){
		//printf("c->blockQ is empty!\n");
	}else{
		if(thr_active->relock == NULL){
			printf("thr_active->relock is null!\n");
			m = c->blockQ->relock;
		}else{
			m = thr_active->relock;
		}
		DeQ(&(c->blockQ),&p);
		p->state = BLOCK_ON_MUTEX;
		EnQ(&(m->blockQ),p);
	}

	/*
	//wake up first thread in c->blockQ
	DeQ(&(c->blockQ),&p);
	if(p==NULL){
		//there's nothing to signal
		//printf("c->blockQ returned null!\n");
	}else{
		//printf("fetched thread %p\n",p);
		m = p->relock;
		if(m == NULL){
			//printf("mutex from thread %p returned null!\n",p);
			m = c->blockQ->relock;
		}else{
			//printf("Fetched mutex %p owner:%p\n",m,m->owner);
			m->owner = thr_active;
		}
		//add thread to m->blockQ
		EnQ(&(m->blockQ),p);
		//printf("Just EnQ'd\n");
	}*/



} /* end CondSignal */

void CondBroadcast( int32 cond_id )
{
	condition_t  *c;
	mutex_t      *m;
	thread_t     *p;
	thread_t	 *tmp;
	c = ConditionOf( cond_id );
	/* TO BE WRITTEN BY YOU! */
	if(c->blockQ == NULL){
		//printf("c->blockQ is empty!\n");
	}else{
		//fetch the mutex we need to lock
		m = thr_active->relock;
		if(m == NULL){
			//printf("thr_active->relock is null\n");
			m = c->blockQ->relock;
			//printf("Relocking mutex %p\n",m );
		}
		//set the mutex's owner to thr_active so it unlocks properly
		m->owner = thr_active;
		//printf("I am %p mutex owner is %p\n",thr_active,m->owner);
		while(c->blockQ != NULL){
			DeQ(&(c->blockQ),&tmp);
			tmp ->state = BLOCK_ON_MUTEX;
			//printf("thread state is %u\n",tmp ->state);
			EnQ(&(m->blockQ),tmp);
			//printf("thread %p to m->blockQ.\n",tmp);
		}
	}
} /* end CondBroadcast */


  /* allocate a new Semaphore */
int32 SemInit( int initial )
{
	int32 i, idx;

	if (initial < 0) {
		fprintf( stderr, "ERROR: Negative initial semaphore value %d!\n", initial);
		exit(0);
	}

	idx = INVALID;
	for( i=0; i < MAX_SEMAPHORES; ++i ) {
	  if (!semaphores[i].used) {
		 semaphores[i].used = true;
		 idx = i; break;
	  }
	}
	if (idx == INVALID) {
		fprintf( stderr, "ERROR: exhaust Semaphores!\n" );
		exit( 0 );
	}
	semaphores[idx].val = initial;
	return idx;

} /* end SemInit */



static semaphore_t *SemaphoreOf( int32 id )
{
	if ((id < 0) || (id > MAX_SEMAPHORES) || (!semaphores[id].used)) {
	fprintf( stderr, "ERROR: invalid semaphore %d!\n", id );
	exit( 0 );
	}
	return (&(semaphores[id]));

} /* end SemaphoreOf */


void SemWait( int32 id )
{
	semaphore_t *s;

	s = SemaphoreOf( id );
	--(s->val);
	if (s->val < 0) {
		thr_active->state = BLOCK_ON_SEM;
		EnQ( &(s->blockQ), thr_active );
		  /* select another thread to run */
	Dispatch();
	}

} /* end SemWait */



void SemSignal( int32 id )
{
	semaphore_t *s;
	thread_t *p;

	s = SemaphoreOf( id );
	++(s->val);
	if (s-> val <= 0) { /* someone was waiting! */
		DeQ( &(s->blockQ), &p );
	AssignQuantum( p );
		AddReady( p, false );
		PreemptIfNecessary();
	}

} /* end SemSignal */




/*================================================
 * End of UVic ROSE addition and native extension
 *================================================
 */


