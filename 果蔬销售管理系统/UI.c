#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>
#include"UI.h"
#include"renderer.h"

#define ESC "\033"
#define CSI "\033["

static char recordType[5][20] = { "","������¼","���ۼ�¼","���¼�¼","���ͼ�¼" };
int typeDirect[5] = { 0,1,-1,1,-1 };

char quanlityText[5][20] = { "","��","��","��" };
char packText[3][20] = { "","ɢװ","��Ԫװ" };



void setTextColor(Renderer* renderer, int r, int g, int b) {
	renderPrintf(renderer, "\033[38;2;%d;%d;%dm", r, g, b);
}
void resetTextColor(Renderer* renderer) {
	renderPrintf(renderer, "\033[39m");
}

void drawRectBorder(Renderer* renderer, Coord pos, Coord const size) {
	renderPrintf(renderer, ESC "(0");
	renderPrintf(renderer, CSI "49;94m");
	setCursorPos(renderer, pos);
	coordPrintf(renderer, pos, "l");
	for (int i = 1; i <= size.y; i++) {
		renderPrintf(renderer, "q");
	}
	renderPrintf(renderer, "k");
	pos.x++;
	for (int i = 1; i <= size.x; i++) {
		coordPrintf(renderer, pos, "x");
		pos.y += size.y + 1;
		coordPrintf(renderer, pos, "x");
		pos.y -= size.y + 1;
		pos.x++;
	}
	coordPrintf(renderer, pos, "m");
	for (int i = 1; i <= size.y; i++) {
		renderPrintf(renderer, "q");
	}
	renderPrintf(renderer, "j");
	renderPrintf(renderer, CSI "0m");
	renderPrintf(renderer, ESC "(B");
	pos.x++;
	setCursorPos(renderer, pos);
}

void drawVerticalLine(Renderer* renderer, Coord pos, int height) {
	renderPrintf(renderer, ESC "(0");
	setCursorPos(renderer, pos);
	coordPrintf(renderer, pos, "l");
	for (int i = 1; i <= height; i++) {
		coordPrintf(renderer, pos, "x");
		pos.x++;
	}
	renderPrintf(renderer, ESC "(B");
}
void drawMenu(Renderer* renderer, Coord origin, const char* title, int n, int firstNum, ...) {
	va_list args;
	va_start(args, firstNum);
	Coord rectSize;
	rectSize.x = n + 1;
	rectSize.y = 62;
	drawRectBorder(renderer, origin, rectSize);
	origin.x++;
	origin.y++;
	coordPrintf(renderer, origin, "%s", title);
	origin.x++;
	for (int i = 0; i < n; i++) {
		char* item = va_arg(args, char*);
		coordPrintf(renderer, origin, "\t%d.%s", firstNum + i, item);
		origin.x++;
	}
	origin.y--;
	origin.x++;
	setCursorPos(renderer, origin);
}


void strMakeLen(char* dest, const char* src, int len) {
	int slen = strlen(src);
	for (int i = 0; i < len - 1; i++) {
		if (i >= slen) dest[i] = ' ';
		else if (i >= len - 4) dest[i] = '.';
		else dest[i] = src[i];
	}
	dest[len - 1] = '\0';
}
void drawInvPage(Renderer* renderer, Coord origin, const char* title, Inventory* start, int pageStartNum, int pageSize) {
	Coord rectSize;
	rectSize.x = 1;
	rectSize.y = 75;
	drawRectBorder(renderer, origin, rectSize);
	rectSize.x = pageSize + 3;
	drawRectBorder(renderer, origin, rectSize);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y / 2 - strlen(title) / 2 }, "%s", title);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y - 7 }, "%d/%dҳ", ((pageStartNum - 1) / pageSize) + 1, ((start->list.root->size - 1) / pageSize) + 1);
	origin.x += 3;
	origin.y++;
}
void drawInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = listEntry(entry, Inventory, list);
	Coord cur = origin;
	CellData cellName[5] = { {drawCellStr,14,0,"����"} ,{drawCellStr,14,0,"Ʒ��"},{drawCellStr,14,0,"���۵���"},
		{drawCellStr,14,0,"ID"},{drawCellStr,14,0,"����"} };
	drawColorBar(renderer, cur, 238, 232, 213, 76);
	drawListItem(renderer, cur, cellName, 5);
	cur.x++;
	CellDataDrawer packDrawer = NULL;
	int n = 0;
	void* qw = NULL;

	listForEachEntry(Inventory, pos, &(start->list), list) {
		if (n == pageSize) break;
		if (pos->prod.pack == BULK) {
			qw = &pos->prod.weight;
			packDrawer = drawCellBULK;
		}
		else if (pos->prod.pack == UNIT) {
			qw = &pos->prod.quantity;
			packDrawer = drawCellUNIT;
		}
		CellData cellData[5] = { {drawCellStr,14,0,pos->prod.kind} ,
									{drawCellStr,14,0,pos->prod.variety},
									{drawCellDouble,14,1,&pos->prod.unitPrice},
									{drawCellInt,14,0,&pos->invID},
									{packDrawer,14,0,qw} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, 76);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 5);
		cur.x++;
		n++;
	}
}


