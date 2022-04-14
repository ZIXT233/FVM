#include<stdlib.h>
#include<string.h>
#include"sale_plan.h"

SSP* SSPCreate() {
	SSP* _new = (SSP*)malloc(sizeof(SSP));
	memset(_new, 0, sizeof(SSP));
	return _new;
}

void SSPDel(SSP* pos) {
	invListClear(pos->optGifts);
	free(pos->optGifts);
	free(pos);
}
void SSPListClear(SSP* head) {
	listForEachEntrySafe(SSP, pos, &head->list, list) {
		listRemove(&pos->list);
		SSPDel(pos);
	}

}
void SSPIDAllocate(SSP* pos, SSP* head) {
	pos->SSPID = ++head->SSPIDCnt;
}
SSP* SSPListInit(SSP* head) {
	head->SSPID = 0;
	head->SSPIDCnt = SSPIDBase;
	listInit(&head->list);
	return head;
}

SSP* SSPQueryID(SSP* head, int SSPID) {
	listForEachEntry(SSP, pos, &head->list, list) {
		if (pos->SSPID == SSPID) {
			return pos;
		}
	}
	return NULL;
}


CSP* CSPCreate() {
	CSP* _new = (CSP*)malloc(sizeof(CSP));
	memset(_new, 0, sizeof(CSP));
	_new->comInv = invListInit(invCreate());
	_new->optGifts = invListInit(invCreate());
	return _new;
}

void CSPDel(CSP* pos) {
	invListClear(pos->optGifts);
	free(pos->optGifts);
	invListClear(pos->comInv);
 	free(pos->comInv);
	free(pos);
}

void CSPIDAllocate(CSP* pos, CSP* head) {
	pos->CSPID = ++head->CSPIDCnt;
}
CSP* CSPListInit(CSP* head) {
	head->CSPID = 0;
	head->CSPIDCnt = CSPIDBase;
	listInit(&head->list);
	return head;
}

CSP* CSPQueryID(CSP* head, int CSPID) {
	listForEachEntry(CSP, pos, &head->list, list) {
		if (pos->CSPID == CSPID) {
			return pos;
		}
	}
	return NULL;
}

bool CSPHasInvID(CSP* pos, int invID) {

	listForEachEntry(Inventory, invPos, &pos->comInv->list, list) {
		if(invPos->invID == invID) return true;
	}
	return false;
}
void CSPListClear(CSP* head) {
	listForEachEntrySafe(CSP,pos, &head->list,list) {
		listRemove(&pos->list);
		CSPDel(pos);
	}

}
CSP* CSPCopyCreate(CSP* src) {
	CSP* cp = (CSP*)malloc(sizeof(CSP));
	memcpy(cp, src, sizeof(CSP));
	cp->comInv = invListInit(invCreate());
	cp->optGifts = invListInit(invCreate());
	listForEachEntry(Inventory, pos, &src->optGifts->list, list) {
		Inventory* giftcp = invCopyCreate(pos);
		listAddTail(&giftcp->list, &cp->optGifts->list);
	}
	listForEachEntry(Inventory, pos, &src->comInv->list, list) {
		Inventory* invcp = invCopyCreate(pos);
		listAddTail(&invcp->list, &cp->comInv->list);
	}
	return cp;
}
CSP* CSPOptionalListGen(CSP* head, Record* preOrder) {
	CSP* optCSP = CSPListInit(CSPCreate()),*cp=NULL;
	Inventory* giftcp = NULL;
	int invCnt;
	listForEachEntry(CSP, cspPos, &head->list,list) {
		invCnt = 0;
		listForEachEntry(Record, recPos, &preOrder->timeList, timeList) {
			if (recPos->CSPID==0 && CSPHasInvID(cspPos,recPos->invID)) {
				invCnt++;
			}
		}
		if (invCnt == cspPos->comInv->list.size) { //preOrder中不存在重复项，故可用计数判断匹配
			listAddTail(&CSPCopyCreate(cspPos)->list, &optCSP->list);
		}
	}
	return optCSP;
}

int CSPAddInv(CSP* csp, Inventory* inv) {
	listForEachEntry(Inventory, pos, &csp->comInv->list, list) {
		if (pos->invID == inv->invID) return COMINV_REPEAT;
	}
	Inventory* cp = invCopyCreate(inv);
	listAddTail(&cp->list, &csp->comInv->list);
	return 0;
}