#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdint.h>
#include <pthread.h>
#include "serial_port.h"

#include <iostream>

static unsigned char GIMBLA_MOVE_LEFT[20] = {0x55, 0xAA, 0xDC, 0x11, 0x30, 0x01, 0xF8, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8};
static unsigned char GIMBLA_MOVE_RIGHT[20] = {0x55, 0xAA, 0xDC, 0x11, 0x30, 0x01, 0x07, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7};
static unsigned char GIMBLA_MOVE_UP[20] = {0x55, 0xAA, 0xDC, 0x11, 0x30, 0x01, 0x00, 0x00, 0x07, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7};
static unsigned char GIMBLA_MOVE_DOWN[20] = {0x55, 0xAA, 0xDC, 0x11, 0x30, 0x01, 0x00, 0x00, 0xF8, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8};
static unsigned char GIMBLA_MOVE_STOP[20] = {0x55, 0xAA, 0xDC, 0x11, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20};

static unsigned char START_TRACKING[20] = {0x55, 0xAA, 0xDC, 0x11, 0x30, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x24};
static unsigned char STOP_TRACKING[20] = {0x55, 0xAA, 0xDC, 0x11, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x20};
static unsigned char POINT_TO_TRACKING[16] = {0x55, 0xAA, 0xDC, 0x11, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0xFF, 0x88, 0x01, 0x18, 0x58};

static unsigned char HOME_POINT[20] = {0x55, 0xAA, 0xDC, 0x11, 0x30, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};


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

		printf("input frame rate (1 : left, 2 : right, 3 : up, 4 : down, 5 : stop, 6 : start tracking 7 : stop tracking, 8 : point to tracking, 0 : home point) :");
		scanf("%x", &lTemp);

		switch(lTemp)
		{
			case 1: /* left */
				printf("gimbal move left\n");
				memcpy(pStr, GIMBLA_MOVE_LEFT, 20);
				lLen = 20;
				return lLen;


			case 2: /* right */
				printf("gimbal move right\n");
				memcpy(pStr, GIMBLA_MOVE_RIGHT, 20);
				lLen = 20;
				return lLen;

			case 3: /* UP */
				printf("gimbal move up\n");
				memcpy(pStr, GIMBLA_MOVE_UP, 20);
				lLen = 20;
				return lLen;

			case 4: /* down */
				printf("gimbal move down\n");
				memcpy(pStr, GIMBLA_MOVE_DOWN, 20);
				lLen = 20;
				return lLen;

			case 5: /* stop */
				printf("gimbal move stop\n");
				memcpy(pStr, GIMBLA_MOVE_STOP, 20);
				lLen = 20;
				return lLen;



			case 6: /* start */
				printf("start tracking\n");
				memcpy(pStr, START_TRACKING, 20);
				lLen = 20;
				return lLen;

			case 7: /* stop */
				printf("stop tracking\n");
				memcpy(pStr, STOP_TRACKING, 20);
				lLen = 20;
				return lLen;

			case 8: /* point to tracking  */
				printf("point to tracking\n");
				memcpy(pStr, POINT_TO_TRACKING, 16);
				lLen = 16;
				return lLen;	



			case 0: /* home point  */
				printf("home point\n");
				memcpy(pStr, HOME_POINT, 20);
				lLen = 20;
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

