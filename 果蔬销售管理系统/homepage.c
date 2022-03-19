#include"homepage.h"

void homepage(FVMO gdata) {
	while (1) {
		cls();
		drawMenu("Homepage", 3,1, "销售系统", "库存管理","退出");
		int select = getSelect();
		switch (select)
		{
		case 1:
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