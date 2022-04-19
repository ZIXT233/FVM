#include<stdio.h>
#include<time.h>
#include<Windows.h>
#include"fvm_objects.h"
#include"homepage.h"
#include"data_storage.h"
#include"system_manage.h"

FILE* stdinLog;
static const char storageDir[20] = "FVMData";

void setConsoleEnv() {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD outMode;
	CONSOLE_SCREEN_BUFFER_INFOEX csbi;
	csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	GetConsoleScreenBufferInfoEx(hStdout, &csbi);
	csbi.ColorTable[15] = RGB(253,246,227);
	csbi.dwSize.X = PanelSize.y;
	SetConsoleScreenBufferInfoEx(hStdout, &csbi);
	GetConsoleMode(hStdout, &outMode);
	SetConsoleMode(hStdout, (outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING));
	HWND hwnd = GetForegroundWindow();
	ShowWindow(hwnd, SW_MAXIMIZE);
}


FVMO* FVMSystemInit() {
	FVMO* gdata = NULL;
	setConsoleEnv();
	int stat = storageStat(storageDir);
	if (stat == STORAGE_AVAILABLE) {
		gdata = storageLoadFVMO(storageDir);
	}
	else if (stat == STORAGE_NO_EXIST) {
		//ȫ�°�װ
		double capital;
		time_t fvmtime;
		uint8_t hash[32];
		gdata = (FVMO*)malloc(sizeof(FVMO));
		setupPage(&capital,&fvmtime,hash);
		gdata->timer = FVMTimerCreate(fvmtime, NULL, NULL);
		gdata->finance = financeInit(financeCreate(),capital);
		memcpy(gdata->passwdSha256, hash, 32);

		gdata->inventory = invListInit(invCreate());
		gdata->historyInventory = invListInit(invCreate());
		gdata->record = recordListInit(recordCreate());
		gdata->SSP = SSPListInit(SSPCreate());
		gdata->CSP = CSPListInit(CSPCreate());
		
	}
	else if (stat == STORAGE_INCOMPLETE) {
		//����
		printf("�����ļ���ȫ,�س����ÿ��ļ���ȫ");
		getchar();
		gdata = storageLoadFVMO(storageDir);
		if (!gdata->inventory) gdata->inventory = invListInit(invCreate());
		if (!gdata->historyInventory) gdata->historyInventory = invListInit(invCreate());
		if (!gdata->record) gdata->record = recordListInit(recordCreate());
		if (!gdata->SSP) gdata->SSP = SSPListInit(SSPCreate());
		if (!gdata->CSP) gdata->CSP = CSPListInit(CSPCreate());
		gdata->renderer = rendererCreate(50000);
	}
	else if (stat == STORAGE_NO_CONFIG) {
		printf("�����ļ���ʧ��ϵͳ����������");
		getchar();
		return NULL;
	}
	gdata->renderer = rendererCreate(50000);
	gdata->pageStack = pageStackCreate(NULL);
	return gdata;
}
int main(void) {
	time_t ti;
	time(&ti);
	char stdinLogFilename[100];
	sprintf_s(stdinLogFilename, 100, "stdin-%lld.log", ti);
	fopen_s(&stdinLog, stdinLogFilename, "w");
	srand(NULL);
	FVMO* gdata = FVMSystemInit();
	if (!gdata) return 0;
	FVMTimerSetClockTask(gdata->timer, clockUpdateTimeBar, gdata);
	homepage(gdata, storageDir);
	rendererDelete(gdata->renderer);
	storageSaveFVMO(storageDir, gdata);
	return 0;
}

//�����>=17
//config���� *
//��Ʒ��Ϣ���� *
//��¼����  *
//���� *
//��Ʒ�¼� *
//�˻��� *
//��Ա�Żݽ����Ż� *
//�˵���С *
//ʱ��ģ���޸� *
//prod����ӵ�λ�� *