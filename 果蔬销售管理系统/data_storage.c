#include<sys/stat.h>
#include<direct.h>
#include<string.h>
#include<stdio.h>
#include"fvm_objects.h"
#include"data_storage.h"

int storageStat(char storageDir[]) {
	struct _stat buf;
	char filename[FILENAME_MAX * 2], * pathcur;
	int nameLimit;
	if (stat(storageDir, &buf) == 0) {
		if (_S_IFDIR & buf.st_mode) {
			strcpy_s(filename, sizeof(filename), storageDir);
			pathcur = filename + strlen(filename);
			*(pathcur++) = '/';
			nameLimit = sizeof(filename) - (pathcur - filename);
			/*strcpy_s(filename, sizeof(filename), filePath);
			strcat_s(filename, sizeof(filename), "/");
			strcat_s(filename, sizeof(filename), CONFIG_FILENAME);*/
			strcpy_s(pathcur, nameLimit, INVENTORY_FILENAME);
			if (stat(filename, &buf) != 0) return STORAGE_INCOMPLETE;
			strcpy_s(pathcur, nameLimit, RECORD_FILENAME);
			if (stat(filename, &buf) != 0) return STORAGE_INCOMPLETE;
			strcpy_s(pathcur, nameLimit, SALE_PLAN_FILENAME);
			if (stat(filename, &buf) != 0) return STORAGE_INCOMPLETE;
			return STORAGE_AVAILABLE;
		}
	}
	else {
		return STORAGE_NO_EXIST;
	}
}

void storageCreate(char storageDir[]) {
	_mkdir(storageDir);
}


FVMO* storageLoadFVMO(char storageDir[]) {  //读取,无文件则置空
	FVMO* gdata = (FVMO*)malloc(sizeof(FVMO));
	char filepath[FILENAME_MAX * 2], * pathcur;
	strcpy_s(filepath, sizeof(filepath), storageDir);
	pathcur = filepath + strlen(filepath);
	*(pathcur++) = '/';
	int nameLimit = sizeof(filepath) - (pathcur - filepath);

	strcpy_s(pathcur, nameLimit, CONFIG_FILENAME);
	//storageLoadConfig(filepath, gdata);
	
	strcpy_s(pathcur, nameLimit, INVENTORY_FILENAME);
	gdata->inventory = storageLoadInventory(filepath);
	strcpy_s(pathcur, nameLimit, RECORD_FILENAME);
	gdata->record = storageLoadRecord(filepath,gdata->inventory);
	strcpy_s(pathcur, nameLimit, SALE_PLAN_FILENAME);
	storageLoadSalePlan(filepath, &gdata->SSP, &gdata->CSP);
	
	return gdata;
}

int storageSaveFVMO(char storageDir[],FVMO* gdata) {
	char filepath[FILENAME_MAX * 2], * pathcur;
	strcpy_s(filepath, sizeof(filepath), storageDir);
	pathcur = filepath + strlen(filepath);
	*(pathcur++) = '/';
	int nameLimit = sizeof(filepath) - (pathcur - filepath);
	int stat;
	strcpy_s(pathcur, nameLimit, INVENTORY_FILENAME);
	storageSaveInventory(filepath, gdata->inventory);
	strcpy_s(pathcur, nameLimit, RECORD_FILENAME);
	storageSaveRecord(filepath, gdata->record);
	strcpy_s(pathcur, nameLimit, SALE_PLAN_FILENAME);
	storageSaveSalePlan(filepath, gdata->SSP, gdata->CSP);
	return 0;
}

