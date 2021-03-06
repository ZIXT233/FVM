#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>
#include"UI.h"
#include"renderer.h"




void drawRectBorder(Renderer* renderer, Coord pos, Coord const size) {
	renderPrintf(renderer, ESC "(0");
	resetBackgroundColor(renderer);
	setTextColor(renderer, 59, 120, 255);
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
	resetBackgroundColor(renderer);
	resetTextColor(renderer);
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
void drawMenu(Renderer* renderer, Coord origin, Coord rectSize, const char* title, int n, int firstNum, ...) {
	va_list args;
	va_start(args, firstNum);
	drawRectBorder(renderer, origin, rectSize);
	coordPrintf(renderer, (Coord) { origin.x, origin.y + 2 }, "%s", title);
	Coord itemRect = rectSize, itemPos = origin, cur;
	itemRect.x--, itemRect.y -= 4;
	itemPos.x += 2, itemPos.y += 4;
	int cols = ((n - 1) / (itemRect.x)) + 1, itemWidth = itemRect.y / cols, itemHight = 1;
	if (n < itemRect.x) {
		itemHight = itemRect.x / n;
	}
	cur = itemPos;
	for (int i = 1, num = firstNum; i <= cols; i++) {
		for (int j = 1; j <= itemRect.x && num < firstNum + n; j++) {
			char* item = va_arg(args, char*);
			coordPrintf(renderer, cur, "%d.%s", num++, item);
			cur.x += itemHight;
		}
		cur.x = itemPos.x;
		cur.y += itemWidth;
	}
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
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y - 7 }, "%d/%d??", ((pageStartNum - 1) / pageSize) + 1, ((start->list.root->size - 1) / pageSize) + 1);
	origin.x += 3;
	origin.y++;
}
void drawInvList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = listEntry(entry, Inventory, list);
	Coord cur = origin;
	CellData cellName[5] = { {drawCellStr,14,0,"????"} ,{drawCellStr,14,0,"????"},{drawCellStr,14,0,"????????"},
		{drawCellStr,14,0,"ID"},{drawCellStr,14,0,"????"} };
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
									{drawCellDouble,14,0,&pos->prod.unitPrice},
									{drawCellInt,14,0,&pos->invID},
									{packDrawer,14,0,qw,pos->prod.unitName} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, invListRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 5);
		cur.x++;
		n++;
	}
}
void drawInvManageList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = listEntry(entry, Inventory, list);
	Coord cur = origin;
	CellData cellName[7] = { {drawCellStr,14,0,"????"} ,{drawCellStr,14,0,"????"},{drawCellStr,14,0,"????????"},
		{drawCellStr,14,0,"ID"},{drawCellStr,7,0,"????"},{drawCellStr,14,0,"??????"}, {drawCellStr,14,0,"????"} };
	drawColorBar(renderer, cur, 238, 232, 213, invManageListRectSize.y);
	drawListItem(renderer, cur, cellName, 7);
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
		CellData cellData[7] = { {drawCellStr,14,0,pos->prod.kind} ,
									{drawCellStr,14,0,pos->prod.variety},
									{drawCellDouble,14,0,&pos->prod.unitPrice},
									{drawCellInt,14,0,&pos->invID},
									{drawCellQuality,7,0,&pos->prod.quality},
									{drawCellDate,14,0,&pos->prod.expiration},
									{packDrawer,14,0,qw,pos->prod.unitName} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, invManageListRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 7);
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


	CellData cellName[8] = { {drawCellStr,12,0,"????"} ,{drawCellStr,12,0,"????ID"},{drawCellStr,12,0,"????ID"},{drawCellStr,12,0,"????"},
	{drawCellStr,12,0,"????"},{drawCellStr,15,0,"????????"}, {drawCellStr,15,0,"????????"},{drawCellStr,22,0,"????"} };
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
								{packDrawer,15,1,qw,pos->prod.unitName},
								{drawCellDouble,15,1,&amount},
								{drawCellTime,22,1,&pos->time} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, RecordRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 8);

		cur.x++;
		n++;
	}recordForEachEnd
}


void drawInvCheckList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Record* start = recordEntry(entry, timeList);
	Coord cur = origin;
	int n = 0;
	void* qw = NULL;
	CellDataDrawer packDrawer = NULL;

	CellData cellName[6] = { {drawCellStr,12,0,"????"} ,{drawCellStr,12,0,"????"},{drawCellStr,10,0,"????ID"},{drawCellStr,17,0,"????????(????ID)"},
							  {drawCellStr,65,0,"????????"},{drawCellStr,12,0,"??????????"} };
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
								  (pos->recID != -1) ? (CellData) { drawCellInt,17,0,& pos->recID } : (CellData) { drawCellStr,17,0," -" },
									{drawCellStr,65,0,pos->addInfo},{packDrawer,12,0,qw,pos->prod.unitName} };


		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, InvCheckRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 6);
		cur.x++;
		n++;
	}
}
void drawTitleWindow(Renderer* renderer, Coord origin, const char* title, Coord rectSize) {
	Coord titleRect;
	titleRect.x = 1;
	titleRect.y = rectSize.y;
	drawRectBorder(renderer, origin, titleRect);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y / 2 - strlen(title) / 2 }, "%s", title);
	rectSize.x += 2;
	drawRectBorder(renderer, origin, rectSize);
}
void drawListPage(Renderer* renderer, Coord origin, const char* title, ListDrawer drawer, ListHead* start, int* pageStartNum, int pageSize, Coord rectSize, void* exArg) {
	Coord titleRect, listPos = origin;
	listPos.x += 3, listPos.y += 1;
	start = listShowPageJump(start, pageStartNum, pageSize);
	drawer(renderer, listPos, start, pageSize, exArg);
	drawTitleWindow(renderer, origin, title, rectSize);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y - 7 }, "%d/%d??", ((*pageStartNum - 1) / pageSize) + 1, ((start->root->size - 1) / pageSize) + 1);
}

void drawSSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	SSP* start = listEntry(entry, SSP, list);
	Coord cur = origin;

	CellData cellName[2] = { {drawCellStr,12,0,"??????"} ,{drawCellStr,12,0,"????ID"} };
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

	CellData cellName[2] = { {drawCellStr,12,0,"??????"} ,{drawCellStr,12,0,"????ID"} };
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
	CellData cellName[3] = { {drawCellStr,12,0,"????"},{drawCellStr,12,0,"????"},{drawCellStr,12,0,"????ID"} };
	drawColorBar(renderer, cur, 238, 232, 213, CSPComInvRectSize.y);
	drawListItem(renderer, cur, cellName, 3);
	//coordPrintf(renderer, origin, "????\t\t????\t\t????ID");
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

	//L????


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
	CellData cellName[9] = { {drawCellStr,12,0,"????"},{drawCellStr,12,0,"????"},{drawCellStr,10,0,"????ID"},
								{drawCellStr,20,0,"????????????/ID"},{drawCellStr,20,0,"????????????/ID"},{drawCellStr,12,0,"????"},
								{drawCellStr,10,0,"??????"},{drawCellStr,7,1,"??????"},{drawCellStr,12,0,"????"} };
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
								{packDrawer,10,0,qw,pos->prod.unitName},{drawCellDouble,7,0,&pos->discount},{drawCellDouble,12,0,&discountAmount} };
		if (pos->type == GIFT) {
			cellData[7] = (CellData){ drawCellStr,7,0,"????" };
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
	setBackGroundColor(renderer, r, g, b);
	for (int i = 0; i < length; i++) {
		renderPrintf(renderer, " ");
	}

}
/*void resetBackgroundColor(Renderer* renderer) {
	renderPrintf(renderer, CSI "49m");
}*/

void drawCellDouble(Renderer* renderer, Coord origin, CellData* cell) {
	double data = *(double*)cell->data;
	if (cell->sign)coordPrintf(renderer, origin, " %+.2lf", data);
	else coordPrintf(renderer, origin, " %.2lf", data);
}
void drawCellBULK(Renderer* renderer, Coord origin, CellData* cell) {
	CellData sub = *cell;
	sub.width -= 1;
	drawCellDouble(renderer, origin, &sub);
	origin.y += cell->width - 2;
	if (cell->unitName == NULL || cell->unitName[0] == '\0')coordPrintf(renderer, origin, "??");
	else coordPrintf(renderer, origin, "%s", cell->unitName);
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
	if (cell->unitName == NULL || cell->unitName[0] == '\0')coordPrintf(renderer, origin, "??");
	else coordPrintf(renderer, origin, "%s", cell->unitName);
}
void drawCellStr(Renderer* renderer, Coord origin, CellData* cell) {
	char* data = (char*)cell->data, output[INFOMAX];
	if (!cell->sign)strMakeLen(output, data, cell->width);
	else strcpy_s(output, cell->width, data);
	coordPrintf(renderer, origin, " %s", output);
}
void drawCellPackStr(Renderer* renderer, Coord origin, CellData* cell) {
	int data = *(int*)cell->data;;
	if (data == BULK) coordPrintf(renderer, origin, " ????");
	else if (data == UNIT) coordPrintf(renderer, origin, " ??????");
}

