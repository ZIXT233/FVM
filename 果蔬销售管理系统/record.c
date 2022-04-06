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
Record* recordQueryID(Record* head, int recID, int direct) {
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

int recordMatch(const Record* rec, const Record* filter) {
	if (filter == NULL) return 1;
	if (!productMatch(&rec->prod, &filter->prod))return 0;
	if (filter->type != 0 && filter->type != rec->type) return 0;
	if (filter->time != TIME_NAN && filter->time > rec->time) return 0;
	if (filter->lastTime != TIME_NAN && filter->lastTime < rec->time) return 0;
	return 1;
}

Record* recordFilterListGen(const Record* head, int type,const Record* filter) {
	Record* filterList = recordListInit(recordCreate()), * cp;
	if (type == TIME_RECORDS) {
		listForEachEntry(Record, pos, &head->timeList, timeList) {
			if (!recordMatch(pos, filter)) continue;
			cp = recordCreate();
			memcpy(cp, pos, sizeof(Record));
			listAddTail(&cp->timeList, &filterList->timeList);
		}
	}
	else if (type == INV_RECORDS) {
		listForEachEntry(Record, pos, &head->IRList, IRList) {
			if (!recordMatch(pos, filter)) continue;
			cp = recordCreate();
			memcpy(cp, pos, sizeof(Record));
			listAddTail(&cp->IRList, &filterList->IRList);
		}
	}
	return filterList;
}

Record* recordShowPageJump(const Record* head,int type, int* pageStart, const int pageSize) {
	Record* showPage = head;
	while (1) {
		int num = 1;
		if (type == TIME_RECORDS) {
			listForEachEntry(Record, pos, &head->timeList, timeList) {
				if (num == *pageStart) {
					showPage = recordEntry(pos->timeList.prev, timeList);
					break;
				}
				num++;
			}
		}else if (type == INV_RECORDS) {
			listForEachEntry(Record, pos, &head->IRList,IRList ) {
				if (num == *pageStart) {
					showPage = recordEntry(pos->IRList.prev, IRList);
					break;
				}
				num++;
			}
		}
		if (showPage != head || *pageStart == 1) return showPage;
		else *pageStart -= pageSize;
	}
}