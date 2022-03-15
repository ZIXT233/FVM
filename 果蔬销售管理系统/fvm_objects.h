#ifndef FVMOBJECTS_H
#define FVMOBJECTS_H
#include"inventory.h"
#include"sale_plan.h"

typedef struct tagFVMObjects {  //Store system's global data
	Inventory* inventory;
	Record* record;
	SingleSalePlan* SSP;
	ComSalePlan* CSP;
}FVMObjects, FVMO;
#endif