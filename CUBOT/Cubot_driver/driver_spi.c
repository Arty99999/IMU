/**
 **********************************************************************************
 * @file        driver_spi.c
 * @brief       驱动层，面向SPI从机的初始化与配置流程，后续参考HNU的开源对代码进行了修改
 * @details
 * @date        2024-07-26
 * @version     V1.0
 * @copyright   Copyright CUMT-CUBOT (c) 2024
 **********************************************************************************
 * @attention
 * 硬件平台: STM32H750VBT \n
 * SDK版本：-++++
 * @par 修改日志:
 * <table>
 * <tr><th>Date         <th>Version     <th>Author      <th>Description
 * <tr><td>2024-07-26   <td>1.0         <td>EmberLuo    <td>创建初始版本
 * </table>
 *
 **********************************************************************************
 ==============================================================================
                            How to use this driver
 ==============================================================================

    1. Include the "driver_spi.h" header file in your project.
        在你的项目中包含"driver_spi.h"头文件。
    2. Create an instance of SPI_Slave_t for each SPI slave device you want to communicate with.
        为要与之通信的每个SPI从设备创建一个SPI_Slave_t实例。
    3. Initialize each SPI slave using the SPI_Slavelnit() function, providing the necessary parameters.
        使用SPI_Slavelnit()函数初始化每个SPI从属，提供必要的参数，如SPI句柄、从属配置和接收回调函数。
    4. Set the SPI mode for each slave using SPL_SetMode(), choosing between DMA, interrupt, or blocking modes.
        使用SPL_SetMode()设置每个从机的SPI模式，在DMA、中断或阻塞模式之间进行选择。
    5. Use the provided functions to communicate with the SPI slaves:SPI_Transmit() to send data SPI_Recv() to receive data SPI_TransRecv() for fullduplex communication.
        使用提供的函数与SPI从站通信:SPI_Transmit()发送数据SPI_Recv()接收数据SPI_TransRecv()进行全双工通信
    6. Implement the necessary callback functions to handle received data or transmission completion events.
        实现必要的回调函数来处理接收到的数据或传输完成事件。

 **********************************************************************************
 * @attention
 * 硬件平台: STM32H750VBT \n
 * SDK版本：-++++
 * if you had modified this file, please make sure your code does not have many
 * bugs, update the version NO., write dowm your name and the date, the most
 * important is make sure the users will have clear and definite understanding
 * through your new brief.
 **********************************************************************************
 * @exception   SPI_NSS有两种模式，SPI_NSS_Hard和SPI_NSS_Soft。
 *              SPI_NSS_Hard，硬件自动拉高拉低片选，在速率上是远比软件方式控制要高的，
 *              缺点是当STM32为主设备时，同一个SPI上面只能接一个从设备。
 *              SPI_NSS_Soft，软件控制，GPIO控制片选拉高拉低，优点是一个SPI上面可以挂多个设备。
 *              SPI_NSS_Soft也是使用较多的方式, 本驱动暂时只考虑这个模式。
 * @todo        考虑将从机编入链表
 **********************************************************************************
 */

#include "driver_spi.h"
SPI_Instance_t SPI_t[6]={{.spiHandler=&hspi1,.DevicesList={&(SPI_t[0].DevicesList),&(SPI_t[0].DevicesList)}}};

void SPI_RegisterSlave(SPI_Instance_t *master, SPI_Slave_t *slave)
{
   list_add(&slave->list, (&master->DevicesList));
	slave->spiHandler=master->spiHandler;
}

/**
 * @brief 设置SPI模式
 *
 * @param spi_ins   SPI从机结构体
 * @param spi_mode  SPI模式枚举
 */
void SPI_SetMode(SPI_Slave_t *spi_ins, SPI_TXRX_MODE_e spi_mode)
{
    if (spi_mode != SPI_DMA_MODE && spi_mode != SPI_IT_MODE && spi_mode != SPI_BLOCK_MODE)
        while (1); // error mode! 请查看是否正确设置模式，或出现指针越界导致模式被异常修改的情况
    spi_ins->spi_work_mode = spi_mode;
}

void SPI_Transmit(SPI_Slave_t *spi_ins, uint8_t *ptr_data, uint8_t len)
{
	int i;
    // 拉低片选,开始传输(选中从机)
    HAL_GPIO_WritePin(spi_ins->chipSelect.gpiox, spi_ins->chipSelect.cs_pin, GPIO_PIN_RESET);
    switch (spi_ins->spi_work_mode) {
        case SPI_DMA_MODE:
            HAL_SPI_Transmit_DMA(spi_ins->spiHandler, ptr_data, len);
            break;
        case SPI_IT_MODE:
            HAL_SPI_Transmit_IT(spi_ins->spiHandler, ptr_data, len);
            break;
        case SPI_BLOCK_MODE:
            HAL_SPI_Transmit(spi_ins->spiHandler, ptr_data, len, 1000); // 默认50ms超时
            // 阻塞模式不会调用回调函数,传输完成后直接拉高片选结束
            HAL_GPIO_WritePin(spi_ins->chipSelect.gpiox, spi_ins->chipSelect.cs_pin, GPIO_PIN_SET);
            break;
        default:
            while (1); // error mode! 请查看是否正确设置模式，或出现指针越界导致模式被异常修改的情况
    }
}

