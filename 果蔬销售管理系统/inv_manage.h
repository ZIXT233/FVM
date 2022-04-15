#ifndef INVMANAGE_H
#define INVMANAGE_H
#include"fvm_objects.h"
#include"UI.h"
static const Coord invListPos = { 2,3 }, invMenuPos = { 21,3 };
void invManage(FVMO gdata);
void purchase(FVMO gdata);
Inventory* creatRandInv();
void invDetails(Inventory* inv,FVMO gdata);
void recordPage(FVMO gdata);
void invRecordPage(Record* invRecord,FVMO gdata);
void recDetails(Record* record,FVMO gdata);
void randomPurchase(FVMO gdata);


#endif