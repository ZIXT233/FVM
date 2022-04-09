#include<string.h>
#include"sale_plan.h"
#include"fvm_objects.h"
#include"UI.h"

static const PageSize = 15;
static Coord SSPListPos = { 2,3 }, SSPMenuPos = { 22,3 };
static Coord CSPListPos = { 2,43 }, CSPMenuPos = { 22,43 };
static Coord invListPos = { 2,83 }, invMenuPos = { 22,83 };

void SSPAdd(SSP*head,Inventory* invHead) {
	int num;
	SSP* ssp = SSPCreate();
	memset(ssp, 0, sizeof(SSP));
	Inventory* inv = NULL;
	breakDeliver(getStrInput("���뷽����:", ssp->planName, INFOMAX, true));
	drawOrdMenu("����Ӧ������", 2, 1, "Ӧ���ڵ�����Ʒ", "Ӧ��������������������Ʒ");
	breakDeliver(getUIntInput("ѡ������:", &num, (IntRange) { 1, 2 }, true));
	if (num == 1) {
		breakDeliver(inputInventoryID(invHead, &ssp->invID, &inv));
	}
	else if (num == 2) {
		ssp->invID = 0;
		breakDeliver(inputProductFilter(&ssp->filter));
	}
	breakDeliver(getUIntInput("������͹�������(Ĭ�ϲ���):", &ssp->filter.quantity, QRANGE, false));
	breakDeliver(getDoubleInput("������͹�������(Ĭ�ϲ���):", &ssp->filter.weight, WRANGE,false));
	breakDeliver(getDoubleInput("������͵���(Ĭ�ϲ���):", &ssp->filter.unitPrice, WRANGE, false));
	breakDeliver(getDoubleInput("��������ܶ�(Ĭ�ϲ���):", &ssp->filter.amount, WRANGE, false));
	breakDeliver(getDateTime("������ʼ����(Ĭ�ϲ���):", &ssp->reqDateStart, false));
	breakDeliver(getDateTime("�����ֹ����(Ĭ�ϲ���):", &ssp->reqDateEnd, false));
	breakDeliver(getDoubleInput("�����ۿ���:", &ssp->discount, (DoubleRange){ 0,1 },true));
	while (1) {
		printf("�����%d����Ʒ(������quit����):\n", ssp->giftNum + 1);
		breakCatch(inputGift(ssp->optGifts + ssp->giftNum, invHead)) break;
		ssp->giftNum++;
	}
	ssp->SSPID = head->invID++;
	listAddTail(&ssp->list, &head->list);
}
void SSPDelete(SSP* head) {
	int sspid;
	SSP* pos;
	breakDeliver(inputSSPID(head, &sspid, &pos));
	listRemove(&pos->list);
	SSPDel(pos);
}
void SSPDetails(Renderer* renderer, SSP* ssp) {
	int select;
	while (1) {
		renderClear(renderer);
		//showSSPDetails(renderer, UI_ORIGIN, ssp);
		drawMenu(renderer, (Coord) { 20, 3 }, "��Ʒ�Żݷ�������", 1, 1,
			"�˳�");
		inputStart(renderer, INPUT_ORIGIN);
		renderPresent(renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			return;
		default:
			break;
		}
	}
}