void SPI_Recv(SPI_Slave_t *spi_ins, uint8_t *ptr_data, uint8_t len)
{

    // 用于稍后回调使用
    spi_ins->rxBuffer.Size = len;
    spi_ins->rxBuffer.Data = ptr_data;
    // 拉低片选,开始传输
    HAL_GPIO_WritePin(spi_ins->chipSelect.gpiox, spi_ins->chipSelect.cs_pin, GPIO_PIN_RESET);
	
    switch (spi_ins->spi_work_mode) {
        case SPI_DMA_MODE:
            HAL_SPI_Receive_DMA(spi_ins->spiHandler, ptr_data, len);
            break;
        case SPI_IT_MODE:
            HAL_SPI_Receive_IT(spi_ins->spiHandler, ptr_data, len);
            break;
        case SPI_BLOCK_MODE:
            HAL_SPI_Receive(spi_ins->spiHandler, ptr_data, len, 1000);
            // 阻塞模式不会调用回调函数,传输完成后直接拉高片选结束
            HAL_GPIO_WritePin(spi_ins->chipSelect.gpiox, spi_ins->chipSelect.cs_pin, GPIO_PIN_SET);
            break;
        default:
            while (1); // error mode! 请查看是否正确设置模式，或出现指针越界导致模式被异常修改的情况
    }
}

void SPI_TransRecv(SPI_Slave_t *spi_ins, uint8_t *ptr_data_rx, uint8_t *ptr_data_tx, uint8_t len)
{
	
	
    // 用于稍后回调使用,请保证ptr_data_rx在回调函数被调用之前仍然在作用域内,否则析构之后的行为是未定义的!!!
    spi_ins->rxBuffer.Size = len;
    spi_ins->rxBuffer.Data = ptr_data_rx;
    // 拉低片选,开始传输
    HAL_GPIO_WritePin(spi_ins->chipSelect.gpiox, spi_ins->chipSelect.cs_pin, GPIO_PIN_RESET);
    switch (spi_ins->spi_work_mode) {
        case SPI_DMA_MODE:
            HAL_SPI_TransmitReceive_DMA(spi_ins->spiHandler, ptr_data_tx, ptr_data_rx, len);
            break;
        case SPI_IT_MODE:
            HAL_SPI_TransmitReceive_IT(spi_ins->spiHandler, ptr_data_tx, ptr_data_rx, len);
            break;
        case SPI_BLOCK_MODE:
            HAL_SPI_TransmitReceive(spi_ins->spiHandler, ptr_data_tx, ptr_data_rx, len, 1000); // 默认50ms超时
            // 阻塞模式不会调用回调函数,传输完成后直接拉高片选结束
            HAL_GPIO_WritePin(spi_ins->chipSelect.gpiox, spi_ins->chipSelect.cs_pin, GPIO_PIN_SET);
            break;
        default:
            while (1); // error mode! 请查看是否正确设置模式，或出现指针越界导致模式被异常修改的情况
    }
}

/**
 * @brief 当SPI接收完成,将会调用此回调函数,可以进行协议解析或其他必须的数据处理等
 *
 * @param h_spi spi handle
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *h_spi)
{
	SPI_Instance_t* SPI=NULL;
	 	SPI_Slave_t *SPI_Slave = NULL;
	list_t *node = NULL;	
	
	for (int i=0;i<6;i++)
		if (SPI_t[i].spiHandler==h_spi)
		SPI=&SPI_t[i];
	
	for (node = SPI->DevicesList.next;    		//< 对循环链表遍历一圈
			 node != (SPI->DevicesList.prev->next);
			 node = node->next)
	{
		SPI_Slave= list_entry(node, SPI_Slave_t, list);  //< 输入链表头部所在结点、被嵌入链表的结构体类型、被嵌入链表的结构体类型中链表结点的名称：即可返回嵌入头部所在结点的结构体
		if (HAL_GPIO_ReadPin(SPI_Slave->chipSelect.gpiox,SPI_Slave->chipSelect.cs_pin)==GPIO_PIN_RESET)
		{
   //    SPI_Slave spi_instance[i]->callback(spi_instance[i]);
		}

	}

}

/**
 * @brief   这是对HAL库的__weak函数的重写,传输使用IT或DMA模式,在传输完成时会调用此函数。
 * @details 在全双工模式下使用 HAL_SPI_TransmitReceive_IT()或HAL_SPI_TransmitReceive_DMA()函数启动SPI传输时,
 *          应该实现HAL_SPI_TxRxCpltCallback()来处理传输完成事件, 因为在这种情况下, HAL_SPI_RxCpltCallback()不会被调用。
 *          和RxCpltCallback共用解析即可,这里只是形式上封装一下,不用重复写。
 * @param h_spi spi handle
 */


