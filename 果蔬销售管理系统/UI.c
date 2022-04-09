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



void setTextColor(Renderer* renderer,int r, int g, int b) {
	renderPrintf(renderer,"\033[38;2;%d;%d;%dm", r, g, b);
}
void resetTextColor(Renderer* renderer) {
	renderPrintf(renderer, "\033[39m");
}

void drawRectBorder(Renderer* renderer,Coord pos, Coord const size) {
	renderPrintf(renderer,ESC "(0");
	renderPrintf(renderer, CSI "49;93m");
	setCursorPos(renderer, pos);
	coordPrintf(renderer, pos,"l");
	for (int i = 1; i <= size.y; i++) {
		renderPrintf(renderer, "q");
	}
	renderPrintf(renderer,"k");
	pos.x++;
	for (int i = 1; i <= size.x; i++) {
		coordPrintf(renderer, pos, "x");
		pos.y += size.y+1;
		coordPrintf(renderer, pos, "x");
		pos.y -= size.y+1;
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
void drawMenu(Renderer *renderer, Coord origin,const char* title, int n, int firstNum, ...) {
	va_list args;
	va_start(args, firstNum);
	Coord rectSize;
	rectSize.x = n+1;
	rectSize.y = 62;
	drawRectBorder(renderer, origin, rectSize);
	origin.x++;
	origin.y++;
	coordPrintf(renderer, origin,"%s", title);
	origin.x++;
	for (int i = 0; i < n; i++) {
		char* item = va_arg(args, char*);
		coordPrintf(renderer,origin,"\t%d.%s", firstNum + i, item);
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
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y+rectSize.y / 2 - strlen(title) / 2 }, "%s",title);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y+rectSize.y - 7 }, "%d/%d页", ((pageStartNum - 1) / pageSize) + 1, ((start->list.root->size - 1) / pageSize) + 1);
	origin.x += 3;
	origin.y++;
	drawInvList(renderer, origin, start, pageSize);
}
void drawInvList(Renderer *renderer, Coord origin,Inventory* start,int pageSize) {
	coordPrintf(renderer, origin,"种类\t\t\t品种\t\t销售单价\tID\t    数量");
	origin.x++;
	int n = 0;
	char kind[INFOMAX], var[INFOMAX];

	listForEachEntry(Inventory, pos, &(start->list), list) {
		if (n == pageSize) break;
		strMakeLen(kind, pos->prod.kind, 14);
		strMakeLen(var, pos->prod.variety, 12);
		if (pos->prod.pack == BULK) {
			coordPrintf(renderer, origin, "%s\t%s\t%8.2lf\t%d\t%8.2lf", kind, var, pos->prod.unitPrice, pos->invID, pos->prod.weight);
		}
		else if (pos->prod.pack == UNIT) {
			coordPrintf(renderer, origin, "%s\t%s\t%8.2lf\t%d\t%8d", kind, var, pos->prod.unitPrice, pos->invID, pos->prod.quantity);
		}
		origin.x++;
		n++;
	}
	while (n++ != pageSize) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(renderer,origin);
}
void drawCart(Renderer * renderer, Coord origin, Inventory* start, int height) {
	Coord rectSize;
	rectSize.x = height + 1;
	rectSize.y = 78;
	drawRectBorder(renderer, origin, rectSize);
	origin.x++;
	origin.y++;
	coordPrintf(renderer, origin, "种类\t\t品种\t\t销售单价\tID\t\t购买量\t总%d", start->list.root->size);
	origin.x++;
	int n = 0;
	char kind[INFOMAX], var[INFOMAX];

	listForEachEntry(Inventory, pos, &(start->list), list) {
		if (n == height) break;
		strMakeLen(kind, pos->prod.kind, 14);
		strMakeLen(var, pos->prod.variety, 12);
		if (pos->prod.pack == BULK) {
			coordPrintf(renderer, origin, "%s\t%s\t%8.2lf\t%d\t%8.2lf", kind, var, pos->prod.unitPrice, pos->invID, pos->prod.weight);
		}
		else if (pos->prod.pack == UNIT) {
			coordPrintf(renderer, origin, "%s\t%s\t%8.2lf\t%d\t%8d", kind, var, pos->prod.unitPrice, pos->invID,pos->prod.quantity);
		}
		origin.x++;
		n++;
	}
	while (n++ != height) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(renderer,origin);
}


