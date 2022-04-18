#include<string.h>
#include"sale.h"
static const int PageSize = 15;
Coord cartPos = { 2,83 }, cartMenuPos = { 21,83 };


int cartAdd(Inventory* cart, Inventory* head) {
	int id;
	Inventory* item, * inv;
	breakDeliver(inputInventoryID(head, &id, &inv));	//查询库存信息
	item = invQueryID(cart, id);						//查询购物车中是否已存在
	int quantity;
	double weight;
	if (item == NULL) {
		if (inv->prod.pack == UNIT) {
			if (inv->prod.quantity <= 0) {
				printf("库存不足\n");
				getchar();
				return -1;
			}
			breakDeliver(getUIntInput("请输入购买数量:", &quantity, (IntRange) { 0, inv->prod.quantity }, true));
			item = invCopyCreate(inv);
			item->prod.quantity = quantity;
			item->prod.amount = item->prod.quantity * item->prod.unitPrice;
		}
		else if (inv->prod.pack == BULK) {
			if (fLessEq(inv->prod.weight, 0)) {
				printf("库存不足");
				getchar();
				return -1;
			}
			breakDeliver(getDoubleInput("请输入购买重量:", &weight, (DoubleRange) { 0, inv->prod.weight }, true));
			item = invCopyCreate(inv);
			centRound(weight);
			item->prod.weight = weight;
			item->prod.amount = item->prod.weight * item->prod.unitPrice;
		}
		listAddTail(&item->list, &cart->list);
	}
	else {
		if (item->prod.pack == UNIT) {
			if (inv->prod.quantity - item->prod.quantity <= 0) {
				printf("库存不足\n");
				getchar();
				return -1;
			}
			breakDeliver(getUIntInput("请输入购买数量:", &quantity, (IntRange) { 0, inv->prod.quantity - item->prod.quantity }, true)); //确保增加后不超出库存量
			item->prod.quantity += quantity;
			item->prod.amount = item->prod.quantity * item->prod.unitPrice;
		}
		else if (inv->prod.pack == BULK) {
			if (fLessEq(inv->prod.weight - item->prod.weight, 0)) {
				printf("库存不足");
				getchar();
				return -1;
			}
			breakDeliver(getDoubleInput("请输入购买重量:", &weight, (DoubleRange) { 0, inv->prod.weight - item->prod.weight }, true));
			centRound(weight);
			item->prod.weight = item->prod.weight + weight;
			item->prod.amount = item->prod.weight * item->prod.unitPrice;
		}
	}

}
int cartDelete(Inventory* cart) {
	int id;
	Inventory* inv;
	breakDeliver(inputInventoryID(cart, &id, &inv));
	if (inv) {
		listRemove(&inv->list);
		invDel(inv);
	}
}
int cartQuantityModify(Inventory* cart, Inventory* head) {
	int id;
	Inventory* item, * inv;
	breakDeliver(inputInventoryID(head, &id, &inv));	//查询库存信息
	item = invQueryID(cart, id);						//查询购物车中是否已存在
	int quantity;
	double weight;
	if (item) {
		if (item->prod.pack == UNIT) {
			breakDeliver(getUIntInput("请输入新购买数量:", &quantity, (IntRange) { 1, inv->prod.quantity }, true)); //确保增加后不超出库存量
			item->prod.quantity = quantity;
		}
		else if (inv->prod.pack == BULK) {
			breakDeliver(getDoubleInput("请输入新购买重量:", &weight, (DoubleRange) { 0.01, inv->prod.weight }, true));
			item->prod.weight = weight;
		}
	}
	else {
		printf("购物车中无此商品");
	}

}
void cartClear(Inventory* cart) {
	invListClear(cart);
}

int inputInventoryIDinPreOrder(Record* head, int* id, Record** pRec) {
	while (1) {
		breakDeliver(getUIntInput("输入商品ID:", id, ALLINT, true));
		listForEachEntry(Record, pos, &head->timeList, timeList) {
			if (pos->invID == *id) {
				*pRec = pos;
				return;
			}
		}
		printf("订单中查无此商品\n");
	}
}

