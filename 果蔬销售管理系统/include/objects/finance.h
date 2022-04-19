#ifndef FINANCE_H
#define FINANCE_H
typedef struct tagFinance {
	double startUpCapital,balance,turnover,profit,cost;
}Finance;

Finance* financeCreate();
Finance* financeInit(Finance* finance,double startUpCapital);
Finance* financeExpend(Finance* finance,double amount);
Finance* financeIncome(Finance* finance,double amount);
#endif