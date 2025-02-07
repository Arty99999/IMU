#include "drv_timer.h"
#include "control_logic.h"
TIM_Object tim14;
TIM_Object tim15;

/*-------------------------------初始化---------------------------------*/
void TIMx_Init(TIM_HandleTypeDef* handle, TIM_ElapsedCallback callback)
{
	if(handle->Instance == TIM14)
	{
	  tim14.Handle = handle;
		tim14.ElapCallback = callback;
	}
	else if(handle->Instance == TIM15)
	{
	  tim15.Handle = handle;
		tim15.ElapCallback = callback;
	}
		
	
}


/*-------------------------------开启定时器溢出中断---------------------------------*/
void TIM_Open(TIM_Object* tim)
{
	HAL_TIM_Base_Start_IT(tim->Handle);
}

/*-------------------------------定时器溢出中断回调---------------------------------*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	
{
	if(htim==(&htim14))
	{
		tim14.ElapCallback();
	}
}