int storageLoadConfig(char filename[], FVMO* fvmo) {
	FILE* cfgFile;
	fopen_s(&cfgFile, filename, "rb");
	if (!cfgFile) return -1;

	FVMTimer timeData;
	if (!fread(&timeData, sizeof(FVMTimer), 1, cfgFile)) return -1;
	fvmo->timer = FVMTimerCreate(timeData.FVMTime, NULL, NULL);

	fvmo->finance = (Finance*)malloc(sizeof(Finance));
	if (!fread(fvmo->finance, sizeof(FVMTimer), 1, cfgFile)) {
		free(fvmo->finance);
		return -1;
	}
	if (!fread(fvmo->passwdSha256, sizeof(fvmo->passwdSha256), 1, cfgFile)) return -1;
	fclose(cfgFile);
	return 0;
}
int storageSaveConfig(char filename[], FVMO* fvmo) {
	FILE* cfgFile;
	fopen_s(&cfgFile, filename, "rb");
	if (!cfgFile) return -1;

	fwrite(fvmo->timer, sizeof(FVMTimer), 1, cfgFile);
	fwrite(fvmo->finance, sizeof(Finance), 1, cfgFile);
	fwrite(fvmo->passwdSha256, sizeof(fvmo->passwdSha256), 1, cfgFile);
	fclose(cfgFile);
	return 0;
}
Inventory* storageLoadInventory(char filename[]) {
	FILE* invFile;
	Inventory* head = NULL, * inv = NULL, data;

	fopen_s(&invFile, filename, "rb");
	if (!invFile) return NULL;
	if (!fread(&data, sizeof(Inventory), 1, invFile)) return NULL;
	head = invCopyCreate(&data);
	head->invRecord = recordListInit(recordCreate());
	listInit(&head->list);
	//head = invListInit(invCreate());
	//head->invID = data.invID;
	//head->invIDCnt = data.invIDCnt;
	int dataSize = data.list.size;
	
	for (int i = 0; i < dataSize; i++) {
		if (!fread(&data, sizeof(Inventory), 1, invFile)) {
			invListClear(head);
			invDel(head);
			return NULL;
		}
		inv = invCopyCreate(&data);
		inv->invRecord = recordListInit(recordCreate());
		/*inv = invCreate();
		inv->invID = data.invID;
		inv->prod = data.prod;*/
		listAddTail(&inv->list, &head->list);
		//商品记录数据由记录加载模块加载
	}
	fclose(invFile);
	return head;
}
int storageSaveInventory(char filename[], Inventory* head) {
	FILE* invFile;
	fopen_s(&invFile, filename, "wb");
	fwrite(head, sizeof(Inventory), 1, invFile);
	listForEachEntry(Inventory, pos, &head->list, list) {
		fwrite(pos, sizeof(Inventory), 1, invFile);
	}
	fclose(invFile);
	return 0;
}

Record* storageLoadRecord(char filename[], Inventory* invHead) { //文件名与关联的库存信息,记录类型构造时无动态分配其他内存，直接按位复制再改指针
	FILE* recFile;
	Record* head = NULL, * rec = NULL, data;
	Inventory* inv = NULL;
	fopen_s(&recFile, filename, "rb");
	if (!recFile) return NULL;
	head = (Record*)malloc(sizeof(Record));
	if (!fread(head, sizeof(Record), 1, recFile)) {
		free(head);
		return NULL;
	}
	int dataSize = head->timeList.size;
	listInit(&head->timeList);
	for (int i = 0; i < dataSize; i++) {
		rec = (Record*)malloc(sizeof(Record));
		if (!fread(rec, sizeof(Record), 1, recFile)) {
			free(rec);
			recordListClear(head);
			recordDel(head);
			return NULL;
		}
		listAddTail(&rec->timeList, &head->timeList);
		if (invHead && (inv = invQueryID(invHead, rec->invID))) {
			listAddTail(&rec->IRList, &inv->invRecord->IRList);
		}
		else {
			rec->IRList = (ListHead){ 0,0,0,0 };
		}
	}
	fclose(recFile);
	return head;
}

int storageSaveRecord(char filename[], Record* head) {
	FILE* recFile;
	fopen_s(&recFile, filename, "wb");
	fwrite(head, sizeof(Record), 1, recFile);
	listForEachEntry(Record, pos, &head->timeList, timeList) {
		fwrite(pos, sizeof(Record), 1, recFile);
	}
	fclose(recFile);
	return 0;
}

