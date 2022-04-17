#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>
#include"UI.h"
#include"renderer.h"

#define ESC "\033"
#define CSI "\033["

static char recordType[5][20] = { "","进货记录","销售记录","更新记录","赠送记录" };
int typeDirect[5] = { 0,1,-1,1,-1 };

char quanlityText[5][20] = { "","优","良","差" };
char packText[3][20] = { "","散装","单元装" };



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
	if (len <= 0) return;
	int slen = strlen(src);
	if (slen + 1 <= len) {
		strcpy_s(dest, slen + 1, src);
	}
	else {
		if (len <= 4) {
			for (int i = 0; i < len - 1; i++) {
				dest[i] = '.';
			}
			dest[len - 1] = '\0';
		}
		else {
			dest[len - 1] = '\0';
			for (int i = 1; i <= 3; i++) {
				dest[len - 1 - i] = '.';
			}
			for (int i = 0; i < len - 4; i++) {
				if (isascii(src[i])) {
					dest[i] = src[i];
				}
				else {
					dest[i] = src[i];
					i++;
					dest[i] = src[i];
				}
			}
		}
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
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y - 7 }, "%d/%d页", ((pageStartNum - 1) / pageSize) + 1, ((start->list.root->size - 1) / pageSize) + 1);
	origin.x += 3;
	origin.y++;
}
void drawInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = listEntry(entry, Inventory, list);
	Coord cur = origin;
	CellData cellName[5] = { {drawCellStr,14,0,"种类"} ,{drawCellStr,14,0,"品种"},{drawCellStr,14,0,"销售单价"},
		{drawCellStr,14,0,"ID"},{drawCellStr,14,0,"数量"} };
	drawColorBar(renderer, cur, 238, 232, 213, invListRectSize.y);
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
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, invListRectSize.y);
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


	CellData cellName[8] = { {drawCellStr,12,0,"类型"} ,{drawCellStr,12,0,"记录ID"},{drawCellStr,12,0,"商品ID"},{drawCellStr,12,0,"种类"},
	{drawCellStr,12,0,"品种"},{drawCellStr,15,0,"商品进出"}, {drawCellStr,15,0,"资金进出"},{drawCellStr,22,0,"时间"} };
	drawColorBar(renderer, cur, 238, 232, 213, RecordRectSize.y);
	drawListItem(renderer, cur, cellName, 8);
	cur.x++;
	recordForEachStart(pos, start, type) {
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
	}recordForEachEnd(pos, start, type)
}


