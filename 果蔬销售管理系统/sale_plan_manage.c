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
	breakDeliver(getStrInput("输入方案名:", ssp->planName, INFOMAX, true));
	drawOrdMenu("方案应用类型", 2, 1, "应用于单个商品", "批量应用于满足条件的商品");
	breakDeliver(getUIntInput("选择类型:", &num, (IntRange) { 1, 2 }, true));
	if (num == 1) {
		breakDeliver(inputInventoryID(invHead, &ssp->invID, &inv));
		ssp->filter.pack = inv->prod.pack;
	}
	else if (num == 2) {
		ssp->invID = 0;
		breakDeliver(inputProductFilter(&ssp->filter));
		breakDeliver(getDoubleInput("输入最低单价(默认不限):", &ssp->filter.unitPrice, WRANGE, false));
	}
	ssp->filter.quantity = ssp->filter.weight = 0;
	if (!ssp->invID || ssp->filter.pack == UNIT) breakDeliver(getUIntInput("输入最低购买数量(默认不限):", &ssp->filter.quantity, QRANGE, false));
	if (!ssp->invID || ssp->filter.pack == BULK) breakDeliver(getDoubleInput("输入最低购买重量(默认不限):", &ssp->filter.weight, WRANGE, false));
	breakDeliver(getDoubleInput("输入最低总额(默认不限):", &ssp->filter.amount, WRANGE, false));
	breakDeliver(getDateTime("输入起始日期(默认不限):", &ssp->reqDateStart, false));
	breakDeliver(getDateTime("输入截止日期(默认不限):", &ssp->reqDateEnd, false));
	breakDeliver(getDoubleInput("输入折扣率:", &ssp->discount, (DoubleRange) { 0, 1 }, true));
	ssp->optGifts = invListInit(invCreate());
	while (1) {
		printf("输入第%d种赠品(或输入quit结束)\n", ssp->optGifts->list.size + 1);
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
	pageStackPush(pageStackCreate("单品销售方案详情"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		showSSPDetails(gdata.renderer, SSPDetailsPos, ssp);
		drawListPage(gdata.renderer, SSPGiftPos, "赠品列表", drawGiftList, &ssp->optGifts->list, &giftPageStart, PageSize, GiftRectSize, NULL);
		drawMenu(gdata.renderer, SSPDetailsMenuPos, "组合优惠方案详情", 3, 1,
			"赠品下一页",
			"赠品上一页",
			"退出");
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
	breakDeliver(getStrInput("输入方案名:", csp->planName, INFOMAX, true));
	while (1) {
		printf("组合的第%d件商品(或输入quit结束)\n", csp->comInv->list.size + 1);
		breakCatch(inputInventoryID(invHead, &invID, &inv)) break;
		if (CSPAddInv(csp, inv) == COMINV_REPEAT) {
			printf("不能重复添加商品\n");
		}
	}
	drawOrdMenu("是否可叠加单件优惠:", 2, 1, "是", "否");
	csp->overlaySingleSP = 1;
	breakDeliver(getUIntInput("选择一项(默认为是):", &csp->overlaySingleSP, (IntRange) { 1, 2 }, false));
	if (csp->overlaySingleSP == 2) csp->overlaySingleSP = 0;
	breakDeliver(getDateTime("输入起始日期(默认不限):", &csp->reqDateStart, false));
	breakDeliver(getDateTime("输入截止日期(默认不限):", &csp->reqDateEnd, false));
	breakDeliver(getDoubleInput("输入折扣率:", &csp->discount, (DoubleRange) { 0, 1 }, true));
	while (1) {
		printf("输入第%d种赠品(或输入quit结束)\n", csp->optGifts->list.size + 1);
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
	pageStackPush(pageStackCreate("组合销售方案详情"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		showCSPDetails(gdata.renderer, CSPDetailsPos, csp);
		drawListPage(gdata.renderer, CSPComInvPos, "组合商品列表", drawComInvList, &csp->comInv->list, &comInvPageStart, PageSize, CSPComInvRectSize, NULL);
		drawListPage(gdata.renderer, CSPGiftPos, "赠品列表", drawGiftList, &csp->optGifts->list, &giftPageStart, PageSize, GiftRectSize, NULL);
		drawMenu(gdata.renderer, CSPDetailsMenuPos, "组合优惠方案详情", 6, 1,
			"组合商品下一页",
			"组合商品上一页",
			"退出",
			"赠品下一页",
			"赠品上一页",
			"退出");
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
	char filterOpt[2][20] = { "商品筛选","取消筛选" };
	SSP* ssp = NULL;
	CSP* csp = NULL;
	Inventory* filterList = NULL, * inv = NULL;
	int select, num;
	Product filter;
	pageStackPush(pageStackCreate("销售方案管理"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawListPage(gdata.renderer, SSPListPos, "单品销售方案列表", drawSSPList,
			&gdata.SSP->list, &SSPPageStart, PageSize, (Coord) { 18, 30 }, NULL);
		drawListPage(gdata.renderer, CSPListPos, "组合销售方案列表", drawCSPList,
			&gdata.CSP->list, &CSPPageStart, PageSize, (Coord) { 18, 30 }, NULL);
		if (inFilter) {
			filterList = invFilterListGen(gdata.inventory, &filter);
			drawInvPage(gdata.renderer, invListPos, "筛选信息", invShowPageJump(filterList, &invPageStart, PageSize), invPageStart, PageSize);
			invListClear(filterList);
			free(filterList);
		}
		else {
			drawInvPage(gdata.renderer, invListPos, "库存信息", invShowPageJump(gdata.inventory, &invPageStart, PageSize), invPageStart, PageSize);
		}
		drawMenu(gdata.renderer, SSPMenuPos, "单品销售方案管理", 6, 1,
			"上一页",
			"下一页",
			"查看详情",
			"添加单品销售方案",
			"删除单品销售方案",
			"退出");
		drawMenu(gdata.renderer, CSPMenuPos, "组合销售方案管理", 5, 11,
			"上一页",
			"下一页",
			"查看详情",
			"添加组合销售方案",
			"删除组合销售方案");
		drawMenu(gdata.renderer, invMenuPos, "商品销售", 4, 21,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品详情");
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
			if (inFilter) {  //取消筛选
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