void drawCellQuality(Renderer* renderer, Coord origin, CellData* cell) {
	int data = *(int*)cell->data;
	coordPrintf(renderer, origin, " %s", qualityText[data]);
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
		coordPrintf(renderer, origin, "??????");
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
		coordPrintf(renderer, origin, "??????");
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
	CellData cellName[9] = { {drawCellStr,12,0,"????"} ,{drawCellStr,12,0,"????"},{drawCellStr,12,0,"????ID"},
		{drawCellStr,12,0,"??????"},{drawCellStr,12,0,"??????"}, {drawCellStr,12,0,"??????"},{drawCellStr,12,0,"??????"},
		{drawCellStr,15,0,"??????"},{drawCellStr,15,0,"??????"} };
	drawColorBar(renderer, cur, 238, 232, 213, statsRectSize.y);
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
									{packDrawer,12,0,qwTable[SALE],pos->prod.unitName},
									{packDrawer,12,0,qwTable[PURCHASE],pos->prod.unitName},
									{packDrawer,12,0,qwTable[GIFT],pos->prod.unitName},
									{packDrawer,12,0,qwTable[DESTROY],pos->prod.unitName},
									{drawCellDouble,15,1,&finance.turnover},
									{drawCellDouble,15,1,&finance.profit} };
		if (n & 1)drawColorBar(renderer, cur, 238, 232, 213, statsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellData, 9);
		cur.x++;
		n++;
	}

	//????????????????
	Coord gFinancePos = origin;
	gFinancePos.x += pageSize + 1;
	char gFTitle[50], * gFcur;
	struct tm sdate, edate;
	gFcur = gFTitle + sprintf_s(gFTitle, 10, "????????");
	/*if (filter) {
		localtime_s(&sdate, &filter->time);
		localtime_s(&edate, &filter->lastTime);
		if (filter->time != TIME_NAN) gFcur += sprintf_s(gFcur, 20, "(%d/%d/%d - ", sdate.tm_year + 1900, sdate.tm_mon + 1, sdate.tm_mday);
		else gFcur += sprintf_s(gFcur, 20, "(???? - ");
		if (filter->lastTime != TIME_NAN) gFcur += sprintf_s(gFcur, 20, "%d/%d/%d)", edate.tm_year + 1900, edate.tm_mon + 1, edate.tm_mday);
		else gFcur += sprintf_s(gFcur, 20, "????)");
	}*/

	CellData financeName[5] = { {drawCellStr,23,0,"????????"} ,{drawCellStr,23,0,"????????"},{drawCellStr,24,0,"??????"},
								{drawCellStr,24,0,"????"},{drawCellStr,24,0,"??????"} };
	cur = gFinancePos;
	drawColorBar(renderer, cur, 238, 232, 213, statsRectSize.y);
	coordPrintf(renderer, (Coord) { cur.x, cur.y + 120 / 2 - strlen(gFTitle) / 2 }, "%s", gFTitle); cur.x++;
	resetBackgroundColor(renderer);
	drawListItem(renderer, cur, financeName, 5); cur.x++;
	CellData financeData[5] = { {drawCellDouble,23,0,&gdata->finance->startUpCapital},{drawCellDouble,23,0,&gdata->finance->balance},
								{drawCellDouble,24,0,&gdata->finance->turnover},{drawCellDouble,24,0,&gdata->finance->cost},
								{drawCellDouble,24,0,&gdata->finance->profit} };
	drawListItem(renderer, cur, financeData, 5);
}
void drawGiftList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize, void* exArg) {
	Inventory* start = invEntry(entry);
	//coordPrintf(renderer, origin, "????\t\t????\t\t????ID\t??????\t????????");


	Coord cur = origin;
	CellData cellName[5] = { {drawCellStr,14,0,"????"},{drawCellStr,14,0,"????"},{drawCellStr,10,0,"????ID"},
		{drawCellStr,10,0,"??????"},{drawCellStr,10,1,"????????"} };
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
			renderPrintf(renderer, "%6.2lf??\t%8.2lf", pos->prod.weight, pos->prod.unitPrice);
		}
		else if (pos->prod.pack == UNIT) {
			renderPrintf(renderer, "%6d??\t%8.2lf", pos->prod.quantity, pos->prod.unitPrice);
		}
		origin.x++;
		n++;*/

		CellData cellData[5] = { {drawCellStr,14,0,pos->prod.kind},
								{drawCellStr,14,0,pos->prod.variety},
								{drawCellInt,10,0,&pos->invID},
								{packDrawer,10,0,qw,pos->prod.unitName},
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
	setTextColor(renderer, 253, 246, 227);
	setBackGroundColor(renderer, 59, 120, 255);
	renderPrintf(renderer, CSI "s" CSI "?25l");
	renderPrintf(renderer, CSI "%d;%dH", STATUS_ORIGIN.x, STATUS_ORIGIN.y + 4);
	renderPrintf(renderer, "%02d:%02d:%02d", sti.tm_hour, sti.tm_min, sti.tm_sec);
	renderPrintf(renderer, CSI "%d;%dH", STATUS_ORIGIN.x, PanelSize.y - 10);
	renderPrintf(renderer, "%d/%d/%d", sti.tm_year + 1900, sti.tm_mon + 1, sti.tm_mday);
	renderPrintf(renderer, CSI "u" CSI "?25h");
	resetBackgroundColor(renderer);
	resetTextColor(renderer);
}
void drawStatusBar(Renderer* renderer, Coord origin, FVMO* gdata) {
	setCursorPos(renderer, origin);
	setTextColor(renderer, 253, 246, 227);
	setBackGroundColor(renderer, 59, 120, 255);
	for (int i = 0; i < PanelSize.y; i++) {
		renderPrintf(renderer, " ");
	}
	Coord pageStatusPos = origin;
	pageStatusPos.y += 15;
	setCursorPos(renderer, pageStatusPos);
	listForEachEntry(PageStack, pos, &gdata->pageStack->list, list) {
		renderPrintf(renderer, "/");
		renderPrintf(renderer, "%s", pos->pageName);
	}
	drawTimeBar(renderer, gdata->timer);
	resetBackgroundColor(renderer);
}

