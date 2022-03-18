#ifndef UI_H
#define UI_H
#include<stdio.h>
#include"fvm_objects.h"

void cls();
void drawMenu(const char* title,int n,...);
int getSelect();
void drawInvList(Inventory* start,int len);
int readline(char* buf, int maxCount, FILE* stream);

#define INPUT_BREAK -2
#define INPUT_NOCHANGE -1
#define INPUT_SUCCESS 0
#define breakDeliver(input) if(input==INPUT_BREAK) return INPUT_BREAK;
int inputProduct(Product* prod);
int getUIntInput(const char* query, int *value,bool strict);
int getStrInput(const char* query, char* value,  int maxCount,bool strict);
int getDoubleInput(const char* query, double *value,bool strict);
int getDateTime(const char* query, time_t *value,bool strict); //默认为当天零时
void showProductDetails(Product * prod);
void showInvDetails(Inventory *inv);
void showRecordDetail(Record * rec);
#endif