void drawRecordList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Record* start = NULL;
	Coord cur = origin;
	int type = *(int*)exArg;
	int n = 0;
	int quantity;
	double weight, amount;
	void* qw = NULL;
	CellDataDrawer packDrawer = NULL;
	struct tm date;
	if (type == TIME_RECORDS) start = (Record*)listEntry(entry, Record, timeList);
	else if (type == INV_RECORDS) start = (Record*)listEntry(entry, Record, IRList);


	CellData cellName[8] = { {drawCellStr,12,0,"����"} ,{drawCellStr,12,0,"��¼ID"},{drawCellStr,12,0,"��ƷID"},{drawCellStr,12,0,"����"},
	{drawCellStr,12,0,"Ʒ��"},{drawCellStr,15,0,"��Ʒ����"}, {drawCellStr,15,0,"�ʽ����"},{drawCellStr,22,0,"ʱ��"} };
	drawColorBar(renderer, cur, 238, 232, 213, RecordRectSize.y);
	drawListItem(renderer, cur, cellName, 8);
	cur.x++;
	recordForEachStart(pos, type, entry) {
		if (n == pageSize) break;
		if (pos->prod.pack == BULK) {
			weight = pos->prod.weight * recordTypeProdDirect[pos->type];
			qw = &weight;
			packDrawer = drawCellBULK;
		}
		else if (pos->prod.pack == UNIT) {
			quantity = pos->prod.quantity * recordTypeProdDirect[pos->type];
			qw = &quantity;
			packDrawer = drawCellUNIT;
		}
		amount = pos->prod.amount * recordTypeFinanceDirect[pos->type];
		CellData cellData[8] = { {drawCellStr,12,0,recordType[pos->type]} ,
								{drawCellInt,12,0, &pos->recID},
								{drawCellInt,12,0, &pos->invID},
								{drawCellStr,12,0,pos->prod.kind},
								{drawCellStr,12,0,pos->prod.variety},
								{packDrawer,15,1,qw},
								{drawCellDouble,15,1,&amount},
								{drawCellTime,22,1,&pos->time} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, RecordRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 8);

		cur.x++;
		n++;
	}recordForEachEnd(pos, type, entry)
}

void drawListPage(Renderer* renderer, Coord origin, const char* title, ListDrawer drawer, ListHead* start, int* pageStartNum, int pageSize, Coord rectSize, void* exArg) {
	Coord titleRect;
	titleRect.x = 1;
	titleRect.y = rectSize.y;
	drawRectBorder(renderer, origin, titleRect);
	if (pageSize + 3 > rectSize.x) rectSize.x = pageSize + 3;
	drawRectBorder(renderer, origin, rectSize);

	start = listShowPageJump(start, pageStartNum, pageSize);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y / 2 - strlen(title) / 2 }, "%s", title);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y - 7 }, "%d/%dҳ", ((*pageStartNum - 1) / pageSize) + 1, ((start->root->size - 1) / pageSize) + 1);
	origin.x += 3;
	origin.y++;
	drawer(renderer, origin, start, pageSize, exArg);
}
void drawSSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	SSP* start = listEntry(entry, SSP, list);
	Coord cur = origin;

	CellData cellName[2] = { {drawCellStr,12,0,"������"} ,{drawCellStr,12,0,"����ID"} };
	drawColorBar(renderer, cur, 238, 232, 213, 31);
	drawListItem(renderer, cur, cellName, 2);
	cur.x++;
	int n = 0;

	listForEachEntry(SSP, pos, &(start->list), list) {
		if (n == pageSize) break;
		CellData cellData[2] = { {drawCellStr,12,0,pos->planName} ,
									{drawCellInt,12,0,&pos->SSPID} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, 31);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 2);
		cur.x++;
		n++;
	}

}
void drawCSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	CSP* start = listEntry(entry, CSP, list);
	Coord cur = origin;

	CellData cellName[2] = { {drawCellStr,12,0,"������"} ,{drawCellStr,12,0,"����ID"} };
	drawColorBar(renderer, cur, 238, 232, 213, 31);
	drawListItem(renderer, cur, cellName, 2);
	cur.x++;
	int n = 0;

	listForEachEntry(CSP, pos, &(start->list), list) {
		if (n == pageSize) break;
		CellData cellData[2] = { {drawCellStr,12,0,pos->planName} ,
									{drawCellInt,12,0,&pos->CSPID} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, 31);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 2);
		cur.x++;
		n++;
	}
}
void drawComInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = invEntry(entry);
	FVMO* gdata = (FVMO*)exArg;
	coordPrintf(renderer, origin, "Ʒ��\t\t����\t\t��ƷID");
	origin.x++;
	int n = 0;
	char kind[30], var[30];
	listForEachEntry(Inventory, pos, &start->list, list) {
		if (n == pageSize) break;

		strMakeLen(kind, pos->prod.kind, 12);
		strMakeLen(var, pos->prod.variety, 10);
		coordPrintf(renderer, origin, "%s\t%s\t%d", kind, var, pos->invID);
		origin.x++;
		n++;
	}
	while (n++ != pageSize) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(renderer, origin);
}
void drawPreOrderList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Record* start = listEntry(entry, Record, timeList);
	FVMO* gdata = (FVMO*)exArg;
	Coord cur = origin;
	SSP* ssp=NULL;
	CSP* csp=NULL;
	void* qw=NULL;
	CellDataDrawer packDrawer = NULL;
	CellData cellName[9] = { {drawCellStr,12,0,"����"},{drawCellStr,12,0,"Ʒ��"},{drawCellStr,12,0,"��ƷID"},
								{drawCellStr,20,0,"��Ʒ���۷���/ID"},{drawCellStr,20,0,"������۷���/ID"},{drawCellStr,12,0,"����"},
								{drawCellStr,12,0,"������"},{drawCellStr,7,1,"�ۿ���"},{drawCellStr,15,0,"���"} };
	drawColorBar(renderer, cur, 238, 232, 213, PreOrderRectSize.y);
	drawListItem(renderer, cur, cellName, 9);
	cur.x++;
	int n = 0;
	
	listForEachEntry(Record, pos, &start->timeList, timeList) {
		if (n == pageSize) break;
		ssp = SSPQueryID(gdata->SSP, pos->SSPID);
		csp = CSPQueryID(gdata->CSP, pos->CSPID);
		if (pos->prod.pack == BULK) {
			qw = &pos->prod.weight;
			packDrawer = drawCellBULK;
		}
		else if (pos->prod.pack == UNIT) {
			qw = &pos->prod.quantity;
			packDrawer = drawCellUNIT;
		}
		CellData cellData[9] = { {drawCellStr,12,0,pos->prod.kind},{drawCellStr,12,0,pos->prod.variety},{drawCellInt,12,0,&pos->invID},
								{drawCellSSP,20,0,ssp},{drawCellCSP,20,0,csp},{drawCellDouble,12,0,&pos->prod.unitPrice},
								{packDrawer,12,0,qw},{drawCellDouble,7,0,&pos->discount},{drawCellDouble,15,0,&pos->prod.amount} };
		if (pos->type == GIFT) {
			cellData[7] = (CellData){ drawCellStr,12,0,"��Ʒ" };
			if (n & 1)drawColorBar(renderer, cur, 100, 200, 100, PreOrderRectSize.y);
			else drawColorBar(renderer, cur, 100, 255, 100, PreOrderRectSize.y);
		}
		else {
			if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, PreOrderRectSize.y);
			else resetBackgroundColor(renderer);
		}
		drawListItem(renderer, cur, cellData, 9);
		cur.x++;
		n++;
	}
	while (n++ != pageSize) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(renderer, origin);
}


