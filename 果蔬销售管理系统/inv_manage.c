#include<string.h>
#include"inv_manage.h"

static const int PageSize = 15;

void invManage(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "商品筛选","取消筛选" };
	Inventory* filterList = NULL, * inv = NULL;
	int select, num;
	Product filter;
	while (1) {
		cls();
		if (inFilter) {
			filterList = invFilterListGen(gdata.inventory, &filter);
			drawInvPage((Coord) { 2, 83 },"筛选信息", invShowPageJump(filterList, &pageStart, PageSize), pageStart, PageSize);
			invListClear(filterList);
			free(filterList);
		}
		drawInvPage(invListPos, "库存信息", invShowPageJump(gdata.inventory, &pageStart, PageSize), pageStart, PageSize);
		drawMenu(invMenuPos, "库存管理", 9, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品进货",
			"商品详情",
			"商品下架/销毁",
			"仓管记录",
			"退出",
			"随机进货");
		inputStart(INPUT_ORIGIN);
		select = getSelect();
		switch (select)
		{
		case 1:
			pageStart -= PageSize;
			if (pageStart < 1) pageStart = 1;
			break;
		case 2:
			pageStart += PageSize;
			break;
		case 3:
			if (inFilter) {  //取消筛选
				inFilter = false;
				pageStart = pageStartSave;
			}
			else if (!inFilter) {
				breakCatch(inputProductFilter(&filter)) {
					break;
				}
				inFilter = true;
				pageStartSave = pageStart;
				pageStart = 1;
			}
			break;
		case 4:

			purchase(gdata);
			break;
		case 5:
			breakCatch(inputInventoryID(gdata.inventory, &num, &inv)) break;
			invDetails(inv);
			break;
		case 6:
			num = getSelect();
			listForEachSafe(pos, &gdata.inventory->list) {
				if (num == invEntry(pos)->invID) {
					listRemove(pos);
					invDel(invEntry(pos));
				}
			}
			break;
		case 7:
			recordPage(gdata);
			break;
			/*case 7:
				num = getSelect();

				listForEachSafe(pos, &gdata.inventory->list) {
					if (num == invEntry(pos)->invID) {
						Inventory* _new = creatRandInv();
						_new->invID = invEntry(pos)->invID;
						listReplace(pos, &_new->list);
						invDel(invEntry(pos));
						break;
					}
				}
				break;
		case 8:
			inFilter = 0;
			pageStart = 1;
			invListClear(gdata.inventory);
			break;*/
		case 8:
			return;
		case 9:
			randomPurchase(gdata);
		default:
			break;
		}
	}
}


char ramdomrange[20][100] = { "苹果","梨","西瓜","桃子","黄瓜","菠萝","香菜香菜香菜香菜香菜香菜香菜香菜","甜甜小草莓" };
char variety[20][100] = { "附魔","超宇宙银河栽培","普通普通普通普通","哈哈哈哈哈哈","福岛县产" };
void randomPurchase(FVMO gdata) {
	Inventory* head = gdata.inventory;
	Inventory* inv = creatRandInv();
	inv->invID = head->prod.quantity++;
	Record* rec = recordCreate();
	rec->type = PURCHASE;
	time(&rec->time);
	rec->prod = inv->prod;
	rec->invID = inv->invID;
	rec->recID = gdata.record->time++;
	rec->addInfo[0] = '\0';
	if (inv->prod.pack == UNIT) rec->prod.amount = inv->prod.quantity * inv->prod.purUPrice;
	else if (inv->prod.pack == BULK) rec->prod.amount = inv->prod.weight * inv->prod.purUPrice;
	listAddTail(&rec->timeList, &gdata.record->timeList);
	listAddTail(&rec->IRList, &inv->invRecord->IRList);;
	listAddTail(&inv->list, &head->list);
	return;
}
void purchase(FVMO gdata) {
	cls();
	Inventory* head = gdata.inventory;
	Inventory* inv = invCreate();
	inv->invRecord = recordListInit(recordCreate());
	inv->invID = head->prod.quantity++;
	breakCatch(inputProduct(&inv->prod)) {
		return;
	}
	Record* rec = recordCreate();
	rec->type = PURCHASE;
	time(&rec->time);
	rec->prod = inv->prod;
	rec->invID = inv->invID;
	rec->recID = gdata.record->time++;
	rec->addInfo[0] = '\0';
	if (inv->prod.pack == UNIT) rec->prod.amount = inv->prod.quantity * inv->prod.purUPrice;
	else if (inv->prod.pack == BULK) rec->prod.amount = inv->prod.weight * inv->prod.purUPrice;
	listAddTail(&rec->timeList, &gdata.record->timeList);
	listAddTail(&rec->IRList, &inv->invRecord->IRList);;
	listAddTail(&inv->list, &head->list);
	return;
}

void invDetails(Inventory* inv) {
	int invID, select;
	struct tm date;

	while (1) {
		cls();
		showInvDetails(inv);
		drawMenu(getCursorPos(), "商品详情", 3, 1,
			"修改信息",
			"仓管记录",
			"退出");
		select = getSelect();
		switch (select)
		{
		case 1:
			break;
		case 2:
			invRecordPage(inv->invRecord);
			break;
		case 3:
			return;
		default:
			break;
		}

	}


}

