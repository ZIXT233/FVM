#ifndef PRODUCT_H
#define PRODUCT_H
#include<time.h>
#define INFOMAX 256
#define TIME_NAN (0x3f3f3f3f3f3f3f3f)
typedef struct tagProduct {
	//Product attributes
	char kind[INFOMAX];
	char variety[INFOMAX];
	time_t expiration;
	int quality;
	enum Packing { BULK=1, UNIT } pack;
	//Product status
	int quantity;
	double weight;
	double amount;
	double unitPrice;
} Product;


inline Product makeProduct(char* kind, char* variety, time_t expiration, enum Packing pack, int quality) {
	return (Product) { kind, variety, expiration, pack, quality };
}
#endif
