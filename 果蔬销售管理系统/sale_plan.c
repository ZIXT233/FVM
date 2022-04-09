#include<stdlib.h>
#include"sale_plan.h"

SSP* SSPCreate() {
	return (SSP*)malloc(sizeof(SSP));
}

void SSPDel(SSP* pos) {
	free(pos);
}

SSP* SSPListInit(SSP* head) {
	head->SSPID = 0;
	head->invID = 1;
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
	return (CSP*)malloc(sizeof(CSP));
}

void CSPDel(CSP* pos) {
	free(pos);
}

CSP* CSPListInit(CSP* head) {
	head->CSPID = 0;
	head->comIDs[0] = 1;
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

