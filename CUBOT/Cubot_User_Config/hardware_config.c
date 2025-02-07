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
	SCB_EnableICache();//ʹ��I-Cache
  SCB_EnableDCache();//ʹ��D-Cache   
	SCB->CACR|=1<<2;   //ǿ��D-Cache͸д,�粻����,ʵ��ʹ���п���������������	
	/*-----------Open MPU------------*/
	HAL_MPU_Disable();
	MPU_Config.Enable=MPU_REGION_ENABLE;
	MPU_Config.Number=MPU_REGION_NUMBER1;//��������� 1
	MPU_Config.BaseAddress= 0x24000000;//����������ַ
	MPU_Config.Size=MPU_REGION_SIZE_512KB;//���ñ�����512k
	MPU_Config.SubRegionDisable=0x00;//��ֹ������
	MPU_Config.TypeExtField=MPU_TEX_LEVEL0;//����������չ��Ϊlevel0
	MPU_Config.AccessPermission=MPU_REGION_FULL_ACCESS;//ȫ���ʣ���Ȩ&�û����ɷ��ʣ�
	MPU_Config.DisableExec=MPU_INSTRUCTION_ACCESS_ENABLE;//����ָ�����
	MPU_Config.IsShareable=MPU_ACCESS_SHAREABLE;//������
	MPU_Config.IsCacheable=MPU_ACCESS_CACHEABLE;//����cache
	MPU_Config.IsBufferable=MPU_ACCESS_NOT_BUFFERABLE;//�������� DMAģʽ��Ҫ��Ϊ������
	HAL_MPU_ConfigRegion(&MPU_Config);
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void HardwareConfig(void)
{
	MPU_init();

	DWT_Init(480);
//	INS_Init(&bmi088.bmi088_Data); //ʹ��BMI088.
		INS_Init(&mpu6050.mpu6050_Data); // ʹ��mpu6050
	

	TIMx_Init(&htim14,&TIM14_Task);
	TIM_Open(&tim14);

}