void drawRecordList(Renderer *renderer, Coord origin,Record* start,int type, int height) {
	int n = 0;
	coordPrintf(renderer, origin,"类型\t\t记录ID\t商品ID\t种类\t\t品种\t\t商品进出\t资金进出\t时间\t总%d",start->timeList.root->size);
	origin.x++;
	char kind[INFOMAX], var[INFOMAX];
	struct tm date;
	if (type == TIME_RECORDS) {
		listForEachEntry(Record, pos, &(start->timeList), timeList)
		{
			if (n == height) break;
			strMakeLen(kind, pos->prod.kind, 14);
			strMakeLen(var, pos->prod.variety, 12);
			localtime_s(&date, &pos->time);
			coordPrintf(renderer, origin, "%s\t", recordType[pos->type]);

			renderPrintf(renderer, "%d\t%d\t", pos->recID, pos->invID);
			renderPrintf(renderer, "%s\t%s\t", kind, var);
			if (pos->prod.pack == BULK) {
				renderPrintf(renderer, "%+.2lf斤\t", pos->prod.weight * typeDirect[pos->type]);
			}
			else if (pos->prod.pack == UNIT) {
				renderPrintf(renderer, "%+6d个\t", pos->prod.quantity * typeDirect[pos->type]);
			}
			renderPrintf(renderer, "%+8.4g\t", pos->prod.amount * (-typeDirect[pos->type]));
			renderPrintf(renderer, "%d/%d/%d %02d:%02d:%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
			origin.x++;
			n++;
		}
	}
	else if (type == INV_RECORDS) {
		listForEachEntry(Record, pos, &(start->IRList), IRList)
		{
			if (n == height) break;
			strMakeLen(kind, pos->prod.kind, 14);
			strMakeLen(var, pos->prod.variety, 12);
			localtime_s(&date, &pos->time);
			coordPrintf(renderer, origin, "%s\t", recordType[pos->type]);

			renderPrintf(renderer, "%d\t%d\t", pos->recID, pos->invID);
			renderPrintf(renderer, "%s\t%s\t", kind, var);
			if (pos->prod.pack == BULK) {
				renderPrintf(renderer, "%+.2lf斤\t", pos->prod.weight * typeDirect[pos->type]);
			}
			else if (pos->prod.pack == UNIT) {
				renderPrintf(renderer, "%+6d个\t", pos->prod.quantity * typeDirect[pos->type]);
			}
			renderPrintf(renderer, "%+8.4g\t", pos->prod.amount * (-typeDirect[pos->type]));
			renderPrintf(renderer, "%d/%d/%d %02d:%02d:%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
			origin.x++;
			n++;
		}
	}
	while (n++ != height) origin.x++;
	setCursorPos(renderer,origin);
}

void drawListPage(Renderer* renderer, Coord origin, const char* title, ListDrawer drawer,ListHead* start,int pageStartNum, int pageSize,Coord rectSize) {
	Coord titleRect;
	titleRect.x = 1;
	titleRect.y = rectSize.y;
	drawRectBorder(renderer, origin, titleRect);
	if (pageSize + 3 > rectSize.x) rectSize.x = pageSize + 3;
	drawRectBorder(renderer, origin, rectSize);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y / 2 - strlen(title) / 2 }, "%s", title);
	coordPrintf(renderer, (Coord) { origin.x + 1, origin.y + rectSize.y - 7 }, "%d/%d页", ((pageStartNum - 1) / pageSize) + 1, ((start->root->size - 1) / pageSize) + 1);
	origin.x += 3;
	origin.y++;
	drawer(renderer, origin, start, pageSize);
}
void drawSSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize) {
	SSP* start = listEntry(entry, SSP, list);
	coordPrintf(renderer, origin, "方案名\t\t方案ID");
	origin.x++;
	int n = 0;
	char name[INFOMAX];

	listForEachEntry(SSP, pos, &(start->list), list) {
		if (n == pageSize) break;
		strMakeLen(name, pos->planName, 20);
		coordPrintf(renderer, origin, "%s\t%d", name,pos->SSPID);
		origin.x++;
		n++;
	}
	while (n++ != pageSize) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(renderer, origin);

}
void drawCSPList(Renderer* renderer, Coord origin, ListHead* entry, int pageSize) {
	CSP* start = listEntry(entry, CSP, list);
	coordPrintf(renderer, origin, "方案名\t\t方案ID");
	origin.x++;
	int n = 0;
	char name[INFOMAX];

	listForEachEntry(CSP, pos, &(start->list), list) {
		if (n == pageSize) break;
		strMakeLen(name, pos->planName, 20);
		coordPrintf(renderer, origin, "%s\t%d",name, pos->CSPID);
		origin.x++;
		n++;
	}
	while (n++ != pageSize) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(renderer, origin);
}
void inputStart(Renderer* renderer, Coord inputOrigin) {
	setCursorPos(renderer, inputOrigin);
	renderPrintf(renderer, ESC "(0");
	for (int i = 0; i <= PanelSize.y; i++) {
		renderPrintf(renderer, "q");
	}
	Coord bottom = inputOrigin;
	bottom.x += 10;
	setCursorPos(renderer, bottom);
	for (int i = 0; i <= PanelSize.y; i++) {
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
int readline(char* buf, int maxCount, FILE* stream) { //从流读取一行存在buf中，最大读入maxCount个字符（包括空字符），丢弃换行符和多余字符
	int i;
	char t;
	for (i = 0; (t = fgetc(stream)) != '\n'; i++) {
		if (i < maxCount - 1) buf[i] = t;
		else if (i == maxCount - 1) buf[i] = '\0';
	}
	if (i < maxCount) buf[i] = '\0';
	if (i > maxCount) return READ_TOO_LONG;
	return READ_SUCCESS;
}

int getSelect() {
	int num;
	char input[INFOMAX];
	printf("选择一项以继续: ");
	readline(input, INFOMAX, stdin);
	if (sscanf_s(input, "%d", &num) == 1) return num;
	return -1;
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
	int num = 0;
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		readline(input, INFOMAX, stdin);
		if (strcmp(input, "quit") == 0) return INPUT_BREAK;
		if (input[0] == '\0') {
			if (!strict) return INPUT_NOCHANGE;
			else continue;
		}
		if (sscanf_s(input, "%d", &num) == 1) {
			if (range.max < range.min || (num >= range.min && num <= range.max)) {
				*value = num;
				return INPUT_SUCCESS;
			}
			else {
				printf("应输入[%d,%d]范围内的整数\n", range.min, range.max);
			}
		}
	}
}
int getStrInput(const char* query, char* value, int maxCount, bool strict) {
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		if (readline(input, maxCount, stdin) == READ_TOO_LONG) {
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
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		readline(input, INFOMAX, stdin);
		if (strcmp(input, "quit") == 0) return INPUT_BREAK;
		if (input[0] == '\0') {
			if (!strict) return INPUT_NOCHANGE;
			else continue;
		}
		if (sscanf_s(input, "%lf", &num) == 1) {
			if (range.max < range.min || (num >= range.min && num <= range.max)) {
				*value = num;
				return INPUT_SUCCESS;
			}
			else {
				printf("应输入[%lf,%lf]范围内的实数\n", range.min, range.max);
			}
		}
	}
}
int getDateTime(const char* query, time_t* value, bool strict) {
	struct tm ti, ti_verify;
	time_t time;
	memset(&ti, 0, sizeof(ti));
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		readline(input, INFOMAX, stdin);
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
			if (memcmp(&ti, &ti_verify, sizeof(ti)) != 0) {
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
int inputRecordID(const Record* head, int type,int* id, Record** pRec) {
	*pRec = NULL;
	while (1) {
		breakDeliver(getUIntInput("请输入记录ID:", id, ALLINT, true));
		if (*pRec = recordQueryID(head,*id,type)) return;
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
	breakDeliver(getUIntInput("选择品质:", &prod->quality, (IntRange){ 1,3 }, true));
	drawOrdMenu("包装方式:", 2, 1, "散装", "单元装");
	breakDeliver(getUIntInput("选择包装方式:", &prod->pack, (IntRange) { 1,2 }, true));
	if (prod->pack == BULK) {
		breakDeliver(getDoubleInput("输入重量:", &prod->weight, WRANGE, true));
	}
	else if (prod->pack = UNIT) {
		breakDeliver(getUIntInput("输入数量:", &prod->quantity, QRANGE, true));
	}
	breakDeliver(getDoubleInput("输入进货单价:", &prod->purUPrice, UPRINCERANGE, true));
	breakDeliver(getDoubleInput("设定销售单价:", &prod->unitPrice, UPRINCERANGE, true));
	//if (prod->pack == BULK) prod->amount = prod->unitPrice * prod->weight;  
	//else if (prod->pack == UNIT) prod->amount = prod->unitPrice * prod->quantity;
	return INPUT_SUCCESS;
}
int inputGift(Gift* gift,Inventory* invHead) {
	memset(gift, 0, sizeof(gift));
	Inventory* inv=NULL;
	breakDeliver(inputInventoryID(invHead, &gift->invID, &inv));
	if (inv->prod.pack == BULK) {
		breakDeliver(getDoubleInput("输入赠送重量:",&gift->weight,(DoubleRange){0,inv->prod.weight},true));
	}
	else if (inv->prod.pack == UNIT) {
		breakDeliver(getUIntInput("输入赠送数量:", &gift->quantity, (IntRange) { 0, inv->prod.quantity }, true));
	}
	breakDeliver(GetDoubleClickTime("输入附加金额(可选，作为余额赠品)", &gift->addPrice,UPRINCERANGE, false));
	return READ_SUCCESS;
}
int inputProductFilter(Product* prod) {
	memset(prod, 0, sizeof(Product));
	prod->expiration = TIME_NAN;
	breakDeliver(getStrInput("输入种类(默认不限):", prod->kind, INFOMAX, false));
	breakDeliver(getStrInput("输入品种(默认不限):", prod->variety, INFOMAX, false));
	breakDeliver(getDateTime("在此日期及之前过期(年.月.日)(默认不限) : ", &prod->expiration, false));
	breakDeliver(getUIntInput("输入品质(默认不限):", &prod->quality, (IntRange) { 1,3 }, false));
	drawOrdMenu("包装方式:", 3, 0, "不限", "散装", "单元装");
	breakDeliver(getUIntInput("选择包装方式(默认不限):", &prod->pack, (IntRange) { 0,2 }, false));
	return INPUT_SUCCESS;
}

int inputRecordFilter(Record* rec) {
	memset(rec, 0, sizeof(Record));
	rec->time = rec->lastTime = TIME_NAN;
	drawOrdMenu("记录类型:", 5, 0, "不限", "进货", "销售记录", "更新记录", "赠予记录");
	breakDeliver(getUIntInput("记录类型(默认不限) : ", &rec->type, (IntRange){ 0,4 }, false));
	breakDeliver(inputProductFilter(&rec->prod));
	breakDeliver(getDateTime("筛选的最早日期(年.月.日)(默认不限) : ", &rec->time, false));
	breakDeliver(getDateTime("筛选的最晚日期(年.月.日)(默认不限) : ", &rec->lastTime, false));
	rec->lastTime += 3600 * 24;
	return INPUT_SUCCESS;
}



void showProductDetails(Renderer* renderer,Coord pos,Product* prod) {  //Only Show product attributes
	struct tm date;
	coordPrintf(renderer, pos, "种类:%s", prod->kind, prod->variety); pos.x++;
	coordPrintf(renderer, pos, "品种:%s", prod->variety); pos.x++;
	coordPrintf(renderer, pos, "品质:%s", quanlityText[prod->quality]); pos.x++;
	localtime_s(&date, &prod->expiration);
	coordPrintf(renderer, pos, "过期日期:%d.%d.%d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday); pos.x++;
	if (prod->pack == BULK) {
		coordPrintf(renderer, pos, "包装方式:散装");
	}
	else if (prod->pack == UNIT) {
		coordPrintf(renderer, pos, "包装方式:单元装");
	}pos.x++;
}

void showInvDetails(Renderer* renderer,Coord pos,Inventory* inv) {
	coordPrintf(renderer, pos, "商品ID:%d\n", inv->invID); pos.x++;
	showProductDetails(renderer, pos, &inv->prod); pos.x +=5;
	if (inv->prod.pack == BULK) {
		coordPrintf(renderer, pos, "库存重量:%.2lf\n", inv->prod.weight);
	}
	else if (inv->prod.pack == UNIT) {
		coordPrintf(renderer, pos, "库存数量:%d\n", inv->prod.quantity);
	}
	pos.x++;
	coordPrintf(renderer, pos, "销售单价:%.2lf\n", inv->prod.unitPrice);
}

char typeQuantityText[5][20] = { "","采购量","销售量","变化量","赠予量" };
char typeAmountText[5][20] = { "","采购总额","销售总额","总价变化","赠品成本" };
void showRecordDetails(Renderer* renderer,Coord pos,Record* rec) {
	struct tm time;
	localtime_s(&time, &rec->time);
	coordPrintf(renderer, pos, "记录ID:%d", rec->recID); pos.x++;
	coordPrintf(renderer, pos, "商品ID:%d", rec->invID); pos.x++;
	coordPrintf(renderer, pos, "类型:%s", recordType[rec->type]); pos.x++;
	coordPrintf(renderer, pos, "记录时间:%d/%d/%d %02d:%02d:%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_hour); pos.x++;
	showProductDetails(renderer,pos,&rec->prod);
	pos.x += 5;
	if (rec->prod.pack == BULK) {
		coordPrintf(renderer, pos, "%s: %.2lf斤\n", typeQuantityText[rec->type], rec->prod.weight); pos.x++;
	}
	else if (rec->prod.pack == UNIT) {
		coordPrintf(renderer, pos, "%s: %d个\n", typeQuantityText[rec->type], rec->prod.quantity); pos.x++;
	}
	switch (rec->type) {
	case PURCHASE:
		coordPrintf(renderer,pos,"采购单价：%.2lf\n", rec->prod.purUPrice);
		break;
	case SALE:
		coordPrintf(renderer, pos, "销售单价：%.2lf\n", rec->prod.unitPrice);
		break;
	case UPDATE:
		coordPrintf(renderer, pos, "售价变化: %.2lf\n", rec->prod.unitPrice);
		break;
	case GIFT:
		coordPrintf(renderer, pos, "采购单价：%.2lf\n", rec->prod.purUPrice);
		break;
	}
	pos.x++;
	coordPrintf(renderer, pos, "%s: %.2lf\n", typeAmountText[rec->type], rec->prod.amount); pos.x++;
	coordPrintf(renderer, pos, "附加信息:%s\n", rec->addInfo);
}
