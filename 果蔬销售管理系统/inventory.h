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

inline Inventory* invCreate() {   //构造
	return (Inventory*)malloc(sizeof(Inventory));
}
inline void invDel(Inventory* pos) {  //析构,删除商品信息但不删除其相关记录
	free(pos);
}
inline Inventory* invEntry(ListHead* entry) {
	return listEntry(entry, Inventory, list);
}
Inventory* invListInit(Inventory* head);
void invListClear(Inventory* head);
Inventory* invQueryID(Inventory* head, int invID);
#endif // !INVENTORY_H
