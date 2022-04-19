#ifndef UI_H
#define UI_H
#include<stdio.h>
#include"fvm_objects.h"
#include"renderer.h"

static const Coord UI_ORIGIN = { 1,1 }, INPUT_ORIGIN = { 35,1 }, NAN_COORD = { -1,-1 }, STATUS_ORIGIN = { 1,1 };
static const Coord PanelSize = { 35,200 };

void drawRectBorder(Renderer* renderer, Coord pos, Coord const size);
void drawMenu(Renderer* renderer, Coord pos, Coord rectSize, const char* title, int n, int firstNum, ...);
void drawTitleWindow(Renderer* renderer, Coord origin, const char* title, Coord rectSize);
void drawStatusBar(Renderer* renderer, Coord origin, FVMO *gdata);


typedef void (*ListDrawer) (Renderer*, Coord, ListHead*, int,void*);
void drawListPage(Renderer* renderer, Coord origin, const char* title, ListDrawer drawer, ListHead* start, int *pageStartNum, int pageSize, Coord rectSize,void* exArg);
typedef struct tagCellData {
	void (*drawer) (Renderer* renderer, Coord origin, struct tagCellData* cell);
	int width, sign;
	const void* data;
	char* unitName;
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
void drawCellQuality(Renderer* renderer, Coord origin, CellData* cell);
void drawCellTime(Renderer* renderer, Coord origin, CellData* cell);
void drawCellDate(Renderer* renderer, Coord origin, CellData* cell);
void drawCellSSP(Renderer* renderer, Coord origin, CellData* cell);
void drawCellCSP(Renderer* renderer, Coord origin, CellData* cell);
void drawListItem(Renderer* renderer, Coord origin, CellData* cells, int cellCount);
typedef struct tagInvStatsArg {
	Record* filter;
	FVMO* gdata;
}InvStatsArg;

//参考
static const Coord statsRectSize = { 19,122 };
void drawInvStatsList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
 

//从这里
static const Coord SSPRectSize = { 16,30 };
void drawSSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord CSPRectSize = { 16,30 }, optCSPRectSize = { 16,30 };
void drawCSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord  CSPComInvRectSize = { 16,40 };
void drawComInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord GiftRectSize = { 16,66 };
void drawGiftList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord invListRectSize = { 16,75 };
void drawInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
//到这里
static const Coord invManageListRectSize = { 16,98 };
void drawInvManageList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord RecordRectSize = { 16,120 };
void drawRecordList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord  PreOrderRectSize = { 16,126 };
void drawPreOrderList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);
static const Coord InvCheckRectSize = { 16,146 };
void drawInvCheckList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg);

static const Coord ProdDetailsRectSize = { 5,75 };
void showProductDetails(Renderer* renderer, Coord pos, Product* prod, int lineNum);
static const Coord InvDetailsRectSize = { 8,75 };
void showInvDetails(Renderer* renderer,Coord pos,Inventory* inv);
static const Coord RecordDetailsRectSize = { 13,75 };
void showRecordDetails(Renderer* renderer,Coord pos,Record* rec);
static const Coord SSPDetailsRectSize = { 18,53 };
void showSSPDetails(Renderer* renderer, Coord pos, SSP* ssp);
static const Coord CSPDetailsRectSize = { 18,38 };
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
int inputSaleRecordFilter(Record* rec);
int inputInventoryID(const Inventory* head, int* id, Inventory** pInv);
int inputInventoryIDHistory(const Inventory* head, const Inventory* historyHead, int* id, Inventory** pInv);
int inputRecordID(const Record* head, int recordType, int* id, Record** pRec);
int inputSSPID(const SSP * head, int* id, SSP * *pSSP);
int inputCSPID(const CSP * head, int* id, CSP * *pCSP);
int getUIntInput(const char* query, int* value, IntRange range, bool strict);
int getStrInput(const char* query, char* value, int maxCount, bool strict);
int getDoubleInput(const char* query, double* value, DoubleRange range, bool strict);
int getDateTime(const char* query, time_t* value, bool strict); //默认为当天00:00
int getTimeInput(const char* query, time_t* value, bool strict);
#endif