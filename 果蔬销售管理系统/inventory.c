#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include"list.h"
#include"inventory.h"

Inventory* invCreate() {   //����
	Inventory* inv = (Inventory*)malloc(sizeof(Inventory));
	memset(inv, 0, sizeof(Inventory));
	inv->invRecord = recordListInit(recordCreate());
	return inv;
}
Inventory* invCopyCreate(const Inventory* src) {
	Inventory* dst = (Inventory*)malloc(sizeof(Inventory));
	memcpy(dst, src, sizeof(Inventory));
	dst->invRecord = NULL;
	return dst;
}
void invDel(Inventory* pos) {  //����,ɾ����Ʒ��Ϣ����ɾ������ؼ�¼
	if(pos->invRecord)free(pos->invRecord);
	free(pos);
}
Inventory* invEntry(ListHead* entry) {
	return listEntry(entry, Inventory, list);
}
void invIDAllocate(Inventory* pos, Inventory* head) {
	pos->invID = ++head->invIDCnt;
}
Inventory* invListInit(Inventory* head) {
	head->invID = 0;
	head->invIDCnt = InventoryIDBase;
	listInit(&head->list);
	return head;
}
void invListClear(Inventory* head) {
	listForEachSafe(pos, &head->list) {
		listRemove(pos);
		invDel(invEntry(pos));
	}

}

Inventory* invQueryID(Inventory* head, int invID) {
	listForEachEntry(Inventory, pos, &head->list, list) {
		if (pos->invID == invID) {
			return pos;
		}
	}
	return NULL;
}

Inventory* invFilterListGen(const Inventory* head, const Product* filter) {
	Inventory* filterList = invListInit(invCreate()), * cp;
	listForEachEntry(Inventory, pos, &head->list, list) {
		if (!productMatch(&pos->prod, filter)) continue;
		cp = invCopyCreate(pos);
		listAddTail(&cp->list, &filterList->list);
	}
	return filterList;
}

Inventory* invShowPageJump(const Inventory* head, int* pageStart, const int pageSize) {
	Inventory* showPage = head;
	while (1) {
		int num = 1;
		listForEachEntry(Inventory, pos, &head->list, list) {
			if (num == *pageStart) {
				showPage = invEntry(pos->list.prev);
				break;
			}
			num++;
		}
		if (showPage != head || *pageStart == 1) return showPage;
		else *pageStart -= pageSize;
	}
}

int invRecCheck(const Inventory* inv, char* errmsg) {
	int quantity = 0;
	double weight = 0;
	listForEachEntry(Record, pos, &inv->invRecord->IRList, IRList) {
		if (!productMatch(&pos->prod, &inv->prod)) {
			strcpy_s(errmsg, INV_CHECK_MSG_MAX, "��¼��Ʒ����������Ʒ���Բ���");
			return pos->recID;
		}
		if (pos->prod.pack == BULK) {
			weight += recordTypeProdDirect[pos->type] * pos->prod.weight;
			if (weight < 0) {
				strcpy_s(errmsg, INV_CHECK_MSG_MAX, "���˼�¼����Ʒ���Ϊ��");
				return pos->recID;
			}
		}
		else if (pos->prod.pack == UNIT) {
			quantity += recordTypeProdDirect[pos->type] * pos->prod.quantity;
			if (quantity < 0) {
				strcpy_s(errmsg, INV_CHECK_MSG_MAX, "���˼�¼����Ʒ���Ϊ��");
				return pos->recID;
			}
		}
	}
	if (inv->prod.pack == BULK && weight != inv->prod.weight) {

		sprintf_s(errmsg, INV_CHECK_MSG_MAX, "��Ʒ���п�������¼����,���ռ�¼Ϊ%.2lf ��", weight);
		return INV_CHECK_REC_NO_MATCH;
	}
	else if (inv->prod.pack == UNIT && quantity != inv->prod.quantity) {
		sprintf_s(errmsg, INV_CHECK_MSG_MAX, "��Ʒ���п�������¼����,���ռ�¼Ϊ%d ��", quantity);
		return INV_CHECK_REC_NO_MATCH;
	}
	return INV_CHECK_CORRECT;
}