int SSPSelect(FVMO gdata, Record* preOrder,bool isMember) {
	Record* rec;
	CSP* csp;
	SSP* ssp;
	int recID, cspID, sspID;
	breakDeliver(inputInventoryIDinPreOrder(preOrder, &recID, &rec));
	if (rec->CSPID && (csp = CSPQueryID(gdata.CSP, rec->CSPID))) {
		if (!csp->overlaySingleSP) {
			printf("该商品所应用组合方案不可与单品方案叠加(按任意键返回)\n");
			getchar();
			return;
		}
	}
	breakDeliver(inputSSPID(gdata.SSP, &sspID, &ssp));
	time_t fvmtime = FVMTimerGetFVMTime(gdata.timer);
	if (ssp->reqDateStart!=TIME_NAN && ssp->reqDateStart > fvmtime) {
		printf("不在此方案可用时间\n");
		getchar();
		return;
	}
	if (ssp->reqDateEnd!=TIME_NAN && ssp->reqDateEnd < fvmtime) {
		printf("不在此方案可用时间\n");
		getchar();
		return;
	}
	if (ssp->reqMember && !isMember) {
		printf("此方案需要会员\n");
		getchar();
		return;
	}
	/*全局条件*/
	if (rec->invID != ssp->invID && !productMatch(&rec->prod, &ssp->filter)) {
		printf("商品不满足该方案条件");
		getchar();
		return;
	}
	else {
		rec->discount = ssp->discount;
		rec->SSPID = ssp->SSPID;
	}
}
int CSPSlect(FVMO gdata,CSP* optCSP, Record* preOrder,bool isMember) {
	CSP* csp;
	int cspID;
	breakDeliver(inputCSPID(optCSP, &cspID, &csp));
	time_t fvmtime = FVMTimerGetFVMTime(gdata.timer);
	if (csp->reqDateStart!=TIME_NAN && csp->reqDateStart > fvmtime) {
		printf("不在此方案可用时间\n");
		getchar();
		return;
	}
	if (csp->reqDateEnd!=TIME_NAN && csp->reqDateEnd < fvmtime) {
		printf("不在此方案可用时间\n");
		getchar();
		return;
	}
	if (csp->reqMember && !isMember) {
		printf("此方案需要会员\n");
		getchar();
		return;
	}
	listForEachEntry(Record, pos, &preOrder->timeList, timeList) {
		if (CSPHasInvID(csp, pos->invID)) {
			if (!csp->overlaySingleSP) {
				pos->SSPID = 0;
				pos->discount = 1;
			}
			pos->discount *= csp->discount;
			pos->CSPID = csp->CSPID;
		}
	}
}
int SSPAppCancel(Record* preOrder) {
	Record* rec;
	int recID;
	breakDeliver(inputInventoryIDinPreOrder(preOrder, &recID, &rec));
	rec->SSPID = 0;
	rec->discount = 1;
}
int CSPAppCancle(FVMO gdata, Record* preOrder) {
	CSP* csp;
	int cspID;
	breakDeliver(inputCSPID(gdata.CSP, &cspID, &csp));
	listForEachEntry(Record, pos, &preOrder->timeList, timeList) {
		if (pos->CSPID == csp->CSPID) {
			pos->CSPID = 0;
		}
	}
}
void SSPAutoApplicate(SSP* head, Record* preOrder,FVMO gdata, bool isMember) {
	listForEachEntry(SSP, sspPos, &head->list, list) {
		time_t fvmtime = FVMTimerGetFVMTime(gdata.timer);
		if (sspPos->reqDateStart!=TIME_NAN && sspPos->reqDateStart > fvmtime) continue;
		if (sspPos->reqDateEnd!=TIME_NAN && sspPos->reqDateEnd < fvmtime) continue;
		if (sspPos->reqMember && !isMember) continue;
		/*
		这里限制SSP的全局条件 ...
		*/
		
		if (sspPos->invID > 0) {
			Inventory* inv;
			listForEachEntry(Record, recPos, &preOrder->timeList, timeList) {
				if (recPos->invID == sspPos->invID && sspPos->discount <= recPos->discount) {
					if (recPos->prod.pack == BULK) {
						if (sspPos->filter.weight > recPos->prod.weight || sspPos->filter.amount > recPos->prod.weight * recPos->prod.unitPrice) {
							continue;
						}
					}
					else if (recPos->prod.pack == UNIT) {
						if (sspPos->filter.quantity > recPos->prod.quantity || sspPos->filter.amount > recPos->prod.quantity * recPos->prod.unitPrice) {
							continue;
						}
					}
					recPos->discount = sspPos->discount;
					recPos->SSPID = sspPos->SSPID;
					break;
				}
			}
		}
		else if (sspPos->invID == 0) {
			listForEachEntry(Record, recPos, &preOrder->timeList, timeList) {
				if (productMatch(&recPos->prod, &sspPos->filter) && sspPos->discount <= recPos->discount) {
					if (sspPos->filter.weight > recPos->prod.weight) continue;
					if (sspPos->filter.quantity > recPos->prod.quantity) continue;
					if (sspPos->filter.unitPrice > recPos->prod.unitPrice) continue;
					if (sspPos->filter.amount > recPos->prod.amount) continue;
					recPos->discount = sspPos->discount;
					recPos->SSPID = sspPos->SSPID;
				}
			}
		}
	}
}


