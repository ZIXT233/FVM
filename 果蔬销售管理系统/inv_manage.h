#ifndef INVMANAGE_H
#define INVMANAGE_H
#include"fvm_objects.h"
#include"UI.h"
void invManage(FVMO gdata);
void purchase(FVMO gdata);
Inventory* creatRandInv();
void invDetails(Renderer* renderer, Inventory* inv);
void recordPage(FVMO gdata);
void invRecordPage(Record* invRecord);
void recDetails(Renderer* renderer, Record* record);
void randomPurchase(FVMO gdata);

static const Coord invListPos = { 2,3 }, invMenuPos = { 21,3 };
#endif