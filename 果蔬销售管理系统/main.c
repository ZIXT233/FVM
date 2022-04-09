#include<stdio.h>
#include<Windows.h>

#include"fvm_objects.h"
#include"homepage.h"

int main(void) {
	HANDLE hStdout=GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD outMode;
	GetConsoleMode(hStdout, &outMode);
	SetConsoleMode(hStdout, (outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING));
	srand(NULL);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
	FVMO gdata;
	gdata.inventory = invListInit(invCreate());
	gdata.record = recordListInit(recordCreate());
	gdata.SSP = SSPListInit(SSPCreate());
	gdata.CSP = CSPListInit(CSPCreate());
	gdata.renderer = rendererCreate(5000);
	homepage(gdata);
	rendererDelete(gdata.renderer);
	return 0;
} 