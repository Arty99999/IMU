#ifndef DRV_TIMER_H
#define DRV_TIMER_H
#include "stm32h7xx_hal.h"
#include "tim.h"


typedef void (*TIM_ElapsedCallback)(void); 


typedef struct
{
	TIM_HandleTypeDef* 	Handle;
	uint32_t ClockTime;             			//< 任务定时器的计数变量
	uint32_t UI_ClockTime;             			//< 任务定时器的计数变量
	uint32_t Vision_FPS;						//< 帧率
	uint32_t auto_shoot_time;					//< 定时打弹用的计时
	uint32_t vision_loss_target_cnt;
	TIM_ElapsedCallback		ElapCallback;		//< 定时器溢出中断
	int16_t Delaytime;
	uint16_t Delay_on;
	uint16_t Delay_close;
	uint16_t HolderTime;
	uint16_t ErrorTime;
}TIM_Object;

/**
  * @brief 初始化定时器的用户回调
  */
void TIMx_Init(TIM_HandleTypeDef* handle, TIM_ElapsedCallback callback);


/**
  * @brief     开启定时器溢出中断
  */
void TIM_Open(TIM_Object* tim);



extern TIM_Object tim1;;
extern TIM_Object tim15;;

#endif
