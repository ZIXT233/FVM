#include<stdio.h>
#include<time.h>
#include<Windows.h>
#include"fvm_objects.h"
#include"homepage.h"
#include"data_storage.h"

FILE* stdinLog;


void setConsoleEnv() {
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD outMode;
	GetConsoleMode(hStdout, &outMode);
	SetConsoleMode(hStdout, (outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING));
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
		//全新安装
		storageCreate(storageDir);
		gdata = (FVMO*)malloc(sizeof(FVMO));
		gdata->inventory = invListInit(invCreate());
		gdata->record = recordListInit(recordCreate());
		gdata->SSP = SSPListInit(SSPCreate());
		gdata->CSP = CSPListInit(CSPCreate());
	}
	else if (stat == STORAGE_INCOMPLETE) {
		//问问
		printf("文件不全,回车键用新文件补全");
		getchar();
		gdata = storageLoadFVMO(storageDir);
		if (!gdata->inventory) gdata->inventory = invListInit(invCreate());
		if (!gdata->record) gdata->record = recordListInit(recordCreate());
		if (!gdata->SSP) gdata->SSP = SSPListInit(SSPCreate());
		if (!gdata->CSP) gdata->CSP = CSPListInit(CSPCreate());
	}
	gdata->order = recordListInit(recordCreate());
	gdata->renderer = rendererCreate(10000);
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