#ifndef SALEPLAN_H
#define SALEPLAN_H
#include"list.h"
#include"inventory.h"

#define MAXGIFTS 100
#define MAXCOMBINE 100

typedef struct tagGift {
	int invID;
	int quantity;
	double weight;
	double addPrice;
} Gift;

typedef struct tagSingleSalePlan {
	char planName[INFOMAX];
	Product filter;
	int invID;
	int SSPID;
	time_t reqDateStart,reqDateEnd;
	bool reqMember;
	int reqPoint;
	double discount;
	char addInfo[INFOMAX];
	Gift optGifts[MAXGIFTS];
	int giftNum;
	ListHead list;
}SingleSalePlan,SSP;

SSP* SSPCreate();
SSP* SSPListInit();
SSP* SSPQueryID(SSP* head,int SSPID);
void SSPDel(SSP* pos);

typedef struct tagComSalePlan {
	char planName[INFOMAX];
	int comIDs[MAXCOMBINE],comSize;
	int CSPID;
	double discount;
	time_t reqDateStart,reqDateEnd;
	bool reqMember;
	int reqPoint;
	bool overlaySingleSP;
	char addInfo[INFOMAX];
	Gift optGifts[MAXGIFTS];
	int giftNum;
	ListHead list;
}ComSalePlan,CSP;

CSP* CSPCreate();
CSP* CSPListInit(CSP* head);
CSP* CSPQueryID(CSP* head,int id);
void CSPDel(CSP* pos);

typedef struct tagInvCSPEX {
	Inventory inv;
	int CSPID[100],idcnt;
}InvCSPEX;

#endif // !SALEPLAN_H
