#ifndef SALEPLAN_H
#define SALEPLAN_H
#include"list.h"
#include"product.h"

#define MAXGIFTS 100
#define MAXCOMBINE 100

typedef struct tagGift {
	int invID;
	int quantity;
	double weight;
	double addPrice;
} Gift;

typedef struct tagSingleSalePlan {
	ProductAttr attr;
	int minQuantity;
	double minWeight;
	double minAmount;
	time_t reqDate;
	bool reqMember;
	int reqPoint;
	double discount;
	Gift optGifts[MAXGIFTS];
	ListHead list;
}SingleSalePlan;
typedef struct tagComSalePlan {
	int comIDs[MAXCOMBINE];
	double discount;
	time_t reqDate;
	bool reqMember;
	int reqPoint;
	bool overlaySingleSP;
	Gift optGifts[MAXGIFTS];
	ListHead list;
}ComSalePlan;
#endif // !SALEPLAN_H
