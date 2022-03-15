#include<stdio.h>

#include"fvm_objects.h"
#include"homepage.h"

int main(void) {
	srand(NULL);
	FVMO gdata;
	gdata.inventory = invListInit(invCreate());
	gdata.record = recordListInit(recordCreate());
	homepage(gdata);
	return 0;
}