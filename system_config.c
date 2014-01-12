/**
  ******************************************************************************
  * @file    system_config.c 
  * @author  zay
  * @version V1.0
  * @date    
  * @brief   System_Configuration
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
//overall header file of STM32F10x
#include "stm32f10x.h"
#include "joystick.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t mode = 0;
uint8_t Direction = Transmitter;
extern uint8_t DMA_sig;
volatile FIFO_TypeDef NunchuckRx;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Ring_buf_Initialization(void);
void Sch_Initialization(void);
void Nunchuck_Initialization(void);

void System_Initialization()
{
  //initial ring buffers of system
  Ring_buf_Initialization();
  
  //initial schedule
  Sch_Initialization();
  
  //initial nunchuck
  Nunchuck_Initialization();
}

//initial ring buffers for 
//1. ask_i2c_data between processing_i2c_data
void Ring_buf_Initialization()
{
  BufferInit(&NunchuckRx);
}

//initial schedule of system
void Sch_Initialization()
{ 
  SCH_Init_Task();
  SCH_Add_Task(Ask_stick_data,0,1);
  SCH_Add_Task(Proc_stick_data,1,1);
}

//initial nunchuck 
void Nunchuck_Initialization()
{
  //check whether bus is busy or not
  //while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  //{
  //  I2C_SoftwareResetCmd(I2C1, ENABLE);
  //}
  
  /* Enable I2C1 event and buffer interrupts */
  I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
  //initial nunchuck function
  mode = 1;
  Direction = Transmitter;
  
  I2C_SoftwareResetCmd(I2C1, DISABLE);
 
  I2C_GenerateSTART(I2C1, ENABLE);
  while (mode != 3)
  {}
  printf(" initial succeed !\n");
  //initial first read action
  DMA_sig = 1;
}  





