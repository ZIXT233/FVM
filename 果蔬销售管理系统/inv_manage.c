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
	pageStackPush(pageStackCreate("库存管理"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inFilter) {
			filterList = invFilterListGen(gdata.inventory, &filter);
			drawListPage(gdata.renderer, invListPos, "筛选信息", drawInvList, &filterList->list, &pageStart, PageSize, invListRectSize, NULL);
			invListClear(filterList);
			invDel(filterList);
		}
		else {
			drawListPage(gdata.renderer, invListPos, "库存信息", drawInvList, &gdata.inventory->list, &pageStart, PageSize, invListRectSize, NULL);
		}

		drawMenu(gdata.renderer, invMenuPos, "库存管理", 9, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品进货",
			"商品详情",
			"商品下架/销毁",
			"仓管记录",
			"退出",
			"随机进货");
		inputStart(gdata.renderer, INPUT_ORIGIN);

		renderPresent(gdata.renderer);
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
			invDetails(inv, gdata);
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
			pageStackPop(gdata.pageStack);
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
	invIDAllocate(inv, head);
	Record* rec = recordCreate();
	rec->type = PURCHASE;
	rec->time = FVMTimerGetFVMTime(gdata.timer);
	rec->prod = inv->prod;
	rec->invID = inv->invID;
	recordIDAllocate(rec, gdata.record);
	rec->addInfo[0] = '\0';
	rec->prod.unitPrice = rec->prod.purUPrice; //进货记录中的单价等价于进货单价
	if (inv->prod.pack == UNIT) rec->prod.amount = inv->prod.quantity * inv->prod.purUPrice;
	else if (inv->prod.pack == BULK) rec->prod.amount = inv->prod.weight * inv->prod.purUPrice;
	financeExpend(gdata.finance, rec->prod.amount);
	listAddTail(&rec->timeList, &gdata.record->timeList);
	listAddTail(&rec->IRList, &inv->invRecord->IRList);;
	listAddTail(&inv->list, &head->list);
	return;
}
void purchase(FVMO gdata) {
	Inventory* head = gdata.inventory;
	Inventory* inv = invCreate();

	invIDAllocate(inv, head);
	inv->prod.weight = inv->prod.quantity = 0;
	breakCatch(inputProduct(&inv->prod)) {
		return;
	}
	Record* rec = recordCreate();
	rec->type = PURCHASE;
	rec->time = FVMTimerGetFVMTime(gdata.timer);
	rec->prod = inv->prod;
	rec->invID = inv->invID;
	recordIDAllocate(rec, gdata.record);
	rec->addInfo[0] = '\0';
	rec->prod.unitPrice = rec->prod.purUPrice; //进货记录中的单价等价于进货单价
	if (inv->prod.pack == UNIT) rec->prod.amount = centRound(inv->prod.quantity * inv->prod.purUPrice);
	else if (inv->prod.pack == BULK) rec->prod.amount = centRound(inv->prod.weight * inv->prod.purUPrice);
	financeExpend(gdata.finance, rec->prod.amount);
	listAddTail(&rec->timeList, &gdata.record->timeList);
	listAddTail(&rec->IRList, &inv->invRecord->IRList);;
	listAddTail(&inv->list, &head->list);
	return;
}

