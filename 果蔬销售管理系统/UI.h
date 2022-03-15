#ifndef UI_H
#define UI_H
#include<stdio.h>
#include"fvm_objects.h"
void cls();
void drawMenu(const char* title,int n,...);
int getSelect();
void drawInvList(Inventory* start,int len);
void readline(char* buf, int maxCount, FILE* stream);
#endif