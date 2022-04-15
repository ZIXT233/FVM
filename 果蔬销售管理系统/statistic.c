#include"statistics.h"
static const Coord StatsPageMenuPos = { 3,3 };
static const Coord statsPos = { 2,3 }, statsMenuPos = { 25,3 };
static const int PageSize = 15;
void statsPage(FVMO gdata) {
	int invPageStart = 1;
	int select, num;
	bool drawGlobalFinance = false;
	Inventory* finInv = NULL;
	Record dateFilter;
	InvStatsArg isa;
	int inDateFilter = 0;
	char filterOpt[2][20] = { "指定时间段","取消指定时间段" }, filterTitle[50],*titleCur;
	struct tm sdate, edate;
	pageStackPush(pageStackCreate("数据统计"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		if (inDateFilter) {
			localtime_s(&sdate, &dateFilter.time);
			localtime_s(&edate, &dateFilter.lastTime);
			titleCur = filterTitle + sprintf_s(filterTitle, 10, "数据统计");
			if (dateFilter.time != TIME_NAN) titleCur += sprintf_s(titleCur, 20, "(%d/%d/%d - ", sdate.tm_year + 1900, sdate.tm_mon + 1, sdate.tm_mday);
			else titleCur += sprintf_s(titleCur, 20, "(不限 - ");
			if (dateFilter.lastTime != TIME_NAN) titleCur += sprintf_s(titleCur, 20, "%d/%d/%d)", edate.tm_year + 1900, edate.tm_mon + 1, edate.tm_mday);
			else titleCur += sprintf_s(titleCur, 20, "不限)");
			isa = (InvStatsArg){ &dateFilter,&gdata };
			drawListPage(gdata.renderer, statsPos, filterTitle, drawInvStatsList, &gdata.inventory->list, &invPageStart, PageSize, statsRectSize, &isa);
		}
		else {
			isa = (InvStatsArg){ NULL,&gdata };
			drawListPage(gdata.renderer, statsPos, "数据统计", drawInvStatsList, &gdata.inventory->list, &invPageStart, PageSize, statsRectSize, &isa);
		}

		drawMenu(gdata.renderer, statsMenuPos, "数据统计",6, 1,
			"上一页",
			"下一页",
			"查看商品记录",
			filterOpt[inDateFilter],
			"数据核对",
			"退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select) {
		case 1:
			invPageStart -= PageSize;
			if (invPageStart < 1) invPageStart = 1;
			break;
		case 2:
			invPageStart += PageSize;
			break;
		case 3:
			drawGlobalFinance = false;
			break;
		case 4:
			if (inDateFilter) {
				inDateFilter = 0;
			}
			else {
				memset(&dateFilter, 0, sizeof(Record));
				dateFilter.time = dateFilter.lastTime = dateFilter.prod.expiration = TIME_NAN;
				breakCatch(getDateTime("起始日期(年.月.日)(默认不限) : ", &dateFilter.time, false)) break;
				breakCatch(getDateTime("截止日期(年.月.日)(默认不限) : ", &dateFilter.lastTime, false)) break;
				if(dateFilter.lastTime!=TIME_NAN)dateFilter.lastTime += 3600 * 24 - 1;
				inDateFilter = 1;
			}
			break;
		case 5:
			break;
		case 6:
			pageStackPop(gdata.pageStack);
			return;
		default:
			break;
		}
	}
}
