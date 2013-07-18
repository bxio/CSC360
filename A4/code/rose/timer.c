#define __timer_t_defined

#include <stdio.h>
#include <windows.h>

#include "jcommon.h"

/*=====================
 * UVIC ROSE Addition
 *=====================
 */

bool InitVMTimer(void) {

    LARGE_INTEGER frequency;

     /* Initialize the vm system timer */
    QueryPerformanceFrequency(&frequency);
    vm_time_frq = frequency.QuadPart;
    if ( vm_time_frq == 0 ) {
      printf( "This computer does not support High Resolution Performance Counter!\n" );
      return(false);
    }
/*
    printf("Timer frequency = %ld Hz\n", vm_time_frq);
*/

    return (true);

} /* InitVMTimer */


void StartVMTimer(void) {

    vm_time_ms = 0;
    QueryPerformanceCounter(&vm_timer_start);

} /* end StartVMTimer */


void UpdateVMTimer(void) {

    LARGE_INTEGER current;

    QueryPerformanceCounter(&current);

    vm_time_ms = (current.QuadPart - vm_timer_start.QuadPart) * 1000 / vm_time_frq;

} /* end UpdateVMTimer */