void drawInvCheckList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Record* start = recordEntry(entry,timeList);
	Coord cur = origin;
	int n = 0;
	void* qw = NULL;
	CellDataDrawer packDrawer = NULL;

	CellData cellName[6] = { {drawCellStr,12,0,"种类"} ,{drawCellStr,12,0,"品种"},{drawCellStr,10,0,"商品ID"},{drawCellStr,17,0,"出错位置(记录ID)"},
							  {drawCellStr,65,0,"错误信息"},{drawCellStr,12,0,"商品库存量"} };
	drawColorBar(renderer, cur, 238, 232, 213, InvCheckRectSize.y);
	drawListItem(renderer, cur, cellName, 6);
	cur.x++;
	listForEachEntry(Record, pos, &start->timeList, timeList) {
		if (n == pageSize) break;
		if (pos->prod.pack == BULK) {
			qw = &pos->prod.weight;
			packDrawer = drawCellBULK;
		}
		else if (pos->prod.pack == UNIT) {
			qw = &pos->prod.quantity;
			packDrawer = drawCellUNIT;
		}
		CellData cellData[6] = { {drawCellStr,12,0,pos->prod.kind},{drawCellStr,12,0,pos->prod.variety},{drawCellInt,10,0,&pos->invID},
								  (pos->recID!=-1)?(CellData){drawCellInt,17,0,&pos->recID}:(CellData){drawCellStr,17,0," -"},
									{drawCellStr,65,0,pos->addInfo},{packDrawer,12,0,qw} };


		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, InvCheckRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 6);
		cur.x++;
		n++;
	}
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
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y - 7 }, "%d/%d页", ((*pageStartNum - 1) / pageSize) + 1, ((start->root->size - 1) / pageSize) + 1);
	origin.x += 3;
	origin.y++;
	drawer(renderer, origin, start, pageSize, exArg);
}
void drawSSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	SSP* start = listEntry(entry, SSP, list);
	Coord cur = origin;

	CellData cellName[2] = { {drawCellStr,12,0,"方案名"} ,{drawCellStr,12,0,"方案ID"} };
	drawColorBar(renderer, cur, 238, 232, 213, SSPRectSize.y);
	drawListItem(renderer, cur, cellName, 2);
	cur.x++;
	int n = 0;

	listForEachEntry(SSP, pos, &(start->list), list) {
		if (n == pageSize) break;
		CellData cellData[2] = { {drawCellStr,12,0,pos->planName} ,
									{drawCellInt,12,0,&pos->SSPID} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, SSPRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 2);
		cur.x++;
		n++;
	}

}
void drawCSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	CSP* start = listEntry(entry, CSP, list);
	Coord cur = origin;

	CellData cellName[2] = { {drawCellStr,12,0,"方案名"} ,{drawCellStr,12,0,"方案ID"} };
	drawColorBar(renderer, cur, 238, 232, 213, CSPRectSize.y);
	drawListItem(renderer, cur, cellName, 2);
	cur.x++;
	int n = 0;

	listForEachEntry(CSP, pos, &(start->list), list) {
		if (n == pageSize) break;
		CellData cellData[2] = { {drawCellStr,12,0,pos->planName} ,
									{drawCellInt,12,0,&pos->CSPID} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, CSPRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 2);
		cur.x++;
		n++;
	}
}
void drawComInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = invEntry(entry);
	FVMO* gdata = (FVMO*)exArg;

	Coord cur = origin;
	CellData cellName[3] = { {drawCellStr,12,0,"品种"},{drawCellStr,12,0,"种类"},{drawCellStr,12,0,"商品ID"} };
	drawColorBar(renderer, cur, 238, 232, 213, CSPComInvRectSize.y);
	drawListItem(renderer, cur, cellName, 3);
	//coordPrintf(renderer, origin, "品种\t\t种类\t\t商品ID");
	cur.x++;
	int n = 0;
	char kind[30], var[30];
	listForEachEntry(Inventory, pos, &start->list, list) {

		if (n == pageSize) break;
		/*
		strMakeLen(kind, pos->prod.kind, 12);
		strMakeLen(var, pos->prod.variety, 10);
		coordPrintf(renderer, origin, "%s\t%s\t%d", kind, var, pos->invID);
		origin.x++;
		n++;
	*/
		CellData cellData[3] = { {drawCellStr,12,0,pos->prod.kind },
								  {drawCellStr,12,0,pos->prod.variety},
								  {drawCellInt,12,0,&pos->invID} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, CSPComInvRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 3);
		cur.x++;
		n++;
	}

	while (n++ != pageSize) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(renderer, origin);

	//L开始


}
void drawPreOrderList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Record* start = listEntry(entry, Record, timeList);
	FVMO* gdata = (FVMO*)exArg;
	Coord cur = origin;
	SSP* ssp = NULL;
	CSP* csp = NULL;
	void* qw = NULL;
	double discountAmount;
	CellDataDrawer packDrawer = NULL;
	CellData cellName[9] = { {drawCellStr,12,0,"种类"},{drawCellStr,12,0,"品种"},{drawCellStr,10,0,"商品ID"},
								{drawCellStr,20,0,"单品销售方案/ID"},{drawCellStr,20,0,"组合销售方案/ID"},{drawCellStr,12,0,"单价"},
								{drawCellStr,10,0,"销售量"},{drawCellStr,7,1,"折扣率"},{drawCellStr,12,0,"金额"} };
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
		discountAmount = pos->prod.amount * pos->discount;
		CellData cellData[9] = { {drawCellStr,12,0,pos->prod.kind},{drawCellStr,12,0,pos->prod.variety},{drawCellInt,10,0,&pos->invID},
								{drawCellSSP,20,0,ssp},{drawCellCSP,20,0,csp},{drawCellDouble,12,0,&pos->prod.unitPrice},
								{packDrawer,10,0,qw},{drawCellDouble,7,0,&pos->discount},{drawCellDouble,12,0,&discountAmount} };
		if (pos->type == GIFT) {
			cellData[7] = (CellData){ drawCellStr,7,0,"赠品" };
			if (n & 1)drawColorBar(renderer, cur, 130, 200, 150, PreOrderRectSize.y);
			else drawColorBar(renderer, cur, 160, 220, 170, PreOrderRectSize.y);
		}
		else {
			if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, PreOrderRectSize.y);
			else resetBackgroundColor(renderer);
		}
		drawListItem(renderer, cur, cellData, 9);
		cur.x++;
		n++;
	}
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
	coordPrintf(renderer, origin, "斤");
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
	coordPrintf(renderer, origin, "个");
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
void drawCellDate(Renderer* renderer, Coord origin, CellData* cell) {
	time_t data = *(time_t*)cell->data;
	struct tm sti;
	localtime_s(&sti, &data);
	coordPrintf(renderer, origin, " %d/%d/%d", sti.tm_year + 1900, sti.tm_mon + 1, sti.tm_mday);
}
void drawCellSSP(Renderer* renderer, Coord origin, CellData* cell) {
	SSP* data = (SSP*)cell->data;
	if (data) {
		CellData name = { drawCellStr,cell->width / 2 + 2,0,data->planName };
		drawCellStr(renderer, origin, &name);
		origin.y += cell->width / 2 + 3;
		coordPrintf(renderer, origin, "/ %d", data->SSPID);
	}
	else {
		origin.y += cell->width / 2 - 2;
		coordPrintf(renderer, origin, "无可用");
	}
}
void drawCellCSP(Renderer* renderer, Coord origin, CellData* cell) {
	CSP* data = (CSP*)cell->data;
	if (data) {
		CellData name = { drawCellStr,cell->width / 2 + 2,0,data->planName };
		drawCellStr(renderer, origin, &name);
		origin.y += cell->width / 2 + 3;
		coordPrintf(renderer, origin, "/ %d", data->CSPID);
	}
	else {
		origin.y += cell->width / 2 - 2;
		coordPrintf(renderer, origin, "无应用");
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
	CellData cellName[9] = { {drawCellStr,12,0,"种类"} ,{drawCellStr,12,0,"品种"},{drawCellStr,12,0,"商品ID"},
		{drawCellStr,12,0,"销售量"},{drawCellStr,12,0,"进货量"}, {drawCellStr,12,0,"赠送量"},{drawCellStr,12,0,"销毁量"},
		{drawCellStr,15,0,"营业额"},{drawCellStr,15,0,"盈利额"} };
	drawColorBar(renderer, cur, 98, 140, 190, statsRectSize.y);
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
		if (n & 1)drawColorBar(renderer, cur, 160, 210, 118, statsRectSize.y);
		else drawColorBar(renderer, cur, 118, 160, 210, statsRectSize.y);
		drawListItem(renderer, cur, cellData, 9);
		cur.x++;
		n++;
	}

	//以下显示商店资金
	Coord gFinancePos = origin;
	gFinancePos.x += pageSize + 1;
	char gFTitle[50], * gFcur;
	struct tm sdate, edate;
	gFcur = gFTitle + sprintf_s(gFTitle, 10, "商店资金");
	/*if (filter) {
		localtime_s(&sdate, &filter->time);
		localtime_s(&edate, &filter->lastTime);
		if (filter->time != TIME_NAN) gFcur += sprintf_s(gFcur, 20, "(%d/%d/%d - ", sdate.tm_year + 1900, sdate.tm_mon + 1, sdate.tm_mday);
		else gFcur += sprintf_s(gFcur, 20, "(不限 - ");
		if (filter->lastTime != TIME_NAN) gFcur += sprintf_s(gFcur, 20, "%d/%d/%d)", edate.tm_year + 1900, edate.tm_mon + 1, edate.tm_mday);
		else gFcur += sprintf_s(gFcur, 20, "不限)");
	}*/

	CellData financeName[5] = { {drawCellStr,23,0,"启动资金"} ,{drawCellStr,23,0,"现有资金"},{drawCellStr,24,0,"营业额"},
								{drawCellStr,24,0,"成本"},{drawCellStr,24,0,"盈利额"} };
	cur = gFinancePos;
	drawColorBar(renderer, cur, 120, 160, 140, statsRectSize.y);
	coordPrintf(renderer, (Coord) { cur.x, cur.y + 120 / 2 - strlen(gFTitle) / 2 }, "%s", gFTitle); cur.x++;
	drawColorBar(renderer, cur, 120, 160, 180, statsRectSize.y);
	drawListItem(renderer, cur, financeName, 5); cur.x++;
	CellData financeData[5] = { {drawCellDouble,23,0,&gdata->finance->startUpCapital},{drawCellDouble,23,0,&gdata->finance->balance},
								{drawCellDouble,24,0,&gdata->finance->turnover},{drawCellDouble,24,0,&gdata->finance->cost},
								{drawCellDouble,24,0,&gdata->finance->profit} };
	drawColorBar(renderer, cur, 120, 80, 140, statsRectSize.y);
	drawListItem(renderer, cur, financeData, 5);
}
void drawGiftList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = invEntry(entry);
	//coordPrintf(renderer, origin, "品种\t\t种类\t\t商品ID\t赠送量\t附加价格");


	Coord cur = origin;
	CellData cellName[5] = { {drawCellStr,14,0,"品种"},{drawCellStr,14,0,"种类"},{drawCellStr,10,0,"商品ID"},
		{drawCellStr,10,0,"赠送量"},{drawCellStr,10,1,"附加价格"} };
	drawColorBar(renderer, cur, 238, 232, 213, GiftRectSize.y);
	drawListItem(renderer, cur, cellName, 5);
	cur.x++;
	int n = 0;
	void* qw = NULL;
	CellDataDrawer packDrawer = NULL;
	listForEachEntry(Inventory, pos, &start->list, list) {
		if (n == pageSize) break;
		if (pos->prod.pack == BULK) {
			qw = &pos->prod.weight;
			packDrawer = drawCellBULK;
		}
		else if (pos->prod.pack == UNIT) {
			qw = &pos->prod.quantity;
			packDrawer = drawCellUNIT;
		}

		/*
		if strMakeLen(kind, pos->prod.kind, 12);
		strMakeLen(var, pos->prod.variety, 10);
		coordPrintf(renderer, origin, "%s\t%s\t%d", kind, var, pos->invID);
		if (pos->prod.pack == BULK) {
			renderPrintf(renderer, "%6.2lf斤\t%8.2lf", pos->prod.weight, pos->prod.unitPrice);
		}
		else if (pos->prod.pack == UNIT) {
			renderPrintf(renderer, "%6d个\t%8.2lf", pos->prod.quantity, pos->prod.unitPrice);
		}
		origin.x++;
		n++;*/

		CellData cellData[5] = { {drawCellStr,14,0,pos->prod.kind},
								{drawCellStr,14,0,pos->prod.variety},
								{drawCellInt,10,0,&pos->invID},
								{packDrawer,10,0,qw},
								{drawCellDouble,10,0,&pos->prod.unitPrice} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, GiftRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 5);
		cur.x++;
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
int readline(char* buf, int maxCount, FILE* stream, int* pNumberRead) { //从流读取一行存在buf中，最大读入maxCount个字符（包括空字符），丢弃换行符和多余字符
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
	getUIntInput("选择一项以继续:", &num, ALLINT, false);
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
				printf("应仅输入合法整数\n");
				continue;
			}
			if (range.max < range.min || (num >= range.min && num <= range.max)) {
				*value = num;
				return INPUT_SUCCESS;
			}
			else {
				printf("应输入[%d,%d]范围内的整数\n", range.min, range.max);
				continue;
			}
		}
		else {
			printf("应输入合法整数\n");
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
			printf("输入字符串过长(不应超过%d)\n", maxCount - 1);
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
			printf("应仅输入合法实数\n");
			continue;
		}
		if (sscanf_s(input, "%lf", &num) == 1) {
			if (range.max < range.min || (num >= range.min && num <= range.max)) {
				*value = num;
				return INPUT_SUCCESS;
			}
			else {
				printf("应输入[%lf,%lf]范围内的实数\n", range.min, range.max);
				continue;
			}
		}
		else {
			printf("应仅输入合法实数\n");
			continue;
		}
	}
}
int getDateTime(const char* query, time_t* value, bool strict) { //无输入则置value为TIME_NAN
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
				printf("输入正确的日期。\n");
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
		breakDeliver(getUIntInput("请输入商品ID:", id, ALLINT, true));
		if (*pInv = invQueryID(head, *id)) return;
		else {
			printf("查无此商品。\n");
		}
	}
}
int inputRecordID(const Record* head, int type, int* id, Record** pRec) {
	*pRec = NULL;
	while (1) {
		breakDeliver(getUIntInput("请输入记录ID:", id, ALLINT, true));
		if (*pRec = recordQueryID(head, *id, type)) return;
		else {
			printf("查无此记录。\n");
		}
	}
}
int inputSSPID(const SSP* head, int* id, SSP** pSSP) {
	*pSSP = NULL;
	while (1) {
		breakDeliver(getUIntInput("请输入单品销售方案ID:", id, ALLINT, true));
		if (*pSSP = SSPQueryID(head, *id)) return;
		else {
			printf("查无此方案。\n");
		}
	}
}
int inputCSPID(const CSP* head, int* id, CSP** pCSP) {
	*pCSP = NULL;
	while (1) {
		breakDeliver(getUIntInput("请输入组合销售方案ID:", id, ALLINT, true));
		if (*pCSP = CSPQueryID(head, *id)) return;
		else {
			printf("查无此方案。\n");
		}
	}
}

