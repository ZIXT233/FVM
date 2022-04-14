#ifndef TIMER_H
#define TIMER_H
#include<time.h>
#include<Windows.h>
#include<process.h>
#include<stdbool.h>

#define TIME_NAN (0x3f3f3f3f3f3f3f3f)

typedef  _beginthreadex_proc_type FVMClockTask;
typedef struct tagFVMTimer {
	time_t OSTime;
	time_t FVMTime,FVMTimeOffset;
	BOOL clockOn;
	FVMClockTask clockTask;
	void* clockTaskArgs;
	HANDLE timeMutex;
	unsigned clockThreadID;
	HANDLE clockThreadHANDLE;
}FVMTimer;
static const int BLOCKTIME_INIT = 960;
FVMTimer* FVMTimerCreate(time_t FVMTime, FVMClockTask clockTask, void* clockTaskArgs);

time_t FVMTimerGetOSTime(FVMTimer* timer);
time_t FVMTimerGetFVMTime(FVMTimer* timer);
void FVMTimerSetFVMTime(FVMTimer* timer, time_t fvmtime);
void FVMTimerSetClockTask(FVMTimer* timer, FVMClockTask clockTask, void* clockTaskArgs);
unsigned __stdcall FVMTimerClock(LPVOID lpParam);
void FVMTimeClockOn(FVMTimer* timer);
void timerClockOff(FVMTimer* timer);

#endif