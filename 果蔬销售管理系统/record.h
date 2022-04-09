#ifndef RECORD_H
#define RECORD_H

#include"list.h"
#include"product.h"
#include<stdlib.h>

typedef struct tagRecord {
	Product prod;
	enum RecordType { PURCHASE=1, SALE, UPDATE, GIFT } type;
	int invID, recID;
	int SSPID, CSPID;
	time_t time;
	char addInfo[INFOMAX];
	ListHead timeList;  //master,default list
	ListHead IRList;  //records of same inventory
	time_t lastTime;  //used as filter
}Record;



Record* recordCreate();
void recordDel(Record* pos); //Îö¹¹
#define recordEntry(ptr,member) listEntry(ptr, Record, member)

Record* recordListInit(Record* head);
void recordListClear(Record* head);
Record* recordQueryID(Record* head, int recID, int direct);
int recordMatch(const Record* rec, const Record* filter);
#define TIME_RECORDS 0
#define INV_RECORDS 1
Record* recordFilterListGen(const Record* head, int type, const Record* filter);
Record* recordShowPageJump(const Record* head, int type, int* pageStart, const int pageSize);

#endif // !RECORD_H