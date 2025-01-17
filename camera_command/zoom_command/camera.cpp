#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "serial_port.h"
#include <iostream>

static unsigned char ZOOM_LEVELS[20][9] = {
    {0x81, 0x01, 0x04, 0x47, 0x00, 0x00, 0x00, 0x00, 0xFF}, // 1x
    {0x81, 0x01, 0x04, 0x47, 0x00, 0x0D, 0x0C, 0x01, 0xFF}, // 2x
    {0x81, 0x01, 0x04, 0x47, 0x01, 0x08, 0x06, 0x0C, 0xFF}, // 3x
    {0x81, 0x01, 0x04, 0x47, 0x02, 0x05, 0x09, 0x04, 0xFF}, // 5x
    {0x81, 0x01, 0x04, 0x47, 0x03, 0x04, 0x02, 0x0D, 0xFF}, // 10x
    {0x81, 0x01, 0x04, 0x47, 0x03, 0x0B, 0x09, 0x00, 0xFF}, // 15x
    {0x81, 0x01, 0x04, 0x47, 0x04, 0x00, 0x00, 0x00, 0xFF}  // 20x
};

Serial_port *mSerialPtr;
pthread_t mReadThreadTid;

uint32_t inputData(unsigned char *str);

int main()
{
    mSerialPtr = new Serial_port("/dev/ttyUSB0", 115200);

    /* precheck 1 : serial check */
    if (!mSerialPtr->init()) {
        delete mSerialPtr;
        mSerialPtr = nullptr;
        return -2;
    }

    unsigned char str[128];
    uint32_t retLen;
    int lRet;

    while (1) {
        memset(str, 0, 128);
        retLen = inputData(str);
        if (retLen > 0) {
            lRet = write(mSerialPtr->fd_w, str, retLen);
            printf("write %d byte, return %d byte\n", retLen, lRet);
        }
    }

    return 0;
}

uint32_t inputData(unsigned char *pStr)
{
    uint32_t zoomLevel;
    uint32_t lLen = 0;

    printf("Input zoom level (1, 2, 3, 5, 20): ");
    scanf("%u", &zoomLevel);

    switch (zoomLevel) {
        case 1:
            memcpy(pStr, ZOOM_LEVELS[0], 9);
            lLen = 9;
            break;
        case 2:
            memcpy(pStr, ZOOM_LEVELS[1], 9);
            lLen = 9;
            break;
        case 3:
            memcpy(pStr, ZOOM_LEVELS[2], 9);
            lLen = 9;
            break;
        case 5:
            memcpy(pStr, ZOOM_LEVELS[3], 9);
            lLen = 9;
            break;
        case 20:
            memcpy(pStr, ZOOM_LEVELS[4], 9);
            lLen = 9;
            break;
        default:
            printf("Invalid input. Please enter one of the predefined zoom levels (1, 2, 3, 5, 20).\n");
            lLen = 0;
            break;
    }

    if (lLen > 0) {
        printf("Zoom Level %u set.\n", zoomLevel);
    }

    return lLen;
}
