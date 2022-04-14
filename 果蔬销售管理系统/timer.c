#include"timer.h"
FVMTimer* FVMTimerCreate(time_t FVMTime, FVMClockTask clockTask, void* clockTaskArgs) {
	FVMTimer* timer = (FVMTimer*)malloc(sizeof(FVMTimer));
	timer->timeMutex = CreateMutex(NULL, FALSE, L"FVMTimerMutex");
	timer->FVMTime = FVMTime;
	time(&timer->OSTime);
	if (timer->FVMTime != TIME_NAN) timer->FVMTimeOffset = timer->FVMTime - timer->OSTime;
	else timer->FVMTimeOffset = 0;
	timer->clockOn = true;
	timer->clockTask = clockTask;
	timer->clockTaskArgs = clockTaskArgs;
	timer->clockThreadHANDLE=_beginthreadex(NULL, 0, FVMTimerClock, timer, 0, &timer->clockThreadID);
	return timer;
}
unsigned __stdcall FVMTimerClock(LPVOID lpParam) {
	FVMTimer* timer = (FVMTimer*)lpParam;
	time_t ti, tiOld;
	unsigned clockTaskID;
	int queryCnt = 0, blocktime = BLOCKTIME_INIT, mul = 1;
	time(&ti);
	tiOld = ti;
	for (tiOld = ti; tiOld == ti; queryCnt++) {
		Sleep(1);
		time(&ti);
	}
	while (timer->clockOn) {
		if (blocktime < 0) blocktime = 0;
		if (blocktime >= 1000) blocktime = BLOCKTIME_INIT;
		Sleep(blocktime);
		for (tiOld = ti, queryCnt = 0; tiOld == ti; queryCnt++) {
			Sleep(1);
			time(&ti);
		}
		WaitForSingleObject(timer->timeMutex, INFINITE);
		timer->OSTime = ti;
		if (timer->FVMTime != TIME_NAN) timer->FVMTime = ti + timer->FVMTimeOffset;
		if (timer->clockTask) _beginthreadex(NULL, 0, timer->clockTask, timer->clockTaskArgs, 0, &clockTaskID);
		ReleaseMutex(timer->timeMutex);
		
		if (queryCnt == 1) blocktime -= mul, mul <<= 1;
		else {
			mul = 1;
			blocktime += (queryCnt - 2) << 4;
		}
	}
	return 0;
}

time_t FVMTimerGetOSTime(FVMTimer* timer) {
	time_t OSTime;
	WaitForSingleObject(timer->timeMutex, INFINITE);
	OSTime = timer->OSTime;
	ReleaseMutex(timer->timeMutex);
	return OSTime;
}

time_t FVMTimerGetFVMTime(FVMTimer* timer) {
	time_t FVMTime;
	WaitForSingleObject(timer->timeMutex, INFINITE);
	FVMTime = timer->OSTime+timer->FVMTimeOffset;
	ReleaseMutex(timer->timeMutex);
	return FVMTime;
}

void FVMTimerSetFVMTime(FVMTimer* timer, time_t fvmtime) {
	WaitForSingleObject(timer->timeMutex, INFINITE);
	timer->FVMTime = fvmtime;
	if (timer->FVMTime != TIME_NAN) timer->FVMTimeOffset = timer->FVMTime - timer->OSTime;
	else timer->FVMTimeOffset = 0;
	ReleaseMutex(timer->timeMutex);
}
void FVMTimerSetClockTask(FVMTimer* timer,FVMClockTask clockTask, void* clockTaskArgs) {
	WaitForSingleObject(timer->timeMutex, INFINITE);
	timer->clockTask = clockTask;
	timer->clockTaskArgs = clockTaskArgs;
	ReleaseMutex(timer->timeMutex);
}

void FVMTimeClockOn(FVMTimer* timer) {
	if (timer->clockOn) return;
	timer->clockOn = true;
	timer->clockThreadHANDLE=_beginthreadex(NULL, 0, FVMTimerClock, timer, 0, &timer->clockThreadID);
}

void timerClockOff(FVMTimer* timer) {
	timer->clockOn = false;
	WaitForSingleObject(timer->clockThreadHANDLE, INFINITE);
}