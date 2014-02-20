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
extern uint8_t I2C1_Buffer_Tx[];
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
  //I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
  //initial nunchuck function
  mode = 1;
  Direction = Transmitter;
  
  GPIO_ResetBits(GPIOB, GPIO_Pin_13);
  
  //I2C_SoftwareResetCmd(I2C1, DISABLE);
 
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on I2C1 EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 
  /* Send I2C2 slave Address for write */
  //I2C_Send7bitAddress(I2C1, 0xA4, I2C_Direction_Transmitter); 
  I2C_Send7bitAddress(I2C1, 0x31, I2C_Direction_Transmitter);
  /* Test on I2C2 EV1 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED));  
  /* Test on I2C1 EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  /*printf(" start DMA !\n");  
  
  I2C1_Buffer_Tx[0] = 0xF0;
  I2C1_Buffer_Tx[1] = 0x55;
  I2C1_Buffer_Tx[2] = 0x01;
  I2C1_Buffer_Tx[3] = 0x02;
  I2C1_Buffer_Tx[4] = 0x03;
  I2C1_Buffer_Tx[5] = 0x04;
  I2C1_Buffer_Tx[6] = 0x05;
  I2C1_Buffer_Tx[7] = 0x06;
  I2C1_Buffer_Tx[8] = 0x07;
  I2C1_Buffer_Tx[9] = 0x08;
  I2C1_Buffer_Tx[10] = 0x09;
  I2C1_Buffer_Tx[11] = 0x0A; */ 
  
  /* Enable I2C1 DMA */
  I2C_DMACmd(I2C1, ENABLE);  
  /* Enable I2C2 DMA */
  I2C_DMACmd(I2C2, ENABLE);

  /* Enable DMA1 Channel5 */
  DMA_Cmd(DMA1_Channel5, ENABLE);
  /* Enable DMA1 Channel6 */
  DMA_Cmd(DMA1_Channel6, ENABLE);
  
  /* DMA1 Channel6 transfer complete test */
  while(!DMA_GetFlagStatus(DMA1_FLAG_TC6));
  
  GPIO_SetBits(GPIOB, GPIO_Pin_13);
  /* Send I2C1 STOP Condition */
  //I2C_GenerateSTART(I2C1, ENABLE);
  
  while (mode != 2)
  {}
  printf(" initial succeed !\n");
  //initial first read action
  DMA_sig = 1;
  
  //initial PB13
  //GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}  





