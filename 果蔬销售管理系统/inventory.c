#include<stdlib.h>
#include"list.h"
#include"inventory.h"

Inventory* invListInit(Inventory* head) {
	head->quantity = 1;
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