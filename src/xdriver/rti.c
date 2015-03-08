#include "rti.h"

#include <stdbool.h>
#include <stddef.h>

#include "driverlib/interrupt.h"
#include "driverlib/systick.h"

#define RTI_MAX_FCNS 10

#define RTI_IS_VALID_ID(id) (((id >= 0) && (id < RTI_MAX_FCNS)) ? true : false)

typedef struct rti_cb {
	rti_time period;
	rti_time count;
	rti_ptr callback;
	void *data;
} rti_cb_t;

bool rti_isInit = false;

struct rti_cb rti_tbl[RTI_MAX_FCNS];

void rti_Init()
{
	if (rti_isInit == true)
		return;
	
	rti_isInit = true;
	
	rti_id i;
	for (i = 0; i < RTI_MAX_FCNS; i++)
		rti_tbl[i].callback = NULL;

	SysTickPeriodSet(80000); // 1ms period for a 80MHz clock
	SysTickIntEnable();

	SysTickEnable();
	
	return;
}

rti_id rti_Register (rti_ptr callback, void *data, rti_time period, rti_time delay)
{
	bool ints_were_disabled = IntMasterDisable();

	rti_id i;
	for (i = 0; i < RTI_MAX_FCNS; i++) 
	{
		if (rti_tbl[i].callback == NULL) 
		{
			rti_tbl[i].callback = callback;
			rti_tbl[i].data = data;
			rti_tbl[i].period = period;
			rti_tbl[i].count = delay;
			break;
		}
	}

	if (ints_were_disabled == false)
	{
		IntMasterEnable();
	}
		
	if (i == RTI_MAX_FCNS)
	{
		while(1)
			; // error
	}

	return i;
}

void rti_SetPeriod(rti_id id, rti_time period)
{
	if (!RTI_IS_VALID_ID(id))
		return;
	
	rti_tbl[id].period = period;
	
	return;
}


void rti_Cancel(rti_id id)
{
	if (!RTI_IS_VALID_ID(id))
		return;
	
	rti_tbl[id].callback = NULL;
}


void rti_Service(void)
{
	// The SYSTICK interrupt is cleared automatically

	rti_id i;
	for (i = 0; i < RTI_MAX_FCNS; i++) 
	{
		if (rti_tbl[i].callback != NULL) 
		{
			if ((--rti_tbl[i].count) == 0) 
			{	
				rti_tbl[i].callback(rti_tbl[i].data, rti_tbl[i].period, i);
				
				if (rti_tbl[i].count != 0) // If the callback deleted itself and registered another function in the same place, count wont be 0
					break;
				
				if (rti_tbl[i].period == RTI_ONCE)
					rti_tbl[i].callback = NULL;
				else
					rti_tbl[i].count = rti_tbl[i].period;
			}
		}
	}

	return;
}
