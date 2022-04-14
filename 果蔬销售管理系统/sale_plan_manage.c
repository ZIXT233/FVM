#include<string.h>
#include"sale_plan.h"
#include"fvm_objects.h"
#include"UI.h"

static const int PageSize = 15;
static const Coord SSPListPos = { 2,3 }, SSPMenuPos = { 22,3 }, SSPDetailsPos = { 2,3 }, SSPDetailsMenuPos = { 22,3 };
static const Coord SSPGiftPos = { 2,57 }, SSPGiftMenuPos = { 22,57 }, GiftRectSize = { 18,65 };
static const Coord CSPListPos = { 2,43 }, CSPMenuPos = { 22,43 }, CSPDetailsPos = { 2,3 }, CSPDetailsMenuPos = { 22,3 };
static const Coord CSPComInvPos = { 2,43 }, CSPComInvMenuPos = { 22,43 }, CSPComInvRectSize = { 18,40 };
static const Coord CSPGiftPos = { 2,99 }, CSPGiftMenuPos = { 22,99 };
static const Coord invListPos = { 2,83 }, invMenuPos = { 22,83 };

int SSPAdd(SSP* head, Inventory* invHead) {
	int num;
	SSP* ssp = SSPCreate();
	Inventory* inv = NULL, * gift = NULL;
	breakDeliver(getStrInput("���뷽����:", ssp->planName, INFOMAX, true));
	drawOrdMenu("����Ӧ������", 2, 1, "Ӧ���ڵ�����Ʒ", "����Ӧ����������������Ʒ");
	breakDeliver(getUIntInput("ѡ������:", &num, (IntRange) { 1, 2 }, true));
	if (num == 1) {
		breakDeliver(inputInventoryID(invHead, &ssp->invID, &inv));
		ssp->filter.pack = inv->prod.pack;
	}
	else if (num == 2) {
		ssp->invID = 0;
		breakDeliver(inputProductFilter(&ssp->filter));
		breakDeliver(getDoubleInput("������͵���(Ĭ�ϲ���):", &ssp->filter.unitPrice, WRANGE, false));
	}
	ssp->filter.quantity = ssp->filter.weight = 0;
	if (!ssp->invID || ssp->filter.pack == UNIT) breakDeliver(getUIntInput("������͹�������(Ĭ�ϲ���):", &ssp->filter.quantity, QRANGE, false));
	if (!ssp->invID || ssp->filter.pack == BULK) breakDeliver(getDoubleInput("������͹�������(Ĭ�ϲ���):", &ssp->filter.weight, WRANGE, false));
	breakDeliver(getDoubleInput("��������ܶ�(Ĭ�ϲ���):", &ssp->filter.amount, WRANGE, false));
	breakDeliver(getDateTime("������ʼ����(Ĭ�ϲ���):", &ssp->reqDateStart, false));
	breakDeliver(getDateTime("�����ֹ����(Ĭ�ϲ���):", &ssp->reqDateEnd, false));
	breakDeliver(getDoubleInput("�����ۿ���:", &ssp->discount, (DoubleRange) { 0, 1 }, true));
	ssp->optGifts = invListInit(invCreate());
	while (1) {
		printf("�����%d����Ʒ(������quit����)\n", ssp->optGifts->list.size + 1);
		breakCatch(inputGift(&gift, invHead)) break;
		listAddTail(&gift->list, &ssp->optGifts->list);
	}
	SSPIDAllocate(ssp, head);
	listAddTail(&ssp->list, &head->list);
}
int SSPDelete(SSP* head) {
	int sspid;
	SSP* pos;
	breakDeliver(inputSSPID(head, &sspid, &pos));
	listRemove(&pos->list);
	SSPDel(pos);
}
void SSPDetails(SSP* ssp,FVMO gdata) {
	int giftPageStart = 1;
	int select;
	pageStackPush(pageStackCreate("��Ʒ���۷�������"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		showSSPDetails(gdata.renderer, SSPDetailsPos, ssp);
		drawListPage(gdata.renderer, SSPGiftPos, "��Ʒ�б�", drawGiftList, &ssp->optGifts->list, &giftPageStart, PageSize, GiftRectSize, NULL);
		drawMenu(gdata.renderer, SSPDetailsMenuPos, "����Żݷ�������", 3, 1,
			"��Ʒ��һҳ",
			"��Ʒ��һҳ",
			"�˳�");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			giftPageStart -= PageSize;
			if (giftPageStart < 1) giftPageStart = 1;
			break;
		case 2:
			giftPageStart += PageSize;
			break;
		case 3:
			pageStackPop(gdata.pageStack);
			return;
		default:
			break;
		}
	}
}

