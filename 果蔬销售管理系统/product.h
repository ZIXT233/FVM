#ifndef PRODUCT_H
#define PRODUCT_H
#include<time.h>
#define INFOMAX 256
typedef struct tagProductAttr {
	char kind[INFOMAX];
	char variety[INFOMAX];
	time_t expiration;
	enum Packing { BULK, UNIT } pack;
	int quanlity;
} ProductAttr;
inline ProductAttr makeProductAttr(char* kind, char* variety, time_t expiration, enum Packing pack, int quality) {
	return (ProductAttr) { kind, variety, expiration, pack, quality };
}
typedef struct tagProductPattern {
	ProductAttr attr;
	int quantity;	//����һ������
	double weight;
	double amount;	//����һ���۸�
}ProductPattern;
#endif