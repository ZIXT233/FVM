#ifndef INVMANAGE_H
#define INVMANAGE_H
#include"fvm_objects.h"
#include"UI.h"
static const Coord invListPos = { 2,3 }, invMenuPos = { 22,3 }, invMenuSize = { 5,98 }, recordMenuSize = { 4,120 }, invRecordMenuSize = { 3,120 };
void invManage(FVMO *gdata);
void purchase(FVMO *gdata);
Inventory* creatRandInv();
void invDetails(Inventory* inv,FVMO *gdata);
void recordPage(FVMO *gdata);
void invRecordPage(Record* invRecord,FVMO *gdata);
void recDetails(Record* record,FVMO *gdata);
void randomPurchase(FVMO *gdata);
void invDestroyManage(FVMO *gdata);


#endif