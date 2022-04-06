#ifndef UI_H
#define UI_H
#include<stdio.h>
#include"fvm_objects.h"

typedef struct tagCoord {
	int x;
	int y;
}Coord;

static const Coord UI_ORIGIN = { 1,1 }, INPUT_ORIGIN = { 37,1 }, NAN_COORD = { -1,-1 };
static const Coord PanelSize = { 35,180 };
Coord getCursorPos();
int coordPrintf(Coord pos, const char* const _Format, ...);
void drawMenu(Coord pos, const char* title, int n, int firstNum, ...);
void drawInvPage(Coord pos, const char* title, Inventory* start, int pageStartNum, int pageSize);
void drawInvList(Coord pos, Inventory* start, int height);
void drawRecordList(Coord pos, Record* start, int record, int height);
void showProductDetails(Product* prod);
void showInvDetails(Inventory* inv);
void showRecordDetails(Record* rec);
void drawRectBorder(Coord pos, Coord const size);
void drawCart(Coord origin, Inventory* start, int height);
void inputStart(Coord inputOrigin);

#define INPUT_BREAK -2
#define INPUT_NOCHANGE -1
#define INPUT_SUCCESS 0
#define breakDeliver(input) if(input==INPUT_BREAK) return INPUT_BREAK;
#define breakCatch(input) if(input==INPUT_BREAK)

int getSelect();
int readline(char* buf, int maxCount, FILE* stream);
void drawOrdMenu(const char* title, int n, int firstNum, ...);
int inputProduct(Product* prod);
int inputProductFilter(Product* prod);
int inputRecordFilter(Record* rec);
int inputInventoryID(const Inventory* head, int* id, Inventory** pInv);
int getUIntInput(const char* query, int* value, IntRange range, bool strict);
int getStrInput(const char* query, char* value, int maxCount, bool strict);
int getDoubleInput(const char* query, double* value, DoubleRange range, bool strict);
int getDateTime(const char* query, time_t* value, bool strict); //默认为当天00:00
#endif