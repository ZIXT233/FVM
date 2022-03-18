#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>
#include"UI.h"


static char recordType[5][20] = { "","进货记录","销售记录","更新记录","赠送记录" };
int typeDirect[5] = { 0,1,-1,1,-1 };

char quanlityText[5][20] = { "","优","良","差" };

void cls() {
	system("cls");
}
void drawMenu(const char* title, int n, ...) {
	va_list args;
	va_start(args, n);
	printf("%s\n", title);
	for (int i = 1; i <= n; i++) {
		char* item = va_arg(args, char*);
		printf("\t%d.%s\n", i, item);
	}
	return 0;
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
void drawInvList(Inventory* start, int len) {
	printf("种类\t\t品种\t\t价格\t\tID\n");
	int n = 0;
	char kind[INFOMAX], var[INFOMAX];

	listForEachEntry(Inventory, pos, &(start->list), list) {
		if (n == len) break;
		strMakeLen(kind, pos->prod.kind, 14);
		strMakeLen(var, pos->prod.variety, 12);
		printf("%s\t%s\t%08.2lf\t%d\n", kind, var, pos->prod.unitPrice, pos->invID);
		n++;
	}
	while (n++ != len) putchar('\n');
}


void drawRecordList(Record* start, int len) {
	int n = 0;
	printf("类型\t\t记录ID\t商品ID\t种类\t\t品种\t\t商品进出\t资金进出\t时间\n");
	char kind[INFOMAX], var[INFOMAX];
	struct tm date;

	listForEachEntry(Record, pos, &(start->timeList), timeList) {
		if (n == len) break;
		strMakeLen(kind, pos->prod.kind, 14);
		strMakeLen(var, pos->prod.variety, 12);
		localtime_s(&date, &pos->time);
		printf("%s\t", recordType[pos->type]);

		printf("%d\t%d\t", pos->recID, pos->invID);
		printf("%s\t%s\t", kind, var);
		if (pos->prod.pack == BULK) {
			printf("%+.2lf斤\t", pos->prod.weight * typeDirect[pos->type]);
		}
		else if (pos->prod.pack == UNIT) {
			printf("%+d个\t", pos->prod.quantity * typeDirect[pos->type]);
		}
		printf("%+.2lf\t", pos->prod.amount * typeDirect[pos->type]);
		printf("%d/%d/%d %d:%d:%d\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
		n++;
	}
	while (n++ != len) putchar('\n');
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

int getUIntInput(const char* query, int* value, bool strict) {
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
		if (sscanf_s(input, "%d", &num) == 1 && num >= 0) {
			*value = num;
			return INPUT_SUCCESS;
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

int getDoubleInput(const char* query, double* value, bool strict) {
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
			*value = num;
			return INPUT_SUCCESS;
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

int inputProduct(Product* prod) {
	breakDeliver(getStrInput("输入种类:", prod->kind, INFOMAX, true));
	breakDeliver(getStrInput("输入品种:", prod->variety, INFOMAX, true));
	breakDeliver(getDateTime("输入过期时间(年.月.日):", &prod->expiration, true));
	drawMenu("品质:", 3, "优", "良", "差");
	prod->quality = 0;
	while (!(prod->quality >= 1 && prod->quality <= 3)) breakDeliver(getUIntInput("选择品质:", &prod->quality, true));
	drawMenu("包装方式:", 2, "散装", "单元装");
	prod->pack = 0;
	while (!(prod->pack == 1 || prod->pack == 2)) breakDeliver(getUIntInput("选择包装方式:", &prod->pack, true));
	if (prod->pack == BULK) {
		breakDeliver(getUIntInput("输入重量:", &prod->weight, true));
	}
	else if (prod->pack = UNIT) {
		breakDeliver(getUIntInput("输入数量:", &prod->quantity, true));
	}
	breakDeliver(getDoubleInput("输入单价:", &prod->unitPrice, true));
	if (prod->pack == BULK) prod->amount = prod->unitPrice * prod->weight;
	else if (prod->pack == UNIT) prod->amount = prod->unitPrice * prod->quantity;
	return INPUT_SUCCESS;
}
int inputProductFilter(Product* prod) {
	memset(prod, 0, sizeof(Product));
	prod->expiration = TIME_NAN;
	breakDeliver(getStrInput("输入种类(默认不限):", prod->kind, INFOMAX, false));
	breakDeliver(getStrInput("输入品种(默认不限):", prod->variety, INFOMAX, false));
	breakDeliver(getDateTime("在此日期及之前过期(年.月.日)(默认不限) : ", &prod->expiration, false));
	breakDeliver(getUIntInput("输入品质(默认不限):", &prod->quality, false));
	drawMenu("包装方式:", 3, "不限", "散装", "单元装");
	prod->pack = 1;
	do {
		breakDeliver(getUIntInput("选择包装方式(默认不限):", &prod->pack, false))
	} while (!(prod->pack >= 1 && prod->pack <= 3));
		prod->pack--;
		return INPUT_SUCCESS;
}

int inputRecordFilter(Record* rec) {
	memset(rec, 0, sizeof(Record));
	rec->type = 1;
	do {
		drawMenu("记录类型:", 5, "不限", "进货", "销售记录", "更新记录", "赠予记录");
		breakDeliver(getDateTime("记录类型(默认不限) : ", &rec->type, false));
	} while (!(rec->type >= 1 && rec->type <= 5));
	rec->type--;
	breakDeliver(inputProductFilter(&rec->prod));
	rec->time = rec->lastTime = TIME_NAN;
	breakDeliver(getDateTime("筛选的最早日期(年.月.日)(默认不限) : ", &rec->time, false));
	breakDeliver(getDateTime("筛选的最晚日期(年.月.日)(默认不限) : ", &rec->lastTime, false));
	rec->lastTime += 3600 * 24;
	return INPUT_SUCCESS;
}



void showProductDetails(Product* prod) {  //Only Show product attributes
	struct tm date;
	printf("种类:%s\n品种:%s\n", prod->kind, prod->variety);
	printf("品质:%s\n", quanlityText[prod->quality]);
	localtime_s(&date, &prod->expiration);
	printf("过期日期:%d.%d.%d\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
	if (prod->pack == BULK) {
		printf("包装方式:散装\n");
	}
	else if (prod->pack == UNIT) {
		printf("包装方式:单元装\n");
	}
}

void showInvDetails(Inventory* inv) {
	printf("商品ID:%d\n", inv->invID);
	showProductDetails(&inv->prod);
	if (inv->prod.pack == BULK) {
		printf("库存重量:%.2lf\n", inv->prod.weight);
	}
	else if (inv->prod.pack == UNIT) {
		printf("库存数量:%d\n", inv->prod.quantity);
	}
}

char typeQuantityText[5][20] = { "","采购量","销售量","变化量","赠予量" };
char typeAmountText[5][20] = { "","采购总额","销售总额","总价变化","赠品成本" };
void showRecordDetails(Record* rec) {
	struct tm time;
	localtime_s(&time, &rec->time);
	printf("记录ID:%d\n商品ID:%d\n", rec->recID, rec->invID);
	printf("类型:%s\n", recordType[rec->type]);
	printf("记录时间:%d/%d/%d %d:%d%d\n", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_hour);
	showProductDetails(&rec->prod);
	if (rec->prod.pack == BULK) {
		printf("%s: %.2lf斤\n", typeQuantityText[rec->type], rec->prod.weight);
	}
	else if (rec->prod.pack == UNIT) {
		printf("%s: %d个\n", typeQuantityText[rec->type], rec->prod.quantity);
	}
	printf("%s: %.2lf\n", typeAmountText[rec->type], rec->prod.amount);
	printf("附加信息:%s\n", rec->addInfo);
}