void storageLoadSalePlan(char filename[], SSP** pSSP, CSP** pCSP) {
	*pSSP = NULL;
	*pCSP = NULL;
	FILE* SPFile;
	SSP* sspHead = NULL, * ssp = NULL, sspData;
	CSP* cspHead = NULL, * csp = NULL, cspData;
	Inventory* giftHead=NULL, * gift = NULL;
	Inventory* comInvHead = NULL, * comInv = NULL;
	fopen_s(&SPFile, filename, "rb");
	if (!SPFile) return;

	//从文件拷贝SSP，指针重置
	sspHead = (SSP*)malloc(sizeof(SSP));
	if (!fread(sspHead, sizeof(SSP), 1, SPFile)) {
		free(sspHead);
		return;
	}
	int sspSize = sspHead->list.size;
	sspHead->optGifts = invListInit(invCreate());
	listInit(&sspHead->list);

	for (int i = 0; i < sspSize; i++) {
		ssp = (SSP*)malloc(sizeof(SSP));
		if (!fread(ssp, sizeof(SSP), 1, SPFile)) {
			SSPDEL:
			free(ssp);
			SSPListClear(sspHead);
			SSPDel(sspHead);
			return;
		}

		giftHead = (Inventory*)malloc(sizeof(Inventory));
		if (!fread(giftHead, sizeof(Inventory), 1, SPFile)) {
			free(giftHead);
			goto SSPDEL;
		}
		int giftSize = giftHead->list.size;
		giftHead->invRecord = NULL;
		listInit(&giftHead->list);

		for (int j = 0; j < giftSize; j++) {
			gift = (Inventory*)malloc(sizeof(Inventory));
			if (!fread(gift, sizeof(Inventory), 1, SPFile)) {
				free(gift);
				invListClear(giftHead);
				invDel(giftHead);
				goto SSPDEL;
			}
			gift->invRecord = NULL;  //冗余操作
			listAddTail(&gift->list, &giftHead->list);
		}
		ssp->optGifts = giftHead;
		listAddTail(&ssp->list, &sspHead->list);
		//商品记录数据由记录加载模块加载
	}
	*pSSP = sspHead;

	//从文件拷贝CSP，指针重置
	cspHead = (CSP*)malloc(sizeof(CSP));
	if (!fread(cspHead, sizeof(CSP), 1, SPFile)) {
		free(cspHead);
	}
	int cspSize = cspHead->list.size;
	cspHead->optGifts = invListInit(invCreate());
	cspHead->comInv = invListInit(invCreate());
	listInit(&cspHead->list);

	for (int i = 0; i < cspSize; i++) {
		csp = (CSP*)malloc(sizeof(CSP));
		if (!fread(csp, sizeof(CSP), 1, SPFile)) {
		CSPDEL:
			free(csp);
			CSPListClear(cspHead);
			CSPDel(cspHead);
			return;
		}

		giftHead = (Inventory*)malloc(sizeof(Inventory));
		if (!fread(giftHead, sizeof(Inventory), 1, SPFile)) {
			free(giftHead);
			goto CSPDEL;
		}
		int giftSize = giftHead->list.size;
		giftHead->invRecord = NULL;
		listInit(&giftHead->list);

		for (int j = 0; j < giftSize; j++) {
			gift = (Inventory*)malloc(sizeof(Inventory));
			if (!fread(gift, sizeof(Inventory), 1, SPFile)) {
				free(gift);
				invListClear(giftHead);
				invDel(giftHead);
				goto CSPDEL;
			}
			gift->invRecord = NULL;  //空
			listAddTail(&gift->list, &giftHead->list);
		}

		comInvHead = (Inventory*)malloc(sizeof(Inventory));
		if (!fread(comInvHead, sizeof(Inventory), 1, SPFile)) {
			free(comInvHead);
			invListClear(giftHead);
			invDel(giftHead);
			goto CSPDEL;
		}
		int comInvSize = comInvHead->list.size;
		comInvHead->invRecord = NULL;
		listInit(&comInvHead->list);

		for (int j = 0; j < comInvSize; j++) {
			comInv = (Inventory*)malloc(sizeof(Inventory));
			if (!fread(comInv, sizeof(Inventory), 1, SPFile)) {
				free(comInv);
				invListClear(comInvHead);
				invDel(comInvHead);
				invListClear(giftHead);
				invDel(giftHead);
				goto CSPDEL;
			}
			comInv->invRecord = NULL;  //冗余操作
			listAddTail(&comInv->list, &comInvHead->list);
		}
		csp->optGifts = giftHead;
		csp->comInv = comInvHead;
		listAddTail(&csp->list, &cspHead->list);
	}
	*pCSP = cspHead;

	fclose(SPFile);
}
int storageSaveSalePlan(char filename[], SSP* sspHead, CSP* cspHead) {
	FILE* SPFile;
	fopen_s(&SPFile, filename, "wb");
	fwrite(sspHead, sizeof(SSP), 1, SPFile);
	listForEachEntry(SSP, pos, &sspHead->list, list) {
		fwrite(pos, sizeof(SSP), 1, SPFile);
		fwrite(pos->optGifts, sizeof(Inventory), 1, SPFile);
		listForEachEntry(Inventory, giftPos, &pos->optGifts->list, list) {
			fwrite(giftPos, sizeof(Inventory), 1, SPFile);
		}
	}
	fwrite(cspHead, sizeof(CSP), 1, SPFile);
	listForEachEntry(CSP, pos, &cspHead->list, list) {
		fwrite(pos, sizeof(CSP), 1, SPFile);
		fwrite(pos->optGifts, sizeof(Inventory), 1, SPFile);
		listForEachEntry(Inventory, giftPos, &pos->optGifts->list, list) {
			fwrite(giftPos, sizeof(Inventory), 1, SPFile);
		}
		fwrite(pos->comInv, sizeof(Inventory), 1, SPFile);
		listForEachEntry(Inventory, comInvPos, &pos->comInv->list, list) {
			fwrite(comInvPos, sizeof(Inventory), 1, SPFile);
		}
	}
	fclose(SPFile);
	return 0;
}