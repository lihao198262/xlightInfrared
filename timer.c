
#include "stm8s.h"

uint32_t TIM3_TimingDelay = 0; 
uint32_t TIM3_TimingDelayUs = 0;

void sleep_ms(uint32_t ms) {
  TIM3_TimingDelay = ms;
  TIM3_TimeBaseInit(TIM3_PRESCALER_16,1000);
  TIM3_SetCounter(0);                           /* 将计数器初值设为0 */
  TIM3_ARRPreloadConfig(DISABLE);	        /* 预装载不使能 */
  TIM3_ITConfig(TIM3_IT_UPDATE , ENABLE);	/* 计数器向上计数/向下计数溢出更新中断 */
  TIM3_Cmd(ENABLE);
  
  while(TIM3_TimingDelay != 0);
}

void sleep_us(uint32_t us) {
  if(us == 0 || us > 1000) return;
  
  TIM3_TimingDelayUs = us;
  TIM3_TimeBaseInit(TIM3_PRESCALER_16,TIM3_TimingDelayUs);
  TIM3_SetCounter(0);                           /* 将计数器初值设为0 */
  TIM3_ARRPreloadConfig(DISABLE);	        /* 预装载不使能 */
  TIM3_ITConfig(TIM3_IT_UPDATE , ENABLE);	/* 计数器向上计数/向下计数溢出更新中断 */
  TIM3_Cmd(ENABLE);  
  while(TIM3_TimingDelayUs != 0);
}

INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
{ 
  if(TIM3_TimingDelay > 0)
  {
    TIM3_TimingDelay--;
  }
  
  if(TIM3_TimingDelayUs > 0)
  {
    TIM3_TimingDelayUs = 0;
  }
  
  TIM3_ClearITPendingBit(TIM3_IT_UPDATE);
  
  if(TIM3_TimingDelay == 0 && TIM3_TimingDelayUs == 0)
  {
    TIM3_Cmd(DISABLE);
  }
}


