#ifndef _DRIVER_DEVICE_H_
#define _DRIVER_DEVICE_H_
typedef enum {
    BLOCK_MODE, // 默认使用阻塞模式
    IT_MODE,
    DMA_MODE,
} Device_MODE;

#endif