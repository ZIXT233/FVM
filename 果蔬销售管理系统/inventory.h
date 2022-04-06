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

Inventory* invCreate();  //����
void invDel(Inventory* pos);  //����,ɾ����Ʒ��Ϣ����ɾ������ؼ�¼
Inventory* invEntry(ListHead* entry);
Inventory* invListInit(Inventory* head);
void invListClear(Inventory* head);
Inventory* invQueryID(Inventory* head, int invID);
Inventory* invFilterListGen(const Inventory* head, const Product* filter);
Inventory* invShowPageJump(const Inventory* head, int* pageStart, const int pageSize);
#endif // !INVENTORY_H
