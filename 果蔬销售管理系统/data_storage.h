#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#define STORAGE_AVAILABLE 0
#define STORAGE_NO_EXIST -1
#define STORAGE_INCOMPLETE -2

#define INVLOAD_FAILED -1

static const char STORAGE_DIR[30] = "FVMOData";
static const char CONFIG_FILENAME[30] = "config.dat";
static const char INVENTORY_FILENAME[30] = "inventory.dat";
static const char RECORD_FILENAME[30] = "record.dat";
static const char SALE_PLAN_FILENAME[30] = "sale_plan.dat";

int storageStat(char storageDir[]);
void storageCreate(char storageDir[]);
FVMO* storageLoadFVMO(char storageDir[]);
int storageSaveFVMO(FVMO* gdata);
Inventory* storageLoadInventory(char filename[]);
int storageSaveInventory(char filename[], Inventory* head);
Record* storageLoadRecord(char filename[], Inventory* invHead);
int storageSaveRecord(char filename[], Record* head);
void storageLoadSalePlan(char filename[], SSP** pSSP, CSP** pCSP);
int storageSaveSalePlan(char filename[], SSP* SSP,CSP* CSP);
#endif