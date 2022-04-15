#include"homepage.h"
#include"inv_manage.h"
#include"sale.h"
#include"sale_plan_manage.h"
#include"statistics.h"

static const Coord StatsPageMenuPos = { 3,3 };
void homepage(FVMO gdata) {
	pageStackPush(pageStackCreate("Homepage"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawMenu(gdata.renderer,StatsPageMenuPos,"Homepage", 5,1, "销售系统", "库存管理","销售方案管理","数据统计", "退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		int select = getSelect();
		switch (select)
		{
		case 1:
			sale(gdata);
			break;
		case 2:
			invManage(gdata);
			break;
		case 3:
			salePlanManage(gdata);
			break;
		case 4:
			statsPage(gdata);
			break;
		case 5:
			pageStackPop(gdata.pageStack);
			return;
		default:
			break;
		}
	}
}