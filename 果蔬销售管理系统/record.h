#ifndef RECORD_H
#define RECORD_H

#include"list.h"
#include"product.h"
#include<stdlib.h>

typedef struct tagRecord {
	Product prod;
	enum RecordType { PURCHASE=1, SALE, UPDATE, GIFT } type;
	int invID, recID;
	time_t time;
	char addInfo[INFOMAX];
	ListHead timeList;  //master,default list
	ListHead IRList;  //records of same inventory
	time_t lastTime;  //used as filter
}Record;



inline Record* recordCreate() {
	return (Record*)malloc(sizeof(Record));
}
inline void recordDel(Record* pos) { //Îö¹¹
	free(pos);
}
#define recordEntry(ptr,member) listEntry(ptr, Record, member)

Record* recordListInit(Record* head);
Record* recordQueryID(Record* head, int recID, int direct);

#endif // !RECORD_H