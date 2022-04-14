#ifndef UI_H
#define UI_H
#include<stdio.h>
#include"fvm_objects.h"
#include"renderer.h"

static const Coord UI_ORIGIN = { 1,1 }, INPUT_ORIGIN = { 35,1 }, NAN_COORD = { -1,-1 }, STATUS_ORIGIN = { 1,1 };
static const Coord PanelSize = { 35,185 };

void drawRectBorder(Renderer* renderer, Coord pos, Coord const size);
void drawMenu(Renderer* renderer, Coord pos, const char* title, int n, int firstNum, ...);
void drawStatusBar(Renderer* renderer, Coord origin, FVMO gdata);
void drawInvPage(Renderer* renderer, Coord pos, const char* title, Inventory* start, int pageStartNum, int pageSize);
typedef void (*ListDrawer) (Renderer*, Coord, ListHead*, int,void*);
void drawListPage(Renderer* renderer, Coord origin, const char* title, ListDrawer drawer, ListHead* start, int *pageStartNum, int pageSize, Coord rectSize,void* exArg);
void drawSSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
void drawCSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
void drawComInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
void drawPreOrderList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
void drawGiftList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
void drawInvList(Renderer* renderer, Coord pos, Inventory* start, int height);
void drawRecordList(Renderer* renderer, Coord pos, Record* start, int record, int height);
void showProductDetails(Product* prod);
void showInvDetails(Renderer* renderer,Coord pos,Inventory* inv);
void showRecordDetails(Renderer* renderer,Coord pos,Record* rec);
void showSSPDetails(Renderer* renderer, Coord pos, SSP* ssp);
void showCSPDetails(Renderer* renderer, Coord pos, CSP* csp);
void drawCart(Renderer* renderer, Coord origin, Inventory* start, int height);
void drawTimeBar(Renderer* renderer,FVMTimer* timer);
void clockUpdateTimeBar(void* arg);
void inputStart(Renderer* renderer, Coord inputOrigin);

#define INPUT_BREAK -2
#define INPUT_NOCHANGE -1
#define INPUT_SUCCESS 0
#define breakDeliver(input) if(input==INPUT_BREAK) return INPUT_BREAK;
#define breakCatch(input) if(input==INPUT_BREAK)

int getSelect();
int readline(char* buf, int maxCount, FILE* stream,int *pNumberRead);
void drawOrdMenu(const char* title, int n, int firstNum, ...);
int inputProduct(Product* prod);
int inputGift(Inventory ** pGift, Inventory * invHead);
int inputProductFilter(Product* prod);
int inputRecordFilter(Record* rec);
int inputInventoryID(const Inventory* head, int* id, Inventory** pInv);
int inputSSPID(const SSP * head, int* id, SSP * *pSSP);
int inputCSPID(const CSP * head, int* id, CSP * *pCSP);
int getUIntInput(const char* query, int* value, IntRange range, bool strict);
int getStrInput(const char* query, char* value, int maxCount, bool strict);
int getDoubleInput(const char* query, double* value, DoubleRange range, bool strict);
int getDateTime(const char* query, time_t* value, bool strict); //默认为当天00:00
#endif