void inputStart(Renderer* renderer, Coord inputOrigin) {

	setCursorPos(renderer, inputOrigin);
	renderPrintf(renderer, ESC "(0");
	for (int i = 0; i < PanelSize.y; i++) {
		renderPrintf(renderer, "q");
	}
	Coord bottom = inputOrigin;
	bottom.x += 15;
	setCursorPos(renderer, bottom);
	for (int i = 0; i < PanelSize.y; i++) {
		renderPrintf(renderer, "q");
	}
	
	renderPrintf(renderer, ESC "(B");
	Coord quitTip = bottom;
	quitTip.y = PanelSize.y - 30;
	coordPrintf(renderer, quitTip, " ????quit??????????");
	renderPrintf(renderer, CSI "%d;%dr", inputOrigin.x + 1, bottom.x - 1);
	inputOrigin.x++;
	setCursorPos(renderer, inputOrigin);
}

#define READ_TOO_LONG -1
#define READ_SUCCESS 0
extern FILE* stdinLog;
extern int demoDelay;
int readline(char* buf, int maxCount, FILE* stream, int* pNumberRead) { //????????????????buf????????????maxCount??????????????????????????????????????????
	int i;
	char t;
	*pNumberRead = 0;
	if (demoDelay > 0) Sleep(demoDelay);
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
	getUIntInput("??????????????:", &num, ALLINT, false);
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
				printf("????????????????\n");
				continue;
			}
			if (range.max < range.min || (num >= range.min && num <= range.max)) {
				*value = num;
				return INPUT_SUCCESS;
			}
			else {
				printf("??????[%d,%d]????????????\n", range.min, range.max);
				continue;
			}
		}
		else {
			printf("??????????????\n");
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
			printf("??????????????(????????%d)\n", maxCount - 1);
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
			printf("????????????????\n");
			continue;
		}
		if (sscanf_s(input, "%lf", &num) == 1) {
			if (fLess(range.max, range.min) || (fGreaterEq(num, range.min) && fLessEq(num, range.max))) {
				*value = num;
				return INPUT_SUCCESS;
			}
			else {
				printf("??????[%lf,%lf]????????????\n", range.min, range.max);
				continue;
			}
		}
		else {
			printf("????????????????\n");
			continue;
		}
	}
}
int getDateTime(const char* query, time_t* value, bool strict) { //??????????value??TIME_NAN
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
				printf("????????????????\n");
			}
			else {
				*value = time;
				return INPUT_SUCCESS;
			}
		}
	}
}
int getTimeInput(const char* query, time_t* value, bool strict) { //??????????value??TIME_NAN
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
		if (sscanf_s(input, "%d.%d.%d/%d:%d:%d", &ti.tm_year, &ti.tm_mon, &ti.tm_mday, &ti.tm_hour, &ti.tm_min, &ti.tm_sec) == 6) {
			ti.tm_year -= 1900;
			ti.tm_mon--;
			ti_verify = ti;
			//ti.tm_hour = 8;
			time = mktime(&ti);
			ti_verify.tm_wday = ti.tm_wday, ti_verify.tm_yday = ti.tm_yday;
			if (time == -1 || memcmp(&ti, &ti_verify, sizeof(ti)) != 0) {
				printf("????????????????\n");
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
		breakDeliver(getUIntInput("??????????ID:", id, ALLINT, true));
		if (*pInv = invQueryID(head, *id)) return;
		else {
			printf("????????????\n");
		}
	}
}
int inputInventoryIDHistory(const Inventory* head, const Inventory* historyHead, int* id, Inventory** pInv) {
	*pInv = NULL;
	while (1) {
		breakDeliver(getUIntInput("??????????ID:", id, ALLINT, true));
		if (*pInv = invQueryID(head, *id)) return;
		else if (*pInv = invQueryID(historyHead, *id)) return;
		else {
			printf("????????????\n");
		}
	}
}

int inputRecordID(const Record* head, int recordType, int* id, Record** pRec) {
	*pRec = NULL;
	while (1) {
		breakDeliver(getUIntInput("??????????ID:", id, ALLINT, true));
		if (*pRec = recordQueryID(head, *id, recordType)) return;
		else {
			printf("????????????\n");
		}
	}
}
int inputSSPID(const SSP* head, int* id, SSP** pSSP) {
	*pSSP = NULL;
	while (1) {
		breakDeliver(getUIntInput("??????????????????ID:", id, ALLINT, true));
		if (*pSSP = SSPQueryID(head, *id)) return;
		else {
			printf("????????????\n");
		}
	}
}
int inputCSPID(const CSP* head, int* id, CSP** pCSP) {
	*pCSP = NULL;
	while (1) {
		breakDeliver(getUIntInput("??????????????????ID:", id, ALLINT, true));
		if (*pCSP = CSPQueryID(head, *id)) return;
		else {
			printf("????????????\n");
		}
	}
}

int inputProduct(Product* prod) {
	breakDeliver(getStrInput("????????:", prod->kind, INFOMAX, true));
	breakDeliver(getStrInput("????????:", prod->variety, INFOMAX, true));
	breakDeliver(getDateTime("??????????(??.??.??):", &prod->expiration, true));
	prod->expiration += 3600 * 24 - 1;
	drawOrdMenu("????:", 3, 1, "??", "??", "??");
	breakDeliver(getUIntInput("????????:", &prod->quality, (IntRange) { 1, 3 }, true));
	drawOrdMenu("????????:", 2, 1, "????", "??????");
	breakDeliver(getUIntInput("????????????:", &prod->pack, (IntRange) { 1, 2 }, true));
	breakDeliver(getStrInput("??????????????????(????):", prod->unitName, 15, false));
	if (prod->pack == BULK) {
		breakDeliver(getDoubleInput("????????:", &prod->weight, WRANGE, true));
		prod->weight = centRound(prod->weight);
	}
	else if (prod->pack == UNIT) {
		breakDeliver(getUIntInput("????????:", &prod->quantity, QRANGE, true));
	}
	breakDeliver(getDoubleInput("????????????:", &prod->purUPrice, UPRINCERANGE, true));
	prod->purUPrice = centRound(prod->purUPrice);
	breakDeliver(getDoubleInput("????????????:", &prod->unitPrice, UPRINCERANGE, true));
	prod->unitPrice = centRound(prod->unitPrice);
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
		breakCatch(getDoubleInput("????????????:", &cp->prod.weight, WRANGE, true)) {
			free(cp);
			return INPUT_BREAK;
		}
		cp->prod.amount = cp->prod.weight;
	}
	else if (inv->prod.pack == UNIT) {
		breakCatch(getUIntInput("????????????:", &cp->prod.quantity, QRANGE, true)) {
			free(cp);
			return INPUT_BREAK;
		}
		cp->prod.amount = cp->prod.quantity;
	}
	cp->prod.unitPrice = 0;
	breakCatch(getDoubleInput("????????????(??????????????????):", &cp->prod.unitPrice, UPRINCERANGE, false)) {
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
	breakDeliver(getStrInput("????????(????????):", prod->kind, INFOMAX, false));
	breakDeliver(getStrInput("????????(????????):", prod->variety, INFOMAX, false));
	breakDeliver(getDateTime("????????????????????(??.??.??)(????????) : ", &prod->expiration, false));
	if (prod->expiration != TIME_NAN) prod->expiration += 3600 * 24 - 1; //????????????????????????
	drawOrdMenu("????:", 4, 0, "????", "??", "??", "??");
	breakDeliver(getUIntInput("????????(????????):", &prod->quality, (IntRange) { 1, 3 }, false));
	drawOrdMenu("????????:", 3, 0, "????", "????", "??????");
	breakDeliver(getUIntInput("????????????(????????):", &prod->pack, (IntRange) { 0, 2 }, false));
	return INPUT_SUCCESS;
}

int inputRecordFilter(Record* rec) {
	memset(rec, 0, sizeof(Record));
	rec->time = rec->lastTime = TIME_NAN;
	drawOrdMenu("????????:", 5, 0, "????", "????", "????????", "????????", "????????","????????");
	breakDeliver(getUIntInput("????????(????????) : ", &rec->type, (IntRange) { 0, 5 }, false));
	breakDeliver(inputProductFilter(&rec->prod));
	breakDeliver(getDateTime("??????????????(??.??.??)(????????) : ", &rec->time, false));
	breakDeliver(getDateTime("??????????????(??.??.??)(????????) : ", &rec->lastTime, false));
	if (rec->lastTime != TIME_NAN) rec->lastTime += 3600 * 24 - 1;
	return INPUT_SUCCESS;
}
int inputSaleRecordFilter(Record* rec) {
	memset(rec, 0, sizeof(Record));
	rec->time = rec->lastTime = TIME_NAN;
	rec->type = SALE;
	breakDeliver(inputProductFilter(&rec->prod));
	breakDeliver(getDateTime("??????????????(??.??.??)(????????) : ", &rec->time, false));
	breakDeliver(getDateTime("??????????????(??.??.??)(????????) : ", &rec->lastTime, false));
	if (rec->lastTime != TIME_NAN) rec->lastTime += 3600 * 24 - 1;
	return INPUT_SUCCESS;
}



void showProductDetails(Renderer* renderer, Coord pos, Product* prod, int lineNum) {  //Only Show product attributes
	struct tm date;
	Coord cur = pos;
	int valWidth = ProdDetailsRectSize.y - 13;
	CellData cellName[6][2] = { {{drawCellStr,12,0,"????"},{drawCellStr,valWidth,0,&prod->kind}},
								{{drawCellStr,12,0,"????"},{drawCellStr,valWidth,1,&prod->variety}},
								{{drawCellStr,12,0,"????"},{drawCellStr,valWidth,0,&qualityText[prod->quality]}},
								{{drawCellStr,12,0,"??????"},{drawCellDate,valWidth,1,&prod->expiration}},
								{{drawCellStr,12,0,"????????"},{drawCellStr,valWidth,0,"????"}},
								{{drawCellStr,12,0,"????????"},{drawCellStr,valWidth,0,"??????"}}
	};
	for (int i = 0; i < 4; i++)
	{
		if (lineNum % 2) drawColorBar(renderer, cur, 238, 232, 213, ProdDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i], 2);
		cur.x++;
		lineNum++;
	}
	if (prod->pack == BULK) {
		if (lineNum % 2) drawColorBar(renderer, cur, 238, 232, 213, ProdDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[4], 2);
	}
	else if (prod->pack == UNIT) {
		if (lineNum % 2) drawColorBar(renderer, cur, 238, 232, 213, ProdDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[5], 2);
	}cur.x++;
}