void drawColorBar(Renderer* renderer, Coord origin, int r, int g, int b, int length) {
	setCursorPos(renderer, origin);
	renderPrintf(renderer, CSI "48;2;%d;%d;%dm", r, g, b);
	for (int i = 0; i < length; i++) {
		renderPrintf(renderer, " ");
	}

}
void resetBackgroundColor(Renderer* renderer) {
	renderPrintf(renderer, CSI "49m");
}

void drawCellDouble(Renderer* renderer, Coord origin, CellData* cell) {
	double data = *(double*)cell->data;
	if (cell->sign)coordPrintf(renderer, origin, " %+G", data);
	else coordPrintf(renderer, origin, " %G", data);
}
void drawCellBULK(Renderer* renderer, Coord origin, CellData* cell) {
	CellData sub = *cell;
	sub.width -= 1;
	drawCellDouble(renderer, origin, &sub);
	origin.y += cell->width - 2;
	coordPrintf(renderer, origin, "��");
}
void drawCellInt(Renderer* renderer, Coord origin, CellData* cell) {
	int data = *(int*)cell->data;
	if (cell->sign) coordPrintf(renderer, origin, " %+d", data);
	else coordPrintf(renderer, origin, " %d", data);
}
void drawCellUNIT(Renderer* renderer, Coord origin, CellData* cell) {
	CellData sub = *cell;
	sub.width -= 1;
	drawCellInt(renderer, origin, &sub);
	origin.y += cell->width - 2;
	coordPrintf(renderer, origin, "��");
}
void drawCellStr(Renderer* renderer, Coord origin, CellData* cell) {
	char* data = (char*)cell->data, output[INFOMAX];
	if (!cell->sign)strMakeLen(output, data, cell->width);
	else strcpy_s(output, cell->width, data);
	coordPrintf(renderer, origin, " %s", output);
}
void drawCellTime(Renderer* renderer, Coord origin, CellData* cell) {
	time_t data = *(time_t*)cell->data;
	struct tm sti;
	localtime_s(&sti, &data);
	coordPrintf(renderer, origin, " %d/%d/%d %02d:%02d:%02d", sti.tm_year + 1900, sti.tm_mon + 1, sti.tm_mday, sti.tm_hour, sti.tm_min, sti.tm_sec);
}
void drawCellSSP(Renderer* renderer, Coord origin, CellData* cell) {
	SSP *data = (SSP*)cell->data;
	if (data) {
		CellData name = { drawCellStr,cell->width / 2+2,0,data->planName };
		drawCellStr(renderer, origin, &name);
		origin.y += cell->width / 2 + 3;
		coordPrintf(renderer, origin, "/ %d", data->SSPID);
	}
	else {
		origin.y += cell->width / 2 - 2;
		coordPrintf(renderer, origin, "�޿���");
	}
}
void drawCellCSP(Renderer* renderer, Coord origin, CellData* cell) {
	CSP* data = (CSP*)cell->data;
	if (data) {
		CellData name = { drawCellStr,cell->width / 2+2,0,data->planName };
		drawCellStr(renderer, origin, &name);
		origin.y += cell->width / 2 + 3;
		coordPrintf(renderer, origin, "/ %d", data->CSPID);
	}
	else {
		origin.y += cell->width / 2 - 2;
		coordPrintf(renderer, origin, "��Ӧ��");
	}
}
void drawListItem(Renderer* renderer, Coord origin, CellData* cells, int cellCount) {
	Coord pos = origin;
	for (int i = 0; i < cellCount; i++) {
		cells[i].drawer(renderer, pos, cells + i);
		pos.y += cells[i].width;
		if (i != cellCount - 1) drawVerticalLine(renderer, pos, 1);
		pos.y++;
	}
}


