/**
 * initsys.c
 */

/*---- Hardware module includes ----*/


#include "initsys.h"
#include "drivers.h"
#include "intr.h"
#include "accel.h"
#include "tplink.h"

/**
 * init_system:
 * 	Called after processor reset.
 */
void
init_system( void )
{
	/*---- Hardware initialization ----*/
	init_critical();

  	drivers_init();

	tplink_init();
	enable_interrupt();
  	
	/*---- Software initialization ----*/

	init_accelerometer();
}
