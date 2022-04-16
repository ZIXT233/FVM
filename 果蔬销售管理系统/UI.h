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


typedef void (*ListDrawer) (Renderer*, Coord, ListHead*, int,void*);
void drawListPage(Renderer* renderer, Coord origin, const char* title, ListDrawer drawer, ListHead* start, int *pageStartNum, int pageSize, Coord rectSize,void* exArg);
typedef struct tagCellData {
	void (*drawer) (Renderer* renderer, Coord origin, struct tagCellData* cell);
	int width, sign;
	const void* data;
}CellData;
void drawVerticalLine(Renderer* renderer, Coord pos, int height);
void drawColorBar(Renderer* renderer, Coord origin, int r, int g, int b, int length);
void resetBackgroundColor(Renderer* renderer);
typedef void (*CellDataDrawer) (Renderer* renderer, Coord origin, CellData* cell);
void drawCellDouble(Renderer* renderer, Coord origin, CellData* cell);
void drawCellBULK(Renderer* renderer, Coord origin, CellData* cell);
void drawCellInt(Renderer* renderer, Coord origin, CellData* cell);
void drawCellUNIT(Renderer* renderer, Coord origin, CellData* cell);
void drawCellStr(Renderer* renderer, Coord origin, CellData* cell);
void drawCellTime(Renderer* renderer, Coord origin, CellData* cell);
void drawCellSSP(Renderer* renderer, Coord origin, CellData* cell);
void drawCellCSP(Renderer* renderer, Coord origin, CellData* cell);
void drawListItem(Renderer* renderer, Coord origin, CellData* cells, int cellCount);
typedef struct tagInvStatsArg {
	Record* filter;
	FVMO* gdata;
}InvStatsArg;

//参考
static const Coord statsRectSize = { 21,122 };
void drawInvStatsList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
 

//从这里
static const Coord SSPRectSize = { 18,30 };
void drawSSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord CSPRectSize = { 18,30 }, optCSPRectSize = { 18,30 };
void drawCSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord  CSPComInvRectSize = { 18,40 };
void drawComInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord GiftRectSize = { 18,80 };
void drawGiftList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord invListRectSize = { 18,75 };
void drawInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
//到这里

static const Coord RecordRectSize = { 18,140 };
void drawRecordList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord  PreOrderRectSize = { 18,133 };
void drawPreOrderList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);


void showProductDetails(Product* prod);
void showInvDetails(Renderer* renderer,Coord pos,Inventory* inv);
void showRecordDetails(Renderer* renderer,Coord pos,Record* rec);
void showSSPDetails(Renderer* renderer, Coord pos, SSP* ssp);
void showCSPDetails(Renderer* renderer, Coord pos, CSP* csp);


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