void drawInvStatsList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = listEntry(entry, Inventory, list);
	Coord cur = origin;
	InvStatsArg* arg = (InvStatsArg*)exArg;

	Record* filter = arg->filter;
	FVMO* gdata = arg->gdata;
	Finance finance;
	CellData cellName[9] = { {drawCellStr,12,0,"����"} ,{drawCellStr,12,0,"Ʒ��"},{drawCellStr,12,0,"��ƷID"},
		{drawCellStr,12,0,"������"},{drawCellStr,12,0,"������"}, {drawCellStr,12,0,"������"},{drawCellStr,12,0,"������"},
		{drawCellStr,15,0,"Ӫҵ��"},{drawCellStr,15,0,"ӯ����"} };
	drawColorBar(renderer, cur, 98, 140, 190, 123);
	drawListItem(renderer, cur, cellName, 9);
	int quantityTable[6];
	double weightTable[6];
	void* qwTable[6];
	CellDataDrawer packDrawer = NULL;
	int pack;
	cur.x++;
	int n = 0;
	listForEachEntry(Inventory, pos, &start->list, list) {
		if (n == pageSize) break;
		finance = recordStatsFinance(pos->invRecord, INV_RECORDS, filter, 0);
		if (pos->prod.pack == BULK) {
			recordStatsWeight(pos->invRecord, INV_RECORDS, filter, weightTable);
			for (int i = 1; i <= RecordTypeNum; i++) qwTable[i] = weightTable + i;
			packDrawer = drawCellBULK;
		}
		else if (pos->prod.pack == UNIT) {
			recordStatsQuantity(pos->invRecord, INV_RECORDS, filter, quantityTable);
			for (int i = 1; i <= RecordTypeNum; i++) qwTable[i] = quantityTable + i;
			packDrawer = drawCellUNIT;
		}
		CellData cellData[9] = { {drawCellStr,12,0,pos->prod.kind} ,
									{drawCellStr,12,0,pos->prod.variety},
									{drawCellInt,12,0,&pos->invID},
									{packDrawer,12,0,qwTable[SALE]},
									{packDrawer,12,0,qwTable[PURCHASE]},
									{packDrawer,12,0,qwTable[GIFT]},
									{packDrawer,12,0,qwTable[DESTROY]},
									{drawCellDouble,15,1,&finance.turnover},
									{drawCellDouble,15,1,&finance.profit} };
		if (n & 1)drawColorBar(renderer, cur, 160, 210, 118, 123);
		else drawColorBar(renderer, cur, 118, 160, 210, 123);
		drawListItem(renderer, cur, cellData, 9);
		cur.x++;
		n++;
	}

	//������ʾ�̵��ʽ�
	Coord gFinancePos = origin;
	gFinancePos.x += pageSize + 1;
	char gFTitle[50], * gFcur;
	struct tm sdate, edate;
	gFcur = gFTitle + sprintf_s(gFTitle, 10, "�̵��ʽ�");
	/*if (filter) {
		localtime_s(&sdate, &filter->time);
		localtime_s(&edate, &filter->lastTime);
		if (filter->time != TIME_NAN) gFcur += sprintf_s(gFcur, 20, "(%d/%d/%d - ", sdate.tm_year + 1900, sdate.tm_mon + 1, sdate.tm_mday);
		else gFcur += sprintf_s(gFcur, 20, "(���� - ");
		if (filter->lastTime != TIME_NAN) gFcur += sprintf_s(gFcur, 20, "%d/%d/%d)", edate.tm_year + 1900, edate.tm_mon + 1, edate.tm_mday);
		else gFcur += sprintf_s(gFcur, 20, "����)");
	}*/

	CellData financeName[5] = { {drawCellStr,23,0,"�����ʽ�"} ,{drawCellStr,23,0,"�����ʽ�"},{drawCellStr,24,0,"Ӫҵ��"},
								{drawCellStr,24,0,"�ɱ�"},{drawCellStr,24,0,"ӯ����"} };
	cur = gFinancePos;
	drawColorBar(renderer, cur, 120, 160, 140, 123);
	coordPrintf(renderer, (Coord) { cur.x, cur.y + 120 / 2 - strlen(gFTitle) / 2 }, "%s", gFTitle); cur.x++;
	drawColorBar(renderer, cur, 120, 160, 180, 123);
	drawListItem(renderer, cur, financeName, 5); cur.x++;
	CellData financeData[5] = { {drawCellDouble,23,0,&gdata->finance->startUpCapital},{drawCellDouble,23,0,&gdata->finance->balance},
								{drawCellDouble,24,0,&gdata->finance->turnover},{drawCellDouble,24,0,&gdata->finance->cost},
								{drawCellDouble,24,0,&gdata->finance->profit} };
	drawColorBar(renderer, cur, 120, 80, 140, 123);
	drawListItem(renderer, cur, financeData, 5);
}
void drawGiftList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = invEntry(entry);
	FVMO* gdata = (FVMO*)exArg;
	coordPrintf(renderer, origin, "Ʒ��\t\t����\t\t��ƷID\t������\t���Ӽ۸�");
	origin.x++;
	int n = 0;
	char kind[30], var[30];
	listForEachEntry(Inventory, pos, &start->list, list) {
		if (n == pageSize) break;

		strMakeLen(kind, pos->prod.kind, 12);
		strMakeLen(var, pos->prod.variety, 10);
		coordPrintf(renderer, origin, "%s\t%s\t%d", kind, var, pos->invID);
		if (pos->prod.pack == BULK) {
			renderPrintf(renderer, "%6.2lf��\t%8.2lf", pos->prod.weight, pos->prod.unitPrice);
		}
		else if (pos->prod.pack == UNIT) {
			renderPrintf(renderer, "%6d��\t%8.2lf", pos->prod.quantity, pos->prod.unitPrice);
		}
		origin.x++;
		n++;
	}
}

