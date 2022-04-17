#include<string.h>
#include"inv_manage.h"

static const int PageSize = 15;


void invManage(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "��Ʒɸѡ","ȡ��ɸѡ" };
	Inventory* filterList = NULL, * inv = NULL;
	int select, num;
	Product filter;
	pageStackPush(pageStackCreate("������"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inFilter) {
			filterList = invFilterListGen(gdata.inventory, &filter);
			drawListPage(gdata.renderer, invListPos, "ɸѡ��Ϣ", drawInvList, &filterList->list, &pageStart, PageSize, invListRectSize, NULL);
			invListClear(filterList);
			invDel(filterList);
		}
		else {
			drawListPage(gdata.renderer, invListPos, "�����Ϣ", drawInvList, &gdata.inventory->list, &pageStart, PageSize, invListRectSize, NULL);
		}

		drawMenu(gdata.renderer, invMenuPos, "������", 9, 1,
			"��һҳ",
			"��һҳ",
			filterOpt[inFilter],
			"��Ʒ����",
			"��Ʒ����",
			"��Ʒ�¼�/����",
			"�ֹܼ�¼",
			"�˳�",
			"�������");
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


char ramdomrange[20][100] = { "ƻ��","��","����","����","�ƹ�","����","������������������������","����С��ݮ" };
char variety[20][100] = { "��ħ","��������������","��ͨ��ͨ��ͨ��ͨ","������������","�����ز�" };
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
	rec->prod.unitPrice = rec->prod.purUPrice; //������¼�еĵ��۵ȼ��ڽ�������
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
	rec->prod.unitPrice = rec->prod.purUPrice; //������¼�еĵ��۵ȼ��ڽ�������
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
	breakDeliver(getStrInput("�����޸�ԭ��:", addinfo, INFOMAX, true));
	drawOrdMenu("ѡ��Ҫ�޸ĵ���Ϣ:", 3, 1, "��Ʒ��Ŀ", "��ƷƷ��", "��Ʒ�ۼ�");
	int select;
	breakDeliver(getUIntInput("ѡ��һ��:", &select, (IntRange) { 1, 3 }, true));
	switch (select)
	{
	case 1:
		if (inv->prod.pack == BULK) {
			double weight;
			breakDeliver(getDoubleInput("�����޸ĺ�Ŀ������:", &weight, WRANGE, true));
			weight = centRound(weight);
			updateRecord = recordCreate();
			updateRecord->prod = inv->prod;
			updateRecord->prod.weight = weight - inv->prod.weight;
			inv->prod.weight = weight;
		}
		else if (inv->prod.pack == UNIT) {
			int quantity;
			breakDeliver(getUIntInput("�����޸ĺ�Ŀ������:", &quantity, QRANGE, true));
			updateRecord = recordCreate();
			updateRecord->prod = inv->prod;
			updateRecord->prod.quantity = quantity - inv->prod.quantity;
			updateRecord->prod.quantity = quantity;
		}
		updateRecord->type = UPDATE;
		strcpy_s(updateRecord->addInfo, INFOMAX, addinfo);
		updateRecord->invID = inv->invID;
		updateRecord->prod.unitPrice=0;
		updateRecord->time = FVMTimerGetFVMTime(gdata.timer);
		recordIDAllocate(updateRecord, gdata.record);
		listAddTail(&updateRecord->timeList, &gdata.record->timeList);
		listAddTail(&updateRecord->IRList, &inv->invRecord->IRList);
		break;
	case 2:
		drawOrdMenu("Ʒ��:", 3, 1, "��", "��", "��");
		int quality;
		double dWeight = 0;
		int dQuantity = 0;
		breakDeliver(getUIntInput("ѡ����Ʒ��:", &quality, (IntRange) { 1, 3 }, true));
		if (inv->prod.pack == BULK) {
			breakDeliver(getDoubleInput("������Ӱ��Ŀ������:", &dWeight, (DoubleRange) { 0, inv->prod.weight }, true));
			dWeight = centRound(dWeight);
			updateRecord = recordCreate();
			updateRecord->prod = inv->prod;
			updateRecord->prod.weight = -dWeight;
			inv->prod.weight = centRound(inv->prod.weight - dWeight);
		}
		else if (inv->prod.pack == UNIT) {
			int dQuantity;
			breakDeliver(getUIntInput("������Ӱ��Ŀ������:", &dQuantity, (IntRange) { 0, inv->prod.quantity }, true));
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
			printf("�������ͬ������Ʒ�����Զ��ϲ�\n");
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
			printf("���ڿ����Ϣ��������Ʒ����Ʒ\n");
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
		breakDeliver(getDoubleInput("�����޸ĺ�����۵���:", &newPrice, UPRINCERANGE, true));
		newPrice = centRound(newPrice);
		updateRecord = recordCreate();
		updateRecord->prod = inv->prod;
		updateRecord->prod.quantity = updateRecord->prod.weight = 0;
		updateRecord->prod.unitPrice = centRound(newPrice-inv->prod.unitPrice);
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
	pageStackPush(pageStackCreate("��Ʒ����"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawTitleWindow(gdata.renderer, invDetailsPos, "��Ʒ����", InvDetailsRectSize);
		showInvDetails(gdata.renderer, (Coord) { invDetailsPos.x + 3, invDetailsPos.y + 1 }, inv);
		drawMenu(gdata.renderer, invDetailsMenuPos, "��Ʒ����", 3, 1,
			"�޸���Ϣ",
			"�ֹܼ�¼",
			"�˳�");
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
	char filterOpt[2][20] = { "��¼ɸѡ","ȡ��ɸѡ" };
	Record* showPage = NULL, * filterList = NULL;
	int select, num, invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	const int timeRec = TIME_RECORDS, invRec = INV_RECORDS;
	pageStackPush(pageStackCreate("�ֹܼ�¼"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inFilter) {
			filterList = recordFilterListGen(gdata.record, TIME_RECORDS, &filter);
			drawListPage(gdata.renderer, RecordListPos, "ɸѡ��Ϣ", drawRecordList, &filterList->timeList, &pageStart, PageSize, RecordRectSize, &timeRec);
			recordListClear(filterList);
			free(filterList);
		}
		else {
			drawListPage(gdata.renderer, RecordListPos, "�ֹܼ�¼", drawRecordList, &gdata.record->timeList, &pageStart, PageSize, RecordRectSize, &timeRec);
		}
		drawMenu(gdata.renderer, invMenuPos, "�ֹܼ�¼", 8, 1,
			"��һҳ",
			"��һҳ",
			filterOpt[inFilter],
			"��Ʒ��¼",
			"��¼����",
			"��¼ɾ��",
			"��¼�����滻",
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
	char filterOpt[2][20] = { "��¼ɸѡ","ȡ��ɸѡ" };
	Record* filterList = NULL;
	int select, num;
	int invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	int invRec = INV_RECORDS;
	pageStackPush(pageStackCreate("��Ʒ�ֹܼ�¼"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inFilter) {
			filterList = recordFilterListGen(invRecord, INV_RECORDS, &filter);
			drawListPage(gdata.renderer, RecordListPos, "ɸѡ��Ϣ", drawRecordList, &filterList->IRList, &pageStart, PageSize, RecordRectSize, &invRec);
			recordListClear(filterList);
			free(filterList);
		}
		else {
			drawListPage(gdata.renderer, RecordListPos, "��Ʒ�ֹܼ�¼", drawRecordList, &invRecord->IRList, &pageStart, PageSize, RecordRectSize, &invRec);
		}
		drawMenu(gdata.renderer, invMenuPos, "�ֹܼ�¼", 7, 1,
			"��һҳ",
			"��һҳ",
			filterOpt[inFilter],
			"��¼����",
			"��¼ɾ��",
			"��¼�����滻",
			"�˳�");
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
			if (inFilter) {  //ȡ��ɸѡ
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
	pageStackPush(pageStackCreate("��¼����"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawTitleWindow(gdata.renderer, RecordDetailsPos, "��¼����", RecordDetailsRectSize);
		showRecordDetails(gdata.renderer, (Coord) { RecordDetailsPos.x + 3, RecordDetailsPos.y + 1 }, record);
		drawMenu(gdata.renderer, RecordDetailsMenuPos, "��¼����", 1, 1,
			"�˳�");
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
		drawMenu("������", 7,
			"��һҳ",
			"��һҳ",
			"����",
			"ɾ��",
			"�滻",
			"���",
			"�˳�");
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