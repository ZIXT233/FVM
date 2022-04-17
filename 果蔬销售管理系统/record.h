#ifndef RECORD_H
#define RECORD_H

#include"list.h"
#include"product.h"
#include"finance.h"
#include<stdlib.h>


typedef struct tagRecord {
	Product prod;
	enum RecordType { PURCHASE=1, SALE, UPDATE, GIFT, DESTROY} type;
	int invID, recID;
	int SSPID, CSPID;
	int recIDCnt;
	time_t time;
	char addInfo[INFOMAX];
	double discount;
	ListHead timeList;  //master,default list
	ListHead IRList;  //records of same inventory
	time_t lastTime;  //used as filter
}Record;
static const int recordTypeProdDirect[6] = { 0,1,-1,1,-1,-1 };
static const int recordTypeFinanceDirect[6] = { 0,-1,1,0,1,0 };

static const int RecordTypeNum = 5;
static const int RecordIDBase = 0000;
Record* recordCreate();
void recordDel(Record* pos); //Îö¹¹
#define recordEntry(ptr,member) listEntry(ptr, Record, member)

Record* recordListInit(Record* head);
void recordIDAllocate(Record *pos,Record* head);
void recordListClear(Record* head);
Record* recordQueryID(Record* head, int recID, int direct);
int recordMatch(const Record* rec, const Record* filter);
#define TIME_RECORDS 0
#define INV_RECORDS 1
Record* recordFilterListGen(const Record* head, int type, const Record* filter);
Record* recordShowPageJump(const Record* head, int type, int* pageStart, const int pageSize);
void recordStatsQuantity(const Record* head, int type, const Record* filter, int quantityTable[]);
void recordStatsWeight(const Record* head, int type, const Record* filter, double weightTable[]);
Finance recordStatsFinance(const Record* head, int type, const Record* filter, double startUpCapital);



#define recordForEachStart(pos,head,recordType) {\
	Record* pos=NULL;\
	if (recordType == TIME_RECORDS) {\
		pos = recordEntry(head->timeList.next, timeList);\
	}\
	else if (recordType == INV_RECORDS) {\
		pos = recordEntry(head->IRList.next, IRList);\
	}\
	while (1){\
		if (recordType == TIME_RECORDS) {\
			if (&pos->timeList == head->timeList.root) break;\
		}\
		else if (recordType == INV_RECORDS) {\
			if (&pos->IRList == head->IRList.root) break;\
		}


#define recordForEachEnd(pos,head,recordType) \
		if (recordType == TIME_RECORDS) {\
			pos = recordEntry(pos->timeList.next, timeList);\
		} else if (recordType == INV_RECORDS) {\
			pos = recordEntry(pos->IRList.next, IRList);\
		}\
	}\
}

#endif // !RECORD_H