static Renderer* timeUpdateRenderer = NULL;
void clockUpdateTimeBar(void* arg) {
	if (!timeUpdateRenderer) timeUpdateRenderer = rendererCreate(500);
	FVMO* gdata = (FVMO*)arg;
	drawTimeBar(timeUpdateRenderer, gdata->timer);
	fwrite(timeUpdateRenderer->buffer, 1, timeUpdateRenderer->offset - timeUpdateRenderer->buffer, stdout);
	renderClear(timeUpdateRenderer);
}
void drawTimeBar(Renderer* renderer, FVMTimer* timer) {
	time_t ti;
	struct tm sti;
	ti = FVMTimerGetFVMTime(timer);
	localtime_s(&sti, &ti);
	renderPrintf(renderer, CSI "104;93m" CSI "s" CSI "?25l");
	renderPrintf(renderer, CSI "%d;%dH", STATUS_ORIGIN.x, STATUS_ORIGIN.y + 4);
	renderPrintf(renderer, "%02d:%02d:%02d", sti.tm_hour, sti.tm_min, sti.tm_sec);
	renderPrintf(renderer, CSI "%d;%dH", STATUS_ORIGIN.x, PanelSize.y - 10);
	renderPrintf(renderer, "%d/%d/%d", sti.tm_year + 1900, sti.tm_mon + 1, sti.tm_mday);
	renderPrintf(renderer, CSI "u" CSI "?25h" CSI "0m");
}
void drawStatusBar(Renderer* renderer, Coord origin, FVMO gdata) {
	setCursorPos(renderer, origin);
	renderPrintf(renderer, CSI "104;93m");
	for (int i = 0; i < PanelSize.y; i++) {
		renderPrintf(renderer, " ");
	}
	Coord pageStatusPos = origin;
	pageStatusPos.y += 15;
	setCursorPos(renderer, pageStatusPos);
	listForEachEntry(PageStack, pos, &gdata.pageStack->list, list) {
		renderPrintf(renderer, "/");
		renderPrintf(renderer, "%s", pos->pageName);
	}
	drawTimeBar(renderer, gdata.timer);
	renderPrintf(renderer, CSI "0m");
}

void inputStart(Renderer* renderer, Coord inputOrigin) {

	setCursorPos(renderer, inputOrigin);
	renderPrintf(renderer, ESC "(0");
	for (int i = 0; i < PanelSize.y; i++) {
		renderPrintf(renderer, "q");
	}
	Coord bottom = inputOrigin;
	bottom.x += 10;
	setCursorPos(renderer, bottom);
	for (int i = 0; i < PanelSize.y; i++) {
		renderPrintf(renderer, "q");
	}
	renderPrintf(renderer, ESC "(B");
	renderPrintf(renderer, CSI "%d;%dr", inputOrigin.x + 1, bottom.x - 1);
	renderPrintf(renderer, CSI "%d;%dH", inputOrigin.x + 1, inputOrigin.y);
	inputOrigin.x++;
	setCursorPos(renderer, inputOrigin);
}

#define READ_TOO_LONG -1
#define READ_SUCCESS 0
extern FILE* stdinLog;
int readline(char* buf, int maxCount, FILE* stream, int* pNumberRead) { //������ȡһ�д���buf�У�������maxCount���ַ����������ַ������������з��Ͷ����ַ�
	int i;
	char t;
	*pNumberRead = 0;
	for (i = 0; (t = fgetc(stream)) != '\n'; i++) {
		if (i < maxCount - 1) buf[i] = t;
	}
	if (i >= maxCount) {
		fwrite(buf, 1, maxCount - 1, stdinLog);
		fputc('\n', stdinLog); fflush(stdinLog);
		buf[maxCount - 1] = '\0';
		*pNumberRead = maxCount - 1;
		return READ_TOO_LONG;
	}
	fwrite(buf, 1, i, stdinLog);
	fputc('\n', stdinLog); fflush(stdinLog);
	buf[i] = '\0';
	*pNumberRead = i;
	return READ_SUCCESS;
}

int getSelect() {
	int num = -1;
	getUIntInput("ѡ��һ���Լ���:", &num, ALLINT, false);
	return num;
}
void drawOrdMenu(const char* title, int n, int firstNum, ...) {
	va_list args;
	va_start(args, firstNum);
	printf("%s\n", title);
	for (int i = 0; i < n; i++) {
		char* item = va_arg(args, char*);
		printf("\t%d.%s\n", firstNum + i, item);
	}
}

int getUIntInput(const char* query, int* value, IntRange range, bool strict) {
	int num = 0, readLen;
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		readline(input, INFOMAX, stdin, &readLen);
		if (strcmp(input, "quit") == 0) return INPUT_BREAK;
		if (input[0] == '\0') {
			if (!strict) return INPUT_NOCHANGE;
			else continue;
		}
		if ((sscanf_s(input, "%d", &num)) == 1) {
			if (sprintf_s(input, INFOMAX, "%d", num) != readLen) {
				printf("Ӧ������Ϸ�����\n");
				continue;
			}
			if (range.max < range.min || (num >= range.min && num <= range.max)) {
				*value = num;
				return INPUT_SUCCESS;
			}
			else {
				printf("Ӧ����[%d,%d]��Χ�ڵ�����\n", range.min, range.max);
				continue;
			}
		}
		else {
			printf("Ӧ����Ϸ�����\n");
			continue;
		}
	}
}
int getStrInput(const char* query, char* value, int maxCount, bool strict) {
	int readLen;
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		if (readline(input, maxCount, stdin, &readLen) == READ_TOO_LONG) {
			printf("�����ַ�������(��Ӧ����%d)\n", maxCount - 1);
			continue;
		}
		if (strcmp(input, "quit") == 0) return INPUT_BREAK;
		if (input[0] == '\0') {
			if (!strict) return INPUT_NOCHANGE;
			else continue;
		}
		strcpy_s(value, maxCount, input);
		return INPUT_SUCCESS;
	}
}

