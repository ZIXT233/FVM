#ifndef PRODUCT_H
#define PRODUCT_H
#include<time.h>
#define INFOMAX 256
#define TIME_NAN (0x3f3f3f3f3f3f3f3f)
typedef struct tagProduct {
	//Product attributes
	char kind[INFOMAX];
	char variety[INFOMAX];
	time_t expiration;
	int quality;
	enum Packing { BULK=1, UNIT } pack;
	//Product status
	int quantity;
	double weight;
	double amount;
	double unitPrice;
	double purUPrice;
} Product;

typedef struct tagIntRange {
	int min, max;
}IntRange;
typedef struct tagDoubleRange {
	double min, max;
}DoubleRange;

static const IntRange QRANGE = { 0,1e7 }, ALLINT = { 0,-1 };
static const DoubleRange WRANGE = { 0,1e7 },UPRINCERANGE = { 0,1e8 }, ALLDOUBLE = { 0,-1 };		
static const double WMINI = 0.01;
static inline int productMatch(const Product* inv, const Product* filter) {
	if (filter == NULL) return 1;
	if (filter->kind[0] != '\0' && strcmp(filter->kind, inv->kind) != 0) return 0;
	if (filter->variety[0] != '\0' && strcmp(filter->variety, inv->variety) != 0) return 0;
	if (filter->pack != 0 && filter->pack != inv->pack) return 0;
	if (filter->expiration != TIME_NAN && filter->expiration < inv->expiration) return 0;
	return 1;
}

#endif
