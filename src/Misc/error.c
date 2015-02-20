#include "error.h"
#include "../common.h"


struct {
	err_action emergency;
	err_print printMsg;
	err_action steady;

}err_data = {NULL, NULL, NULL};

void err_Init(err_action emergency, err_print printMsg, err_action steady)
{
	err_data.emergency = emergency;
	err_data.printMsg = printMsg;
	err_data.steady = steady;
}

void err_Throw(char* errMsg)
{
	if(err_data.emergency != NULL)
	{
		err_data.emergency();
	}

	if(err_data.printMsg != NULL)
	{
		err_data.printMsg(errMsg);
	}

	if(err_data.steady != NULL)
	{
		while(1)
		{
			err_data.steady();
		}
	}
	else
	{
		while(1);	// do nothing
	}

}
