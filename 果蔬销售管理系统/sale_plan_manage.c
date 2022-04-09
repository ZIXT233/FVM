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
	breakDeliver(getStrInput("输入方案名:", ssp->planName, INFOMAX, true));
	drawOrdMenu("方案应用类型", 2, 1, "应用于单个商品", "应用于满足条件的所有商品");
	breakDeliver(getUIntInput("选择类型:", &num, (IntRange) { 1, 2 }, true));
	if (num == 1) {
		breakDeliver(inputInventoryID(invHead, &ssp->invID, &inv));
	}
	else if (num == 2) {
		ssp->invID = 0;
		breakDeliver(inputProductFilter(&ssp->filter));
	}
	breakDeliver(getUIntInput("输入最低购买数量(默认不限):", &ssp->filter.quantity, QRANGE, false));
	breakDeliver(getDoubleInput("输入最低购买重量(默认不限):", &ssp->filter.weight, WRANGE,false));
	breakDeliver(getDoubleInput("输入最低单价(默认不限):", &ssp->filter.unitPrice, WRANGE, false));
	breakDeliver(getDoubleInput("输入最低总额(默认不限):", &ssp->filter.amount, WRANGE, false));
	breakDeliver(getDateTime("输入起始日期(默认不限):", &ssp->reqDateStart, false));
	breakDeliver(getDateTime("输入截止日期(默认不限):", &ssp->reqDateEnd, false));
	breakDeliver(getDoubleInput("输入折扣率:", &ssp->discount, (DoubleRange){ 0,1 },true));
	while (1) {
		printf("输入第%d件赠品(或输入quit结束):\n", ssp->giftNum + 1);
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
		drawMenu(renderer, (Coord) { 20, 3 }, "单品优惠方案详情", 1, 1,
			"退出");
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
	breakDeliver(getStrInput("输入方案名:", csp->planName, INFOMAX, true));
	while (1) {
		printf("组合的第%d件商品(或输入quit结束)\n", csp->comSize + 1);
		breakCatch(inputInventoryID(invHead, csp->comIDs + csp->comSize, &inv)) break;
		csp->comSize++;
	}
	drawOrdMenu("是否可叠加单件优惠:", 2, 1, "是", "否");
	csp->overlaySingleSP = 1;
	breakDeliver(getUIntInput("选择一项(默认为是):", &csp->overlaySingleSP, (IntRange) { 1, 2 }, false));
	breakDeliver(getDateTime("输入起始日期(默认不限):", &csp->reqDateStart, false));
	breakDeliver(getDateTime("输入截止日期(默认不限):", &csp->reqDateEnd, false));
	while (1) {
		printf("输入第%d件赠品(或输入quit结束)\n", csp->giftNum + 1);
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
		drawMenu(renderer, (Coord) { 20, 3 }, "单品优惠方案详情", 1, 1,
			"退出");
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
	char filterOpt[2][20] = { "商品筛选","取消筛选" };
	SSP* ssp = NULL;
	CSP* csp = NULL;
	Inventory* filterList=NULL,*inv=NULL;
	int select, num;
	Product filter;
	while (1) {
		renderClear(gdata.renderer);
		drawListPage(gdata.renderer, SSPListPos, "单品销售方案列表", drawSSPList, 
			listShowPageJump(&gdata.SSP->list, &SSPPageStart, PageSize), 
			SSPPageStart, PageSize, (Coord) { 18, 30 });
		drawListPage(gdata.renderer, CSPListPos, "组合销售方案列表", drawCSPList, 
			listShowPageJump(&gdata.CSP->list, &CSPPageStart, PageSize), 
			CSPPageStart, PageSize, (Coord) { 18, 30 });
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