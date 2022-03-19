#ifndef UI_H
#define UI_H
#include<stdio.h>
#include"fvm_objects.h"

void cls();
void drawMenu(const char* title, int n, int firstNum, ...);
int getSelect();

int readline(char* buf, int maxCount, FILE* stream);

#define INPUT_BREAK -2
#define INPUT_NOCHANGE -1
#define INPUT_SUCCESS 0
#define breakDeliver(input) if(input==INPUT_BREAK) return INPUT_BREAK;
#define breakCatch(input) if(input==INPUT_BREAK)
void drawInvList(Inventory* start, int len);
void drawRecordList(Record* start, int len);
int inputProduct(Product* prod);
int inputProductFilter(Product* prod);
int inputRecordFilter(Record* rec);
int getUIntInput(const char* query, int* value, IntRange range, bool strict);
int getStrInput(const char* query, char* value, int maxCount, bool strict);
int getDoubleInput(const char* query, double* value, DoubleRange range, bool strict);
int getDateTime(const char* query, time_t* value, bool strict); //默认为当天00:00
void showProductDetails(Product* prod);
void showInvDetails(Inventory* inv);
void showRecordDetails(Record* rec);
#endif