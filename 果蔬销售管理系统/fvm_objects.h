#ifndef FVMOBJECTS_H
#define FVMOBJECTS_H
#include<stdlib.h>
#include<stdint.h>
#include"inventory.h"
#include"sale_plan.h"
#include"renderer.h"
#include"timer.h"
#include"finance.h"


#define PAGE_NAME_MAX 256
typedef struct tagPageStack {
	char pageName[PAGE_NAME_MAX];
	ListHead list;
}PageStack;

static inline PageStack* pageStackCreate(char *pageName) {
	PageStack* page=(PageStack*)malloc(sizeof(PageStack));
	if(pageName)strcpy_s(page->pageName, PAGE_NAME_MAX, pageName);
	listInit(&page->list);
	return page;
}
static inline void pageStackDel(PageStack* pos) {
	listRemove(&pos->list);
	free(pos);
}
static inline void pageStackPush(PageStack* _new, PageStack* head) {
	listAddTail(&_new->list, &head->list);
}
static inline PageStack* pageStackTop(PageStack* head) {
	return listEntry(head->list.prev,PageStack, list);
}
static inline void pageStackPop(PageStack* head) {
	PageStack* tmp= listEntry(head->list.prev, PageStack, list);
	listRemove(head->list.prev);
	free(tmp);
}

typedef struct tagFVMObjects {  //Store system's global data
	Inventory* inventory,*historyInventory;
	Record* record,*order;
	SingleSalePlan* SSP;
	ComSalePlan* CSP;
	Renderer* renderer;
	FVMTimer* timer;
	PageStack* pageStack;
	Finance* finance;
	uint8_t passwdSha256[32];
}FVMObjects, FVMO;


#endif