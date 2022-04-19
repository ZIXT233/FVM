#include"homepage.h"
#include"inv_manage.h"
#include"sale.h"
#include"sale_plan_manage.h"
#include"data_storage.h"
#include"statistics.h"

static const Coord homepageMenuPos = { 3,3 },homepageMenuSize = { 15, 40};
void homepage(FVMO *gdata, char* storageDir) {
	char passwd[INFOMAX];
	pageStackPush(pageStackCreate("Homepage"), gdata->pageStack);
	while (1) {
		renderClear(gdata->renderer);
		drawStatusBar(gdata->renderer, STATUS_ORIGIN, gdata);
		drawMenu(gdata->renderer,homepageMenuPos, homepageMenuSize,"Homepage", 7,1, "����ϵͳ", "������","���۷�������","����ͳ��", "ϵͳ����", "���ݱ���", "�˳�");
		inputStart(gdata->renderer, INPUT_ORIGIN);
		renderPresent(gdata->renderer);
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
			breakCatch(getStrInput("�������Ա����:", passwd, INFOMAX, true)) break;
			if (passwdVerify(passwd, gdata->passwdSha256)) {
				systemManage(gdata);;
			}
			else {
				printf("�������!");
				Sleep(3000);
			}
			break;
		case 6:
			storageSaveFVMO(storageDir, gdata);
			break;
		case 7:
			pageStackPop(gdata->pageStack);
			return;
		default:
			break;
		}
	}
}