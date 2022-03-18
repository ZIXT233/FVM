#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>
#include"UI.h"


static char recordType[5][20] = { "","������¼","���ۼ�¼","���¼�¼","���ͼ�¼" };
int typeDirect[5] = { 0,1,-1,1,-1 };

char quanlityText[5][20] = { "","��","��","��" };

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
	printf("����\t\tƷ��\t\t�۸�\t\tID\n");
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
	printf("����\t\t��¼ID\t��ƷID\t����\t\tƷ��\t\t��Ʒ����\t�ʽ����\tʱ��\n");
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
			printf("%+.2lf��\t", pos->prod.weight * typeDirect[pos->type]);
		}
		else if (pos->prod.pack == UNIT) {
			printf("%+d��\t", pos->prod.quantity * typeDirect[pos->type]);
		}
		printf("%+.2lf\t", pos->prod.amount * typeDirect[pos->type]);
		printf("%d/%d/%d %d:%d:%d\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
		n++;
	}
	while (n++ != len) putchar('\n');
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
				printf("������ȷ�����ڡ�\n");
			}
			else {
				*value = time;
				return INPUT_SUCCESS;
			}
		}
	}
}

int inputProduct(Product* prod) {
	breakDeliver(getStrInput("��������:", prod->kind, INFOMAX, true));
	breakDeliver(getStrInput("����Ʒ��:", prod->variety, INFOMAX, true));
	breakDeliver(getDateTime("�������ʱ��(��.��.��):", &prod->expiration, true));
	drawMenu("Ʒ��:", 3, "��", "��", "��");
	prod->quality = 0;
	while (!(prod->quality >= 1 && prod->quality <= 3)) breakDeliver(getUIntInput("ѡ��Ʒ��:", &prod->quality, true));
	drawMenu("��װ��ʽ:", 2, "ɢװ", "��Ԫװ");
	prod->pack = 0;
	while (!(prod->pack == 1 || prod->pack == 2)) breakDeliver(getUIntInput("ѡ���װ��ʽ:", &prod->pack, true));
	if (prod->pack == BULK) {
		breakDeliver(getUIntInput("��������:", &prod->weight, true));
	}
	else if (prod->pack = UNIT) {
		breakDeliver(getUIntInput("��������:", &prod->quantity, true));
	}
	breakDeliver(getDoubleInput("���뵥��:", &prod->unitPrice, true));
	if (prod->pack == BULK) prod->amount = prod->unitPrice * prod->weight;
	else if (prod->pack == UNIT) prod->amount = prod->unitPrice * prod->quantity;
	return INPUT_SUCCESS;
}
int inputProductFilter(Product* prod) {
	memset(prod, 0, sizeof(Product));
	prod->expiration = TIME_NAN;
	breakDeliver(getStrInput("��������(Ĭ�ϲ���):", prod->kind, INFOMAX, false));
	breakDeliver(getStrInput("����Ʒ��(Ĭ�ϲ���):", prod->variety, INFOMAX, false));
	breakDeliver(getDateTime("�ڴ����ڼ�֮ǰ����(��.��.��)(Ĭ�ϲ���) : ", &prod->expiration, false));
	breakDeliver(getUIntInput("����Ʒ��(Ĭ�ϲ���):", &prod->quality, false));
	drawMenu("��װ��ʽ:", 3, "����", "ɢװ", "��Ԫװ");
	prod->pack = 1;
	do {
		breakDeliver(getUIntInput("ѡ���װ��ʽ(Ĭ�ϲ���):", &prod->pack, false))
	} while (!(prod->pack >= 1 && prod->pack <= 3));
		prod->pack--;
		return INPUT_SUCCESS;
}

int inputRecordFilter(Record* rec) {
	memset(rec, 0, sizeof(Record));
	rec->type = 1;
	do {
		drawMenu("��¼����:", 5, "����", "����", "���ۼ�¼", "���¼�¼", "�����¼");
		breakDeliver(getDateTime("��¼����(Ĭ�ϲ���) : ", &rec->type, false));
	} while (!(rec->type >= 1 && rec->type <= 5));
	rec->type--;
	breakDeliver(inputProductFilter(&rec->prod));
	rec->time = rec->lastTime = TIME_NAN;
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
}

char typeQuantityText[5][20] = { "","�ɹ���","������","�仯��","������" };
char typeAmountText[5][20] = { "","�ɹ��ܶ�","�����ܶ�","�ܼ۱仯","��Ʒ�ɱ�" };
void showRecordDetails(Record* rec) {
	struct tm time;
	localtime_s(&time, &rec->time);
	printf("��¼ID:%d\n��ƷID:%d\n", rec->recID, rec->invID);
	printf("����:%s\n", recordType[rec->type]);
	printf("��¼ʱ��:%d/%d/%d %d:%d%d\n", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_hour);
	showProductDetails(&rec->prod);
	if (rec->prod.pack == BULK) {
		printf("%s: %.2lf��\n", typeQuantityText[rec->type], rec->prod.weight);
	}
	else if (rec->prod.pack == UNIT) {
		printf("%s: %d��\n", typeQuantityText[rec->type], rec->prod.quantity);
	}
	printf("%s: %.2lf\n", typeAmountText[rec->type], rec->prod.amount);
	printf("������Ϣ:%s\n", rec->addInfo);
}
