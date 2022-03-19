#include"record.h"


Record* recordCreate() {
	return (Record*)malloc(sizeof(Record));
}
void recordDel(Record* pos) { //Îö¹¹
	free(pos);
}
Record* recordListInit(Record* head) {
	head->recID = 0;
	head->time = 1;
	listInit(&head->timeList);
	listInit(&head->IRList);
	return head;
}

void recordListClear(Record* head) {
	listForEachEntrySafe(Record, pos, &head->timeList, timeList) {
		recordDel(pos);
	}
}
Record* recordQueryID(Record* head, int recID,int direct) {
	if (direct == 0) {
		listForEachEntry(Record, pos, &head->timeList, timeList) {
			if (pos->recID == recID) {
				return pos;
			}
		}
	}
	else if (direct = 1) {
		listForEachEntry(Record, pos, &head->IRList, IRList) {
			if (pos->recID == recID) {
				return pos;
			}
		}
	}
	return NULL;
}