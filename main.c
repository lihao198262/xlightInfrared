
#include "stm8s.h"
#include "nrf24l01.h"
#include "timer.h"

int main( void )
{
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  enableInterrupts(); 
  
  nRF24L01_Init();
  
  Infrared_Init();
  
  uint8_t data[] = {0xA6,0x12,0x00,0x02,0x40,0x40,0x00,0x20,0x00,0x00,0x00,0x00,0x84, 0xDE};
  
  while(1)
  {
    nRF24L01_Set_RX_Mode();
    if(nRF24L01_RX_Data())
    {
      GPIO_WriteReverse(GPIOC, GPIO_PIN_2);
      
      Haier_Infrared_Send(data, 14);  
      
      sleep_ms(100);
    }
  }
 }
