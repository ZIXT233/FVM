#ifndef INVENTORY_H
#define INVENTORY_H
#include<time.h>
#include<stdbool.h>
#include"list.h"
#include"product.h"
#include"record.h"


typedef struct tagInventory {
	int invID;
	int invIDCnt;
	Product prod;
	Record* invRecord;
	ListHead list;
}Inventory;

static const int InventoryIDBase = 0;
Inventory* invCreate();  //����
Inventory* invCopyCreate(const Inventory* src);

void invDel(Inventory* pos);  //����,ɾ����Ʒ��Ϣ����ɾ������ؼ�¼
void invIDAllocate(Inventory* pos, Inventory* head);
Inventory* invEntry(ListHead* entry);
Inventory* invListInit(Inventory* head);
void invListClear(Inventory* head);
Inventory* invQueryID(Inventory* head, int invID);
Inventory* invFilterListGen(const Inventory* head, const Product* filter);
Inventory* invShowPageJump(const Inventory* head, int* pageStart, const int pageSize);
#define INV_CHECK_MSG_MAX 100
#define INV_CHECK_REC_NO_MATCH -1
#define INV_CHECK_CORRECT 0
int invRecCheck(const Inventory* inv, char* errmsg);
#endif // !INVENTORY_H
