#include"system_manage.h"

static Coord setupPagePos = { 10,15 }, setupPageRectSize = { 4,60 };
static Coord settingPagePos = { 10,15 }, settingPageRectSize = { 4,60 }, systemManageMenuPos = { 22,15 };
void setupPage(double *pCapital,time_t* pTime,uint8_t *hash) {
	int inputCur = 0,valWidth,select;
	Renderer* renderer = rendererCreate(2000);
	char passwd[INFOMAX];
	valWidth = setupPageRectSize.y - 13;
	CellData settings[3][2] = { {{drawCellStr,12,0,"��ʼ�ʽ�"},{drawCellStr,valWidth,0,"δ�趨"}},
								{{drawCellStr,12,0,"��ʼʱ��"},{drawCellStr,valWidth,0,"δ�趨"}},
								{{drawCellStr,12,0,"����Ա����"},{drawCellStr,valWidth,0,"δ�趨"}} };
	Coord welcomPos = setupPagePos,cur;
	welcomPos.x += 3, welcomPos.y++;
	while (1) {
		renderClear(renderer);
		drawTitleWindow(renderer, setupPagePos, "Setup", setupPageRectSize);
		cur = welcomPos;
		drawColorBar(renderer, cur, 238, 232, 213, setupPageRectSize.y);
		coordPrintf(renderer, cur, "  ��ӭʹ�ù������۹���ϵͳ! ����ʹ�ã����趨��ʼ��Ϣ:");
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
			breakCatch(getDoubleInput("�����ʼ�ʽ�:", pCapital, UPRINCERANGE, true)) break;
			*pCapital = centRound(*pCapital);
			settings[inputCur++][1] = (CellData){ drawCellDouble,valWidth,0,pCapital };
			break;
		case 1:
			drawOrdMenu("ʱ��ģʽ", 2, 1, "����ʱ��", "�������ϵͳʱ��");
			breakCatch(getUIntInput("ѡ��ʱ��ģʽ:", &select, (IntRange) { 1, 2 }, true)) break;
			if (select == 1) {
				breakCatch(getTimeInput("�����ʼʱ��(��.��.��/ʱ:��:��):", pTime, true)) break;
				settings[inputCur++][1] = (CellData){ drawCellTime,valWidth,0,pTime };
			}
			else if (select == 2) {
				settings[inputCur++][1] = (CellData){ drawCellStr,valWidth,0,"�������ϵͳʱ��"};
				*pTime = TIME_NAN;
			}
			break;
		case 2:
			breakCatch(getStrInput("�������Ա����:", passwd, INFOMAX, true)) break;
			sha256(passwd, strlen(passwd), hash);
			return;
		default:
			break;
		}
	}
}
static const Coord timeSettingMenuPos = { 3,3 };
void timeSetting(FVMO gdata) {
	pageStackPush(pageStackCreate("ʱ���趨"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawMenu(gdata.renderer, timeSettingMenuPos, "ʱ���趨", 4, 1, 
			"���һ��",
			"�����ʮ��", 
			"ֱ���趨ʱ��",
			"�˳�");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		int select = getSelect();
		switch (select)
		{
		case 1:
			if (gdata.timer->FVMTime == TIME_NAN) {
				printf("��֧�ֶ���ʱ��ģʽ\n");
				getchar();
				break;
			}
			FVMTimerSetFVMTime(gdata.timer,FVMTimerGetFVMTime(gdata.timer) + 3600 * 24);
			break;
		case 2:
			if (gdata.timer->FVMTime == TIME_NAN) {
				printf("��֧�ֶ���ʱ��ģʽ\n");
				getchar();
				break;
			}
			FVMTimerSetFVMTime(gdata.timer,FVMTimerGetFVMTime(gdata.timer) + 3600 * 24 *30);
			break;
		case 3:
			int num;
			time_t fvmtime;
			drawOrdMenu("ʱ��ģʽ", 2, 1, "����ʱ��", "�������ϵͳʱ��");
			breakCatch(getUIntInput("ѡ��ʱ��ģʽ:", &num, (IntRange) { 1, 2 }, true)) break;
			if (num == 1) {
				breakCatch(getTimeInput("����ʱ��(��.��.��/ʱ:��:��):", &fvmtime, true)) break;
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

int demoDelay = 0;  //ȫ�ֱ���������readline

void systemManage(FVMO gdata) {
	int valWidth, select;
	valWidth = settingPageRectSize.y - 13;
	CellData settings[2][2] = { {{drawCellStr,12,0,"ʱ��ģʽ"}, { drawCellStr,valWidth,0,NULL}},
								{{drawCellStr,12,0,"��ʾ��ʱ"}, { drawCellInt,valWidth,0,&demoDelay}} };
	Coord settingPos = settingPagePos, cur;
	settingPos.x += 3, settingPos.y++;
	pageStackPush(pageStackCreate("ϵͳ����"), gdata.pageStack);
	while (1) {
		renderClear(gdata.renderer);
		drawStatusBar(gdata.renderer, STATUS_ORIGIN, gdata);
		drawTitleWindow(gdata.renderer, settingPagePos, "ϵͳ�趨", settingPageRectSize);
		cur = settingPos;
		if (gdata.timer->FVMTime == TIME_NAN) {
			settings[0][1].data = "�������ϵͳʱ��";
		}
		else {
			settings[0][1].data = "����ʱ��";
		}
		for (int i = 0; i < 2; i++) {
			if (i & 1)   drawColorBar(gdata.renderer, cur, 238, 232, 213, settingPageRectSize.y);
			else  resetBackgroundColor(gdata.renderer);
			drawListItem(gdata.renderer, cur, settings[i], 2);
			cur.x++;
		}
		drawMenu(gdata.renderer, systemManageMenuPos, "ϵͳ����", 4, 1,
			"ʱ���趨", "�޸Ĺ���Ա����", "������ʾ��ʱ", "�˳�");
		inputStart(gdata.renderer, INPUT_ORIGIN);
		renderPresent(gdata.renderer);
		select = getSelect();
		switch (select) {
		case 1:
			timeSetting(gdata);
			break;
		case 2:
			char passwd[INFOMAX];
			breakCatch(getStrInput("�������Ա����:", passwd, INFOMAX, true)) break;
			if (passwdVerify(passwd, gdata.passwdSha256)) {
				breakCatch(getStrInput("�����¹���Ա����:", passwd, INFOMAX, true)) break;
				sha256(passwd, strlen(passwd), gdata.passwdSha256);
				printf("���óɹ�\n");
				getchar();
			}
			else {
				printf("�������!");
				Sleep(3000);
			}
			break;
		case 3:
			breakCatch(getUIntInput("������ʾ��ʱ(����):", &demoDelay, (IntRange) { 0, 60000 }, true)) break;
			break;
		case 4:
			pageStackPop(gdata.pageStack);
			return;
		default:
			break;
		}
	}
}

