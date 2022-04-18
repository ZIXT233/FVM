#include<string.h>
#include"inv_manage.h"

static const int PageSize = 15;


void invManage(FVMO gdata) {
	int inFilter = 0, inHistoryInv = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "商品筛选","取消筛选" };
	char historyOpt[2][20] = { "查看历史商品","取消查看历史商品" };
	Inventory* filterList = NULL, * inv = NULL, * optList = NULL;
	int select, num;
	Product filter;
	pageStackPush(pageStackCreate("库存管理"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inHistoryInv) {
			optList = gdata.historyInventory;
		}
		else {
			optList = gdata.inventory;
		}
		if (inFilter) {
			filterList = invFilterListGen(optList, &filter);
			drawListPage(gdata.renderer, invListPos, "筛选信息", drawInvList, &filterList->list, &pageStart, PageSize, invListRectSize, NULL);
			invListClear(filterList);
			invDel(filterList);
		}
		else {
			drawListPage(gdata.renderer, invListPos, inHistoryInv ? "历史商品信息" : "库存信息", drawInvList, &optList->list, &pageStart, PageSize, invListRectSize, NULL);
		}

		drawMenu(gdata.renderer, invMenuPos, "库存管理", 10, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品进货",
			"商品详情",
			"商品下架/销毁",
			"仓管记录",
			"退出",
			"随机进货",
			historyOpt[inHistoryInv]);
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
			breakCatch(inputInventoryIDHistory(gdata.inventory, gdata.historyInventory, &num, &inv)) break;
			invDetails(inv, gdata);
			break;
		case 6:
			invDestroyManage(gdata);
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
			break;
		case 10:
			if (inHistoryInv) {
				inHistoryInv = 0;
			}
			else {
				inHistoryInv = 1;
			}
		default:
			break;
		}
	}
}