void showInvDetails(Renderer* renderer, Coord pos, Inventory* inv) {
	int curLineNum = 3;
	Coord cur = pos;
	int valWidth = InvDetailsRectSize.y - 13;
	CellDataDrawer packDrawer = NULL;
	void* qw = NULL;
	if (inv->prod.pack == BULK) {
		packDrawer = drawCellBULK;
		qw = &inv->prod.weight;
	}
	else if (inv->prod.pack == UNIT) {
		packDrawer = drawCellUNIT;
		qw = &inv->prod.quantity;
	}
	CellData cellName[3][2] = { {{drawCellStr,12,0,"????ID"},{drawCellInt,valWidth,0,&inv->invID}},
								{{drawCellStr,12,0,"??????"},{packDrawer,12,0,qw,inv->prod.unitName}},
								{{drawCellStr,12,0,"????????"},{drawCellDouble,valWidth,0,&inv->prod.unitPrice}}
	};
	for (int i = 0, line = 0; i < 3; i++) {
		if (i == 1) {
			showProductDetails(renderer, cur, &inv->prod, line);
			cur.x += ProdDetailsRectSize.x;
			line += ProdDetailsRectSize.x;
		}
		if (line % 2) drawColorBar(renderer, cur, 238, 232, 213, InvDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i], 2);
		cur.x++, line++;
	}
}

