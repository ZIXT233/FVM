#include"record.h"



Record* recordListInit(Record* head) {
	listInit(&head->timeList);
	listInit(&head->IRList);
	return head;
}

void recordListClear(Record* head) {
	listForEachEntrySafe(Record, pos, &head->IRList, IRList) {
		;
	}
}