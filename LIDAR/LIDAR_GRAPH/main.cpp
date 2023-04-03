#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <pthread.h>

#include <linux/limits.h>


#include <iostream>

#include "serial_port.h"
#include "circularBuffer.h"


static unsigned char ENTER_CONFIG[8] = {0xAA, 0x55, 0xF0, 0x00, 0x01, 0x00, 0x00, 0x02};
static unsigned char EXIT_CONFIG[8] = {0xAA, 0x55, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x02};
static unsigned char FRAME_RATE_1000[6] = {0x5A, 0x06, 0x03, 0xE8, 0x03, 0x4E};
static unsigned char FRAME_RATE_500[6] = {0x5A, 0x06, 0x03, 0xF4, 0x01, 0x58};
static unsigned char FRAME_RATE_250[6] = {0x5A, 0x06, 0x03, 0xFA, 0x00, 0x5D};
static unsigned char FRAME_RATE_125[6] = {0x5A, 0x06, 0x03, 0x7D, 0x00, 0xE0};
static unsigned char FRAME_RATE_100[6] = {0x5A, 0x06, 0x03, 0x64, 0x00, 0x67};


Serial_port *mSerialPtr;
pthread_t mReadThreadTid;
pthread_t mParseThreadTid;

void* readThread(void *args);
void* parseThread(void *args);
uint32_t inputData(unsigned char *str);
void printPacket(unsigned char *cp, uint32_t len);

int fd;
char file_path[128];

CircularBuffer *cbuf;

