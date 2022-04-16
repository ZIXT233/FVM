#include<stdlib.h>
#include<string.h>
#include"list.h"
#include"inventory.h"

Inventory* invCreate() {   //构造
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
void invDel(Inventory* pos) {  //析构,删除商品信息但不删除其相关记录
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