const static Coord PreOrderPos = { 2,3 }, PreOrderMenu = { 22, 3 };
const static Coord optCSPPos = { 2,130 }, optCSPMenuPos = { 22,130 };
const static Coord SSPGiftPos = { 2,130 }, SSPGiftMenuPos = { 22,130 };
const static Coord SalePlanMenu = { 22,3 };
#define SSPMAX 1000
#define CSPMAX 1000

int settleProc(FVMO gdata, Record* preOrder) {
	Inventory* inv = NULL;
	Record* rec = NULL;
	time_t ti;
	ti = FVMTimerGetFVMTime(gdata.timer);
	listForEachEntrySafe(Record, pos, &preOrder->timeList, timeList) {
		pos->time = ti;
		pos->prod.amount = centRound(pos->prod.amount * pos->discount);
		//listAddTail(&preOrder->IRList, &gdata.order->IRList);
		//recordIDAllocate(preOrder, gdata.order);


		/*
		* 这里处理账单
		*/
		financeIncome(gdata.finance, pos->prod.amount);

		//处理库存
		inv = invQueryID(gdata.inventory, pos->invID);
		inv->prod.quantity -= pos->prod.quantity;
		inv->prod.weight = centRound(inv->prod.weight - pos->prod.weight);

		//添加记录
		rec = recordCreate();
		memcpy(rec, pos, sizeof(Record));
		recordIDAllocate(rec, gdata.record);
		listAddTail(&rec->timeList, &gdata.record->timeList);
		listAddTail(&rec->IRList, &inv->invRecord->IRList);
	}
	free(preOrder);
	return 0;
}