void invDestroy(Inventory* inv, FVMO gdata) {  //only set quantity to 0 and make record
	Record* deRecord = recordCreate();
	deRecord->type = DESTROY;
	deRecord->prod = inv->prod;
	deRecord->invID = inv->invID;
	deRecord->time = FVMTimerGetFVMTime(gdata.timer);
	recordIDAllocate(deRecord, gdata.record);
	listAddTail(&deRecord->timeList, &gdata.record->timeList);
	listAddTail(&deRecord->IRList, &inv->invRecord->IRList);
	inv->prod.weight = inv->prod.quantity = 0;
	listRemove(&inv->list);
}
void invDestroyManage(FVMO gdata) {
	drawOrdMenu("商品下架/销毁:", 4, 1, "下架所有无货商品", "销毁所有过期商品", "销毁所有品质为差的商品", "销毁指定ID商品");
	int select, invID;
	Inventory* inv = NULL;
	breakDeliver(getUIntInput("选择一项:", &select, (IntRange) { 1, 4 }, true));
	switch (select) {
	case 1:
		listForEachEntrySafe(Inventory, pos, &gdata.inventory->list, list) {
			if (pos->prod.pack == BULK) {
				if (fLessEq(pos->prod.weight, 0)) {
					invDestroy(pos, gdata);
					listAddTail(&pos->list, &gdata.historyInventory->list);
				}
			}
			else if (pos->prod.pack == UNIT) {
				if (pos->prod.quantity <= 0) {
					invDestroy(pos, gdata);
					listAddTail(&pos->list, &gdata.historyInventory->list);
				}
			}
		}
		break;
	case 2:
		listForEachEntrySafe(Inventory, pos, &gdata.inventory->list, list) {
			if (pos->prod.expiration <= FVMTimerGetFVMTime(gdata.timer)) {
				invDestroy(pos, gdata);
				listAddTail(&pos->list, &gdata.historyInventory->list);
			}
		}
		break;
	case 3:
		listForEachEntrySafe(Inventory, pos, &gdata.inventory->list, list) {
			if (pos->prod.quality == BAD) {
				invDestroy(pos, gdata);
				listAddTail(&pos->list, &gdata.historyInventory->list);
			}
		}
		break;
	case 4:
		breakDeliver(inputInventoryID(gdata.inventory, &invID, &inv));
		invDestroy(inv, gdata);
		listAddTail(&inv->list, &gdata.historyInventory->list);
		break;
	default:
		break;
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


void invUpdate(Inventory* inv, FVMO gdata) {
	Record* updateRecord = NULL;
	double newPrice = 0;
	char addinfo[INFOMAX];
	breakDeliver(getStrInput("输入修改原因:", addinfo, INFOMAX, true));
	drawOrdMenu("选择要修改的信息:", 3, 1, "商品数目", "商品品质", "商品售价");
	int select;
	breakDeliver(getUIntInput("选择一项:", &select, (IntRange) { 1, 3 }, true));
	switch (select)
	{
	case 1:
		if (inv->prod.pack == BULK) {
			double weight;
			breakDeliver(getDoubleInput("输入修改后的库存重量:", &weight, WRANGE, true));
			weight = centRound(weight);
			updateRecord = recordCreate();
			updateRecord->prod = inv->prod;
			updateRecord->prod.weight = weight - inv->prod.weight;
			inv->prod.weight = weight;
		}
		else if (inv->prod.pack == UNIT) {
			int quantity;
			breakDeliver(getUIntInput("输入修改后的库存数量:", &quantity, QRANGE, true));
			updateRecord = recordCreate();
			updateRecord->prod = inv->prod;
			updateRecord->prod.quantity = quantity - inv->prod.quantity;
			inv->prod.quantity = quantity;
		}
		updateRecord->type = UPDATE;
		strcpy_s(updateRecord->addInfo, INFOMAX, addinfo);
		updateRecord->invID = inv->invID;
		updateRecord->prod.unitPrice = 0;
		updateRecord->time = FVMTimerGetFVMTime(gdata.timer);
		recordIDAllocate(updateRecord, gdata.record);
		listAddTail(&updateRecord->timeList, &gdata.record->timeList);
		listAddTail(&updateRecord->IRList, &inv->invRecord->IRList);
		break;
	case 2:
		drawOrdMenu("品质:", 3, 1, "优", "良", "差");
		int quality;
		double dWeight = 0;
		int dQuantity = 0;
		breakDeliver(getUIntInput("选择新品质:", &quality, (IntRange) { 1, 3 }, true));
		if (inv->prod.pack == BULK) {
			breakDeliver(getDoubleInput("输入受影响的库存重量:", &dWeight, (DoubleRange) { 0, inv->prod.weight }, true));
			dWeight = centRound(dWeight);
			updateRecord = recordCreate();
			updateRecord->prod = inv->prod;
			updateRecord->prod.weight = -dWeight;
			inv->prod.weight = centRound(inv->prod.weight - dWeight);
		}
		else if (inv->prod.pack == UNIT) {
			int dQuantity;
			breakDeliver(getUIntInput("输入受影响的库存数量:", &dQuantity, (IntRange) { 0, inv->prod.quantity }, true));
			updateRecord = recordCreate();
			updateRecord->prod = inv->prod;
			updateRecord->prod.quantity = -dQuantity;
			inv->prod.quantity -= dQuantity;
		}
		updateRecord->type = UPDATE;
		strcpy_s(updateRecord->addInfo, INFOMAX, addinfo);
		updateRecord->invID = inv->invID;
		updateRecord->time = FVMTimerGetFVMTime(gdata.timer);
		recordIDAllocate(updateRecord, gdata.record);
		listAddTail(&updateRecord->timeList, &gdata.record->timeList);
		listAddTail(&updateRecord->IRList, &inv->invRecord->IRList);

		Inventory* matchInv = NULL;
		Product newProd = inv->prod;
		newProd.quality = quality;
		listForEachEntry(Inventory, pos, &gdata.inventory->list, list) {
			if (!productMatch(&newProd, &pos->prod)) continue;
			if (newProd.expiration != pos->prod.expiration) continue;
			matchInv = pos;
			break;
		}
		if (matchInv) {
			updateRecord = recordCreate();
			updateRecord->prod = matchInv->prod;
			updateRecord->invID = matchInv->invID;
			updateRecord->type = UPDATE;
			if (updateRecord->prod.pack == BULK) {
				updateRecord->prod.weight = dWeight;
				matchInv->prod.weight = centRound(matchInv->prod.weight + dWeight);
			}
			else if (updateRecord->prod.pack == UNIT) {
				updateRecord->prod.quantity = dQuantity;
				matchInv->prod.quantity += dQuantity;
			}
			printf("库存中有同属性商品，已自动合并\n");
		}
		else {
			Inventory* newInv = invCopyCreate(inv);
			newInv->prod.quality = quality;
			invIDAllocate(newInv, gdata.inventory);
			newInv->invRecord = recordListInit(recordCreate());

			updateRecord = recordCreate();
			updateRecord->prod = newInv->prod;
			updateRecord->invID = newInv->invID;
			updateRecord->type = UPDATE;
			if (updateRecord->prod.pack == BULK) {
				newInv->prod.weight = dWeight;
				updateRecord->prod.weight = dWeight;
			}
			else if (updateRecord->prod.pack == UNIT) {
				newInv->prod.quantity = dQuantity;
				updateRecord->prod.quantity = dQuantity;
			}

			listAddTail(&newInv->list, &gdata.inventory->list);
			matchInv = newInv;
			printf("已在库存信息中生成新品质商品\n");
		}
		updateRecord->type = UPDATE;
		strcpy_s(updateRecord->addInfo, INFOMAX, addinfo);
		updateRecord->time = FVMTimerGetFVMTime(gdata.timer);
		recordIDAllocate(updateRecord, gdata.record);
		listAddTail(&updateRecord->timeList, &gdata.record->timeList);
		listAddTail(&updateRecord->IRList, &matchInv->invRecord->IRList);
		getchar();
		break;
	case 3:
		breakDeliver(getDoubleInput("输入修改后的销售单价:", &newPrice, UPRINCERANGE, true));
		newPrice = centRound(newPrice);
		updateRecord = recordCreate();
		updateRecord->prod = inv->prod;
		updateRecord->prod.quantity = updateRecord->prod.weight = 0;
		updateRecord->prod.unitPrice = centRound(newPrice - inv->prod.unitPrice);
		updateRecord->type = UPDATE;
		strcpy_s(updateRecord->addInfo, INFOMAX, addinfo);
		updateRecord->invID = inv->invID;
		updateRecord->time = FVMTimerGetFVMTime(gdata.timer);
		recordIDAllocate(updateRecord, gdata.record);
		listAddTail(&updateRecord->timeList, &gdata.record->timeList);
		listAddTail(&updateRecord->IRList, &inv->invRecord->IRList);
		inv->prod.unitPrice = newPrice;
		break;
	default:
		break;
	}
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
			invUpdate(inv, gdata);
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
	inv->prod.purUPrice = centRound(rand() % 50 + ((double)rand() / RAND_MAX));
	inv->prod.unitPrice = centRound(inv->prod.purUPrice * (1 + (double)rand() / RAND_MAX));
	if (inv->prod.pack == UNIT) inv->prod.quantity = rand() % 1000;
	else inv->prod.weight = centRound(rand() % 1000 + ((double)rand() / RAND_MAX));
	return inv;
}

void recordReplace(FVMO gdata) {
	int select, recID, invID;
	Record* rec = NULL, * nxt = NULL;
	Inventory* inv = NULL;
	char errmsg[INV_CHECK_MSG_MAX];
	drawOrdMenu("记录修改", 2, 1, "删除记录并退账", "新增记录");
	breakDeliver(getUIntInput("选择一项:", &select, (IntRange) { 1, 2 }, true));


	if (select == 1) {
		breakDeliver(inputRecordID(gdata.record, TIME_RECORDS, &recID, &rec));
		inv = invQueryID(gdata.inventory, rec->invID);
		if (!inv) {
			invQueryID(gdata.historyInventory, rec->invID);
			if (!inv) return;
		}

		nxt = recordEntry(rec->IRList.next, IRList);
		listRemove(&rec->IRList);
		recID = invRecCheck(inv, errmsg);
		if (recID && recID != INV_CHECK_REC_NO_MATCH) {
			printf("经验证，删除此记录将导致记录%d处发生错误:%s\n", recID, errmsg);
			breakDeliver(getUIntInput("仍要删除?(1.是 2.否)", &select, (IntRange) { 1, 2 }, true));
			if (select == 2) {
				listAddTail(&rec->IRList, &nxt->IRList);
				return;
			}
		}
		if (inv->prod.pack == BULK) {
			inv->prod.weight = centRound(inv->prod.weight - recordTypeProdDirect[rec->type] * rec->prod.weight);
		}
		else if (inv->prod.pack == UNIT) {
			inv->prod.quantity -= recordTypeProdDirect[rec->type] * rec->prod.quantity;
		}
		switch (rec->type) {
		case PURCHASE:
			financeExpend(gdata.finance, -rec->prod.amount);
			break;
		case SALE:
			financeIncome(gdata.finance, -rec->prod.amount);
			break;
		case UPDATE: //更新记录中只有退货涉及到金额操作
			if (rec->prod.amount) financeExpend(gdata.finance, -rec->prod.amount);
			break;
		case GIFT:
			financeIncome(gdata.finance, -rec->prod.amount);
			break;
		}
		listRemove(&rec->timeList);
		recordDel(rec);
	}
	else if (select == 2) {
		Record tmp;
		memset(&tmp, 0, sizeof(Record));
		drawOrdMenu("记录类型:", 5, 1, "进货记录", "销售记录", "更新记录", "赠予记录", "销毁记录");
		breakDeliver(getUIntInput("选择记录类型 : ", &tmp.type, (IntRange) { 1, 4 }, true));
		printf("输入对应的商品ID:\n");
		breakDeliver(inputInventoryIDHistory(gdata.inventory, gdata.historyInventory, &invID, &inv));
		tmp.prod = inv->prod;
		tmp.invID = inv->invID;
		char query[INFOMAX];
		sprintf_s(query, INFOMAX, "输入%s:", typeQuantityText[tmp.type]);
		if (inv->prod.pack == BULK) {
			breakDeliver(getDoubleInput(query, &tmp.prod.weight, WRANGE, true));
			tmp.prod.weight = centRound(tmp.prod.weight);
		}
		else if (inv->prod.pack == UNIT) {
			breakDeliver(getUIntInput(query, &tmp.prod.quantity, QRANGE, true));
		}
		if (tmp.type == DESTROY) {
			tmp.prod.amount = tmp.prod.unitPrice = 0;
		}
		else {
			sprintf_s(query, INFOMAX, "输入%s:", typeAmountText[tmp.type]);
			breakDeliver(getDoubleInput(query, &tmp.prod.amount, UPRINCERANGE, true));
			sprintf_s(query, INFOMAX, "输入%s:", typeUpriceText[tmp.type]);
			breakDeliver(getDoubleInput(query, &tmp.prod.unitPrice, UPRINCERANGE, true));
			if (tmp.type == SALE) {
				breakDeliver(getDoubleInput("输入折扣率:", &tmp.discount, (DoubleRange) { 0, 1 }, true));
			}
		}
		breakDeliver(getTimeInput("输入记录时间:", &tmp.time, true));
		breakDeliver(getStrInput("输入附加说明:(可选)", tmp.addInfo, INFOMAX, false));
		//输入完毕，开始验证
		rec = recordCreate();
		memcpy(rec, &tmp, sizeof(Record));
		Record* irnxt = inv->invRecord;
		listForEachEntry(Record, pos, &inv->invRecord->IRList, IRList) {
			if (pos->time > rec->time) {
				irnxt = pos;
				break;
			}
		}
		listAddTail(&rec->IRList, &irnxt->IRList);
		rec->recID = -1001;
		recID = invRecCheck(inv, errmsg);
		if (recID && recID != INV_CHECK_REC_NO_MATCH) {
			if (rec->recID == -1001) {
				printf("经验证，添加此记录将导致此记录处发生错误:%s\n", errmsg);
			}
			else {
				printf("经验证，添加此记录将导致记录%d处发生错误:%s\n", recID, errmsg);
			}
			breakDeliver(getUIntInput("仍要添加?(1.是 2.否)", &select, (IntRange) { 1, 2 }, true));
			if (select == 2) {
				listRemove(&rec->IRList);
				recordDel(rec);
				return;
			}
		}
		//加入总记录
		nxt = gdata.record;
		listForEachEntry(Record, pos, &gdata.record->timeList, timeList) {
			if (pos->time > rec->time) {
				nxt = pos;
				break;
			}
		}
		recordIDAllocate(rec, gdata.record);
		listAddTail(&rec->timeList, &nxt->timeList);
		//开始入账
		if (inv->prod.pack == BULK) {
			inv->prod.weight = centRound(inv->prod.weight + recordTypeProdDirect[rec->type] * rec->prod.weight);
		}
		else if (inv->prod.pack == UNIT) {
			inv->prod.quantity += recordTypeProdDirect[rec->type] * rec->prod.quantity;
		}
		switch (rec->type) {
		case PURCHASE:
			financeExpend(gdata.finance, rec->prod.amount);
			break;
		case SALE:
			financeIncome(gdata.finance, rec->prod.amount);
			break;
		case UPDATE: //更新记录中只有退货涉及到金额操作
			inv->prod.unitPrice = centRound(inv->prod.unitPrice + rec->prod.unitPrice);
			if (rec->prod.amount) financeExpend(gdata.finance, rec->prod.amount);
			break;
		case GIFT:
			financeIncome(gdata.finance, rec->prod.amount);
			break;
		}
	}

}
static const Coord RecordListPos = { 2,3 };
void recordPage(FVMO gdata) {
	int inFilter = 0, inInvRecord = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "记录筛选","取消筛选" };
	char invRecordOpt[2][20] = { "指定商品","取消指定商品" };
	Record* filterList = NULL, * invRecord = NULL;
	int select, num, invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	const int timeRec = TIME_RECORDS, invRec = INV_RECORDS;
	pageStackPush(pageStackCreate("仓管记录"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inInvRecord) {
			if (inFilter) {
				filterList = recordFilterListGen(invRecord, INV_RECORDS, &filter);
				drawListPage(gdata.renderer, RecordListPos, "筛选信息", drawRecordList, &filterList->IRList, &pageStart, PageSize, RecordRectSize, &invRec);
				recordListClear(filterList);
				free(filterList);
			}
			else {
				drawListPage(gdata.renderer, RecordListPos, "商品仓管记录", drawRecordList, &invRecord->IRList, &pageStart, PageSize, RecordRectSize, &invRec);
			}
		}
		else {
			if (inFilter) {
				filterList = recordFilterListGen(gdata.record, TIME_RECORDS, &filter);
				drawListPage(gdata.renderer, RecordListPos, "筛选信息", drawRecordList, &filterList->timeList, &pageStart, PageSize, RecordRectSize, &timeRec);
				recordListClear(filterList);
				free(filterList);
			}
			else {
				drawListPage(gdata.renderer, RecordListPos, "仓管记录", drawRecordList, &gdata.record->timeList, &pageStart, PageSize, RecordRectSize, &timeRec);
			}
		}
		drawMenu(gdata.renderer, invMenuPos, "仓管记录", 7, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			invRecordOpt[inInvRecord],
			"记录详情",
			"记录修改(管理员)",
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
			if (inInvRecord) {
				inInvRecord = 0;
			}
			else {
				breakCatch(inputInventoryIDHistory(gdata.inventory, gdata.historyInventory, &num, &inv)) break;

				invRecord = inv->invRecord;
				inInvRecord = 1;
			}
			break;
		case 5:
			breakCatch(inputRecordID(gdata.record, TIME_RECORDS, &recID, &rec)) break;
			recDetails(rec, gdata);
			break;
		case 6:
			char passwd[INFOMAX];
			breakCatch(getStrInput("输入管理员密码:", passwd, INFOMAX, true)) break;
			if (passwdVerify(passwd, gdata.passwdSha256)) {
				recordReplace(gdata);
			}
			else {
				printf("密码错误!");
				Sleep(3000);
			}
			break;
		case 7:
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
		drawMenu(gdata.renderer, invMenuPos, "仓管记录", 5, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"记录详情",
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