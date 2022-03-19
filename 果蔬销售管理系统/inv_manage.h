#ifndef INVMANAGE_H
#define INVMANAGE_H
#include"fvm_objects.h"
#include"UI.h"
void invManage(FVMO gdata);
void purchase(FVMO gdata);
Inventory* creatRandInv();
void invDetails(Inventory* inv);
void recordPage(FVMO gdata);
void invRecordPage(Record* invRecord);
void recDetails(Record* record);
void randomPurchase(FVMO gdata);
#endif