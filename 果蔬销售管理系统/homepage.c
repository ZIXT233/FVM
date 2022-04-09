#include"homepage.h"
#include"inv_manage.h"
#include"sale.h"
#include"sale_plan_manage.h"

void homepage(FVMO gdata) {
	while (1) {
		renderClear(gdata.renderer);
		drawMenu(gdata.renderer,UI_ORIGIN,"Homepage", 4,1, "����ϵͳ", "������","���۷�������", "�˳�");
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
			return;
		default:
			break;
		}
	}
}