#ifndef RECORD_H
#define RECORD_H

#include"list.h"
#include"product.h"
#include<stdlib.h>
typedef struct tagRecord {
	ProductAttr attr;
	enum RecordType { PURCHASE, SALE, UPDATE, GIFT } type;
	int invId;
	time_t date;
	double amount;
	int quantity;
	double weight;
	char addInfo[INFOMAX];
	ListHead timeList;
	ListHead IRList;  //records of same inventory
}Record;

inline Record* recordCreate() {
	return (Record*)malloc(sizeof(Record));
}
inline void recordDel(Record* pos) { //Îö¹¹
	free(pos);
}
#define recordEntry(ptr,member) listEntry(ptr, Record, member)

Record* recordListInit(Record* head);

#endif // !RECORD_H