void showSSPDetails(Renderer* renderer, Coord pos, SSP* ssp) {
	struct tm date;
	//????
	Coord cur = pos;
	int valWidth = SSPDetailsRectSize.y - 19;
	int count = 0;

	//coordPrintf(renderer, pos, "%s", ssp->planName); pos.x++;
	CellData tmpcellName[2] = { {drawCellStr,18,0,"????????"},drawCellStr,valWidth,0,ssp->planName };
	drawListItem(renderer, cur, tmpcellName, 2); cur.x++;
	CellData cellName0[17][2] = { {{drawCellStr,18,0,"????"},{drawCellStr,valWidth,0,"??????????ID????"}},
		/*1*/	{{drawCellStr,18,0,"????"},{drawCellStr,valWidth,0,"????????????????????????"}},
				{{drawCellStr,18,0,"????????ID"},{drawCellInt,valWidth,0,&ssp->invID}},
				{{drawCellStr,18,0,"????????"},{drawCellStr,valWidth,0,(ssp->reqMember) ? "??" : "??"}},
				{{drawCellStr,18,0,"????"} , { drawCellStr,valWidth,0, ssp->filter.kind } },
		/*5*/   {{drawCellStr,18,0,"????"} ,{ drawCellStr, valWidth, 0, ssp->filter.variety }},
				{{drawCellStr,18,0,"????"},{ drawCellStr,valWidth,0,qualityText[ssp->filter.quality]}},
				{{drawCellStr,18,0,"????????"}, { drawCellPackStr, valWidth, 0,&ssp->filter.pack }},
				{{drawCellStr,18,0,"??????"},{drawCellDate,valWidth,0,&ssp->filter.expiration}},
				{{drawCellStr,18,0,"????????"},{ drawCellDouble,valWidth,0,&ssp->filter.unitPrice }},
		/*10*/	{{drawCellStr,18,0,"????????????"}, { drawCellBULK,17,0,&ssp->filter.weight}},
				{{drawCellStr,18,0,"????????????"},{ drawCellUNIT,17,0,&ssp->filter.quantity }},
				{{drawCellStr,18,0,"????????"}, { drawCellDouble,valWidth,0,&ssp->filter.amount }},
				{{drawCellStr,18,0,"????????"}, { drawCellDate,valWidth,0,&ssp->reqDateStart }},
				{{drawCellStr,18,0,"????????"},{ drawCellDate,valWidth,0,&ssp->reqDateEnd }},
				{{drawCellStr,18,0,"??????"},{drawCellDouble,valWidth,0,&ssp->discount}},
				{{drawCellStr,18,0,"????????"},{drawCellStr,valWidth,0,ssp->addInfo}},

	};
	long long judge[7] = { ssp->filter.kind[0],ssp->filter.variety[0],ssp->filter.quality,ssp->filter.pack,ssp->filter.unitPrice,ssp->filter.weight,ssp->filter.quantity };

	if (ssp->invID) {
		drawColorBar(renderer, cur, 238, 232, 213, SSPDetailsRectSize.y); count++;
		drawListItem(renderer, cur, cellName0[0], 2); cur.x++;
		resetBackgroundColor(renderer); count++;
		drawListItem(renderer, cur, cellName0[2], 2); cur.x++;
		drawColorBar(renderer, cur, 238, 232, 213, SSPDetailsRectSize.y); count++;
		drawListItem(renderer, cur, cellName0[3], 2), cur.x++;
		if (ssp->filter.pack == BULK) {
			resetBackgroundColor(renderer); count++;
			if (ssp->filter.weight) drawListItem(renderer, cur, cellName0[10], 2), cur.x++;
			else {
				cellName0[10][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
				drawListItem(renderer, cur, cellName0[9], 2), cur.x++;
			}

		}
		else {
			resetBackgroundColor(renderer); count++;
			if (ssp->filter.quantity) drawListItem(renderer, cur, cellName0[10], 2), cur.x++;
			else {
				cellName0[11][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
				drawListItem(renderer, cur, cellName0[11], 2), cur.x++;
			}

		}
	}
	else {
		drawColorBar(renderer, cur, 238, 232, 213, SSPDetailsRectSize.y); count++;
		drawListItem(renderer, cur, cellName0[1], 2); cur.x++;
		resetBackgroundColor(renderer); count++;
		drawListItem(renderer, cur, cellName0[3], 2), cur.x++;
		for (int i = 0; i < 8; i++) {
			int cn = 0;
			if (count % 2)  resetBackgroundColor(renderer), count++;
			else  drawColorBar(renderer, cur, 238, 232, 213, SSPDetailsRectSize.y), count++;
			if (i == 4) {
				if (ssp->filter.expiration != TIME_NAN) drawListItem(renderer, cur, cellName0[i + 4], 2), cur.x++;
				else {
					cellName0[i + 4][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 4], 2), cur.x++;
				}
			}
			else {

				if (judge[cn]) drawListItem(renderer, cur, cellName0[i + 4], 2), cur.x++;
				else {
					cellName0[i + 4][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 4], 2), cur.x++;
				}
				cn++;
			}
			/*if (i == 0) {
				if (ssp->filter.kind[0])  drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				else {
					cellName0[i + 3][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				}
			}
			else if (i == 1) {

				if (ssp->filter.variety[0])  drawListItem(renderer, cur, cellName0[i + 3 + 1], 2), cur.x++;
				else {
					cellName0[i + 3 + 1][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				}
			}
			else if (i == 2) {

				if (ssp->filter.quality)  drawListItem(renderer, cur, cellName0[i + 3 + 1], 2), cur.x++;
				else {
					cellName0[i + 3 + 1][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				}
			}
			else if (i == 3) {

				if (ssp->filter.pack)  drawListItem(renderer, cur, cellName0[i + 3 + 1], 2), cur.x++;
				else {
					cellName0[i + 3 + 1][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				}
			}
			else if (i == 4) {
				if (ssp->filter.expiration != TIME_NAN) drawListItem(renderer, cur, cellName0[i], 2), cur.x++;
				else {
					cellName0[i][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				}
			}
			else if (i == 5) {
				if (ssp->filter.unitPrice)  drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				else {
					cellName0[i + 3][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				}
			}
			else if (i == 6) {
				if (ssp->filter.weight)  drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				else {
					cellName0[i + 3][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				}
			}
			else if (i == 7) {
				if (ssp->filter.quantity)  drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				else {
					cellName0[i + 3][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
					drawListItem(renderer, cur, cellName0[i + 3], 2), cur.x++;
				}
			}
			*/
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (count % 2)  resetBackgroundColor(renderer), count++;
		else  drawColorBar(renderer, cur, 238, 232, 213, SSPDetailsRectSize.y), count++;
		switch (i) {
		case 0:
			if (ssp->filter.amount) drawListItem(renderer, cur, cellName0[i + 12], 2), cur.x++;
			else {
				cellName0[i + 12][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
				drawListItem(renderer, cur, cellName0[i + 12], 2), cur.x++;
			}
			break;
		case 1:
			if (ssp->reqDateStart != TIME_NAN) drawListItem(renderer, cur, cellName0[i + 12], 2), cur.x++;
			else {
				cellName0[i + 12][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
				drawListItem(renderer, cur, cellName0[i + 12], 2), cur.x++;
			}
			break;
		case 2:
			if (ssp->reqDateEnd != TIME_NAN)drawListItem(renderer, cur, cellName0[i + 12], 2), cur.x++;
			else {
				cellName0[i + 12][1] = ((CellData) { drawCellStr, valWidth, 0, "????" });
				drawListItem(renderer, cur, cellName0[i + 12], 2), cur.x++;
			}
			break;
		}
	}
	if (count % 2) resetBackgroundColor(renderer), count++;
	else    drawColorBar(renderer, cur, 238, 232, 213, SSPDetailsRectSize.y), count++;
	drawListItem(renderer, cur, cellName0[15], 2), cur.x++;
	if (count % 2) resetBackgroundColor(renderer), count++;
	else    drawColorBar(renderer, cur, 238, 232, 213, SSPDetailsRectSize.y), count++;
	drawListItem(renderer, cur, cellName0[16], 2), cur.x++;
}


void showCSPDetails(Renderer* renderer, Coord pos, CSP* csp) {
	struct tm date;
	//????	

	Coord cur = pos;
	int valWidth = CSPDetailsRectSize.y - 19;
	CellData cellName[6][2] = { {{drawCellStr,18,0,"????????"},{drawCellStr,valWidth,0,csp->planName}},
								{{drawCellStr,18,0,"????????"},(csp->reqDateStart != TIME_NAN) ? ((CellData) { drawCellDate,valWidth,1,& csp->reqDateStart }) : ((CellData) { drawCellStr,valWidth,0,"????" })},
								{{drawCellStr,18,0,"????????"},(csp->reqDateEnd != TIME_NAN) ? ((CellData) { drawCellDate,valWidth,1,& csp->reqDateEnd }) : ((CellData) { drawCellStr,valWidth,0,"????" })},
								{{drawCellStr,18,0,"??????"},{drawCellDouble,valWidth,0,&csp->discount}},
								{{drawCellStr,18,0,"????????????????"},(csp->overlaySingleSP) ? ((CellData) { drawCellStr,valWidth,0,"??" }) : ((CellData) { drawCellStr,valWidth,0,"??" })},
								{{drawCellStr,18,0,"????????"},{drawCellStr,valWidth,0,csp->addInfo}}
	};
	/*
if (csp->reqDateStart != TIME_NAN) {
	localtime_s(&date, &csp->reqDateStart);
	coordPrintf(renderer, pos, "????????:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
	pos.x++;
}
else coordPrintf(renderer, pos, "????????:????"), pos.x++;
if (csp->reqDateEnd != TIME_NAN) {
	localtime_s(&date, &csp->reqDateEnd);
	coordPrintf(renderer, pos, "????????:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
	pos.x++;
}
else coordPrintf(renderer, pos, "????????:????"), pos.x++;

coordPrintf(renderer, pos, "??????:%.2lf", csp->discount), pos.x++;
coordPrintf(renderer, pos, "????????????????????????:");
if (csp->overlaySingleSP) renderPrintf(renderer, "??");
else renderPrintf(renderer, "??");

pos.x++;
coordPrintf(renderer, pos, "????????:%s", csp->addInfo), pos.x++;
*/
	for (int i = 0; i < 6; i++)
	{
		if (i % 2) drawColorBar(renderer, cur, 238, 232, 213, CSPDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i], 2);
		cur.x++;
	}cur.x++;

}


void showRecordDetails(Renderer* renderer, Coord pos, Record* rec) {
	struct tm time;
	Coord cur = pos;
	CellDataDrawer packDrawer = NULL;
	void* qw = NULL;
	if (rec->prod.pack == BULK) {
		packDrawer = drawCellBULK;
		qw = &rec->prod.weight;
	}
	else if (rec->prod.pack == UNIT) {
		packDrawer = drawCellUNIT;
		qw = &rec->prod.quantity;
	}
	int valWidth = ProdDetailsRectSize.y - 13;
	CellData cellName[8][2] =
	{ {{drawCellStr,12,0,"????ID"},{drawCellInt,12,0,&rec->recID}},
	  {{drawCellStr,12,0,"????ID"},{drawCellInt,12,0,&rec->invID}},
	  {{drawCellStr,12,0,"????"},{drawCellStr,12,0,&recordType[rec->type]}},
	  {{drawCellStr,12,0,"????????"},{drawCellTime,12,0,&rec->time}},
	  {{drawCellStr,12,0,&typeQuantityText[rec->type]},{packDrawer,12,0,qw,rec->prod.unitName}},
	  {{drawCellStr,12,0,&typeUpriceText[rec->type]},{drawCellDouble,12,0,&rec->prod.unitPrice}},
	  {{drawCellStr,12,0,&typeAmountText[rec->type]},{drawCellDouble,12,0,&rec->prod.amount}},
	  {{drawCellStr,12,0,"????????"},{drawCellStr,valWidth,0,&rec->addInfo}}
	};
	for (int i = 0, line = 0; i < 8; i++)
	{
		if (i == 4) {
			showProductDetails(renderer, cur, &rec->prod, line);
			line += ProdDetailsRectSize.x;
			cur.x += ProdDetailsRectSize.x;
		}
		if (line % 2) drawColorBar(renderer, cur, 238, 232, 213, InvDetailsRectSize.y);
		else resetBackgroundColor(renderer);
		drawListItem(renderer, cur, cellName[i], 2);
		cur.x++, line++;
	}
}
