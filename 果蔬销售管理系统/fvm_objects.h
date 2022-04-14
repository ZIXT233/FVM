#ifndef FVMOBJECTS_H
#define FVMOBJECTS_H
#include"inventory.h"
#include"sale_plan.h"
#include"renderer.h"

typedef struct tagFVMObjects {  //Store system's global data
	Inventory* inventory;
	Record* record,*order;
	SingleSalePlan* SSP;
	ComSalePlan* CSP;
	Renderer* renderer;
}FVMObjects, FVMO;
#endif