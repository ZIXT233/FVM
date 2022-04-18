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
static const int recordTypeFinanceDirect[6] = { 0,-1,1,1,1,0 };
static char recordType[6][20] = { "","进货记录","销售记录","更新记录","赠送记录","销毁记录" };
static char typeQuantityText[6][20] = { "","采购量","销售量","变化量","赠予量","销毁量"};
static char typeAmountText[6][20] = { "","采购总额","销售总额","总价变化","赠品额外收费",""};
static char typeUpriceText[6][20] = { "","采购单价","销售单价","售价变化","赠品额外单价",""};

static const int RecordTypeNum = 5;
static const int RecordIDBase = 0000;
Record* recordCreate();
void recordDel(Record* pos); //析构
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
		pos = recordEntry(&head->timeList, timeList);\
	}\
	else if (recordType == INV_RECORDS) {\
		pos = recordEntry(&head->IRList, IRList);\
	}\
	while (1){\
		if (recordType == TIME_RECORDS) {\
			pos = recordEntry(pos->timeList.next, timeList);\
		} else if (recordType == INV_RECORDS) {\
			pos = recordEntry(pos->IRList.next, IRList);\
		}\
		if (recordType == TIME_RECORDS) {\
			if (&pos->timeList == head->timeList.root) break;\
		}\
		else if (recordType == INV_RECORDS) {\
			if (&pos->IRList == head->IRList.root) break;\
		}


#define recordForEachEnd }}

#endif // !RECORD_H