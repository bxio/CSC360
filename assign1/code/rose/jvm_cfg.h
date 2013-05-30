#ifndef _JVM_CFG_H_
#define _JVM_CFG_H_

  /* standard limits */
#define MAX_REFERENCES	1024
#define MAX_STRINGS	1024
#define MAX_THREADS	64    /* including the main() thread */
#define MAX_MUTEXES	32    /* max. no. of Mutexes supported */
#define MAX_CONDITIONS	32    /* max. no. of Conditions supported */
#define MAX_SEMAPHORES	32    /* max. no. of Semaphores supported */
#define MAX_HEAPSIZE	128000
#define MAX_TIMERS	0
#define MAX_EVENTS	0
#define MAX_PRI_LEVELS	4     /* 0 is the highest */

  /* priority levels */
#define SYSTEM_LEVEL    0
#define REAL_TIME_LEVEL 1
#define USER_LEVEL      2
#define DAEMON_LEVEL    3

#endif // _JVM_CFG_H_
