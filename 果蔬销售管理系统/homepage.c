#include"homepage.h"
#include"inv_manage.h"
#include"sale.h"

void homepage(FVMO gdata) {
	while (1) {
		cls();
		drawMenu(UI_ORIGIN,"Homepage", 3,1, "����ϵͳ", "������","�˳�");
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