#define SETTLE_SUCCESS 0
#define SETTLE_BREAK -1
int settlement(FVMO gdata, Record* preOrder) {
	int orderPageStart = 1, optCSPPageStart = 1;
	int select, num;
	pageStackPush(pageStackCreate("订单结算"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, PreOrderPos, "最终订单", drawPreOrderList, &preOrder->timeList, &orderPageStart, PageSize, PreOrderRectSize, &gdata);
		drawMenu(gdata.renderer, PreOrderMenu, "订单结算", 4, 1,
			"上一页",
			"下一页",
			"确认结算",
			"退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			orderPageStart -= PageSize;
			if (orderPageStart < 1) orderPageStart = 1;
			break;
		case 2:
			orderPageStart += PageSize;
			break;
		case 3:
			settleProc(gdata, preOrder);
			pageStackPop(gdata.pageStack);
			return SETTLE_SUCCESS;
		case 4:
			pageStackPop(gdata.pageStack);
			return SETTLE_BREAK;
		default:
			break;
		}
	}
}
int giftAddToOrder(FVMO gdata, Record* preOrder, Inventory* gift, int SSPID, int CSPID) {
	Inventory* inv = NULL;
	Record* rec = NULL;
	int mul = 1;
	if (gift->prod.unitPrice) {
		breakDeliver(getUIntInput("该赠品为余额赠品，请输入份数(默认一份):", &mul, QRANGE, false));
	}
	if (inv = invQueryID(gdata.inventory, gift->invID)) {
		if (gift->prod.pack == BULK) {
			if (gift->prod.weight * mul > inv->prod.weight) {
				printf("库存余量不足(任意键返回)\n");
				getchar();
				return -1;
			}
		}
		else if (gift->prod.pack == UNIT) {
			if (gift->prod.quantity * mul > inv->prod.quantity) {
				printf("库存余量不足(任意键返回)\n");
				getchar();
				return -1;
			}
		}

		rec = recordCreate();
		rec->prod = gift->prod;
		rec->prod.quantity *= mul;
		rec->prod.weight *= mul;
		rec->prod.amount *= mul;
		rec->addInfo[0] = '\0';
		rec->discount = 1;
		rec->invID = gift->invID;
		rec->SSPID = SSPID;
		rec->CSPID = CSPID;
		rec->type = GIFT;
		listAddTail(&rec->timeList, &preOrder->timeList);
		return 0;
	}
	else {
		printf("库存中已无此赠品(任意键继续)\n");
		getchar();
		return -1;
	}
}
int giftSelect(FVMO gdata, Record* preOrder) {
	int sspid[SSPMAX], cspid[CSPMAX], sspidSize, cspidSize, sspidCur, cspidCur, giftInvID;
	int orderPageStart = 1, giftPageStart = 1, select;
	SSP* ssp = NULL;
	CSP* csp = NULL;
	Record* giftRec = NULL;
	Inventory* gift = NULL;
	bool repeat;
	sspidSize = cspidSize = sspidCur = cspidCur = 0;

	listForEachEntry(Record, pos, &preOrder->timeList, timeList) {
		if (pos->SSPID) {
			repeat = false;
			for (int i = 0; i < sspidSize; i++) {
				if (sspid[i] == pos->SSPID) {
					repeat = true;
					break;
				}
			}
			if (!repeat) {
				sspid[sspidSize++] = pos->SSPID;
			}
		}
		if (pos->CSPID) {
			repeat = false;
			for (int i = 0; i < cspidSize; i++) {
				if (cspid[i] == pos->CSPID) {
					repeat = true;
					break;
				}
			}
			if (!repeat) {
				cspid[cspidSize++] = pos->CSPID;
			}
		}
	}
	pageStackPush(pageStackCreate("赠品选择"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, PreOrderPos, "订单", drawPreOrderList, &preOrder->timeList, &orderPageStart, PageSize, PreOrderRectSize, &gdata);

		if (sspidCur < sspidSize) {
			ssp = SSPQueryID(gdata.SSP, sspid[sspidCur]);
			csp = NULL;
			if (ssp->optGifts->list.size == 0) {
				sspidCur++;
				continue;
			}
			drawListPage(gdata.renderer, SSPGiftPos, ssp->planName, drawGiftList, &ssp->optGifts->list, &giftPageStart, PageSize, GiftRectSize, &gdata);
		}
		else if (cspidCur < cspidSize) {
			csp = CSPQueryID(gdata.CSP, cspid[cspidCur]);
			ssp = NULL;
			if (csp->optGifts->list.size == 0) {
				cspidCur++;
				continue;
			}
			drawListPage(gdata.renderer, SSPGiftPos, csp->planName, drawGiftList, &csp->optGifts->list, &giftPageStart, PageSize, GiftRectSize, &gdata);
		}
		else {
			drawTitleWindow(gdata.renderer, SSPGiftPos, "已无可选赠品", GiftRectSize);
		}
		drawMenu(gdata.renderer, SalePlanMenu, "赠品选择", 6, 1,
			"订单上一页",
			"订单下一页",
			"选择当前方案赠品",
			"下一方案赠品",
			"前往结算",
			"退出");
		drawMenu(gdata.renderer, optCSPMenuPos, "赠品浏览", 2, 11,
			"上一页",
			"下一页");

		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select) {
		case 1:
			orderPageStart -= PageSize;
			if (orderPageStart < 1) orderPageStart = 1;
			break;
		case 2:
			orderPageStart += PageSize;
			break;
		case 3:
			if (!(ssp || csp)) {
				printf("已无可选赠品(任意键继续)\n");
				getchar();
				break;
			}
			if (ssp) {
				do {
					breakCatch(inputInventoryID(ssp->optGifts, &giftInvID, &gift)) break;
				} while (giftAddToOrder(gdata, preOrder, gift, ssp->SSPID, 0) != 0);
				sspidCur++;
			}
			else if (csp) {
				do {
					breakCatch(inputInventoryID(csp->optGifts, &giftInvID, &gift)) break;
				} while (giftAddToOrder(gdata, preOrder, gift, 0, csp->CSPID) != 0);
				cspidCur++;
			}
			break;
		case 4:
			if (ssp) {
				sspidCur++;
			}
			else if (csp) {
				cspidCur++;
			}
			else {
				printf("已无下一方案(任意键继续)\n");
				getchar();
				break;
			}
			break;
		case 5:
			if (settlement(gdata, preOrder) == SETTLE_SUCCESS) {
				pageStackPop(gdata.pageStack);
				return SETTLE_SUCCESS;
			}
			else {
				break;
			}
		case 6:
			listForEachEntrySafe(Record, pos, &preOrder->timeList, timeList) {
				if (pos->type == GIFT) {
					listRemove(&pos->timeList);
					recordDel(pos);
				}
			}
			pageStackPop(gdata.pageStack);
			return SETTLE_BREAK;
		case 11:
			giftPageStart -= PageSize;
			if (giftPageStart < 1) giftPageStart = 1;
			break;
		case 12:
			giftPageStart += PageSize;
			break;

		default:
			break;
		}
		//列出用到的方案
		//枚举每个方案选择礼物
		//选择完毕进入结算确认页面
	}
}
int salePlanSelect(FVMO gdata, Inventory* cart,bool isMember) {
	Record* preOrder = recordListInit(recordCreate()), * rec = NULL;
	CSP* optCSP = NULL, * csp;
	int orderPageStart = 1, optCSPPageStart = 1;
	int select, num, cspID;
	listForEachEntry(Inventory, pos, &cart->list, list) {
		rec = recordCreate();
		rec->prod = pos->prod;
		rec->discount = 1;
		rec->invID = pos->invID;
		rec->SSPID = rec->CSPID = 0;
		rec->type = SALE;
		listAddTail(&rec->timeList, &preOrder->timeList);
	}
	SSPAutoApplicate(gdata.SSP, preOrder,gdata,isMember);
	pageStackPush(pageStackCreate("销售方案选择"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, PreOrderPos, "订单", drawPreOrderList, &preOrder->timeList, &orderPageStart, PageSize, PreOrderRectSize, &gdata);
		if (optCSP) {
			CSPListClear(optCSP);
			CSPDel(optCSP);
		}
		optCSP = CSPOptionalListGen(gdata.CSP, preOrder);
		drawListPage(gdata.renderer, optCSPPos, "可用组合销售方案", drawCSPList, &optCSP->list, &optCSPPageStart, PageSize, optCSPRectSize, NULL);
		drawMenu(gdata.renderer, SalePlanMenu, "销售方案选择", 8, 1,
			"订单上一页",
			"订单下一页",
			"应用单品销售方案",
			"应用组合销售方案",
			"取消应用单品销售方案",
			"取消应用组合销售方案",
			"下一步(赠品选择)",
			"退出");
		drawMenu(gdata.renderer, optCSPMenuPos, "可用组合销售方案浏览", 3, 11,
			"上一页",
			"下一页",
			"方案详情");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select) {
		case 1:
			orderPageStart -= PageSize;
			if (orderPageStart < 1) orderPageStart = 1;
			break;
		case 2:
			orderPageStart += PageSize;
			break;
		case 3:
			SSPSelect(gdata, preOrder,isMember);
			break;
		case 4:
			CSPSlect(gdata,optCSP, preOrder,isMember);
			break;
		case 5:
			SSPAppCancel(preOrder);
			break;
		case 6:
			CSPAppCancle(gdata, preOrder);
			break;
		case 7:
			if (giftSelect(gdata, preOrder) == SETTLE_SUCCESS) {
				pageStackPop(gdata.pageStack);
				return SETTLE_SUCCESS;
			}
			else {
				break;
			}

		case 8:
			if (optCSP) {
				CSPListClear(optCSP);
				free(optCSP);
			}
			recordListClear(preOrder);
			free(preOrder);
			pageStackPop(gdata.pageStack);
			return SETTLE_BREAK;
		case 11:
			optCSPPageStart -= PageSize;
			if (optCSPPageStart < 1) optCSPPageStart = 1;
			break;
		case 12:
			optCSPPageStart += PageSize;
			break;
		case 13:
			breakCatch(inputCSPID(gdata.CSP, &num, &csp)) break;
			CSPDetails(csp, gdata);
		default:
			break;
		}
	}
	//1.生成初始记录
	//2.遍历ssp匹配可行商品，自动应用最优折扣
	//3.遍历csp列出可用方案
	//4.确认方案
	//5.列出可选礼品
	//6.结算，生成订单
}

