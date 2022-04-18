#include"statistics.h"
#include"inv_manage.h"
static const Coord StatsPageMenuPos = { 3,3 };
static const Coord statsPos = { 2,3 }, statsMenuPos = { 25,3 };
static const Coord InvCheckPos = { 2,3 }, financeCheckPos = { 22,4 }, statsCheckMenuPos = { 25,3 };
static const int PageSize = 15;

void statsCheckPage(FVMO gdata) {

	int errPageStart = 1;
	int select, num;

	Record* errList = recordListInit(recordCreate()), * errinfo = NULL, * rec = NULL;
	char errmsg[INV_CHECK_MSG_MAX];
	int errRecID, recID;
	listForEachEntry(Inventory, pos, &gdata.inventory->list, list) {
		if (errRecID = invRecCheck(pos, errmsg)) {
			errinfo = recordCreate();
			errinfo->recID = errRecID;
			errinfo->invID = pos->invID;
			strcpy_s(errinfo->addInfo, INV_CHECK_MSG_MAX, errmsg);
			errinfo->prod = pos->prod;
			listAddTail(&errinfo->timeList, &errList->timeList);
		}
	}

	bool financeError = false;
	Finance checkFinance = recordStatsFinance(gdata.record, TIME_RECORDS, NULL, gdata.finance->startUpCapital);
	if (gdata.finance->balance != checkFinance.balance || gdata.finance->cost != checkFinance.cost ||
		gdata.finance->turnover != checkFinance.turnover || gdata.finance->profit != checkFinance.profit) {
		financeError = true;
	}
	CellData financeData[3][6] = {
		{ {drawCellStr,23,0,"�ʽ�˶�"}, {drawCellStr,23,0,"�����ʽ�"} ,{drawCellStr,23,0,"�����ʽ�"},
		  {drawCellStr,24,0,"Ӫҵ��"},{drawCellStr,24,0,"�ɱ�"},{drawCellStr,24,0,"ӯ����"} },
		{ {drawCellStr,23,0,"�̵��ʽ�"} ,{drawCellDouble,23,0,&gdata.finance->startUpCapital},{drawCellDouble,23,0,&gdata.finance->balance},
		  {drawCellDouble,24,0,&gdata.finance->turnover},{drawCellDouble,24,0,&gdata.finance->cost},{drawCellDouble,24,0,&gdata.finance->profit} },
		{ {drawCellStr,23,0,"��¼�����ʽ�"} ,{drawCellDouble,23,0,&checkFinance.startUpCapital},{drawCellDouble,23,0,&checkFinance.balance},
		  {drawCellDouble,24,0,&checkFinance.turnover},{drawCellDouble,24,0,&checkFinance.cost},{drawCellDouble,24,0,&checkFinance.profit} }
	};

	pageStackPush(pageStackCreate("���ݺ˶�"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (errList->timeList.size) {
			drawListPage(gdata.renderer, InvCheckPos, "�����Ϣ�˶Գ��ִ���", drawInvCheckList, &errList->timeList, &errPageStart, PageSize, InvCheckRectSize, NULL);
		}
		else {
			drawRectBorder(gdata.renderer, InvCheckPos, InvCheckRectSize);
			Coord midPos = InvCheckPos;
			midPos.x += InvCheckRectSize.x / 2;
			midPos.y += InvCheckRectSize.y / 2 - 10;
			coordPrintf(gdata.renderer, midPos, "�����Ϣ���ݺ˶�����");
		}
		Coord cur = financeCheckPos;
		for (int i = 0; i < 3; i++) {
			if (financeError) drawColorBar(gdata.renderer, cur, 230, 140, 140, InvCheckRectSize.y);
			else drawColorBar(gdata.renderer, cur, 160, 220, 170, InvCheckRectSize.y);
			drawListItem(gdata.renderer, cur, financeData[i], 6);
			cur.x++;
		}
		drawMenu(gdata.renderer, statsCheckMenuPos, "���ݺ˶�", 4, 1,
			"��һҳ",
			"��һҳ",
			"�鿴��¼����",
			"�˳�");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select) {
		case 1:
			errPageStart -= PageSize;
			if (errPageStart < 1) errPageStart = 1;
			break;
		case 2:
			errPageStart += PageSize;
			break;
		case 3:
			breakCatch(inputRecordID(gdata.record, TIME_RECORDS, &recID, &rec)) break;
			recDetails(rec, gdata);
			break;
		case 4:
			recordListClear(errList);
			recordDel(errList);
			pageStackPop(gdata.pageStack);
			return;
		default:
			break;
		}
	}
}
void statsPage(FVMO gdata) {
	int invPageStart = 1;
	int inHistoryInv=0;
	int select, num;
	bool drawGlobalFinance = false;
	Inventory* finInv = NULL, * inv = NULL,*optList=NULL;
	Record dateFilter;
	InvStatsArg isa;
	int inDateFilter = 0;
	char filterOpt[2][20] = { "ָ��ʱ���","ȡ��ָ��ʱ���" }, filterTitle[50], * titleCur;
	char historyOpt[2][20] = { "ͳ����ʷ��Ʒ","ȡ��ͳ����ʷ��Ʒ" };
	struct tm sdate, edate;
	pageStackPush(pageStackCreate("����ͳ��"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inHistoryInv) {
			optList = gdata.historyInventory;
		}
		else {
			optList = gdata.inventory;
		}
		if (inDateFilter) {
			localtime_s(&sdate, &dateFilter.time);
			localtime_s(&edate, &dateFilter.lastTime);
			titleCur = filterTitle + sprintf_s(filterTitle, 10, "����ͳ��");
			if (dateFilter.time != TIME_NAN) titleCur += sprintf_s(titleCur, 20, "(%d/%d/%d - ", sdate.tm_year + 1900, sdate.tm_mon + 1, sdate.tm_mday);
			else titleCur += sprintf_s(titleCur, 20, "(���� - ");
			if (dateFilter.lastTime != TIME_NAN) titleCur += sprintf_s(titleCur, 20, "%d/%d/%d)", edate.tm_year + 1900, edate.tm_mon + 1, edate.tm_mday);
			else titleCur += sprintf_s(titleCur, 20, "����)");
			isa = (InvStatsArg){ &dateFilter,&gdata };
			drawListPage(gdata.renderer, statsPos, filterTitle, drawInvStatsList, &optList->list, &invPageStart, PageSize, statsRectSize, &isa);
		}
		else {
			isa = (InvStatsArg){ NULL,&gdata };
			drawListPage(gdata.renderer, statsPos, "����ͳ��", drawInvStatsList, &optList->list, &invPageStart, PageSize, statsRectSize, &isa);
		}

		drawMenu(gdata.renderer, statsMenuPos, "����ͳ��", 7, 1,
			"��һҳ",
			"��һҳ",
			"�鿴��Ʒ��¼",
			filterOpt[inDateFilter],
			historyOpt[inHistoryInv],
			"���ݺ˶�",
			"�˳�");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select) {
		case 1:
			invPageStart -= PageSize;
			if (invPageStart < 1) invPageStart = 1;
			break;
		case 2:
			invPageStart += PageSize;
			break;
		case 3:
			breakCatch(inputInventoryID(gdata.inventory, &num, &inv)) break;
			invRecordPage(inv->invRecord, gdata);
			break;
		case 4:
			if (inDateFilter) {
				inDateFilter = 0;
			}
			else {
				memset(&dateFilter, 0, sizeof(Record));
				dateFilter.time = dateFilter.lastTime = dateFilter.prod.expiration = TIME_NAN;
				breakCatch(getDateTime("��ʼ����(��.��.��)(Ĭ�ϲ���) : ", &dateFilter.time, false)) break;
				breakCatch(getDateTime("��ֹ����(��.��.��)(Ĭ�ϲ���) : ", &dateFilter.lastTime, false)) break;
				if (dateFilter.lastTime != TIME_NAN)dateFilter.lastTime += 3600 * 24 - 1;
				inDateFilter = 1;
			}
			break;
		case 5:
			if (inHistoryInv) {
				inHistoryInv = 0;
			}
			else {
				inHistoryInv = 1;
			}
			break;
		case 6:
			statsCheckPage(gdata);
			break;
		case 7:
			pageStackPop(gdata.pageStack);
			return;
		default:
			break;
		}
	}
}