static const Coord invDetailsPos = { 2,3 }, invDetailsMenuPos = { 22,3 };
void invDetails(Inventory* inv, FVMO gdata) {
	int invID, select;
	struct tm date;
	pageStackPush(pageStackCreate("商品详情"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawTitleWindow(gdata.renderer, invDetailsPos, "商品详情", InvDetailsRectSize);
		showInvDetails(gdata.renderer, (Coord) { invDetailsPos.x + 3, invDetailsPos.y + 1 }, inv);
		drawMenu(gdata.renderer, invDetailsMenuPos, "商品详情", 3, 1,
			"修改信息",
			"仓管记录",
			"退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			break;
		case 2:
			invRecordPage(inv->invRecord, gdata);
			break;
		case 3:
			pageStackPop(gdata.pageStack);
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
	inv->prod.purUPrice = centRound(300 * (double)rand() / RAND_MAX);
	inv->prod.unitPrice = centRound(inv->prod.purUPrice + 20.0 * rand() / RAND_MAX);
	if (inv->prod.pack == UNIT) inv->prod.quantity = rand() * 100;
	else inv->prod.weight = centRound((double)10000 * rand() / RAND_MAX);
	return inv;
}

static const Coord RecordListPos = { 2,3 };
void recordPage(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "记录筛选","取消筛选" };
	Record* showPage = NULL, * filterList = NULL;
	int select, num, invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	const int timeRec = TIME_RECORDS, invRec = INV_RECORDS;
	pageStackPush(pageStackCreate("仓管记录"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inFilter) {
			filterList = recordFilterListGen(gdata.record, TIME_RECORDS, &filter);
			drawListPage(gdata.renderer, RecordListPos, "筛选信息", drawRecordList, &filterList->timeList, &pageStart, PageSize, RecordRectSize, &timeRec);
			recordListClear(filterList);
			free(filterList);
		}
		else {
			drawListPage(gdata.renderer, RecordListPos, "仓管记录", drawRecordList, &gdata.record->timeList, &pageStart, PageSize, RecordRectSize, &timeRec);
		}
		drawMenu(gdata.renderer, invMenuPos, "仓管记录", 8, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品记录",
			"记录详情",
			"记录删除",
			"记录更改替换",
			"退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
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
			invRecordPage(inv->invRecord, gdata);
			break;
		case 5:
			breakCatch(inputRecordID(gdata.record, TIME_RECORDS, &recID, &rec)) break;
			if (recID < 0) break;
			recDetails(rec, gdata);
			break;
		case 6:
			num = getSelect();
			listForEachSafe(pos, &gdata.record->timeList) {
				if (num == (rec = recordEntry(pos, timeList))->recID) {
					listRemove(pos);
					listRemove(&rec->IRList);
					recordDel(rec);
				}
			}
			break;
		case 7:
			break;
		case 8:
			pageStackPop(gdata.pageStack);
			return;
		}

	}
}
void invRecordPage(Record* invRecord, FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "记录筛选","取消筛选" };
	Record* filterList = NULL;
	int select, num;
	int invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	int invRec = INV_RECORDS;
	pageStackPush(pageStackCreate("商品仓管记录"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inFilter) {
			filterList = recordFilterListGen(invRecord, INV_RECORDS, &filter);
			drawListPage(gdata.renderer, RecordListPos, "筛选信息", drawRecordList, &filterList->IRList, &pageStart, PageSize, RecordRectSize, &invRec);
			recordListClear(filterList);
			free(filterList);
		}
		else {
			drawListPage(gdata.renderer, RecordListPos, "商品仓管记录", drawRecordList, &invRecord->IRList, &pageStart, PageSize, RecordRectSize, &invRec);
		}
		drawMenu(gdata.renderer, invMenuPos, "仓管记录", 7, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"记录详情",
			"记录删除",
			"记录更改替换",
			"退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
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
			breakCatch(inputRecordID(invRecord, INV_RECORDS, &recID, &rec)) break;
			recDetails(rec, gdata);
			break;
		case 5:
			num = getSelect();
			listForEachSafe(pos, &invRecord->IRList) {
				if (num == (rec = recordEntry(pos, IRList))->recID) {
					listRemove(pos);
					listRemove(&rec->timeList);
					recordDel(rec);
				}
			}
			break;
		case 6:
			break;
		case 7:
			pageStackPop(gdata.pageStack);
			return;
		}

	}
}

static const Coord RecordDetailsPos = { 2,3 }, RecordDetailsMenuPos = { 22,3 };
void recDetails(Record* record, FVMO gdata) {
	int select;
	pageStackPush(pageStackCreate("记录详情"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawTitleWindow(gdata.renderer, RecordDetailsPos, "记录详情", RecordDetailsRectSize);
		showRecordDetails(gdata.renderer, (Coord) { RecordDetailsPos.x + 3, RecordDetailsPos.y + 1 }, record);
		drawMenu(gdata.renderer, RecordDetailsMenuPos, "记录详情", 1, 1,
			"退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			pageStackPop(gdata.pageStack);
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