Inventory* creatRandInv() {
	Inventory* inv = invCreate();
	strcpy_s(inv->prod.kind, INFOMAX, ramdomrange[rand() % 8]);
	strcpy_s(inv->prod.variety, INFOMAX, variety[rand() % 5]);
	inv->prod.pack = rand() % 2 + 1;
	inv->prod.quality = rand() % 3 + 1;
	inv->prod.expiration = time(NULL) * 1.5 * rand() / RAND_MAX;
	inv->prod.purUPrice = 300 * (double)rand() / RAND_MAX;
	inv->prod.unitPrice = inv->prod.purUPrice + 20.0 * rand() / RAND_MAX;
	if (inv->prod.pack == UNIT) inv->prod.quantity = rand() * 100;
	else inv->prod.weight = (double)10000 * rand() / RAND_MAX;
	inv->invRecord = recordListInit(recordCreate());
	return inv;
}


void recordPage(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "记录筛选","取消筛选" };
	Record* showPage = NULL, * filterList = NULL;
	int select, num, invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	while (1) {
		cls();
		if (inFilter) {
			filterList = recordFilterListGen(gdata.record, TIME_RECORDS, &filter);
			drawRecordList(UI_ORIGIN, recordShowPageJump(filterList, TIME_RECORDS, &pageStart, PageSize),TIME_RECORDS, PageSize);
			recordListClear(filterList);
			free(filterList);
		}
		else {
			drawRecordList(UI_ORIGIN, recordShowPageJump(gdata.record, TIME_RECORDS, &pageStart, PageSize),TIME_RECORDS, PageSize);
		}
		drawMenu(getCursorPos(), "仓管记录", 8, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品记录",
			"记录详情",
			"记录删除",
			"记录更改替换",
			"退出");
		select = getSelect();
		switch (select)
		{
		case 1:
			pageStart -= PageSize;
			if (pageStart < 1) pageStart = 1;
			break;
		case 2:
			pageStart += PageSize;
			break;
		case 3:
			if (inFilter) {  //取消筛选
				inFilter = false;
				pageStart = pageStartSave;
			}
			else if (!inFilter) {
				breakCatch(inputRecordFilter(&filter)) {
					break;
				}
				inFilter = true;
				pageStartSave = pageStart;
				pageStart = 1;
			}
			break;
		case 4:
			breakCatch(inputInventoryID(gdata.inventory, &num, &inv)) break;
			invRecordPage(inv->invRecord);
			break;
		case 5:
			while (1) {
				recID = -1;
				breakCatch(getUIntInput("请输入记录ID:", &recID, ALLINT, true)) break;
				if (rec = recordQueryID(gdata.record, recID, 0)) break;
				else {
					printf("无此记录。\n");
				}
			}
			if (recID < 0) break;
			recDetails(rec);
			break;
		case 6:
			num = getSelect();
			listForEachSafe(pos, &gdata.record->timeList) {
				if (num == (rec = recordEntry(pos, timeList))->invID) {
					listRemove(pos);
					listRemove(&rec->IRList);
					recordDel(rec);
				}
			}
			break;
		case 7:
			break;
		case 8:
			return;
		}

	}
}
void invRecordPage(Record* invRecord) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "记录筛选","取消筛选" };
	Record* filterList = NULL;
	int select, num;
	int invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	while (1) {
		cls();
		if (inFilter) {
			filterList = recordFilterListGen(invRecord, INV_RECORDS, &filter);
			drawRecordList(UI_ORIGIN, recordShowPageJump(filterList, INV_RECORDS, &pageStart, PageSize),INV_RECORDS, PageSize);
			recordListClear(filterList);
			free(filterList);
		}
		else {
			drawRecordList(UI_ORIGIN, recordShowPageJump(invRecord, INV_RECORDS, &pageStart, PageSize),INV_RECORDS, PageSize);
		}
		drawMenu(getCursorPos(), "仓管记录", 7, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"记录详情",
			"记录删除",
			"记录更改替换",
			"退出");
		select = getSelect();
		switch (select) {
		case 1:
			pageStart -= PageSize;
			if (pageStart < 1) pageStart = 1;
			break;
		case 2:
			pageStart += PageSize;
			break;
		case 3:
			if (inFilter) {  //取消筛选
				inFilter = false;
				pageStart = pageStartSave;
			}
			else if (!inFilter) {
				breakCatch(inputRecordFilter(&filter)) {
					break;
				}
				inFilter = true;
				pageStartSave = pageStart;
				pageStart = 1;
			}
			break;
		case 4:
			while (1) {
				recID = -1;
				breakCatch(getUIntInput("请输入记录ID:", &recID, ALLINT, true)) break;
				if (rec = recordQueryID(invRecord, recID, 1)) break;
				else {
					printf("无此记录。\n");
				}
			}
			if (recID < 0) break;
			recDetails(rec);
			break;
		case 5:
			num = getSelect();
			listForEachSafe(pos, &invRecord->IRList) {
				if (num == (rec = recordEntry(pos, IRList))->invID) {
					listRemove(pos);
					listRemove(&rec->timeList);
					recordDel(rec);
				}
			}
			break;
		case 6:
			break;
		case 7:
			return;
		}

	}
}

void recDetails(Record* record) {
	int invID, select;
	Inventory* inv;
	struct tm date;
	while (1) {
		cls();

		showRecordDetails(record);
		drawMenu(getCursorPos(), "记录详情", 1, 1,
			"退出");
		select = getSelect();
		switch (select)
		{
		case 1:
			return;
		default:
			break;
		}

	}


}
/*
void invManage(FVMO gdata) {
	Inventory* pageStart = gdata.inventory;
	ListHead* tmp;
	while (1) {
		cls();
		drawInvList(pageStart, PageSize);
		drawMenu("库存管理", 7,
			"上一页",
			"下一页",
			"进货",
			"删除",
			"替换",
			"清空",
			"退出");
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
}*/