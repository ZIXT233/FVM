#include"inv_manage.h"

const int PageSize = 15;
void invManage(FVMO gdata) {
	Inventory* pageStart = gdata.inventory;
	ListHead* tmp;
	while (1) {
		cls();
		drawInvList(pageStart, PageSize);
		drawMenu("¿â´æ¹ÜÀí", 7,
			"ÉÏÒ»Ò³",
			"ÏÂÒ»Ò³",
			"½ø»õ",
			"É¾³ý",
			"Ìæ»»",
			"Çå¿Õ",
			"ÍË³ö");
		int select = getSelect();
		int num;
		Inventory* inv;
		switch (select)
		{
		case 1:
			pageStart = invEntry(listMove(&pageStart->list, -PageSize));
			break;
		case 2:
			tmp = listMove(&pageStart->list, PageSize);
			if (tmp->next != tmp->root) pageStart = invEntry(tmp);
			break;
		case 3:
			purchase(gdata);
			break;
		case 4:
			num = getSelect();
			listForEachSafe(pos, &gdata.inventory->list) {
				if (num == invEntry(pos)->invID) {
					if (invEntry(pos)->invID <= pageStart->invID) {
						pageStart = invEntry(pageStart->list.next);
					}
					listRemove(pos);
					invDel(invEntry(pos));
					if (pageStart->list.next == pageStart->list.root) {
						pageStart = invEntry(listMove(&pageStart->list, -PageSize));
					}
					break;
				}
			}
			break;
		case 5:
			num = getSelect();

			listForEachSafe(pos, &gdata.inventory->list) {
				if (num==invEntry(pos)->invID) {
					Inventory* _new = creatRandInv();
					if (pos == &pageStart->list) pageStart = _new;
					_new->invID = invEntry(pos)->invID;
					listReplace(pos, &_new->list);
					invDel(invEntry(pos));
					break;
				}
			}
			break;
		case 6:
			pageStart = gdata.inventory;
			invListClear(gdata.inventory);
			break;
		case 7:
			return;
		default:
			break;
		}
	}
}

#include<string.h>
char ramdomrange[20][100] = { "Æ»¹û","Àæ","Î÷¹Ï","ÌÒ×Ó","»Æ¹Ï","²¤ÂÜ","Ïã²ËÏã²ËÏã²ËÏã²ËÏã²ËÏã²ËÏã²ËÏã²Ë","ÌðÌðÐ¡²ÝÝ®" };
char variety[20][100] = { "¸½Ä§","³¬ÓîÖæÒøºÓÔÔÅà","ÆÕÍ¨ÆÕÍ¨ÆÕÍ¨ÆÕÍ¨","¹þ¹þ¹þ¹þ¹þ¹þ","¸£µºÏØ²ú" };
void purchase(FVMO gdata) {
	Inventory* head = gdata.inventory;
	Inventory* inv = creatRandInv();
	inv->invID = head->quantity++;
	Record* rec = recordCreate();
	rec->type = PURCHASE;
	rec->attr = inv->attr;
	rec->quantity = inv->quantity;
	rec->weight = inv->weight;
	rec->invId = inv->invID;
	listAddTail(&rec->timeList, &gdata.record->timeList);
	listAddTail(&rec->IRList, &inv->invRecord->IRList);;
	listAddTail(&inv->list, &head->list);
	return;
}

Inventory *creatRandInv() {
	Inventory* inv = invCreate();
	strcpy_s(inv->attr.kind, INFOMAX, ramdomrange[rand() % 8]);
	strcpy_s(inv->attr.variety, INFOMAX, variety[rand() % 5]);
	inv->attr.pack = rand() % 2;
	inv->attr.quanlity = rand() % 3;
	inv->attr.expiration = time(NULL) + 3600 * 72;
	inv->unitPrice = (double)(rand() % 100) / 100 + rand();
	if (inv->attr.pack) inv->quantity = rand();
	else inv->weight = (double)(rand() % 100) / 100 + rand();
	inv->invRecord = recordListInit(recordCreate());
	return inv;
}