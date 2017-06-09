
#include "stm8s.h"

uint32_t TIM3_TimingDelay = 0; 
uint32_t TIM3_TimingDelayUs = 0;

void sleep_ms(uint32_t ms) {
  TIM3_TimingDelay = ms;
  TIM3_TimeBaseInit(TIM3_PRESCALER_16,1000);
  TIM3_SetCounter(0);                           /* ����������ֵ��Ϊ0 */
  TIM3_ARRPreloadConfig(DISABLE);	        /* Ԥװ�ز�ʹ�� */
  TIM3_ITConfig(TIM3_IT_UPDATE , ENABLE);	/* ���������ϼ���/���¼�����������ж� */
  TIM3_Cmd(ENABLE);
  
  while(TIM3_TimingDelay != 0);
}

void sleep_us(uint32_t us) {
  if(us == 0 || us > 1000) return;
  
  TIM3_TimingDelayUs = us;
  TIM3_TimeBaseInit(TIM3_PRESCALER_16,TIM3_TimingDelayUs);
  TIM3_SetCounter(0);                           /* ����������ֵ��Ϊ0 */
  TIM3_ARRPreloadConfig(DISABLE);	        /* Ԥװ�ز�ʹ�� */
  TIM3_ITConfig(TIM3_IT_UPDATE , ENABLE);	/* ���������ϼ���/���¼�����������ж� */
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