void saleReturn(FVMO gdata) {  
	int recID;
	int select;
	Record* rec;
	do {
		breakDeliver(inputRecordID(gdata.record, TIME_RECORDS, &recID, &rec));
	} while (rec->type != SALE);
	if (strcmp(rec->addInfo, "已退货")==0) {
		printf("该商品已退货\n");
		getchar();
		return;
	}
	Inventory* inv = invQueryID(gdata.inventory, rec->invID);
	if (!inv) {
		invQueryID(gdata.historyInventory, rec->invID);
		if (!inv) return;
		drawOrdMenu("退货商品已下架，是否重新上架?", 2, 1, "是", "否");
		breakDeliver(getUIntInput("选择一项:", &select, (IntRange) { 1, 2 }, true));
		if (select == 1) {
			listRemove(&inv->list);
			listAddTail(&inv->list, &gdata.inventory->list);
		}
	}

	Record* updateRecord = recordCreate();
	updateRecord->type = UPDATE;
	strcpy_s(rec->addInfo, INFOMAX, "已退货");
	sprintf_s(updateRecord->addInfo, INFOMAX, "退货,对应销售记录:%d",rec->recID);
	updateRecord->time = FVMTimerGetFVMTime(gdata.timer);
	updateRecord->invID = inv->invID;
	updateRecord->prod = rec->prod;
	if (rec->prod.pack == BULK) {
		updateRecord->prod.weight = rec->prod.weight;
		inv->prod.weight = centRound(inv->prod.weight + rec->prod.weight);
	}
	else if (rec->prod.pack == UNIT) {
		updateRecord->prod.quantity = rec->prod.quantity;
		inv->prod.quantity += rec->prod.quantity;
	}
	updateRecord->prod.unitPrice = 0;   //价格改变0
	updateRecord->prod.amount = -rec->prod.amount;
	financeIncome(gdata.finance, -rec->prod.amount); //退钱

	recordIDAllocate(updateRecord, gdata.record);
	listAddTail(&updateRecord->timeList, &gdata.record->timeList);
	listAddTail(&updateRecord->IRList, &inv->invRecord->IRList);
	printf("成功退货\n");
	getchar();
}

