#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>
#include"UI.h"

void cls() {
	system("cls");
}
void drawMenu(const char* title, int n, ...) {
	va_list args;
	va_start(args, n);
	printf("%s:\n", title);
	for (int i = 1; i <= n; i++) {
		char* item = va_arg(args, char*);
		printf("\t%d.%s\n", i, item);
	}
	return 0;
}

void strMakeLen(char*dest, const char* src, int len) {
	int slen = strlen(src);
	for (int i = 0; i < len-1; i++) {
		if (i >= slen) dest[i] = ' ';
		else if (i >= len - 4) dest[i] = '.';
		else dest[i] = src[i];
	}
	dest[len - 1] = '\0';
}
void drawInvList(Inventory* start, int len) {
	printf("����\t\tƷ��\t\t�۸�\t\tID\t��%d\n", listSize(&start->list));
	int n = 0;
	char kind[INFOMAX], var[INFOMAX];
	
	listForEachEntry(Inventory, pos, &(start->list), list) {
		if (n == len) break;
		strMakeLen(kind, pos->attr.kind, 14);
		strMakeLen(var, pos->attr.variety, 12);
		printf("%s\t%s\t%08.2lf\t%d\n", kind, var,pos->unitPrice, pos->invID);
		n++;
	}
	while (n++ != len) putchar('\n');
}
void readline(char* buf, int maxCount, FILE* stream) { //������ȡһ�д���buf�У�������maxCount���ַ����������ַ������������з��Ͷ����ַ�
	int i;
	char t;
	for (i=0; (t = fgetc(stream)) != '\n'; i++) {
		if (i < maxCount - 1) buf[i] = t;
		else if (i == maxCount - 1) buf[i] = '\0';
	}
	if (i < maxCount) buf[i] = '\0';
}
int getSelect() {
	int num;
	char input[INFOMAX];
	printf("ѡ��һ���Լ���: ");
	readline(input, INFOMAX, stdin);
	if (sscanf_s(input, "%d", &num) == 1) return num;
	return -1;
}

