#include"system_manage.h"

static Coord setupPagePos = { 10,15 }, setupPageRectSize = { 4,60 };
static Coord settingPagePos = { 10,15 }, settingPageRectSize = { 4,60 }, systemManageMenuPos = { 22,15 };
void setupPage(double *pCapital,time_t* pTime,uint8_t *hash) {
	int inputCur = 0,valWidth,select;
	Renderer* renderer = rendererCreate(2000);
	char passwd[INFOMAX];
	valWidth = setupPageRectSize.y - 13;
	CellData settings[3][2] = { {{drawCellStr,12,0,"初始资金"},{drawCellStr,valWidth,0,"未设定"}},
								{{drawCellStr,12,0,"初始时间"},{drawCellStr,valWidth,0,"未设定"}},
								{{drawCellStr,12,0,"管理员密码"},{drawCellStr,valWidth,0,"未设定"}} };
	Coord welcomPos = setupPagePos,cur;
	welcomPos.x += 3, welcomPos.y++;
	while (1) {
		renderClear(renderer);
		drawTitleWindow(renderer, setupPagePos, "Setup", setupPageRectSize);
		cur = welcomPos;
		drawColorBar(renderer, cur, 238, 232, 213, setupPageRectSize.y);
		coordPrintf(renderer, cur, "  欢迎使用果蔬销售管理系统! 初次使用，请设定初始信息:");
		cur.x++;
		for (int i = 0; i < 3; i++) {
			if (i & 1)   drawColorBar(renderer, cur, 238, 232, 213, setupPageRectSize.y);
			else  resetBackgroundColor(renderer);
			drawListItem(renderer, cur, settings[i], 2);
			cur.x++;
		}
		inputStart(renderer, INPUT_ORIGIN);
		renderPresent(renderer);
		switch (inputCur) {
		case 0:
			breakCatch(getDoubleInput("输入初始资金:", pCapital, UPRINCERANGE, true)) break;
			*pCapital = centRound(*pCapital);
			settings[inputCur++][1] = (CellData){ drawCellDouble,valWidth,0,pCapital };
			break;
		case 1:
			drawOrdMenu("时间模式", 2, 1, "独立时间", "跟随操作系统时间");
			breakCatch(getUIntInput("选择时间模式:", &select, (IntRange) { 1, 2 }, true)) break;
			if (select == 1) {
				breakCatch(getTimeInput("输入初始时间(年.月.日/时:分:秒):", pTime, true)) break;
				settings[inputCur++][1] = (CellData){ drawCellTime,valWidth,0,pTime };
			}
			else if (select == 2) {
				settings[inputCur++][1] = (CellData){ drawCellStr,valWidth,0,"跟随操作系统时间"};
				*pTime = TIME_NAN;
			}
			break;
		case 2:
			breakCatch(getStrInput("输入管理员密码:", passwd, INFOMAX, true)) break;
			sha256(passwd, strlen(passwd), hash);
			return;
		default:
			break;
		}
	}
}
static const Coord timeSettingMenuPos = { 3,3 };
void timeSetting(FVMO gdata) {
	pageStackPush(pageStackCreate("时间设定"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawMenu(gdata.renderer, timeSettingMenuPos, "时间设定", 4, 1, 
			"向后一天",
			"向后三十天", 
			"直接设定时间",
			"退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		int select = getSelect();
		switch (select)
		{
		case 1:
			if (gdata.timer->FVMTime == TIME_NAN) {
				printf("仅支持独立时间模式\n");
				getchar();
				break;
			}
			FVMTimerSetFVMTime(gdata.timer,FVMTimerGetFVMTime(gdata.timer) + 3600 * 24);
			break;
		case 2:
			if (gdata.timer->FVMTime == TIME_NAN) {
				printf("仅支持独立时间模式\n");
				getchar();
				break;
			}
			FVMTimerSetFVMTime(gdata.timer,FVMTimerGetFVMTime(gdata.timer) + 3600 * 24 *30);
			break;
		case 3:
			int num;
			time_t fvmtime;
			drawOrdMenu("时间模式", 2, 1, "独立时间", "跟随操作系统时间");
			breakCatch(getUIntInput("选择时间模式:", &num, (IntRange) { 1, 2 }, true)) break;
			if (num == 1) {
				breakCatch(getTimeInput("输入时间(年.月.日/时:分:秒):", &fvmtime, true)) break;
				FVMTimerSetFVMTime(gdata.timer, fvmtime);
			}
			else if (num == 2) {
				FVMTimerSetFVMTime(gdata.timer, TIME_NAN);
			}
			break;
		case 4:
			pageStackPop(gdata.pageStack);
			return;
		default:
			break;
		}
	}
}

int demoDelay = 0;  //全局变量传递至readline

void systemManage(FVMO gdata) {
	int valWidth, select;
	valWidth = settingPageRectSize.y - 13;
	CellData settings[2][2] = { {{drawCellStr,12,0,"时间模式"}, { drawCellStr,valWidth,0,NULL}},
								{{drawCellStr,12,0,"演示延时"}, { drawCellInt,valWidth,0,&demoDelay}} };
	Coord settingPos = settingPagePos, cur;
	settingPos.x += 3, settingPos.y++;
	pageStackPush(pageStackCreate("系统管理"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawTitleWindow(gdata.renderer, settingPagePos, "系统设定", settingPageRectSize);
		cur = settingPos;
		if (gdata.timer->FVMTime == TIME_NAN) {
			settings[0][1].data = "跟随操作系统时间";
		}
		else {
			settings[0][1].data = "独立时间";
		}
		for (int i = 0; i < 2; i++) {
			if (i & 1)   drawColorBar(gdata.renderer, cur, 238, 232, 213, settingPageRectSize.y);
			else  resetBackgroundColor(gdata.renderer);
			drawListItem(gdata.renderer, cur, settings[i], 2);
			cur.x++;
		}
		drawMenu(gdata.renderer, systemManageMenuPos, "系统管理", 4, 1,
			"时间设定", "修改管理员密码", "设置演示延时", "退出");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select) {
		case 1:
			timeSetting(gdata);
			break;
		case 2:
			char passwd[INFOMAX];
			breakCatch(getStrInput("输入管理员密码:", passwd, INFOMAX, true)) break;
			if (passwdVerify(passwd, gdata.passwdSha256)) {
				breakCatch(getStrInput("输入新管理员密码:", passwd, INFOMAX, true)) break;
				sha256(passwd, strlen(passwd), gdata.passwdSha256);
				printf("设置成功\n");
				getchar();
			}
			else {
				printf("密码错误!");
				Sleep(3000);
			}
			break;
		case 3:
			breakCatch(getUIntInput("输入演示延时(毫秒):", &demoDelay, (IntRange) { 0, 60000 }, true)) break;
			break;
		case 4:
			pageStackPop(gdata.pageStack);
			return;
		default:
			break;
		}
	}
}