int inputProduct(Product* prod) {
	breakDeliver(getStrInput("输入种类:", prod->kind, INFOMAX, true));
	breakDeliver(getStrInput("输入品种:", prod->variety, INFOMAX, true));
	breakDeliver(getDateTime("输入过期时间(年.月.日):", &prod->expiration, true));
	drawOrdMenu("品质:", 3, 1, "优", "良", "差");
	breakDeliver(getUIntInput("选择品质:", &prod->quality, (IntRange) { 1, 3 }, true));
	drawOrdMenu("包装方式:", 2, 1, "散装", "单元装");
	breakDeliver(getUIntInput("选择包装方式:", &prod->pack, (IntRange) { 1, 2 }, true));
	if (prod->pack == BULK) {
		breakDeliver(getDoubleInput("输入重量:", &prod->weight, WRANGE, true));
	}
	else if (prod->pack == UNIT) {
		breakDeliver(getUIntInput("输入数量:", &prod->quantity, QRANGE, true));
	}
	breakDeliver(getDoubleInput("输入进货单价:", &prod->purUPrice, UPRINCERANGE, true));
	breakDeliver(getDoubleInput("设定销售单价:", &prod->unitPrice, UPRINCERANGE, true));
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
		breakCatch(getDoubleInput("输入赠送重量:", &cp->prod.weight, WRANGE, true)) {
			free(cp);
			return INPUT_BREAK;
		}
		cp->prod.amount = cp->prod.weight;
	}
	else if (inv->prod.pack == UNIT) {
		breakCatch(getUIntInput("输入赠送数量:", &cp->prod.quantity, QRANGE, true)) {
			free(cp);
			return INPUT_BREAK;
		}
		cp->prod.amount = cp->prod.quantity;
	}
	cp->prod.unitPrice = 0;
	breakCatch(getDoubleInput("输入附加金额(可选，作为余额赠品):", &cp->prod.unitPrice, UPRINCERANGE, false)) {
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
	breakDeliver(getStrInput("输入种类(默认不限):", prod->kind, INFOMAX, false));
	breakDeliver(getStrInput("输入品种(默认不限):", prod->variety, INFOMAX, false));
	breakDeliver(getDateTime("在此日期及之前过期(年.月.日)(默认不限) : ", &prod->expiration, false));
	drawOrdMenu("品质:", 4, 0, "不限", "优", "良", "差");
	breakDeliver(getUIntInput("输入品质(默认不限):", &prod->quality, (IntRange) { 1, 3 }, false));
	drawOrdMenu("包装方式:", 3, 0, "不限", "散装", "单元装");
	breakDeliver(getUIntInput("选择包装方式(默认不限):", &prod->pack, (IntRange) { 0, 2 }, false));
	return INPUT_SUCCESS;
}

