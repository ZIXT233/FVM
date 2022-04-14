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
			drawInvPage(gdata.renderer, invListPos,"ɸѡ��Ϣ", invShowPageJump(filterList, &pageStart, PageSize), pageStart, PageSize);
			invListClear(filterList);
			invDel(filterList);
		}
		else {
			drawInvPage(gdata.renderer, invListPos, "�����Ϣ", invShowPageJump(gdata.inventory, &pageStart, PageSize), pageStart, PageSize);
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
			invDetails(inv,gdata);
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
	if (inv->prod.pack == UNIT) rec->prod.amount = inv->prod.quantity * inv->prod.purUPrice;
	else if (inv->prod.pack == BULK) rec->prod.amount = inv->prod.weight * inv->prod.purUPrice;
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
	if (inv->prod.pack == UNIT) rec->prod.amount = inv->prod.quantity * inv->prod.purUPrice;
	else if (inv->prod.pack == BULK) rec->prod.amount = inv->prod.weight * inv->prod.purUPrice;
	listAddTail(&rec->timeList, &gdata.record->timeList);
	listAddTail(&rec->IRList, &inv->invRecord->IRList);;
	listAddTail(&inv->list, &head->list);
	return;
}

void invDetails(Inventory* inv,FVMO gdata) {
	int invID, select;
	struct tm date;
	pageStackPush(pageStackCreate("��Ʒ����"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		showInvDetails(gdata.renderer,UI_ORIGIN,inv);
		drawMenu(gdata.renderer, (Coord) { 20, 3 }, "��Ʒ����", 3, 1,
			"�޸���Ϣ",
			"�ֹܼ�¼",
			"�˳�");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			break;
		case 2:
			invRecordPage(inv->invRecord,gdata);
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
	inv->prod.purUPrice = 300 * (double)rand() / RAND_MAX;
	inv->prod.unitPrice = inv->prod.purUPrice + 20.0 * rand() / RAND_MAX;
	inv->prod.weight = inv->prod.quantity = 0;
	if (inv->prod.pack == UNIT) inv->prod.quantity = rand() * 100;
	else inv->prod.weight = (double)10000 * rand() / RAND_MAX;
	inv->invRecord = recordListInit(recordCreate());
	return inv;
}


void recordPage(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "��¼ɸѡ","ȡ��ɸѡ" };
	Record* showPage = NULL, * filterList = NULL;
	int select, num, invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	pageStackPush(pageStackCreate("�ֹܼ�¼"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inFilter) {
			filterList = recordFilterListGen(gdata.record, TIME_RECORDS, &filter);
			drawRecordList(gdata.renderer,UI_ORIGIN, recordShowPageJump(filterList, TIME_RECORDS, &pageStart, PageSize),TIME_RECORDS, PageSize);
			recordListClear(filterList);
			free(filterList);
		}
		else {
			drawRecordList(gdata.renderer, UI_ORIGIN, recordShowPageJump(gdata.record, TIME_RECORDS, &pageStart, PageSize),TIME_RECORDS, PageSize);
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
		inputStart(gdata.renderer,INPUT_ORIGIN);
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
			invRecordPage(gdata.renderer,gdata);
			break;
		case 5:
			while (1) {
				recID = -1;
				breakCatch(getUIntInput("�������¼ID:", &recID, ALLINT, true)) break;
				if (rec = recordQueryID(gdata.record, recID, 0)) break;
				else {
					printf("�޴˼�¼��\n");
				}
			}
			if (recID < 0) break;
			recDetails(rec,gdata);
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
			pageStackPop(gdata.pageStack);
			return;
		}

	}
}
void invRecordPage(Record* invRecord,FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1;
	char filterOpt[2][20] = { "��¼ɸѡ","ȡ��ɸѡ" };
	Record* filterList = NULL;
	int select, num;
	int invID, recID;
	Record filter;
	Inventory* inv = NULL;
	Record* rec = NULL;
	pageStackPush(pageStackCreate("��Ʒ�ֹܼ�¼"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inFilter) {
			filterList = recordFilterListGen(invRecord, INV_RECORDS, &filter);
			drawRecordList(gdata.renderer, UI_ORIGIN, recordShowPageJump(filterList, INV_RECORDS, &pageStart, PageSize),INV_RECORDS, PageSize);
			recordListClear(filterList);
			free(filterList);
		}
		else {
			drawRecordList(gdata.renderer, UI_ORIGIN, recordShowPageJump(invRecord, INV_RECORDS, &pageStart, PageSize),INV_RECORDS, PageSize);
		}
		drawMenu(gdata.renderer,invMenuPos, "�ֹܼ�¼", 7, 1,
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
			while (1) {
				recID = -1;
				breakCatch(getUIntInput("�������¼ID:", &recID, ALLINT, true)) break;
				if (rec = recordQueryID(invRecord, recID, 1)) break;
				else {
					printf("�޴˼�¼��\n");
				}
			}
			if (recID < 0) break;
			recDetails(rec,gdata);
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
			pageStackPop(gdata.pageStack);
			return;
		}

	}
}

void recDetails(Record* record,FVMO gdata) {
	int select;
	pageStackPush(pageStackCreate("��¼����"), gdata.pageStack);
	while (1) {	
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		showRecordDetails(gdata.renderer,UI_ORIGIN,record);
		drawMenu(gdata.renderer, (Coord) { 20, 3 }, "��¼����", 1, 1,
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