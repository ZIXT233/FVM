#include<stdlib.h>
#include"finance.h"
#include"product.h"

Finance* financeCreate() {
	return (Finance*)malloc(sizeof(Finance));
}
Finance* financeInit(Finance* finance, double startUpCapital) {
	finance->balance = finance->startUpCapital = startUpCapital;
	finance->profit = finance->cost = finance->turnover = 0;
	return finance;
}
Finance* financeExpend(Finance* finance, double amount) {
	finance->cost = centRound(finance->cost + amount);
	finance->profit = centRound(finance->profit - amount);
	finance->balance = centRound(finance->balance - amount);
}
Finance* financeIncome(Finance* finance, double amount) {
	finance->turnover = centRound(finance->turnover + amount);
	finance->profit = centRound(finance->profit + amount);
	finance->balance = centRound(finance->balance + amount);
}