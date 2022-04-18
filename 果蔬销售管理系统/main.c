#include<stdio.h>
#include<time.h>
#include<Windows.h>
#include"fvm_objects.h"
#include"homepage.h"
#include"data_storage.h"
#include"sha256.h"

FILE* stdinLog;


void setConsoleEnv() {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD outMode;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleMode(hStdout, &outMode);
	SetConsoleMode(hStdout, (outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING));
	HWND hwnd = GetForegroundWindow();
	ShowWindow(hwnd, SW_MAXIMIZE);
}

static const char storageDir[20] = "FVMData";
FVMO* FVMSystemInit() {
	FVMO* gdata = NULL;
	setConsoleEnv();
	int stat = storageStat(storageDir);
	if (stat == STORAGE_AVAILABLE) {
		gdata = storageLoadFVMO(storageDir);
	}
	else if (stat == STORAGE_NO_EXIST) {
		//ȫ�°�װ
		storageCreate(storageDir);
		gdata = (FVMO*)malloc(sizeof(FVMO));
		gdata->inventory = invListInit(invCreate());
		gdata->record = recordListInit(recordCreate());
		gdata->SSP = SSPListInit(SSPCreate());
		gdata->CSP = CSPListInit(CSPCreate());
		
	}
	else if (stat == STORAGE_INCOMPLETE) {
		//����
		printf("�ļ���ȫ,�س��������ļ���ȫ");
		getchar();
		gdata = storageLoadFVMO(storageDir);
		if (!gdata->inventory) gdata->inventory = invListInit(invCreate());
		if (!gdata->record) gdata->record = recordListInit(recordCreate());
		if (!gdata->SSP) gdata->SSP = SSPListInit(SSPCreate());
		if (!gdata->CSP) gdata->CSP = CSPListInit(CSPCreate());
	}
	gdata->historyInventory = invListInit(invCreate());
	gdata->finance = financeInit(financeCreate(),100000);
	gdata->order = recordListInit(recordCreate());
	gdata->renderer = rendererCreate(50000);
	gdata->pageStack = pageStackCreate(NULL);
	gdata->timer = FVMTimerCreate(TIME_NAN, NULL, NULL);
	return gdata;
}
HANDLE f_Mutex;
int main(void) {
	time_t ti;
	time(&ti);
	char stdinLogFilename[100];
	sprintf_s(stdinLogFilename, 100, "stdin-%lld.log", ti);
	fopen_s(&stdinLog, stdinLogFilename, "w");
	f_Mutex = CreateMutex(NULL, FALSE, NULL);
	srand(NULL);
	FVMO* gdata = FVMSystemInit();
	FVMTimerSetClockTask(gdata->timer, clockUpdateTimeBar, gdata);
	homepage(*gdata);
	rendererDelete(gdata->renderer);
	storageSaveFVMO(storageDir, gdata);
	return 0;
}

//�����>=17
//config����
//��Ʒ��Ϣ���� *
//��¼����  *
//����
//��Ʒ�¼� *
//�˻��� *
//��Ա�Żݽ����Ż�
//�˵���С
//ʱ��ģ���޸�
//prod����ӵ�λ��