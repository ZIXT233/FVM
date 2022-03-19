#include<stdlib.h>
#include"list.h"
#include"inventory.h"

Inventory* invCreate() {   //����
	return (Inventory*)malloc(sizeof(Inventory));
}
void invDel(Inventory* pos) {  //����,ɾ����Ʒ��Ϣ����ɾ������ؼ�¼
	free(pos);
}
Inventory* invEntry(ListHead* entry) {
	return listEntry(entry, Inventory, list);
}
Inventory* invListInit(Inventory* head) {
	head->prod.quantity = 1;
	head->invID = 0;
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