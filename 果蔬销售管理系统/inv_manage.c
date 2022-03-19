#include<string.h>
#include"inv_manage.h"

const int PageSize = 15;
int productMatch(const Product* inv, const Product* filter) {
	if (filter == NULL) return 1;
	if (filter->kind[0] != '\0' && strcmp(filter->kind, inv->kind) != 0) return 0;
	if (filter->variety[0] != '\0' && strcmp(filter->variety, inv->variety) != 0) return 0;
	if (filter->pack != 0 && filter->pack != inv->pack) return 0;
	if (filter->expiration != TIME_NAN && filter->expiration < inv->expiration) return 0;
	return 1;
}
Inventory* invShowPageGen(const Inventory* head, const Product* filter, int* pageStart) {
	Inventory* showPage = invListInit(invCreate()), * cp;
	while (1) {
		int num = 1;
		listForEachEntry(Inventory, pos, &head->list, list) {
			if (num >= *pageStart + PageSize) break;
			if (!productMatch(&pos->prod, filter)) continue;
			if (num >= *pageStart) {
				cp = invCreate();
				memcpy(cp, pos, sizeof(Inventory));
				listAddTail(&cp->list, &showPage->list);
			}
			num++;
		}
		if (showPage->list.size != 0 || *pageStart == 1) return showPage;
		else *pageStart -= PageSize;
	}
}

void randomPurchase(FVMO gdata);
void invManage(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, * pageStartSave = 1;
	char filterOpt[2][20] = { "商品筛选","取消筛选" };
	Inventory* showPage = NULL;
	int select, num;
	Product filter;
	while (1) {
		cls();
		showPage = invShowPageGen(gdata.inventory, inFilter ? &filter : NULL, &pageStart);
		drawInvList(showPage, PageSize);
		invListClear(showPage);
		free(showPage);
		drawMenu("库存管理", 9,1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品进货",
			"商品详情",
			"商品下架/销毁",
			"仓管记录",
			"退出",
			"随机进货");
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
				if (inputProductFilter(&filter) == INPUT_BREAK) {
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
			invDetails(gdata);
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
	inputProduct(&inv->prod);
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

void invDetails(FVMO gdata) {
	int invID, select;
	Inventory* inv;
	struct tm date;
	while (1) {
		breakDeliver(getUIntInput("请输入商品ID:", &invID,ALLINT, true));
		if (inv = invQueryID(gdata.inventory, invID)) break;
		else {
			printf("库存无此商品。\n");
		}
	}
	while (1) {
		cls();
		showInvDetails(inv);
		drawMenu("商品详情", 3,1,
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
	if (inv->prod.pack == UNIT) inv->prod.quantity = rand()*100;
	else inv->prod.weight = (double)10000 * rand() / RAND_MAX;
	inv->invRecord = recordListInit(recordCreate());
	return inv;
}

int recordMatch(const Record* rec, const Record* filter) {
	if (filter == NULL) return 1;
	if (!productMatch(&rec->prod, &filter->prod))return 0;
	if (filter->type != 0 && filter->type != rec->type) return 0;
	if (filter->time != TIME_NAN && filter->time > rec->time) return 0;
	if (filter->lastTime != TIME_NAN && filter->lastTime < rec->time) return 0;
	return 1;
}
Record* recordShowPageGen(const Record* head, const Record* filter, int* pageStart, int direct) {
	Record* showPage = recordListInit(recordCreate()), * cp;
	while (1) {
		int num = 1;
		if (direct == 0) {
			listForEachEntry(Record, pos, &head->timeList, timeList) {
				if (num >= *pageStart + PageSize) break;
				if (!recordMatch(pos, filter)) continue;
				if (num >= *pageStart) {
					cp = recordCreate();
					memcpy(cp, pos, sizeof(Record));
					listAddTail(&cp->timeList, &showPage->timeList);
				}
				num++;
			}
		}
		else if (direct == 1) {
			listForEachEntry(Record, pos, &head->IRList, IRList) {
				if (num >= *pageStart + PageSize) break;
				if (!recordMatch(pos, filter)) continue;
				if (num >= *pageStart) {
					cp = recordCreate();
					memcpy(cp, pos, sizeof(Record));
					listAddTail(&cp->timeList, &showPage->timeList);
				}
				num++;
			}
		}
		if (showPage->timeList.size != 0 || *pageStart == 1) return showPage;
		else *pageStart -= PageSize;
	}
}



void recordPage(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, * pageStartSave = 1;
	char filterOpt[2][20] = { "记录筛选","取消筛选" };
	Record* showPage = NULL;
	int select, num, invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	while (1) {
		cls();
		showPage = recordShowPageGen(gdata.record, inFilter ? &filter : NULL, &pageStart, 0);
		drawRecordList(showPage, PageSize);
		recordListClear(showPage);
		free(showPage);
		drawMenu("仓管记录", 8,1,
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
				if (inputRecordFilter(&filter) == INPUT_BREAK) {
					break;
				}
				inFilter = true;
				pageStartSave = pageStart;
				pageStart = 1;
			}
			break;
		case 4:
			while (1) {
				invID = -1;
				if (getUIntInput("请输入商品ID:", &invID,ALLINT, true) == INPUT_BREAK) break;
				if (inv = invQueryID(gdata.inventory, invID)) break;
				else {
					printf("库存无此商品。\n");
				}
			}
			if (invID < 0) break;
			invRecordPage(inv->invRecord);
			break;
		case 5:
			while (1) {
				recID = -1;
				if (getUIntInput("请输入记录ID:", &recID, ALLINT,true) == INPUT_BREAK) break;
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
	int pageStart = 1, * pageStartSave = 1;
	char filterOpt[2][20] = { "记录筛选","取消筛选" };
	Record* showPage = NULL;
	int select, num;
	int invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	while (1) {
		cls();
		showPage = recordShowPageGen(invRecord, inFilter ? &filter : NULL, &pageStart, 1);
		drawRecordList(showPage, PageSize);
		recordListClear(showPage);
		free(showPage);
		drawMenu("仓管记录", 7,1,
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
				if (inputRecordFilter(&filter) == INPUT_BREAK) {
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
				if (getUIntInput("请输入记录ID:", &recID, ALLINT,true) == INPUT_BREAK) break;
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
		drawMenu("记录详情", 1,1,
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