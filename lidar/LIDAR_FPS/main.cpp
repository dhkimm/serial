#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdint.h>
#include <pthread.h>
#include "serial_port.h"

#include <iostream>

static unsigned char ENTER_CONFIG[8] = {0xAA, 0x55, 0xF0, 0x00, 0x01, 0x00, 0x00, 0x02};
static unsigned char FRAME_RATE_1000[6] = {0x5A, 0x06, 0x03, 0xE8, 0x03, 0x4E};
static unsigned char FRAME_RATE_500[6] = {0x5A, 0x06, 0x03, 0xF4, 0x01, 0x58};
static unsigned char FRAME_RATE_250[6] = {0x5A, 0x06, 0x03, 0xFA, 0x00, 0x5D};
static unsigned char FRAME_RATE_125[6] = {0x5A, 0x06, 0x03, 0x7D, 0x00, 0xE0};
static unsigned char FRAME_RATE_100[6] = {0x5A, 0x06, 0x03, 0x64, 0x00, 0x67};


Serial_port *mSerialPtr;
pthread_t mReadThreadTid;

void* readThread(void *args);
uint32_t inputData(unsigned char *str);
void printPacket(unsigned char *cp, uint32_t len);

int main()
{
#if 1
	mSerialPtr = new Serial_port("/dev/ttyUSB0", 115200);

	/* precheck 1 : serial check */
	if(!mSerialPtr->init()) {
		delete mSerialPtr;
		mSerialPtr = nullptr;
		return -2;
	}

	pthread_create(&mReadThreadTid, NULL, readThread, NULL);

#endif
	unsigned char str[128];
	uint32_t retLen;
	int lRet;

	while(1) {
		memset(str, 0, 128);

		retLen = inputData(str);

		lRet = write(mSerialPtr->fd_w, str, retLen);

		printf("write %d byte, return %d byte\n", retLen, lRet);
	}


	return 0;
}

unsigned char checkSum(unsigned char *cp, uint32_t len)
{
	unsigned char lCh = 0;

	for(uint32_t cnt = 0; cnt < len; ++cnt) {
		lCh += cp[cnt];
	}

	return lCh;
}

void printPacket(unsigned char *cp, uint32_t len)
{
	printf("[%s] print packet start\n", __func__);

	for(int cnt = 0;cnt < len; ++cnt) {
		printf("[%02X]", cp[cnt]);
	}
	printf("\n");

	return ;
}

uint32_t inputData(unsigned char *pStr)
{
	uint8_t lTemp;
	uint32_t lLen = 0;;
	uint32_t lCheck;

	while(1) {

		printf("input frame rate (0 : config, 1 : 1000, 2 : 500, 3 : 250, 4 : 125, 5 : 100) :");
		scanf("%x", &lTemp);

		switch(lTemp)
		{
			case 0: /* config */
				printf("enter config mode\n");
				memcpy(pStr, ENTER_CONFIG, 8);
				lLen = 8;
				return lLen;


			case 1: /* 1000 */
				printf("set frame 1000\n");
				memcpy(pStr, FRAME_RATE_1000, 6);
				lLen = 6;
				return lLen;

			case 2: /* 500 */
				printf("set frame 500\n");
				memcpy(pStr, FRAME_RATE_500, 6);
				lLen = 6;
				return lLen;

			case 3: /* 250 */
				printf("set frame 250\n");
				memcpy(pStr, FRAME_RATE_250, 6);
				lLen = 6;
				return lLen;

			case 4: /* 125 */
				printf("set frame 125\n");
				memcpy(pStr, FRAME_RATE_125, 6);
				lLen = 6;
				return lLen;

			case 5: /* 100 */
				printf("set frame 100\n");
				memcpy(pStr, FRAME_RATE_100, 6);
				lLen = 6;
				return lLen;

			default:
				printf("wrong input try again! %d\n", lTemp);
				break;
		}
	}
#if 0
		printf("입력 : ");
		scanf("%x", &lTemp);
		printf("입력 데이터 : %x\n", lTemp);

		pStr[lLen] = lTemp;
		++lLen;

		printf("입력 종료를 하시겠습니까? (exit '0') : ");
		scanf("%d", &lCheck);

		if(lCheck == 0) {
			break;
		}
	}

#if 1 //checksum test
	pStr[lLen + 1] = checkSum(pStr, lLen);
	++lLen;
#endif //checksum test

	printPacket(pStr, lLen);
#endif

	return lLen;
}

void* readThread(void *args)
{
	int lRet;
	unsigned char buffer[128];

	while(1) {
		//printf("read packet test\n");
		/* read uart serial data */
		memset(buffer, 0, 128);
		lRet = read(mSerialPtr->fd_r, buffer, 128);

		if(buffer[0] == 0x5A && buffer[1] == 0x06 && buffer[2] == 0x03) {
			printPacket(buffer, lRet);
		}

		//printPacket(buffer, lRet);
	}

	return NULL;
}