void saleExchange(FVMO gdata) {
	int recID;
	int select;
	bool destroy = false;
	Record* rec;
	do {
		breakDeliver(inputRecordID(gdata.record, TIME_RECORDS, &recID, &rec));
	} while (rec->type != SALE);
	if (strcmp(rec->addInfo, "已换货")==0) {
		printf("该商品已换货\n");
		getchar();
		return;
	}
	
	Inventory* inv = invQueryID(gdata.inventory, rec->invID);
	if (!inv) {
		invQueryID(gdata.historyInventory, rec->invID);
		if (!inv) return;
		printf("换货商品已下架");
		getchar();
		return;
	}
	if (inv->prod.pack==BULK) {
		if (fLess(inv->prod.weight, rec->prod.weight)) {
			printf("库存不足");
			getchar();
		}
	}
	else if (inv->prod.pack = UNIT) {
		if(inv->prod.quantity<rec->prod.quantity) {
			printf("库存不足");
			getchar();
		}
	}
	drawOrdMenu("是否丢弃回退商品?", 2, 1, "是", "否");
	breakDeliver(getUIntInput("选择一项:", &select, (IntRange) { 1, 2 }, true));
	if (select==1) destroy = true;
	Record* updateRecord = recordCreate();
	updateRecord->type = UPDATE;
	strcpy_s(rec->addInfo, INFOMAX, "已换货");
	sprintf_s(updateRecord->addInfo, INFOMAX, "换货,对应销售记录:%d", rec->recID);
	updateRecord->time = FVMTimerGetFVMTime(gdata.timer);
	updateRecord->invID = inv->invID;
	updateRecord->prod = rec->prod;
	if (destroy) {
		if (rec->prod.pack == BULK) {
			updateRecord->prod.weight = -rec->prod.weight;
			inv->prod.weight = centRound(inv->prod.weight - rec->prod.weight);
		}
		else if (rec->prod.pack == UNIT) {
			updateRecord->prod.quantity = -rec->prod.quantity;
			inv->prod.quantity -= rec->prod.quantity;
		}
	}
	else {
		updateRecord->prod.weight = updateRecord->prod.quantity=0;
	}
	updateRecord->prod.unitPrice = 0;   //价格改变0
	updateRecord->prod.amount = 0;
	recordIDAllocate(updateRecord, gdata.record);
	listAddTail(&updateRecord->timeList, &gdata.record->timeList);
	listAddTail(&updateRecord->IRList, &inv->invRecord->IRList);
	printf("成功换货\n");
	getchar();
}
static const Coord SaleRecordPos = { 2,3 };
void saleReturnExchangePage(FVMO gdata) {
	int inCustomFilter = 0, inInvRecord = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "记录筛选","取消筛选" };
	char invRecordOpt[2][20] = { "指定商品","取消指定商品" };
	Record* filterList = NULL, * invRecord = NULL;
	int select, num;

	Record filter, saleFilter;
	memset(&saleFilter, 0, sizeof(Record));
	saleFilter.type = SALE;
	saleFilter.time = saleFilter.lastTime = saleFilter.prod.expiration = TIME_NAN;
	filter = saleFilter;

	Inventory* inv = NULL;
	Record* rec = NULL;
	const int timeRec = TIME_RECORDS, invRec = INV_RECORDS;
	pageStackPush(pageStackCreate("仓管记录"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inInvRecord) {
			filterList = recordFilterListGen(invRecord, INV_RECORDS, &filter);
			drawListPage(gdata.renderer, SaleRecordPos, "商品销售记录", drawRecordList, &filterList->IRList, &pageStart, PageSize, RecordRectSize, &invRec);
		}
		else {
			filterList = recordFilterListGen(gdata.record, TIME_RECORDS, &filter);
			drawListPage(gdata.renderer, SaleRecordPos, "销售记录", drawRecordList, &filterList->timeList, &pageStart, PageSize, RecordRectSize, &timeRec);
		}
		recordListClear(filterList);
		free(filterList);

		drawMenu(gdata.renderer, invMenuPos, "商品退换", 7, 1,
			"上一页",
			"下一页",
			filterOpt[inCustomFilter],
			invRecordOpt[inInvRecord],
			"商品退货",
			"商品换货",
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
			if (inCustomFilter) {  //取消筛选
				inCustomFilter = 0;
				filter = saleFilter;
				pageStart = pageStartSave;
			}
			else {
				breakCatch(inputSaleRecordFilter(&filter)) {
					break;
				}
				inCustomFilter = 1;
				pageStartSave = pageStart;
				pageStart = 1;
			}
			break;
		case 4:
			if (inInvRecord) {
				inInvRecord = 0;
			}
			else {
				breakCatch(inputInventoryID(gdata.inventory, &num, &inv)) break;
				invRecord = inv->invRecord;
				inInvRecord = 1;
			}
			break;
		case 5:
			saleReturn(gdata);
			break;
		case 6:
			saleExchange(gdata);
			break;
		case 7:
			pageStackPop(gdata.pageStack);
			return;
		}

	}
}
void sale(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1, cartPageStart = 1;
	char filterOpt[2][20] = { "商品筛选","取消筛选" };
	char memberOpt[2][20] = { "会员:否","会员:是" };
	int isMember=0;
	Inventory* filterList = NULL, * inv = NULL, * cart = invListInit(invCreate());
	int select, num;
	Product filter;
	pageStackPush(pageStackCreate("销售系统"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, cartPos, "购物车", drawInvList, &cart->list, &cartPageStart, PageSize, invListRectSize, NULL);
		drawMenu(gdata.renderer, cartMenuPos, "购物车", 6, 11,
			"上一页",
			"下一页",
			"移除商品",
			"数量更改",
			memberOpt[isMember],
			"清空购物车");
		if (inFilter) {
			filterList = invFilterListGen(gdata.inventory, &filter);
			drawListPage(gdata.renderer, invListPos, "筛选信息", drawInvList, &filterList->list, &pageStart, PageSize, invListRectSize, NULL);
			invListClear(filterList);
			free(filterList);
		}
		else {
			drawListPage(gdata.renderer, invListPos, "库存信息", drawInvList, &gdata.inventory->list, &pageStart, PageSize, invListRectSize, NULL);
		}
		drawMenu(gdata.renderer, saleMenuPos, "商品销售", 8, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品详情",
			"加入购物车",
			"前往结算",
			"商品退换",
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
				breakCatch(inputProductFilter(&filter)) {
					break;
				}
				inFilter = true;
				pageStartSave = pageStart;
				pageStart = 1;
			}
			break;
		case 4:
			breakCatch(inputInventoryID(gdata.inventory, &num, &inv)) break;
			invDetails(inv, gdata);
			break;
		case 5:
			cartAdd(cart, gdata.inventory);
			break;
		case 6:
			if (cart->list.size >= 1) {
				if (salePlanSelect(gdata, cart,isMember) == SETTLE_SUCCESS) {
					invListClear(cart);
				}
			}
			break;
		case 7:
			saleReturnExchangePage(gdata);
			break;
		case 8:
			invListClear(cart);
			invDel(cart);
			pageStackPop(gdata.pageStack);
			return;
		case 11:
			cartPageStart -= PageSize;
			if (cartPageStart < 1) cartPageStart = 1;
			break;
		case 12:
			cartPageStart += PageSize;
			break;
		case 13:
			cartDelete(cart);
			break;
		case 14:
			cartQuantityModify(cart, gdata.inventory);
			break;
		case 15:
			if (isMember) {
				isMember = 0;
			}
			else {
				isMember = 1;
			}
			break;
		case 16:
			cartClear(cart);
			break;
		default:
			break;
		}
	}
}