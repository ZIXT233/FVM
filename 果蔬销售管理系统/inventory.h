#ifndef INVENTORY_H
#define INVENTORY_H
#include<time.h>
#include<stdbool.h>
#include"list.h"
#include"product.h"
#include"record.h"


typedef struct tagInventory {
	int invID;
	Product prod;
	Record* invRecord;
	ListHead list;
}Inventory;

inline Inventory* invCreate() {   //����
	return (Inventory*)malloc(sizeof(Inventory));
}
inline void invDel(Inventory* pos) {  //����,ɾ����Ʒ��Ϣ����ɾ������ؼ�¼
	free(pos);
}
inline Inventory* invEntry(ListHead* entry) {
	return listEntry(entry, Inventory, list);
}
Inventory* invListInit(Inventory* head);
void invListClear(Inventory* head);
Inventory* invQueryID(Inventory* head, int invID);
#endif // !INVENTORY_H
