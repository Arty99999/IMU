#include "hardware_config.h"
#include "control_logic.h"
#include "drv_timer.h"
#include "tim.h"
#include "ins.h"
#include "bmi088.h"
#include "mpu6050.h"
#include "driver_counter.h"
void MPU_init(void)
{
		//MUP structure variable define
	MPU_Region_InitTypeDef MPU_Config;
	
	/*-----------Open FPU--------*///High speed FLOAT calculate
	SCB->CPACR |= ((3UL << (10*2))|(3UL << (11*2)));  /* set CP10 and CP11 Full Access */
	/*-----------Open Cache------------*/
	SCB_EnableICache();//使能I-Cache
  SCB_EnableDCache();//使能D-Cache   
	SCB->CACR|=1<<2;   //强制D-Cache透写,如不开启,实际使用中可能遇到各种问题	
	/*-----------Open MPU------------*/
	HAL_MPU_Disable();
	MPU_Config.Enable=MPU_REGION_ENABLE;
	MPU_Config.Number=MPU_REGION_NUMBER1;//保护区编号 1
	MPU_Config.BaseAddress= 0x24000000;//保护区基地址
	MPU_Config.Size=MPU_REGION_SIZE_512KB;//设置保护区512k
	MPU_Config.SubRegionDisable=0x00;//禁止子区域
	MPU_Config.TypeExtField=MPU_TEX_LEVEL0;//设置类型扩展域为level0
	MPU_Config.AccessPermission=MPU_REGION_FULL_ACCESS;//全访问（特权&用户都可访问）
	MPU_Config.DisableExec=MPU_INSTRUCTION_ACCESS_ENABLE;//允许指令访问
	MPU_Config.IsShareable=MPU_ACCESS_SHAREABLE;//允许共享
	MPU_Config.IsCacheable=MPU_ACCESS_CACHEABLE;//允许cache
	MPU_Config.IsBufferable=MPU_ACCESS_NOT_BUFFERABLE;//不允许缓冲 DMA模式下要设为不允许
	HAL_MPU_ConfigRegion(&MPU_Config);
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void HardwareConfig(void)
{
	MPU_init();

	DWT_Init(480);
//	INS_Init(&bmi088.bmi088_Data); //使用BMI088.
		INS_Init(&mpu6050.mpu6050_Data); // 使用mpu6050
	

	TIMx_Init(&htim14,&TIM14_Task);
	TIM_Open(&tim14);

}

