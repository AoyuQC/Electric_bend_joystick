/**
  ******************************************************************************
  * @file    joystick_data_rw.c 
  * @author  zay
  * @version V1.0
  * @date    
  * @brief   1.Ask_stick_data;
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
//overall header file of STM32F10x
#include "stm32f10x.h"
#include "joystick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
__IO uint8_t Tx1_Idx = 0, Rx1_Idx = 0;
extern uint8_t Direction;
extern uint8_t DMA_sig;
extern uint8_t I2C1_Buffer_Rx[];
extern volatile FIFO_TypeDef NunchuckRx;
uint8_t reset_count = 0;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void Ask_stick_data()
{
  if (DMA_sig == 1)
  {
    // put DMA data into ring buffer
    BufferPut(&NunchuckRx, '$');
    int i = 0;
    for (i = 0; i < 6; i++)
    {
      BufferPut(&NunchuckRx, I2C1_Buffer_Rx[i]);
    }
    BufferPut(&NunchuckRx, '*');
    
    //initial next communicate
    Direction = Transmitter; 
    uint8_t temp = I2C_ReceiveData(I2C1);
    I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
    //Send I2C1 START condition
    I2C_GenerateSTART(I2C1, ENABLE);

    DMA_sig = 0;
  }
}
  