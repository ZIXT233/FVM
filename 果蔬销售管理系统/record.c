#include<string.h>
#include"record.h"
#include"inventory.h"


Record* recordCreate() {
	Record* _new = (Record*)malloc(sizeof(Record));
	memset(_new, 0, sizeof(Record));
	return _new;
}
void recordDel(Record* pos) { //析构
	free(pos);
}
void recordIDAllocate(Record* pos, Record* head) {
	pos->recID = ++head->recIDCnt;
}
Record* recordListInit(Record* head) {
	head->recID = 0;
	head->recIDCnt = RecordIDBase;
	head->time = 1;
	listInit(&head->timeList);
	listInit(&head->IRList);
	return head;
}

void recordListClear(Record* head) { //不提供INV_RECORDS方向的删除：INV_REOCORD链方向不分配内存，元素内存都由所属TIME_REOCRDS链管理
	listForEachEntrySafe(Record, pos, &head->timeList, timeList) {
		recordDel(pos);
	}
}
Record* recordQueryID(Record* head, int recID, int direct) {
	recordForEachStart(pos, head, direct) {
		if (pos->recID == recID) {
			return pos;
		}
	}recordForEachEnd(pos, head, direct)
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

Record* recordFilterListGen(const Record* head, int type, const Record* filter) {
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

Record* recordShowPageJump(const Record* head, int type, int* pageStart, const int pageSize) {
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
		}
		else if (type == INV_RECORDS) {
			listForEachEntry(Record, pos, &head->IRList, IRList) {
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

Finance recordFinanceOpt(const Record* rec, Finance finance) {
	switch (rec->type)
	{
	case PURCHASE:
		financeExpend(&finance, rec->prod.amount);
		break;
	case SALE:
		financeIncome(&finance, rec->prod.amount);
		break;
	case GIFT:
		financeIncome(&finance, rec->prod.amount);
		break;
	case UPDATE:
		break;
	case DESTROY:
		break;
	default:
		break;
	}
	return finance;
}

void recordStatsWeight(const Record* head, int type, const Record* filter, double weightTable[]) {
	for (int i = 1; i <= RecordTypeNum; i++) weightTable[i] = 0;
	recordForEachStart(pos, head, type) {
		listForEachEntry(Record, pos, &head->timeList, timeList) {
			if (!recordMatch(pos, filter)) continue;
			weightTable[pos->type] += recordTypeProdDirect[pos->type] * pos->prod.weight;
		}
	}recordForEachEnd(pos, head, type)
}

void recordStatsQuantity(const Record* head, int type, const Record* filter, int quantityTable[]) {
	for (int i = 1; i <= RecordTypeNum; i++) quantityTable[i] = 0;
	recordForEachStart(pos, head, type) {
		listForEachEntry(Record, pos, &head->IRList, IRList) {
			if (!recordMatch(pos, filter)) continue;
			quantityTable[pos->type] += recordTypeProdDirect[pos->type] * pos->prod.quantity;
		}
	}recordForEachEnd(pos, head, type)
}
Finance recordStatsFinance(const Record* head, int type, const Record* filter, double startUpCapital) {
	Finance finance;
	financeInit(&finance, startUpCapital);
	recordForEachStart(pos, head, type) {
		if (!recordMatch(pos, filter)) continue;
		finance = recordFinanceOpt(pos, finance);
	}recordForEachEnd(pos, head, type)
		return finance;
}
