#ifndef INVMANAGE_H
#define INVMANAGE_H
#include"fvm_objects.h"
#include"UI.h"
void invManage(FVMO gdata);
void purchase(FVMO gdata);
Inventory* creatRandInv();
void invDetails(FVMO gdata);
void recordPage(FVMO gdata);
void invRecordPage(Inventory* inv);
void recDetails(Record* record);

#endif