int inputRecordFilter(Record* rec) {
	memset(rec, 0, sizeof(Record));
	rec->time = rec->lastTime = TIME_NAN;
	drawOrdMenu("记录类型:", 5, 0, "不限", "进货", "销售记录", "更新记录", "赠予记录");
	breakDeliver(getUIntInput("记录类型(默认不限) : ", &rec->type, (IntRange) { 0, 4 }, false));
	breakDeliver(inputProductFilter(&rec->prod));
	breakDeliver(getDateTime("筛选的最早日期(年.月.日)(默认不限) : ", &rec->time, false));
	breakDeliver(getDateTime("筛选的最晚日期(年.月.日)(默认不限) : ", &rec->lastTime, false));
	if (rec->lastTime != TIME_NAN) rec->lastTime += 3600 * 24 - 1;
	return INPUT_SUCCESS;
}



void showProductDetails(Renderer* renderer, Coord pos, Product* prod,int lineNum) {  //Only Show product attributes
	struct tm date;
	Coord cur = pos;
	int valWidth = ProdDetailsRectSize.y - 13;
	CellData cellName[6][2] = { {{drawCellStr,12,0,"种类"},{drawCellStr,valWidth,0,&prod->kind}},
								{{drawCellStr,12,0,"品种"},{drawCellStr,valWidth,1,&prod->variety}},
								{{drawCellStr,12,0,"品质"},{drawCellStr,valWidth,0,&quanlityText[prod->quality]}},
								{{drawCellStr,12,0,"过期日期"},{drawCellDate,valWidth,1,&prod->expiration}},
								{{drawCellStr,12,0,"包装方式"},{drawCellStr,valWidth,0,"散装"}},
								{{drawCellStr,12,0,"包装方式"},{drawCellStr,valWidth,0,"单元装"}}
	};
	for (int i = 0; i < 4; i++)
	{
		if (lineNum%2==0) drawColorBar(renderer, cur, 238, 232, 213, ProdDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i], 2);
		cur.x++;
		lineNum++;
	}
	if (prod->pack == BULK) {
		if (lineNum % 2==0) drawColorBar(renderer, cur, 238, 232, 213, ProdDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[4], 2);
	}
	else if (prod->pack == UNIT) {
		if (lineNum % 2==0) drawColorBar(renderer, cur, 238, 232, 213, ProdDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[5], 2);
	}cur.x++;
}