void CSPAdd(CSP* head, Inventory* invHead) {
	int num;
	CSP* csp = CSPCreate();
	memset(csp, 0, sizeof(CSP));
	Inventory* inv = NULL;
	breakDeliver(getStrInput("���뷽����:", csp->planName, INFOMAX, true));
	while (1) {
		printf("��ϵĵ�%d����Ʒ(������quit����)\n", csp->comSize + 1);
		breakCatch(inputInventoryID(invHead, csp->comIDs + csp->comSize, &inv)) break;
		csp->comSize++;
	}
	drawOrdMenu("�Ƿ�ɵ��ӵ����Ż�:", 2, 1, "��", "��");
	csp->overlaySingleSP = 1;
	breakDeliver(getUIntInput("ѡ��һ��(Ĭ��Ϊ��):", &csp->overlaySingleSP, (IntRange) { 1, 2 }, false));
	breakDeliver(getDateTime("������ʼ����(Ĭ�ϲ���):", &csp->reqDateStart, false));
	breakDeliver(getDateTime("�����ֹ����(Ĭ�ϲ���):", &csp->reqDateEnd, false));
	while (1) {
		printf("�����%d����Ʒ(������quit����)\n", csp->giftNum + 1);
		breakCatch(inputGift(csp->optGifts + csp->giftNum, invHead)) break;
		csp->giftNum++;
	}
	csp->CSPID = head->comIDs[0]++;
	listAddTail(&csp->list, &head->list);

}
void CSPDelete(CSP* head) {
	int cspid;
	CSP* pos;
	breakDeliver(inputCSPID(head, &cspid, &pos));
	listRemove(&pos->list);
	CSPDel(pos);
}
void CSPDetails(Renderer* renderer, CSP* csp) {
	int select;
	while (1) {
		renderClear(renderer);
		//showSSPDetails(renderer, UI_ORIGIN, ssp);
		drawMenu(renderer, (Coord) { 20, 3 }, "��Ʒ�Żݷ�������", 1, 1,
			"�˳�");
		inputStart(renderer, INPUT_ORIGIN);
		renderPresent(renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			return;
		default:
			break;
		}
	}
}
void salePlanManage(FVMO gdata) {
	int inFilter = 0;
	int SSPPageStart = 1, CSPPageStart = 1;
	int invPageStart = 1, invPageStartSave;
	char filterOpt[2][20] = { "��Ʒɸѡ","ȡ��ɸѡ" };
	SSP* ssp = NULL;
	CSP* csp = NULL;
	Inventory* filterList=NULL,*inv=NULL;
	int select, num;
	Product filter;
	while (1) {
		renderClear(gdata.renderer);
		drawListPage(gdata.renderer, SSPListPos, "��Ʒ���۷����б�", drawSSPList, 
			listShowPageJump(&gdata.SSP->list, &SSPPageStart, PageSize), 
			SSPPageStart, PageSize, (Coord) { 18, 30 });
		drawListPage(gdata.renderer, CSPListPos, "������۷����б�", drawCSPList, 
			listShowPageJump(&gdata.CSP->list, &CSPPageStart, PageSize), 
			CSPPageStart, PageSize, (Coord) { 18, 30 });
		if (inFilter) {
			filterList = invFilterListGen(gdata.inventory, &filter);
			drawInvPage(gdata.renderer, invListPos, "ɸѡ��Ϣ", invShowPageJump(filterList, &invPageStart, PageSize), invPageStart, PageSize);
			invListClear(filterList);
			free(filterList);
		}
		else {
			drawInvPage(gdata.renderer, invListPos, "�����Ϣ", invShowPageJump(gdata.inventory, &invPageStart, PageSize), invPageStart, PageSize);
		}
		drawMenu(gdata.renderer, SSPMenuPos, "��Ʒ���۷�������", 6, 1,
			"��һҳ",
			"��һҳ",
			"�鿴����",
			"��ӵ�Ʒ���۷���",
			"ɾ����Ʒ���۷���",
			"�˳�");
		drawMenu(gdata.renderer, CSPMenuPos, "������۷�������", 5, 11,
			"��һҳ",
			"��һҳ",
			"�鿴����",
			"���������۷���",
			"ɾ��������۷���");
		drawMenu(gdata.renderer, invMenuPos, "��Ʒ����", 4, 21,
			"��һҳ",
			"��һҳ",
			filterOpt[inFilter],
			"��Ʒ����");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			SSPPageStart -= PageSize;
			if (SSPPageStart < 1) SSPPageStart = 1;
			break;
		case 2:
			SSPPageStart += PageSize;
			break;
		case 3:
			breakCatch(inputSSPID(gdata.SSP, &num, &ssp)) break;
			SSPDetails(gdata.renderer, ssp);
			break;
		case 4:
			SSPAdd(gdata.SSP,gdata.inventory);
			break;
		case 5:
			SSPDelete(gdata.SSP);
			break;
		case 6:
			return;
		case 11:
			CSPPageStart -= PageSize;
			if (CSPPageStart < 1) CSPPageStart = 1;
			break;
		case 12:
			CSPPageStart += PageSize;
			break;
		case 13:
			breakCatch(inputCSPID(gdata.CSP, &num, &csp)) break;
			CSPDetails(gdata.renderer, csp);
		case 14:
			CSPAdd(gdata.CSP,gdata.inventory);
			break;
		case 15:
			CSPDelete(gdata.CSP);
			break;
		case 21:
			invPageStart -= PageSize;
			if (invPageStart < 1) invPageStart = 1;
			break;
		case 22:
			invPageStart += PageSize;
			break;
		case 23:
			if (inFilter) {  //ȡ��ɸѡ
				inFilter = false;
				invPageStart = invPageStartSave;
			}
			else if (!inFilter) {
				breakCatch(inputProductFilter(&filter)) {
					break;
				}
				inFilter = true;
				invPageStartSave = invPageStart;
				invPageStart = 1;
			}
			break;
		case 24:
			breakCatch(inputInventoryID(gdata.inventory, &num, &inv)) break;
			invDetails(gdata.renderer, inv);
			break;
		default:
			break;
		}
	}
}