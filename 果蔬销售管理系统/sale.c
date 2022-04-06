#include<string.h>
#include"sale.h"
static const int PageSize = 15;
Coord cartPos = { 2,83 }, cartMenuPos = { 21,83 };


void cartAdd(Inventory* cart, Inventory* head) {
	int id;
	Inventory* item,*inv;
	breakDeliver(inputInventoryID(head, &id, &inv));	//查询库存信息
	item = invQueryID(cart, id);						//查询购物车中是否已存在
	int quantity;
	double weight;
	if (item == NULL) {
		Inventory* item = invCreate();
		memcpy_s(item, sizeof(Inventory), inv, sizeof(Inventory));
		if (item->prod.pack == UNIT) {
			breakDeliver(getUIntInput("请输入购买数量:", &quantity, (IntRange) { 1, inv->prod.quantity }, true));
			item->prod.quantity = quantity;
		}
		else if (item->prod.pack == BULK) {
			breakDeliver(getDoubleInput("请输入购买重量:", &weight, (DoubleRange) { 0.01, inv->prod.weight }, true));
			item->prod.weight = weight;
		}
		listAddTail(&item->list, &cart->list);
	}
	else {
		if (item->prod.pack == UNIT) {
			breakDeliver(getUIntInput("请输入购买数量:", &quantity, (IntRange) { 1, inv->prod.quantity - item->prod.quantity }, true)); //确保增加后不超出库存量
			item->prod.quantity += quantity;
		}
		else if (inv->prod.pack == BULK) {
			breakDeliver(getDoubleInput("请输入购买重量:", &weight, (DoubleRange) { 0.01, inv->prod.weight - item->prod.weight }, true));
			item->prod.weight += weight;
		}
	}

}
void cartDelete(Inventory* cart) {
	int id;
	Inventory* inv;
	breakDeliver(inputInventoryID(cart, &id, &inv));
	if (inv) {
		listRemove(&inv->list);
	}
}
void cartQuantityModify(Inventory* cart, Inventory* head) {
	int id;
	Inventory* item, * inv;
	breakDeliver(inputInventoryID(head, &id, &inv));	//查询库存信息
	item = invQueryID(cart, id);						//查询购物车中是否已存在
	int quantity;
	double weight;
	if (item) {
		if (item->prod.pack == UNIT) {
			breakDeliver(getUIntInput("请输入新购买数量:", &quantity, (IntRange) { 1, inv->prod.quantity }, true)); //确保增加后不超出库存量
			item->prod.quantity = quantity;
		}
		else if (inv->prod.pack == BULK) {
			breakDeliver(getDoubleInput("请输入新购买重量:", &weight, (DoubleRange) { 0.01, inv->prod.weight }, true));
			item->prod.weight = weight;
		}
	}
	else {
		printf("购物车中无此商品");
	}

}
void cartClear(Inventory* cart) {
	invListClear(cart);
}
void sale(FVMO gdata) {
	int inFilter = 0;
	int pageStart = 1, pageStartSave = 1, cartPageStart = 1;
	char filterOpt[2][20] = { "商品筛选","取消筛选" };
	Inventory* filterList = NULL, * inv = NULL, * cart = invListInit(invCreate());
	int select, num;
	Product filter;
	while (1) {
		cls();
		drawInvPage(cartPos, "购物车",invShowPageJump(cart, &cartPageStart, PageSize), cartPageStart, PageSize);
		drawMenu(cartMenuPos, "购物车", 5, 11,
			"上一页",
			"下一页",
			"移除商品",
			"数量更改",
			"清空购物车");
		if (inFilter) {
			filterList = invFilterListGen(gdata.inventory, &filter);
			drawInvPage(saleListPos,"筛选信息", invShowPageJump(filterList, &pageStart, PageSize), pageStart, PageSize);
			invListClear(filterList);
			free(filterList);
		}
		else {
			drawInvPage(saleListPos, "库存信息", invShowPageJump(gdata.inventory, &pageStart, PageSize), pageStart, PageSize);
		}
		drawMenu(saleMenuPos, "商品销售", 7, 1,
			"上一页",
			"下一页",
			filterOpt[inFilter],
			"商品详情",
			"加入购物车",
			"前往结算",
			"退出");
		inputStart(INPUT_ORIGIN);
		select = getSelect();
		switch (select)
		{
		case 1:
			pageStart -= PageSize;
			if (pageStart < 1) pageStart = 1;
			break;
		case 2:
			pageStart += PageSize;
			break;
		case 3:
			if (inFilter) {  //取消筛选
				inFilter = false;
				pageStart = pageStartSave;
			}
			else if (!inFilter) {
				breakCatch(inputProductFilter(&filter)) {
					break;
				}
				inFilter = true;
				pageStartSave = pageStart;
				pageStart = 1;
			}
			break;
		case 4:
			breakCatch(inputInventoryID(gdata.inventory, &num, &inv)) break;
			invDetails(inv);
			break;
		case 5:
			cartAdd(cart,gdata.inventory);
			break;
		case 6:
			//settlement(gdata, cart);
			break;
		case 7:
			return;
		case 11:
			cartPageStart -= PageSize;
			if (cartPageStart < 1) cartPageStart = 1;
			break;
		case 12:
			cartPageStart += PageSize;
			break;
		case 13:
			cartDelete(cart);
			break;
		case 14:
			cartQuantityModify(cart, gdata.inventory);
			break;
		case 15:
			cartClear(cart);
			break;
		default:
			break;
		}
	}
}