void showInvDetails(Renderer* renderer, Coord pos, Inventory* inv) {
	int curLineNum = 3;
	Coord cur = pos;
	int valWidth = InvDetailsRectSize.y - 13;
	CellData cellName[4][2] = { {{drawCellStr,12,0,"商品ID"},{drawCellInt,valWidth,0,&inv->invID}},
								{{drawCellStr,12,0,"库存重量"},{drawCellBULK,12,0,&inv->prod.weight}},
								{{drawCellStr,12,0,"库存数量"},{drawCellUNIT,12,0,&inv->prod.quantity}},
								{{drawCellStr,12,0,"销售单价"},{drawCellDouble,valWidth,0,&inv->prod.unitPrice}}
	};
	drawColorBar(renderer, cur, 238, 232, 213, InvDetailsRectSize.y);
	drawListItem(renderer, cur, cellName[0], 2);
	cur.x++;
	showProductDetails(renderer, cur, &inv->prod,1); cur.x += ProdDetailsRectSize.x;
	if (inv->prod.pack == BULK) {
		drawColorBar(renderer, cur, 238, 232, 213, InvDetailsRectSize.y);
		drawListItem(renderer, cur, cellName[1], 2);
	}
	else if (inv->prod.pack == UNIT) {
		drawColorBar(renderer, cur, 238, 232, 213, InvDetailsRectSize.y);
		drawListItem(renderer, cur, cellName[2], 2);
	}
	cur.x++;
	resetBackgroundColor(renderer);
	drawListItem(renderer, cur, cellName[3], 2);
}
static const Coord SSPDetailsRectSize = { 18,53 };
void showSSPDetails(Renderer* renderer, Coord pos, SSP* ssp) {
	drawRectBorder(renderer, pos, SSPDetailsRectSize);
	struct tm date;
	pos.x++;
	pos.y++;
	coordPrintf(renderer, pos, "%s", ssp->planName); pos.x++;
	if (ssp->invID) {
		coordPrintf(renderer, pos, "类型:应用于指定ID商品"); pos.x++;
		coordPrintf(renderer, pos, "应用商品ID:%d", ssp->invID); pos.x++;
		if (ssp->filter.pack == BULK) {
			coordPrintf(renderer, pos, "最低购买重量:");
			if (ssp->filter.weight) {
				renderPrintf(renderer, "%.2lf", ssp->filter.weight);
			}
			else {
				renderPrintf(renderer, "不限");
			}
		}
		else if (ssp->filter.pack == UNIT) {
			coordPrintf(renderer, pos, "最低购买数量:");
			if (ssp->filter.quantity) {
				renderPrintf(renderer, "%d", ssp->filter.quantity);
			}
			else {
				renderPrintf(renderer, "不限");
			}
		}
		pos.x++;
	}
	else {
		coordPrintf(renderer, pos, "类型:批量应用于满足条件的商品"); pos.x++;
		if (ssp->filter.kind[0]) coordPrintf(renderer, pos, "种类:%s", ssp->filter.kind), pos.x++;
		else coordPrintf(renderer, pos, "品种:不限"), pos.x++;
		if (ssp->filter.variety[0]) coordPrintf(renderer, pos, "品种:%s", ssp->filter.variety), pos.x++;
		else coordPrintf(renderer, pos, "品种:不限"), pos.x++;
		if (ssp->filter.expiration != TIME_NAN) {
			localtime_s(&date, &ssp->filter.expiration);
			coordPrintf(renderer, pos, "在此日期及前过期:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday), pos.x++;
		}
		else coordPrintf(renderer, pos, "在此日期及前过期:不限"), pos.x++;
		if (ssp->filter.quality) coordPrintf(renderer, pos, "品质:%s", quanlityText[ssp->filter.quality]), pos.x++;
		else coordPrintf(renderer, pos, "品质:不限"), pos.x++;
		if (ssp->filter.pack) coordPrintf(renderer, pos, "包装方式:%s", packText[ssp->filter.pack]), pos.x++;
		else coordPrintf(renderer, pos, "包装方式:不限"), pos.x++;
		if (ssp->filter.unitPrice)coordPrintf(renderer, pos, "最低单价:%.2lf", ssp->filter.unitPrice), pos.x++;
		else coordPrintf(renderer, pos, "最低单价:不限"), pos.x++;
		if (ssp->filter.weight) coordPrintf(renderer, pos, "最低购买重量:%.2lf", ssp->filter.weight), pos.x++;
		else coordPrintf(renderer, pos, "最低购买重量:不限"), pos.x++;
		if (ssp->filter.quantity) coordPrintf(renderer, pos, "最低购买数量:%d", ssp->filter.quantity), pos.x++;
		else coordPrintf(renderer, pos, "最低购买数量:不限"), pos.x++;
	}
	if (ssp->filter.amount)coordPrintf(renderer, pos, "最低总额:%.2lf", ssp->filter.amount), pos.x++;
	else coordPrintf(renderer, pos, "最低总额:不限"), pos.x++;
	if (ssp->reqDateStart != TIME_NAN) {
		localtime_s(&date, &ssp->reqDateStart);
		coordPrintf(renderer, pos, "开始日期:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
		pos.x++;
	}
	else coordPrintf(renderer, pos, "开始日期:不限"), pos.x++;
	if (ssp->reqDateEnd != TIME_NAN) {
		localtime_s(&date, &ssp->reqDateEnd);
		coordPrintf(renderer, pos, "截止日期:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
		pos.x++;
	}
	else coordPrintf(renderer, pos, "截止日期:不限"), pos.x++;

	coordPrintf(renderer, pos, "折扣率:%.2lf", ssp->discount), pos.x++;
	coordPrintf(renderer, pos, "附加说明:%s", ssp->addInfo), pos.x++;
}
static const Coord CSPDetailsRectSize = { 18,38 };
void showCSPDetails(Renderer* renderer, Coord pos, CSP* csp) {
	drawRectBorder(renderer, pos, CSPDetailsRectSize);
	struct tm date;
	pos.x++;
	pos.y++;
	if (csp->reqDateStart != TIME_NAN) {
		localtime_s(&date, &csp->reqDateStart);
		coordPrintf(renderer, pos, "开始日期:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
		pos.x++;
	}
	else coordPrintf(renderer, pos, "开始日期:不限"), pos.x++;
	if (csp->reqDateEnd != TIME_NAN) {
		localtime_s(&date, &csp->reqDateEnd);
		coordPrintf(renderer, pos, "截止日期:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
		pos.x++;
	}
	else coordPrintf(renderer, pos, "截止日期:不限"), pos.x++;

	coordPrintf(renderer, pos, "折扣率:%.2lf", csp->discount), pos.x++;
	coordPrintf(renderer, pos, "可与单品销售方案叠加应用:");
	if (csp->overlaySingleSP) renderPrintf(renderer, "是");
	else renderPrintf(renderer, "否");
	pos.x++;
	coordPrintf(renderer, pos, "附加说明:%s", csp->addInfo), pos.x++;
}

char typeQuantityText[5][20] = { "","采购量","销售量","变化量","赠予量" };
char typeAmountText[5][20] = { "","采购总额","销售总额","总价变化","赠品额外收费" };
char typeUpriceText[5][20] = { "","采购单价","销售单价","售价变化","赠品额外单价" };
void showRecordDetails(Renderer* renderer, Coord pos, Record* rec) {
	struct tm time;
	Coord cur = pos;
	CellData cellName[9][2] =
	{ {{drawCellStr,12,0,"记录ID"},{drawCellInt,12,0,&rec->recID}},
	  {{drawCellStr,12,0,"商品ID"},{drawCellInt,12,0,&rec->invID}},
	  {{drawCellStr,12,0,"类型"},{drawCellStr,12,0,&recordType[rec->type]}},
	  {{drawCellStr,12,0,"记录时间"},{drawCellDate,12,0,&rec->time}},
	  {{drawCellStr,12,0,&typeQuantityText[rec->type]},{drawCellBULK,12,0,&rec->prod.weight}},
	  {{drawCellStr,12,0,&typeQuantityText[rec->type]},{drawCellUNIT,12,0,&rec->prod.quantity}},
	  {{drawCellStr,12,0,&typeUpriceText[rec->type]},{drawCellDouble,12,0,&rec->prod.purUPrice}},
	  {{drawCellStr,12,0,&typeAmountText[rec->type]},{drawCellDouble,12,0,&rec->prod.amount}},
	  {{drawCellStr,12,0,"附加信息"},{drawCellStr,12,0,&rec->addInfo}}
	};
	int i;
	for (i=0; i < 4; i++)
	{
		if(i%2==0)drawColorBar(renderer, cur, 238, 232, 213, InvDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i], 2);
		cur.x++;
	}
	showProductDetails(renderer, cur, &rec->prod,i++);
	cur.x += ProdDetailsRectSize.x;
	if (rec->prod.pack == BULK) {
		resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i], 2);
	}
	else if (rec->prod.pack == UNIT) {
		resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i+1], 2);
	}
	i += 2;
	for (; i < 9; i++)
	{
		if(i%2==0) drawColorBar(renderer, cur, 238, 232, 213, InvDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i], 2);
		cur.x++;
	}
}