int CSPAdd(CSP* head, Inventory* invHead) {
	int num;
	bool repeat;
	CSP* csp = CSPCreate();
	Inventory* inv = NULL, * gift = NULL;
	int invID;
	breakDeliver(getStrInput("���뷽����:", csp->planName, INFOMAX, true));
	while (1) {
		printf("��ϵĵ�%d����Ʒ(������quit����)\n", csp->comInv->list.size + 1);
		breakCatch(inputInventoryID(invHead, &invID, &inv)) break;
		if (CSPAddInv(csp, inv) == COMINV_REPEAT) {
			printf("�����ظ������Ʒ\n");
		}
	}
	drawOrdMenu("�Ƿ�ɵ��ӵ����Ż�:", 2, 1, "��", "��");
	csp->overlaySingleSP = 1;
	breakDeliver(getUIntInput("ѡ��һ��(Ĭ��Ϊ��):", &csp->overlaySingleSP, (IntRange) { 1, 2 }, false));
	if (csp->overlaySingleSP == 2) csp->overlaySingleSP = 0;
	breakDeliver(getDateTime("������ʼ����(Ĭ�ϲ���):", &csp->reqDateStart, false));
	breakDeliver(getDateTime("�����ֹ����(Ĭ�ϲ���):", &csp->reqDateEnd, false));
	breakDeliver(getDoubleInput("�����ۿ���:", &csp->discount, (DoubleRange) { 0, 1 }, true));
	while (1) {
		printf("�����%d����Ʒ(������quit����)\n", csp->optGifts->list.size + 1);
		breakCatch(inputGift(&gift, invHead)) break;
		listAddTail(&gift->list, &csp->optGifts->list);
	}
	CSPIDAllocate(csp, head);
	listAddTail(&csp->list, &head->list);

}
int CSPDelete(CSP* head) {
	int cspid;
	CSP* pos;
	breakDeliver(inputCSPID(head, &cspid, &pos));
	listRemove(&pos->list);
	CSPDel(pos);
}
void CSPDetails(CSP* csp,FVMO gdata) {
	int giftPageStart = 1, comInvPageStart = 1;
	int select;
	pageStackPush(pageStackCreate("������۷�������"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		showCSPDetails(gdata.renderer, CSPDetailsPos, csp);
		drawListPage(gdata.renderer, CSPComInvPos, "�����Ʒ�б�", drawComInvList, &csp->comInv->list, &comInvPageStart, PageSize, CSPComInvRectSize, NULL);
		drawListPage(gdata.renderer, CSPGiftPos, "��Ʒ�б�", drawGiftList, &csp->optGifts->list, &giftPageStart, PageSize, GiftRectSize, NULL);
		drawMenu(gdata.renderer, CSPDetailsMenuPos, "����Żݷ�������", 6, 1,
			"�����Ʒ��һҳ",
			"�����Ʒ��һҳ",
			"�˳�",
			"��Ʒ��һҳ",
			"��Ʒ��һҳ",
			"�˳�");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select)
		{
		case 1:
			comInvPageStart -= PageSize;
			if (comInvPageStart < 1) comInvPageStart = 1;
			break;
		case 2:
			comInvPageStart += PageSize;
			break;
		case 3:
			return;
		case 4:
			giftPageStart -= PageSize;
			if (giftPageStart < 1) giftPageStart = 1;
			break;
		case 5:
			giftPageStart += PageSize;
			break;
		case 6:
			pageStackPop(gdata.pageStack);
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
	Inventory* filterList = NULL, * inv = NULL;
	int select, num;
	Product filter;
	pageStackPush(pageStackCreate("���۷�������"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, SSPListPos, "��Ʒ���۷����б�", drawSSPList,
			&gdata.SSP->list, &SSPPageStart, PageSize, (Coord) { 18, 30 }, NULL);
		drawListPage(gdata.renderer, CSPListPos, "������۷����б�", drawCSPList,
			&gdata.CSP->list, &CSPPageStart, PageSize, (Coord) { 18, 30 }, NULL);
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
			SSPDetails(ssp,gdata);
			break;
		case 4:
			SSPAdd(gdata.SSP, gdata.inventory);
			break;
		case 5:
			SSPDelete(gdata.SSP);
			break;
		case 6:
			pageStackPop(gdata.pageStack);
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
			CSPDetails(csp,gdata);
			break;
		case 14:
			CSPAdd(gdata.CSP, gdata.inventory);
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