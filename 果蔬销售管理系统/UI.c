#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>
#include"UI.h"

#define ESC "\033"
#define CSI "\033["

static char recordType[5][20] = { "","������¼","���ۼ�¼","���¼�¼","���ͼ�¼" };
int typeDirect[5] = { 0,1,-1,1,-1 };

char quanlityText[5][20] = { "","��","��","��" };

void setCursorPos(Coord pos) {
	//printf("\033[%d;%dH", pos.x, pos.y);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsole, (COORD) { pos.y - 1, pos.x - 1 });
	
}
void drawRectBorder(Coord pos, Coord const size);
void cls()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	SMALL_RECT scrollRect;
	COORD scrollTarget;
	CHAR_INFO fill;
	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}
	// Scroll the rectangle of the entire buffer.
	scrollRect.Left = 0;
	scrollRect.Top = 0;
	scrollRect.Right = csbi.dwMaximumWindowSize.X;
	scrollRect.Bottom = csbi.dwMaximumWindowSize.Y;

	// Scroll it upwards off the top of the buffer with a magnitude of the entire height.
	scrollTarget.X = 0;
	scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

	// Fill with empty spaces with the buffer's default text attribute.
	fill.Char.UnicodeChar = TEXT(' ');
	fill.Attributes = csbi.wAttributes;
	// Do the scroll
	ScrollConsoleScreenBuffer(hConsole, &scrollRect, NULL, scrollTarget, &fill);

	// Move the cursor to the top left corner too.

	setCursorPos(UI_ORIGIN);
}
void inputStart(Coord inputOrigin) {
	setCursorPos(inputOrigin);
	printf(ESC "(0");
	for (int i = 0; i <= PanelSize.y; i++) {
		printf("q"); 
	}
	Coord bottom = inputOrigin;
	bottom.x += 10;
	setCursorPos(bottom);
	for (int i = 0; i <= PanelSize.y; i++) {
		printf("q");
	}
	printf(ESC "(B");
	printf(CSI "%d;%dr", inputOrigin.x + 1, bottom.x-1);
	printf(CSI "%d;%dH", inputOrigin.x + 1, inputOrigin.y);
	inputOrigin.x++;
	setCursorPos(inputOrigin);
}
Coord getCursorPos() {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
	Coord pos;
	pos.x = csbiInfo.dwCursorPosition.Y+1;
	pos.y = csbiInfo.dwCursorPosition.X+1;
	return pos;
}


void setTextColor(int r, int g, int b) {
	printf("\033[38;2;%d;%d;%dm", r, g, b);
}
void resetTextColor() {
	printf("\033[39m");
}


