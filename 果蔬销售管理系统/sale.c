#include<string.h>
#include"sale.h"
static const int PageSize = 15;
Coord cartPos = { 2,83 }, cartMenuPos = { 21,83 };


int cartAdd(Inventory* cart, Inventory* head) {
	int id;
	Inventory* item, * inv;
	breakDeliver(inputInventoryID(head, &id, &inv));	//��ѯ�����Ϣ
	item = invQueryID(cart, id);						//��ѯ���ﳵ���Ƿ��Ѵ���
	int quantity;
	double weight;
	if (item == NULL) {
		if (inv->prod.pack == UNIT) {
			if (inv->prod.quantity <= 0) {
				printf("��治��\n");
				getchar();
				return -1;
			}
			breakDeliver(getUIntInput("�����빺������:", &quantity, (IntRange) { 0, inv->prod.quantity }, true));
			item = invCopyCreate(inv);
			item->prod.quantity = quantity;
			item->prod.amount = item->prod.quantity * item->prod.unitPrice;
		}
		else if (inv->prod.pack == BULK) {
			if (fLessEq(inv->prod.weight, 0)) {
				printf("��治��");
				getchar();
				return -1;
			}
			breakDeliver(getDoubleInput("�����빺������:", &weight, (DoubleRange) { 0, inv->prod.weight }, true));
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
				printf("��治��\n");
				getchar();
				return -1;
			}
			breakDeliver(getUIntInput("�����빺������:", &quantity, (IntRange) { 0, inv->prod.quantity - item->prod.quantity }, true)); //ȷ�����Ӻ󲻳��������
			item->prod.quantity += quantity;
			item->prod.amount = item->prod.quantity * item->prod.unitPrice;
		}
		else if (inv->prod.pack == BULK) {
			if (fLessEq(inv->prod.weight - item->prod.weight, 0)) {
				printf("��治��");
				getchar();
				return -1;
			}
			breakDeliver(getDoubleInput("�����빺������:", &weight, (DoubleRange) { 0, inv->prod.weight - item->prod.weight }, true));
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
	breakDeliver(inputInventoryID(head, &id, &inv));	//��ѯ�����Ϣ
	item = invQueryID(cart, id);						//��ѯ���ﳵ���Ƿ��Ѵ���
	int quantity;
	double weight;
	if (item) {
		if (item->prod.pack == UNIT) {
			breakDeliver(getUIntInput("�������¹�������:", &quantity, (IntRange) { 1, inv->prod.quantity }, true)); //ȷ�����Ӻ󲻳��������
			item->prod.quantity = quantity;
		}
		else if (inv->prod.pack == BULK) {
			breakDeliver(getDoubleInput("�������¹�������:", &weight, (DoubleRange) { 0.01, inv->prod.weight }, true));
			item->prod.weight = weight;
		}
	}
	else {
		printf("���ﳵ���޴���Ʒ");
	}

}
void cartClear(Inventory* cart) {
	invListClear(cart);
}

int inputInventoryIDinPreOrder(Record* head, int* id, Record** pRec) {
	while (1) {
		breakDeliver(getUIntInput("������ƷID:", id, ALLINT, true));
		listForEachEntry(Record, pos, &head->timeList, timeList) {
			if (pos->invID == *id) {
				*pRec = pos;
				return;
			}
		}
		printf("�����в��޴���Ʒ\n");
	}
}

int SSPSelect(FVMO gdata, Record* preOrder) {
	Record* rec;
	CSP* csp;
	SSP* ssp;
	int recID, cspID, sspID;
	breakDeliver(inputInventoryIDinPreOrder(preOrder, &recID, &rec));
	if (rec->CSPID && (csp = CSPQueryID(gdata.CSP, rec->CSPID))) {
		if (!csp->overlaySingleSP) {
			printf("����Ʒ��Ӧ����Ϸ��������뵥Ʒ��������(�����������)\n");
			getchar();
			return;
		}
	}
	breakDeliver(inputSSPID(gdata.SSP, &sspID, &ssp));
	/*ȫ������*/
	if (rec->invID != ssp->invID && !productMatch(&rec->prod, &ssp->filter)) {
		printf("��Ʒ������÷�������");
		return;
	}
	else {
		rec->discount = ssp->discount;
		rec->SSPID = ssp->SSPID;
	}
}
int CSPSlect(CSP* optCSP, Record* preOrder) {
	CSP* csp;
	int cspID;
	breakDeliver(inputCSPID(optCSP, &cspID, &csp));
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
void SSPAutoApplicate(SSP* head, Record* preOrder) {
	listForEachEntry(SSP, sspPos, &head->list, list) {
		/*
		��������SSP��ȫ������ ...
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
		pos->prod.amount = centRound(pos->prod.amount*pos->discount);
		listAddTail(&preOrder->IRList, &gdata.order->IRList);
		recordIDAllocate(preOrder, gdata.order);


		/*
		* ���ﴦ���˵�
		*/
		financeIncome(gdata.finance, pos->prod.amount);

		//������
		inv = invQueryID(gdata.inventory, pos->invID);
		inv->prod.quantity -= pos->prod.quantity;
		inv->prod.weight = centRound(inv->prod.weight-pos->prod.weight);

		//��Ӽ�¼
		rec = recordCreate();
		memcpy(rec, pos, sizeof(Record));
		recordIDAllocate(rec, gdata.record);
		listAddTail(&rec->timeList, &gdata.record->timeList);
		listAddTail(&rec->IRList, &inv->invRecord->IRList);
	}
	return 0;
}

#define SETTLE_SUCCESS 0
#define SETTLE_BREAK -1
int settlement(FVMO gdata, Record* preOrder) {
	int orderPageStart = 1, optCSPPageStart = 1;
	int select, num;
	pageStackPush(pageStackCreate("��������"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, PreOrderPos, "���ն���", drawPreOrderList, &preOrder->timeList, &orderPageStart, PageSize, PreOrderRectSize, &gdata);
		drawMenu(gdata.renderer, PreOrderMenu, "��������", 4, 1,
			"��һҳ",
			"��һҳ",
			"ȷ�Ͻ���",
			"�˳�");
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
		breakDeliver(getUIntInput("����ƷΪ�����Ʒ�����������(Ĭ��һ��):", &mul, QRANGE, false));
	}
	if (inv = invQueryID(gdata.inventory, gift->invID)) {
		if (gift->prod.pack == BULK) {
			if (gift->prod.weight * mul > inv->prod.weight) {
				printf("�����������(���������)\n");
				getchar();
				return -1;
			}
		}
		else if (gift->prod.pack == UNIT) {
			if (gift->prod.quantity * mul > inv->prod.quantity) {
				printf("�����������(���������)\n");
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
		printf("��������޴���Ʒ(���������)\n");
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
	pageStackPush(pageStackCreate("��Ʒѡ��"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, PreOrderPos, "����", drawPreOrderList, &preOrder->timeList, &orderPageStart, PageSize, PreOrderRectSize, &gdata);

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
			drawTitleWindow(gdata.renderer, SSPGiftPos,"���޿�ѡ��Ʒ", GiftRectSize);
		}
		drawMenu(gdata.renderer, SalePlanMenu, "��Ʒѡ��", 6, 1,
			"������һҳ",
			"������һҳ",
			"ѡ��ǰ������Ʒ",
			"��һ������Ʒ",
			"ǰ������",
			"�˳�");
		drawMenu(gdata.renderer, optCSPMenuPos, "��Ʒ���", 2, 11,
			"��һҳ",
			"��һҳ");

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
				printf("���޿�ѡ��Ʒ(���������)\n");
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
				printf("������һ����(���������)\n");
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
		//�г��õ��ķ���
		//ö��ÿ������ѡ������
		//ѡ����Ͻ������ȷ��ҳ��
	}
}
int salePlanSelect(FVMO gdata, Inventory* cart) {
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
	SSPAutoApplicate(gdata.SSP, preOrder);
	pageStackPush(pageStackCreate("���۷���ѡ��"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, PreOrderPos, "����", drawPreOrderList, &preOrder->timeList, &orderPageStart, PageSize, PreOrderRectSize, &gdata);
		if (optCSP) {
			CSPListClear(optCSP);
			CSPDel(optCSP);
		}
		optCSP = CSPOptionalListGen(gdata.CSP, preOrder);
		drawListPage(gdata.renderer, optCSPPos, "����������۷���", drawCSPList, &optCSP->list, &optCSPPageStart, PageSize, optCSPRectSize, NULL);
		drawMenu(gdata.renderer, SalePlanMenu, "���۷���ѡ��", 8, 1,
			"������һҳ",
			"������һҳ",
			"Ӧ�õ�Ʒ���۷���",
			"Ӧ��������۷���",
			"ȡ��Ӧ�õ�Ʒ���۷���",
			"ȡ��Ӧ��������۷���",
			"��һ��(��Ʒѡ��)",
			"�˳�");
		drawMenu(gdata.renderer, optCSPMenuPos, "����������۷������", 3, 11,
			"��һҳ",
			"��һҳ",
			"��������");
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
			SSPSelect(gdata, preOrder);
			break;
		case 4:
			CSPSlect(optCSP, preOrder);
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
	//1.���ɳ�ʼ��¼
	//2.����sspƥ�������Ʒ���Զ�Ӧ�������ۿ�
	//3.����csp�г����÷���
	//4.ȷ�Ϸ���
	//5.�г���ѡ��Ʒ
	//6.���㣬���ɶ���
}
void sale(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1, cartPageStart = 1;
	char filterOpt[2][20] = { "��Ʒɸѡ","ȡ��ɸѡ" };
	Inventory* filterList = NULL, * inv = NULL, * cart = invListInit(invCreate());
	int select, num;
	Product filter;
	pageStackPush(pageStackCreate("����ϵͳ"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, cartPos, "���ﳵ", drawInvList, &cart->list, &cartPageStart, PageSize, invListRectSize, NULL);
		drawMenu(gdata.renderer, cartMenuPos, "���ﳵ", 5, 11,
			"��һҳ",
			"��һҳ",
			"�Ƴ���Ʒ",
			"��������",
			"��չ��ﳵ");
		if (inFilter) {
			filterList = invFilterListGen(gdata.inventory, &filter);
			drawListPage(gdata.renderer, invListPos, "ɸѡ��Ϣ", drawInvList, &filterList->list, &pageStart, PageSize, invListRectSize, NULL);
			invListClear(filterList);
			free(filterList);
		}
		else {
			drawListPage(gdata.renderer, invListPos, "�����Ϣ", drawInvList, &gdata.inventory->list, &pageStart, PageSize, invListRectSize, NULL);
		}
		drawMenu(gdata.renderer, saleMenuPos, "��Ʒ����", 7, 1,
			"��һҳ",
			"��һҳ",
			filterOpt[inFilter],
			"��Ʒ����",
			"���빺�ﳵ",
			"ǰ������",
			"�˳�");
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
			if (inFilter) {  //ȡ��ɸѡ
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
				if (salePlanSelect(gdata, cart) == SETTLE_SUCCESS) {
					invListClear(cart);
				}
			}
			break;
		case 7:
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
			cartClear(cart);
			break;
		default:
			break;
		}
	}
}