int main()
{
#if 1
	mSerialPtr = new Serial_port("/dev/ttyUSB0", 115200);

	/* precheck 1 : serial check */
	if(!mSerialPtr->init()) {
		delete mSerialPtr;
		mSerialPtr = NULL;
		return -2;
	}

	cbuf = new CircularBuffer(4096);

	pthread_create(&mReadThreadTid, NULL, readThread, NULL);
	pthread_create(&mParseThreadTid, NULL, parseThread, NULL);
#endif

	unsigned char str[128];
	uint32_t retLen;
	int lRet;

#if 1 // data write in file
	struct tm lCurrentTm;
	time_t lCurrentTime = time(NULL);

	localtime_r(&lCurrentTime, &lCurrentTm);

	uint32_t lYear = lCurrentTm.tm_year + 1900;
	uint32_t lMonth = lCurrentTm.tm_mon + 1;
	uint32_t lDay = lCurrentTm.tm_mday;
	uint32_t lHour = lCurrentTm.tm_hour;
	uint32_t lMin = lCurrentTm.tm_min;
	uint32_t lSec = lCurrentTm.tm_sec;

	char touch_command[PATH_MAX] = { };

	memset(file_path, 0, 128);
	sprintf(file_path, "./data_%04d_%02d%02d_%02d%02d%02d.txt", lYear, lMonth, lDay, lHour, lMin, lSec);
	sprintf(touch_command, "touch %s", file_path);
	system(touch_command);
#endif

	while(1) {
		memset(str, 0, 128);
		retLen = inputData(str);
		lRet = write(mSerialPtr->fd_w, str, retLen);

		if(lRet != retLen) {
			printf("write length (%d), return value (%d)\n", retLen, lRet);
		}
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

	char wcommand[128];

	while(1) {

		printf("input frame rate (0 : enter config, 1 : 1000, 2 : 500, 3 : 250, 4 : 125, 5 : 100, 9 : exit config) :");
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

				memset(wcommand, 0, 128);
				sprintf(wcommand, "echo \"==================== frame rate 1000\" >> %s", file_path);
				system(wcommand);
				return lLen;

			case 2: /* 500 */
				printf("set frame 500\n");
				memcpy(pStr, FRAME_RATE_500, 6);
				lLen = 6;

				memset(wcommand, 0, 128);
				sprintf(wcommand, "echo \"==================== frame rate 500\" >> %s", file_path);
				system(wcommand);

				return lLen;

			case 3: /* 250 */
				printf("set frame 250\n");
				memcpy(pStr, FRAME_RATE_250, 6);
				lLen = 6;

				memset(wcommand, 0, 128);
				sprintf(wcommand, "echo \"==================== frame rate 250\" >> %s", file_path);
				system(wcommand);

				return lLen;

			case 4: /* 125 */
				printf("set frame 125\n");
				memcpy(pStr, FRAME_RATE_125, 6);
				lLen = 6;

				memset(wcommand, 0, 128);
				sprintf(wcommand, "echo \"==================== frame rate 125\" >> %s", file_path);
				system(wcommand);

				return lLen;

			case 5: /* 100 */
				printf("set frame 100\n");
				memcpy(pStr, FRAME_RATE_100, 6);
				lLen = 6;

				memset(wcommand, 0, 128);
				sprintf(wcommand, "echo \"==================== frame rate 100\" >> %s", file_path);
				system(wcommand);

				return lLen;

			case 9: /* config */
				printf("exit config mode\n");
				memcpy(pStr, EXIT_CONFIG, 8);
				lLen = 8;
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
	unsigned char wbuffer[128];

	while(1) {
		//printf("read packet test\n");
		/* read uart serial data */
		memset(buffer, 0, 128);
		lRet = read(mSerialPtr->fd_r, buffer, 128);

		printPacket(buffer, lRet);

		if(lRet > 0) {
			cbuf->write(buffer, lRet);			
		}

		usleep(1000);
#if 0
		if(buffer[0] == 0x5A && buffer[1] == 0x06 && buffer[2] == 0x03) {
			printf("\n\n ============== frame rate change response\n");
			printPacket(buffer, lRet);
		}

		if(buffer[0] == 0xAA && buffer[1] == 0x55 && buffer[2] == 0xF0) {
			printf("\n\n ============== config mode response %s\n", (buffer[4] == 0x01) ? "enter" : "exit");
			printPacket(buffer, lRet);
		}


		if(buffer[0] == 0x59 && buffer[1] == 0x59) {
			printf("\n\n ============== standard data\n");	
			printPacket(buffer, lRet);

			lDist = buffer[3] << 8 | buffer[2];
			printf("distance : %d\n", lDist);
			printf("strength : %d\n", buffer[5] << 8 | buffer[4]);

			clock_gettime(CLOCK_REALTIME, &lTs);

			lCurrentTime = time(NULL);

			localtime_r(&lTs.tv_sec, &lCurrentTm);

			lYear = lCurrentTm.tm_year + 1900;
			lMonth = lCurrentTm.tm_mon + 1;
			lDay = lCurrentTm.tm_mday;
			lHour = lCurrentTm.tm_hour;
			lMin = lCurrentTm.tm_min;
			lSec = lCurrentTm.tm_sec;

			char wcommand[128];
			memset(wcommand, 0, 128);
			sprintf(wcommand, "echo \"%02d%02d_%02d:%02d:%02d.%03u dist : %d\" >> %s", lMonth, lDay, lHour, lMin, lSec, lTs.tv_nsec / 1000000  ,lDist, file_path);

			system(wcommand);
		}
#endif

	}

	return NULL;
}

void *parseThread(void *args)
{
	uint8_t pre[32];
	uint8_t buffer[128];
	uint8_t wcommand[128];
	uint8_t temp;

	struct timespec lTs;
	struct tm lCurrentTm;
	time_t lCurrentTime;

	uint32_t lYear;
	uint32_t lMonth;
	uint32_t lDay;
	uint32_t lHour;
	uint32_t lMin;
	uint32_t lSec;

	uint16_t lDist;

	while(1) {
		if(cbuf->size() > 2) {
			//check head
			cbuf->preread(pre, 4);

			/* distance data */
			if(pre[0] == 0x59 && pre[1] == 0x59) {
				
				if(cbuf->size() > 9) {
					cbuf->read(buffer, 9);
				
					printf("\n\n ============== standard data\n");	
					lDist = buffer[3] << 8 | buffer[2];

					clock_gettime(CLOCK_REALTIME, &lTs);
					lCurrentTime = time(NULL);
					localtime_r(&lTs.tv_sec, &lCurrentTm);

					lYear = lCurrentTm.tm_year + 1900;
					lMonth = lCurrentTm.tm_mon + 1;
					lDay = lCurrentTm.tm_mday;
					lHour = lCurrentTm.tm_hour;
					lMin = lCurrentTm.tm_min;
					lSec = lCurrentTm.tm_sec;

					memset(wcommand, 0, 128);
					sprintf((char *)wcommand, "echo \"%02d-%02d %02d:%02d:%02d.%03u %d\" >> %s", lMonth, lDay, lHour, lMin, lSec, lTs.tv_nsec / 1000000  ,lDist, (char *)file_path);

					system((char *)wcommand);
					
				} 

				continue;
			}

			if(pre[0] == 0xAA && pre[1] == 0x55 && pre[2] == 0xF0) {
				if(cbuf->size() > 8) {
					cbuf->read(buffer, 8);
					printf("\n\n ============== config mode response %s\n", (buffer[4] == 0x01) ? "enter" : "exit");
				}
				continue;
			}
			
			// skip byte
			if(cbuf-> size() > 16) {
				cbuf->read(&temp, 1);
			}
		}

		usleep(1000);
	}

	return NULL;
}
