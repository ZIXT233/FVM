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

Inventory* invCreate();  //构造
void invDel(Inventory* pos);  //析构,删除商品信息但不删除其相关记录
Inventory* invEntry(ListHead* entry);
Inventory* invListInit(Inventory* head);
void invListClear(Inventory* head);
Inventory* invQueryID(Inventory* head, int invID);
Inventory* invFilterListGen(const Inventory* head, const Product* filter);
Inventory* invShowPageJump(const Inventory* head, int* pageStart, const int pageSize);
#endif // !INVENTORY_H