int getDoubleInput(const char* query, double* value, DoubleRange range, bool strict) {
	double num = 0;
	int readLen;
	bool isReal;
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		readline(input, INFOMAX, stdin, &readLen);
		if (strcmp(input, "quit") == 0) return INPUT_BREAK;
		if (input[0] == '\0') {
			if (!strict) return INPUT_NOCHANGE;
			else continue;
		}
		isReal = true;
		for (int i = 0; i < readLen; i++) {
			if (!isascii(input[i]) || (!isdigit(input[i]) && input[i] != '.')) {
				isReal = false;
				break;
			}
		}
		if (!isReal) {
			printf("Ӧ������Ϸ�ʵ��\n");
			continue;
		}
		if (sscanf_s(input, "%lf", &num) == 1) {
			if (range.max < range.min || (num >= range.min && num <= range.max)) {
				*value = num;
				return INPUT_SUCCESS;
			}
			else {
				printf("Ӧ����[%lf,%lf]��Χ�ڵ�ʵ��\n", range.min, range.max);
				continue;
			}
		}
		else {
			printf("Ӧ������Ϸ�ʵ��\n");
			continue;
		}
	}
}
int getDateTime(const char* query, time_t* value, bool strict) { //����������valueΪTIME_NAN
	struct tm ti, ti_verify;
	time_t time;
	int readLen;
	*value = TIME_NAN;
	memset(&ti, 0, sizeof(ti));
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		readline(input, INFOMAX, stdin, &readLen);
		if (strcmp(input, "quit") == 0) return INPUT_BREAK;
		if (input[0] == '\0') {
			if (!strict) return INPUT_NOCHANGE;
			else continue;
		}
		if (sscanf_s(input, "%d.%d.%d", &ti.tm_year, &ti.tm_mon, &ti.tm_mday) == 3) {
			ti.tm_year -= 1900;
			ti.tm_mon--;
			ti_verify = ti;
			//ti.tm_hour = 8;
			time = mktime(&ti);
			ti_verify.tm_wday = ti.tm_wday, ti_verify.tm_yday = ti.tm_yday;
			if (time == -1 || memcmp(&ti, &ti_verify, sizeof(ti)) != 0) {
				printf("������ȷ�����ڡ�\n");
			}
			else {
				*value = time;
				return INPUT_SUCCESS;
			}
		}
	}
}
int inputInventoryID(const Inventory* head, int* id, Inventory** pInv) {
	*pInv = NULL;
	while (1) {
		breakDeliver(getUIntInput("��������ƷID:", id, ALLINT, true));
		if (*pInv = invQueryID(head, *id)) return;
		else {
			printf("���޴���Ʒ��\n");
		}
	}
}
int inputRecordID(const Record* head, int type, int* id, Record** pRec) {
	*pRec = NULL;
	while (1) {
		breakDeliver(getUIntInput("�������¼ID:", id, ALLINT, true));
		if (*pRec = recordQueryID(head, *id, type)) return;
		else {
			printf("���޴˼�¼��\n");
		}
	}
}
int inputSSPID(const SSP* head, int* id, SSP** pSSP) {
	*pSSP = NULL;
	while (1) {
		breakDeliver(getUIntInput("�����뵥Ʒ���۷���ID:", id, ALLINT, true));
		if (*pSSP = SSPQueryID(head, *id)) return;
		else {
			printf("���޴˷�����\n");
		}
	}
}
int inputCSPID(const CSP* head, int* id, CSP** pCSP) {
	*pCSP = NULL;
	while (1) {
		breakDeliver(getUIntInput("������������۷���ID:", id, ALLINT, true));
		if (*pCSP = CSPQueryID(head, *id)) return;
		else {
			printf("���޴˷�����\n");
		}
	}
}

int inputProduct(Product* prod) {
	breakDeliver(getStrInput("��������:", prod->kind, INFOMAX, true));
	breakDeliver(getStrInput("����Ʒ��:", prod->variety, INFOMAX, true));
	breakDeliver(getDateTime("�������ʱ��(��.��.��):", &prod->expiration, true));
	drawOrdMenu("Ʒ��:", 3, 1, "��", "��", "��");
	breakDeliver(getUIntInput("ѡ��Ʒ��:", &prod->quality, (IntRange) { 1, 3 }, true));
	drawOrdMenu("��װ��ʽ:", 2, 1, "ɢװ", "��Ԫװ");
	breakDeliver(getUIntInput("ѡ���װ��ʽ:", &prod->pack, (IntRange) { 1, 2 }, true));
	if (prod->pack == BULK) {
		breakDeliver(getDoubleInput("��������:", &prod->weight, WRANGE, true));
	}
	else if (prod->pack == UNIT) {
		breakDeliver(getUIntInput("��������:", &prod->quantity, QRANGE, true));
	}
	breakDeliver(getDoubleInput("�����������:", &prod->purUPrice, UPRINCERANGE, true));
	breakDeliver(getDoubleInput("�趨���۵���:", &prod->unitPrice, UPRINCERANGE, true));
	//if (prod->pack == BULK) prod->amount = prod->unitPrice * prod->weight;  
	//else if (prod->pack == UNIT) prod->amount = prod->unitPrice * prod->quantity;
	return INPUT_SUCCESS;
}
int inputGift(Inventory** pGift, Inventory* invHead) {
	Inventory* inv = NULL, * cp = NULL;
	int invID;
	breakDeliver(inputInventoryID(invHead, &invID, &inv));
	cp = invCopyCreate(inv);
	if (inv->prod.pack == BULK) {
		breakCatch(getDoubleInput("������������:", &cp->prod.weight, WRANGE, true)) {
			free(cp);
			return INPUT_BREAK;
		}
		cp->prod.amount = cp->prod.weight;
	}
	else if (inv->prod.pack == UNIT) {
		breakCatch(getUIntInput("������������:", &cp->prod.quantity, QRANGE, true)) {
			free(cp);
			return INPUT_BREAK;
		}
		cp->prod.amount = cp->prod.quantity;
	}
	cp->prod.unitPrice = 0;
	breakCatch(getDoubleInput("���븽�ӽ��(��ѡ����Ϊ�����Ʒ):", &cp->prod.unitPrice, UPRINCERANGE, false)) {
		free(cp);
		return INPUT_BREAK;
	}
	cp->prod.amount *= cp->prod.unitPrice;
	*pGift = cp;
	return READ_SUCCESS;
}
int inputProductFilter(Product* prod) {
	memset(prod, 0, sizeof(Product));
	prod->expiration = TIME_NAN;
	breakDeliver(getStrInput("��������(Ĭ�ϲ���):", prod->kind, INFOMAX, false));
	breakDeliver(getStrInput("����Ʒ��(Ĭ�ϲ���):", prod->variety, INFOMAX, false));
	breakDeliver(getDateTime("�ڴ����ڼ�֮ǰ����(��.��.��)(Ĭ�ϲ���) : ", &prod->expiration, false));
	drawOrdMenu("Ʒ��:", 4, 0, "����", "��", "��", "��");
	breakDeliver(getUIntInput("����Ʒ��(Ĭ�ϲ���):", &prod->quality, (IntRange) { 1, 3 }, false));
	drawOrdMenu("��װ��ʽ:", 3, 0, "����", "ɢװ", "��Ԫװ");
	breakDeliver(getUIntInput("ѡ���װ��ʽ(Ĭ�ϲ���):", &prod->pack, (IntRange) { 0, 2 }, false));
	return INPUT_SUCCESS;
}