int coordPrintf(Coord pos, const char* const _Format, ...) {
	va_list aptr;
	int ret;
	va_start(aptr, _Format);
	setCursorPos(pos);
	ret = vprintf(_Format, aptr);
	va_end(aptr);
	return ret;
}
void drawRectBorder(Coord pos, Coord const size) {
	printf(ESC "(0");
	printf(CSI "49;93m");
	setCursorPos(pos);
	coordPrintf(pos,"l");
	for (int i = 1; i <= size.y; i++) {
		printf("q");
	}
	printf("k");
	pos.x++;
	for (int i = 1; i <= size.x; i++) {
		coordPrintf(pos, "x");
		pos.y += size.y+1;
		coordPrintf(pos, "x");
		pos.y -= size.y+1;
		pos.x++;
	}
	coordPrintf(pos, "m");
	for (int i = 1; i <= size.y; i++) {
		printf("q");
	}
	printf("j");
	printf(CSI "0m");
	printf(ESC "(B");
	pos.x++;
	setCursorPos(pos);
}
void drawMenu(Coord origin,const char* title, int n, int firstNum, ...) {
	va_list args;
	va_start(args, firstNum);
	Coord rectSize;
	rectSize.x = n+1;
	rectSize.y = 62;
	drawRectBorder(origin, rectSize);
	origin.x++;
	origin.y++;
	coordPrintf(origin,"%s", title);
	origin.x++;
	for (int i = 0; i < n; i++) {
		char* item = va_arg(args, char*);
		coordPrintf(origin,"\t%d.%s", firstNum + i, item);
		origin.x++;
	}
	origin.y--;
	origin.x++;
	setCursorPos(origin);
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

void strMakeLen(char* dest, const char* src, int len) {
	int slen = strlen(src);
	for (int i = 0; i < len - 1; i++) {
		if (i >= slen) dest[i] = ' ';
		else if (i >= len - 4) dest[i] = '.';
		else dest[i] = src[i];
	}
	dest[len - 1] = '\0';
}
void drawInvPage(Coord origin, const char* title, Inventory* start, int pageStartNum, int pageSize) {
	Coord rectSize;
	rectSize.x = 1;
	rectSize.y = 75;
	drawRectBorder(origin, rectSize);
	rectSize.x = pageSize + 3;
	drawRectBorder(origin, rectSize);
	coordPrintf((Coord) { origin.x + 1, origin.y+rectSize.y / 2 - strlen(title) / 2 }, "%s",title);
	coordPrintf((Coord) { origin.x + 1, origin.y+rectSize.y - 7 }, "%d/%dҳ", ((pageStartNum - 1) / pageSize) + 1, ((start->list.root->size - 1) / pageSize) + 1);
	origin.x += 3;
	origin.y++;
	drawInvList(origin, start, pageSize);
}
void drawInvList(Coord origin,Inventory* start,int pageSize) {
	coordPrintf(origin,"����\t\t\tƷ��\t\t���۵���\tID\t    ����");
	origin.x++;
	int n = 0;
	char kind[INFOMAX], var[INFOMAX];

	listForEachEntry(Inventory, pos, &(start->list), list) {
		if (n == pageSize) break;
		strMakeLen(kind, pos->prod.kind, 14);
		strMakeLen(var, pos->prod.variety, 12);
		if (pos->prod.pack == BULK) {
			coordPrintf(origin, "%s\t%s\t%8.2lf\t%d\t%8.2lf", kind, var, pos->prod.unitPrice, pos->invID, pos->prod.weight);
		}
		else if (pos->prod.pack == UNIT) {
			coordPrintf(origin, "%s\t%s\t%8.2lf\t%d\t%8d", kind, var, pos->prod.unitPrice, pos->invID, pos->prod.quantity);
		}
		origin.x++;
		n++;
	}
	while (n++ != pageSize) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(origin);
}
void drawCart(Coord origin, Inventory* start, int height) {
	Coord rectSize;
	rectSize.x = height + 1;
	rectSize.y = 78;
	drawRectBorder(origin, rectSize);
	origin.x++;
	origin.y++;
	coordPrintf(origin, "����\t\tƷ��\t\t���۵���\tID\t\t������\t��%d", start->list.root->size);
	origin.x++;
	int n = 0;
	char kind[INFOMAX], var[INFOMAX];

	listForEachEntry(Inventory, pos, &(start->list), list) {
		if (n == height) break;
		strMakeLen(kind, pos->prod.kind, 14);
		strMakeLen(var, pos->prod.variety, 12);
		if (pos->prod.pack == BULK) {
			coordPrintf(origin, "%s\t%s\t%8.2lf\t%d\t%8.2lf", kind, var, pos->prod.unitPrice, pos->invID, pos->prod.weight);
		}
		else if (pos->prod.pack == UNIT) {
			coordPrintf(origin, "%s\t%s\t%8.2lf\t%d\t%8d", kind, var, pos->prod.unitPrice, pos->invID,pos->prod.quantity);
		}
		origin.x++;
		n++;
	}
	while (n++ != height) origin.x++;
	origin.y--;
	origin.x++;
	setCursorPos(origin);
}


void drawRecordList(Coord origin,Record* start,int type, int height) {
	int n = 0;
	coordPrintf(origin,"����\t\t��¼ID\t��ƷID\t����\t\tƷ��\t\t��Ʒ����\t�ʽ����\tʱ��\t��%d",start->timeList.root->size);
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
			coordPrintf(origin, "%s\t", recordType[pos->type]);

			printf("%d\t%d\t", pos->recID, pos->invID);
			printf("%s\t%s\t", kind, var);
			if (pos->prod.pack == BULK) {
				printf("%+.2lf��\t", pos->prod.weight * typeDirect[pos->type]);
			}
			else if (pos->prod.pack == UNIT) {
				printf("%+6d��\t", pos->prod.quantity * typeDirect[pos->type]);
			}
			printf("%+8.4g\t", pos->prod.amount * (-typeDirect[pos->type]));
			printf("%d/%d/%d %02d:%02d:%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
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
			coordPrintf(origin, "%s\t", recordType[pos->type]);

			printf("%d\t%d\t", pos->recID, pos->invID);
			printf("%s\t%s\t", kind, var);
			if (pos->prod.pack == BULK) {
				printf("%+.2lf��\t", pos->prod.weight * typeDirect[pos->type]);
			}
			else if (pos->prod.pack == UNIT) {
				printf("%+6d��\t", pos->prod.quantity * typeDirect[pos->type]);
			}
			printf("%+8.4g\t", pos->prod.amount * (-typeDirect[pos->type]));
			printf("%d/%d/%d %02d:%02d:%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
			origin.x++;
			n++;
		}
	}
	while (n++ != height) origin.x++;
	setCursorPos(origin);
}

#define READ_TOO_LONG -1
#define READ_SUCCESS 0
int readline(char* buf, int maxCount, FILE* stream) { //������ȡһ�д���buf�У�������maxCount���ַ����������ַ������������з��Ͷ����ַ�
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
	printf("ѡ��һ���Լ���: ");
	readline(input, INFOMAX, stdin);
	if (sscanf_s(input, "%d", &num) == 1) return num;
	return -1;
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
				printf("Ӧ����[%d,%d]��Χ�ڵ�����\n", range.min, range.max);
			}
		}
	}
}
int getStrInput(const char* query, char* value, int maxCount, bool strict) {
	char input[INFOMAX];
	while (1) {
		printf("%s", query);
		if (readline(input, maxCount, stdin) == READ_TOO_LONG) {
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
				printf("Ӧ����[%lf,%lf]��Χ�ڵ�ʵ��\n", range.min, range.max);
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

int inputProduct(Product* prod) {
	breakDeliver(getStrInput("��������:", prod->kind, INFOMAX, true));
	breakDeliver(getStrInput("����Ʒ��:", prod->variety, INFOMAX, true));
	breakDeliver(getDateTime("�������ʱ��(��.��.��):", &prod->expiration, true));
	drawMenu(getCursorPos(),"Ʒ��:", 3, 1, "��", "��", "��");
	breakDeliver(getUIntInput("ѡ��Ʒ��:", &prod->quality, (IntRange){ 1,3 }, true));
	drawMenu(getCursorPos(),"��װ��ʽ:", 2, 1, "ɢװ", "��Ԫװ");
	breakDeliver(getUIntInput("ѡ���װ��ʽ:", &prod->pack, (IntRange) { 1,2 }, true));
	if (prod->pack == BULK) {
		breakDeliver(getDoubleInput("��������:", &prod->weight, WRANGE, true));
	}
	else if (prod->pack = UNIT) {
		breakDeliver(getUIntInput("��������:", &prod->quantity, QRANGE, true));
	}
	breakDeliver(getDoubleInput("�����������:", &prod->purUPrice, UPRINCERANGE, true));
	breakDeliver(getDoubleInput("�趨���۵���:", &prod->unitPrice, UPRINCERANGE, true));
	//if (prod->pack == BULK) prod->amount = prod->unitPrice * prod->weight;  
	//else if (prod->pack == UNIT) prod->amount = prod->unitPrice * prod->quantity;
	return INPUT_SUCCESS;
}
int inputProductFilter(Product* prod) {
	memset(prod, 0, sizeof(Product));
	prod->expiration = TIME_NAN;
	breakDeliver(getStrInput("��������(Ĭ�ϲ���):", prod->kind, INFOMAX, false));
	breakDeliver(getStrInput("����Ʒ��(Ĭ�ϲ���):", prod->variety, INFOMAX, false));
	breakDeliver(getDateTime("�ڴ����ڼ�֮ǰ����(��.��.��)(Ĭ�ϲ���) : ", &prod->expiration, false));
	breakDeliver(getUIntInput("����Ʒ��(Ĭ�ϲ���):", &prod->quality, (IntRange) { 1,3 }, false));
	drawOrdMenu("��װ��ʽ:", 3, 0, "����", "ɢװ", "��Ԫװ");
	breakDeliver(getUIntInput("ѡ���װ��ʽ(Ĭ�ϲ���):", &prod->pack, (IntRange) { 0,2 }, false));
	return INPUT_SUCCESS;
}

int inputRecordFilter(Record* rec) {
	memset(rec, 0, sizeof(Record));
	rec->time = rec->lastTime = TIME_NAN;
	drawMenu(getCursorPos(),"��¼����:", 5, 0, "����", "����", "���ۼ�¼", "���¼�¼", "�����¼");
	breakDeliver(getUIntInput("��¼����(Ĭ�ϲ���) : ", &rec->type, (IntRange){ 0,4 }, false));
	breakDeliver(inputProductFilter(&rec->prod));
	breakDeliver(getDateTime("ɸѡ����������(��.��.��)(Ĭ�ϲ���) : ", &rec->time, false));
	breakDeliver(getDateTime("ɸѡ����������(��.��.��)(Ĭ�ϲ���) : ", &rec->lastTime, false));
	rec->lastTime += 3600 * 24;
	return INPUT_SUCCESS;
}



void showProductDetails(Product* prod) {  //Only Show product attributes
	struct tm date;
	printf("����:%s\nƷ��:%s\n", prod->kind, prod->variety);
	printf("Ʒ��:%s\n", quanlityText[prod->quality]);
	localtime_s(&date, &prod->expiration);
	printf("��������:%d.%d.%d\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
	if (prod->pack == BULK) {
		printf("��װ��ʽ:ɢװ\n");
	}
	else if (prod->pack == UNIT) {
		printf("��װ��ʽ:��Ԫװ\n");
	}
}

void showInvDetails(Inventory* inv) {
	printf("��ƷID:%d\n", inv->invID);
	showProductDetails(&inv->prod);
	if (inv->prod.pack == BULK) {
		printf("�������:%.2lf\n", inv->prod.weight);
	}
	else if (inv->prod.pack == UNIT) {
		printf("�������:%d\n", inv->prod.quantity);
	}
	printf("���۵���:%.2lf\n", inv->prod.unitPrice);
}

char typeQuantityText[5][20] = { "","�ɹ���","������","�仯��","������" };
char typeAmountText[5][20] = { "","�ɹ��ܶ�","�����ܶ�","�ܼ۱仯","��Ʒ�ɱ�" };
void showRecordDetails(Record* rec) {
	struct tm time;
	localtime_s(&time, &rec->time);
	printf("��¼ID:%d\n��ƷID:%d\n", rec->recID, rec->invID);
	printf("����:%s\n", recordType[rec->type]);
	printf("��¼ʱ��:%d/%d/%d %02d:%02d:%02d\n", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_hour);
	showProductDetails(&rec->prod);
	if (rec->prod.pack == BULK) {
		printf("%s: %.2lf��\n", typeQuantityText[rec->type], rec->prod.weight);
	}
	else if (rec->prod.pack == UNIT) {
		printf("%s: %d��\n", typeQuantityText[rec->type], rec->prod.quantity);
	}
	switch (rec->type) {
	case PURCHASE:
		printf("�ɹ����ۣ�%.2lf\n", rec->prod.purUPrice);
		break;
	case SALE:
		printf("���۵��ۣ�%.2lf\n", rec->prod.unitPrice);
		break;
	case UPDATE:
		printf("�ۼ۱仯: %.2lf\n", rec->prod.unitPrice);
		break;
	case GIFT:
		printf("�ɹ����ۣ�%.2lf\n", rec->prod.purUPrice);
		break;
	}
	printf("%s: %.2lf\n", typeAmountText[rec->type], rec->prod.amount);
	printf("������Ϣ:%s\n", rec->addInfo);
}
