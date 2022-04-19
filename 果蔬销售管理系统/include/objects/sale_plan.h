#ifndef SALEPLAN_H
#define SALEPLAN_H
#include"list.h"
#include"inventory.h"

#define MAXGIFTS 100
#define MAXCOMBINE 100



typedef struct tagSingleSalePlan {
	char planName[INFOMAX];
	Product filter;
	int invID;
	int SSPID,SSPIDCnt;
	time_t reqDateStart,reqDateEnd;
	bool reqMember;
	int reqPoint;
	double discount;
	char addInfo[INFOMAX];
	Inventory* optGifts;
	ListHead list;
}SingleSalePlan,SSP;

static const int SSPIDBase = 0000;
SSP* SSPCreate();
SSP* SSPListInit();
void SSPListClear(SSP* head);
void SSPIDAllocate(SSP* pos, SSP* head);
SSP* SSPQueryID(SSP* head,int SSPID);
void SSPDel(SSP* pos);


typedef struct tagComSalePlan {
	char planName[INFOMAX];
	Inventory *comInv;
	int CSPID,CSPIDCnt;
	double discount;
	time_t reqDateStart,reqDateEnd;
	bool reqMember;
	int reqPoint;
	bool overlaySingleSP;
	char addInfo[INFOMAX];
	Inventory* optGifts;
	ListHead list;
}ComSalePlan,CSP;
#define COMINV_REPEAT -1
static const int CSPIDBase = 0000;
CSP* CSPCreate();
CSP* CSPListInit(CSP* head);
void CSPIDAllocate(CSP* pos, CSP* head);
CSP* CSPQueryID(CSP* head,int id);
bool CSPHasInvID(CSP* pos, int invID);
CSP* CSPOptionalListGen(CSP* head, Record* preOrder, bool isMember, time_t fvmtime);
void CSPListClear(CSP* head);
int CSPAddInv(CSP* csp, Inventory* inv);
void CSPDel(CSP* pos);

typedef struct tagInvCSPEX {
	Inventory inv;
	int CSPID[100],idcnt;
}InvCSPEX;

#endif // !SALEPLAN_H