int inputRecordFilter(Record* rec) {
	memset(rec, 0, sizeof(Record));
	rec->time = rec->lastTime = TIME_NAN;
	drawOrdMenu("��¼����:", 5, 0, "����", "����", "���ۼ�¼", "���¼�¼", "�����¼");
	breakDeliver(getUIntInput("��¼����(Ĭ�ϲ���) : ", &rec->type, (IntRange) { 0, 4 }, false));
	breakDeliver(inputProductFilter(&rec->prod));
	breakDeliver(getDateTime("ɸѡ����������(��.��.��)(Ĭ�ϲ���) : ", &rec->time, false));
	breakDeliver(getDateTime("ɸѡ����������(��.��.��)(Ĭ�ϲ���) : ", &rec->lastTime, false));
	if (rec->lastTime != TIME_NAN) rec->lastTime += 3600 * 24 - 1;
	return INPUT_SUCCESS;
}



void showProductDetails(Renderer* renderer, Coord pos, Product* prod) {  //Only Show product attributes
	struct tm date;
	coordPrintf(renderer, pos, "����:%s", prod->kind, prod->variety); pos.x++;
	coordPrintf(renderer, pos, "Ʒ��:%s", prod->variety); pos.x++;
	coordPrintf(renderer, pos, "Ʒ��:%s", quanlityText[prod->quality]); pos.x++;
	localtime_s(&date, &prod->expiration);
	coordPrintf(renderer, pos, "��������:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday); pos.x++;
	if (prod->pack == BULK) {
		coordPrintf(renderer, pos, "��װ��ʽ:ɢװ");
	}
	else if (prod->pack == UNIT) {
		coordPrintf(renderer, pos, "��װ��ʽ:��Ԫװ");
	}pos.x++;
}

void showInvDetails(Renderer* renderer, Coord pos, Inventory* inv) {
	coordPrintf(renderer, pos, "��ƷID:%d\n", inv->invID); pos.x++;
	showProductDetails(renderer, pos, &inv->prod); pos.x += 5;
	if (inv->prod.pack == BULK) {
		coordPrintf(renderer, pos, "�������:%.2lf\n", inv->prod.weight);
	}
	else if (inv->prod.pack == UNIT) {
		coordPrintf(renderer, pos, "�������:%d\n", inv->prod.quantity);
	}
	pos.x++;
	coordPrintf(renderer, pos, "���۵���:%.2lf\n", inv->prod.unitPrice);
}
static const Coord SSPDetailsRectSize = { 18,53 };
void showSSPDetails(Renderer* renderer, Coord pos, SSP* ssp) {
	drawRectBorder(renderer, pos, SSPDetailsRectSize);
	struct tm date;
	pos.x++;
	pos.y++;
	coordPrintf(renderer, pos, "%s", ssp->planName); pos.x++;
	if (ssp->invID) {
		coordPrintf(renderer, pos, "����:Ӧ����ָ��ID��Ʒ"); pos.x++;
		coordPrintf(renderer, pos, "Ӧ����ƷID:%d", ssp->invID); pos.x++;
		if (ssp->filter.pack == BULK) {
			coordPrintf(renderer, pos, "��͹�������:");
			if (ssp->filter.weight) {
				renderPrintf(renderer, "%.2lf", ssp->filter.weight);
			}
			else {
				renderPrintf(renderer, "����");
			}
		}
		else if (ssp->filter.pack == UNIT) {
			coordPrintf(renderer, pos, "��͹�������:");
			if (ssp->filter.quantity) {
				renderPrintf(renderer, "%d", ssp->filter.quantity);
			}
			else {
				renderPrintf(renderer, "����");
			}
		}
		pos.x++;
	}
	else {
		coordPrintf(renderer, pos, "����:����Ӧ����������������Ʒ"); pos.x++;
		if (ssp->filter.kind[0]) coordPrintf(renderer, pos, "����:%s", ssp->filter.kind), pos.x++;
		else coordPrintf(renderer, pos, "Ʒ��:����"), pos.x++;
		if (ssp->filter.variety[0]) coordPrintf(renderer, pos, "Ʒ��:%s", ssp->filter.variety), pos.x++;
		else coordPrintf(renderer, pos, "Ʒ��:����"), pos.x++;
		if (ssp->filter.expiration != TIME_NAN) {
			localtime_s(&date, &ssp->filter.expiration);
			coordPrintf(renderer, pos, "�ڴ����ڼ�ǰ����:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday), pos.x++;
		}
		else coordPrintf(renderer, pos, "�ڴ����ڼ�ǰ����:����"), pos.x++;
		if (ssp->filter.quality) coordPrintf(renderer, pos, "Ʒ��:%s", quanlityText[ssp->filter.quality]), pos.x++;
		else coordPrintf(renderer, pos, "Ʒ��:����"), pos.x++;
		if (ssp->filter.pack) coordPrintf(renderer, pos, "��װ��ʽ:%s", packText[ssp->filter.pack]), pos.x++;
		else coordPrintf(renderer, pos, "��װ��ʽ:����"), pos.x++;
		if (ssp->filter.unitPrice)coordPrintf(renderer, pos, "��͵���:%.2lf", ssp->filter.unitPrice), pos.x++;
		else coordPrintf(renderer, pos, "��͵���:����"), pos.x++;
		if (ssp->filter.weight) coordPrintf(renderer, pos, "��͹�������:%.2lf", ssp->filter.weight), pos.x++;
		else coordPrintf(renderer, pos, "��͹�������:����"), pos.x++;
		if (ssp->filter.quantity) coordPrintf(renderer, pos, "��͹�������:%d", ssp->filter.quantity), pos.x++;
		else coordPrintf(renderer, pos, "��͹�������:����"), pos.x++;
	}
	if (ssp->filter.amount)coordPrintf(renderer, pos, "����ܶ�:%.2lf", ssp->filter.amount), pos.x++;
	else coordPrintf(renderer, pos, "����ܶ�:����"), pos.x++;
	if (ssp->reqDateStart != TIME_NAN) {
		localtime_s(&date, &ssp->reqDateStart);
		coordPrintf(renderer, pos, "��ʼ����:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
		pos.x++;
	}
	else coordPrintf(renderer, pos, "��ʼ����:����"), pos.x++;
	if (ssp->reqDateEnd != TIME_NAN) {
		localtime_s(&date, &ssp->reqDateEnd);
		coordPrintf(renderer, pos, "��ֹ����:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
		pos.x++;
	}
	else coordPrintf(renderer, pos, "��ֹ����:����"), pos.x++;

	coordPrintf(renderer, pos, "�ۿ���:%.2lf", ssp->discount), pos.x++;
	coordPrintf(renderer, pos, "����˵��:%s", ssp->addInfo), pos.x++;
}
static const Coord CSPDetailsRectSize = { 18,38 };
void showCSPDetails(Renderer* renderer, Coord pos, CSP* csp) {
	drawRectBorder(renderer, pos, CSPDetailsRectSize);
	struct tm date;
	pos.x++;
	pos.y++;
	if (csp->reqDateStart != TIME_NAN) {
		localtime_s(&date, &csp->reqDateStart);
		coordPrintf(renderer, pos, "��ʼ����:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
		pos.x++;
	}
	else coordPrintf(renderer, pos, "��ʼ����:����"), pos.x++;
	if (csp->reqDateEnd != TIME_NAN) {
		localtime_s(&date, &csp->reqDateEnd);
		coordPrintf(renderer, pos, "��ֹ����:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
		pos.x++;
	}
	else coordPrintf(renderer, pos, "��ֹ����:����"), pos.x++;

	coordPrintf(renderer, pos, "�ۿ���:%.2lf", csp->discount), pos.x++;
	coordPrintf(renderer, pos, "���뵥Ʒ���۷�������Ӧ��:");
	if (csp->overlaySingleSP) renderPrintf(renderer, "��");
	else renderPrintf(renderer, "��");
	pos.x++;
	coordPrintf(renderer, pos, "����˵��:%s", csp->addInfo), pos.x++;
}

char typeQuantityText[5][20] = { "","�ɹ���","������","�仯��","������" };
char typeAmountText[5][20] = { "","�ɹ��ܶ�","�����ܶ�","�ܼ۱仯","��Ʒ�����շ�" };
char typeUpriceText[5][20] = { "","�ɹ�����","���۵���","�ۼ۱仯","��Ʒ���ⵥ��" };
void showRecordDetails(Renderer* renderer, Coord pos, Record* rec) {
	struct tm time;
	localtime_s(&time, &rec->time);
	coordPrintf(renderer, pos, "��¼ID:%d", rec->recID); pos.x++;
	coordPrintf(renderer, pos, "��ƷID:%d", rec->invID); pos.x++;
	coordPrintf(renderer, pos, "����:%s", recordType[rec->type]); pos.x++;
	coordPrintf(renderer, pos, "��¼ʱ��:%d/%d/%d %02d:%02d:%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_hour); pos.x++;
	showProductDetails(renderer, pos, &rec->prod);
	pos.x += 5;
	if (rec->prod.pack == BULK) {
		coordPrintf(renderer, pos, "%s: %.2lf��\n", typeQuantityText[rec->type], rec->prod.weight); pos.x++;
	}
	else if (rec->prod.pack == UNIT) {
		coordPrintf(renderer, pos, "%s: %d��\n", typeQuantityText[rec->type], rec->prod.quantity); pos.x++;
	}
	coordPrintf(renderer, pos, "%s��%.2lf\n", typeUpriceText[rec->type], rec->prod.purUPrice); pos.x++;
	coordPrintf(renderer, pos, "%s: %.2lf\n", typeAmountText[rec->type], rec->prod.amount); pos.x++;
	coordPrintf(renderer, pos, "������Ϣ:%s\n", rec->addInfo);
}
