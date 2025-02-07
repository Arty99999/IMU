#ifndef DRV_TIMER_H
#define DRV_TIMER_H
#include "stm32h7xx_hal.h"
#include "tim.h"


typedef void (*TIM_ElapsedCallback)(void); 


typedef struct
{
	TIM_HandleTypeDef* 	Handle;
	uint32_t ClockTime;             			//< ����ʱ���ļ�������
	uint32_t UI_ClockTime;             			//< ����ʱ���ļ�������
	uint32_t Vision_FPS;						//< ֡��
	uint32_t auto_shoot_time;					//< ��ʱ���õļ�ʱ
	uint32_t vision_loss_target_cnt;
	TIM_ElapsedCallback		ElapCallback;		//< ��ʱ������ж�
	int16_t Delaytime;
	uint16_t Delay_on;
	uint16_t Delay_close;
	uint16_t HolderTime;
	uint16_t ErrorTime;
}TIM_Object;

/**
  * @brief ��ʼ����ʱ�����û��ص�
  */
void TIMx_Init(TIM_HandleTypeDef* handle, TIM_ElapsedCallback callback);


/**
  * @brief     ������ʱ������ж�
  */
void TIM_Open(TIM_Object* tim);



extern TIM_Object tim14;
extern TIM_Object tim15;;

#endif
