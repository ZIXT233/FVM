#include"homepage.h"
#include"inv_manage.h"
#include"sale.h"

void homepage(FVMO gdata) {
	while (1) {
		renderClear(gdata.renderer);
		drawMenu(gdata.renderer,UI_ORIGIN,"Homepage", 3,1, "����ϵͳ", "������","�˳�");
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
			return;
		default:
			break;
		}
	}
}