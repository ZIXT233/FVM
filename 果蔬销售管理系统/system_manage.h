#ifndef SYSTEM_MANAGE_H
#define SYSTEM_MANAGE_H
#include<stdlib.h>
#include<stdio.h>
#include"UI.h"
#include"fvm_objects.h"
#include"data_storage.h"
#include"sha256.h"
#include"timer.h"

void setupPage(double* pCapital, time_t* pTime, uint8_t* hash);
//void systemManage(FVMO* gdata);
void systemManage(FVMO gdata);
#endif