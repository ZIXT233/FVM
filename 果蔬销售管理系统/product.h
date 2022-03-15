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
	int quantity;	//满足一定数量
	double weight;
	double amount;	//满足一定价格
}ProductPattern;
#endif