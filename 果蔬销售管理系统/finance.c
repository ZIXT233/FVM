#include<stdlib.h>
#include"finance.h"

Finance* financeCreate() {
	return (Finance*)malloc(sizeof(Finance));
}
Finance* financeInit(Finance* finance,double startUpCapital) {
	finance->balance = finance->startUpCapital = startUpCapital;
	finance->profit = finance->cost = finance->turnover = 0;
	return finance;
}
Finance* financeExpend(Finance* finance,double amount) {
	finance->cost += amount;
	finance->profit -= amount;
	finance->balance -= amount;
}
Finance* financeIncome(Finance* finance, double amount) {
	finance->turnover += amount;
	finance->profit += amount;
	finance->balance += amount;
}