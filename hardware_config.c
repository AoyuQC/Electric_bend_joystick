/**
  ******************************************************************************
  * @file    hardware_config.c 
  * @author  zay
  * @version V1.0
  * @date    
  * @brief   Hardware_Configuration
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "hardware_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define I2C1_DR_Address    ((uint32_t)0x40005410)
#define I2C1_SLAVE_ADDRESS7    0x30
#define BufferSize             6
#define ClockSpeed             100000 //20khz
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t I2C1_Buffer_Rx[BufferSize] = {0,0,0,0,0,0};
uint8_t I2C1_Buffer_Tx[2];
__IO uint16_t ADCConvertedValue;    
DMA_InitTypeDef DMA_InitStructure;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
//void EXTI_Configuration(void);
//void TIM_Configuration(void);
void USART_Configuration(void);
void NVIC_Configuration(void);
void DMA_Configuration(void);
void I2C_Configuration(void);

void Hardware_Configuration()
{
  
  //Config RCC(clock PLL flash periph_clock)
  RCC_Configuration();
  
  //Config GPIO
  GPIO_Configuration();
  
  //Config EXTI
  //EXTI_Configuration();
  
  //Config TIM
  //TIM_Configuration();
  
  //Config USART
  USART_Configuration();
  
  //Config NVIC
  NVIC_Configuration();
  
  //Config DMA
  DMA_Configuration();
  
  //Config I2C
  I2C_Configuration();
}
 
//Config RCC
void RCC_Configuration()
{
  //PLL = 72MHZ
  //SYSCLK = 72MHZ
  //HCLK = 72MHZ
  //PCLK1 = 72MHZ
  //PCLK2 = 36MHZ(MAX)
  //SYSCLK Config
  //Enable HSI
  RCC->CR |= 0x00000001; 
  //Check HSI status
  //while(!((RCC->CR & 0x00000002)>>1));
  //Enable HSEON 8Mhz
  RCC->CR |= 0x00010000; 
  while(!RCC_GetFlagStatus(RCC_FLAG_HSERDY))
    
  //Enable Prefetch Buffer
  FLASH->ACR |= 0x00000010; //PRFTBE(4):1

  //Flash 2 wait state
  FLASH->ACR &= 0xFFFFFFFA; //LATENCY(2:0):010
  FLASH->ACR |= 0x00000002;   

  //HCLK(AHB) = SYSCLK
  RCC->CFGR &= 0xFFFFFF0F; //HPRE(4:7):0000
  
  //PCLK2(APB2) = HCLK/2
  RCC->CFGR &= 0xFFFFE7FF; //PPRE2(13:11):100
  RCC->CFGR |= 0x00002000;
  
  //PCLK1(APB1) = HCLK
  RCC->CFGR &= 0xFFFFFBFF; //PPRE1(10:8):0xx  
    
  //PLLCLK = 8MHz * 9 = 72 MHz 
  RCC->CFGR |= 0x001D0000;
  RCC->CFGR &= 0xFFFDFFFF; //PLLSRC(16):1 PLLXTPRE(17):0 PLLMUL(21:18):0111
  //Enable PLL
  RCC->CR |= 0x01000000;//PLLON(24):1
  //Check PLL status
  while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
  
  //SYSCLK = PLLCLK(72 MHz)
  //RCC->CFGR |= 0x00000002;//SW(1:0):10
  //RCC->CFGR &= 0xFFFFFFFE;
  RCC->CFGR &= 0xFFFFFFFC;
  RCC->CFGR |= 0x00000002;
  //Check clock source
  while(!(((RCC->CFGR & 0x0000000C)>>2)==0x2)); //SWS(3:2):10
  
  //AHB Config
  //enable DMA1 clock
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  //APB2 Config
  //enable USART1 clock
  RCC->APB2ENR |= 0x00004000;//USART1(14):1
  //enable GPIOA clock
  RCC->APB2ENR |= 0x00000004; //IOPA(4):1
  //enable GPIOB clock
  RCC->APB2ENR |= 0x00000008; //IOPB(3):1
  //enable GPIOD clock
  RCC->APB2ENR |= 0x00000020; //IOPD(5):1
  //enable GPIOE clock
  RCC->APB2ENR |= 0x00000040; //IOPE(6):1
  //enable GPIOF clock
  RCC->APB2ENR |= 0x00000080; //IOPF(7):1
  //enable GPIOG clock
  RCC->APB2ENR |= 0x00000100; //IOPG(8):1
  //enable AFIO clock
  RCC->APB2ENR |= 0x00000001; //AFIO(1):1 
  
  //APB1 Config
  //enable I2C1 clock
  RCC->APB1ENR |= 0x00200000; //I2C1EN(21):1
  //enable TIM2 clock
  /*RCC->APB1ENR |= 0x00000001;//TIM2(0):1
  //enable TIM3 clock
  RCC->APB1ENR |= 0x00000002;//TIM3(1):1
  //enable TIM4 clock
  RCC->APB1ENR |= 0x00000004;//TIM4(2):1
  //enable TIM5 clock
  RCC->APB1ENR |= 0x00000008;//TIM5(3):1*/
  
  //Setup SysTick Timer for 1 msec interrupts
  if (SysTick_Config(SystemFrequency / 1000))
  { 
    // Capture error 
   while (1);
  }
}

//Config GPIO
void GPIO_Configuration()
{
  //USART1
  //TX:PA9 ouput AF push-pull 2Mhz 1010
  GPIOA->CRH &= 0xFFFFFFAF;
  GPIOA->CRH |= 0x000000A0;
  //RX:PA10 input floating 0100
  GPIOA->CRH &= 0xFFFFF4FF;
  GPIOA->CRH |= 0x00000400;
  
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF, ENABLE);
  
  //EXTI
  //EXTI Line 3
  //EXTI3:PD3 input floating 0100
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  //EXTI Line 6
  //EXTI6:PD6 input floating 0100
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  //test for FSMC_A6 <-> EXTI12  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  //test for FSMC_A7 <-> EXTI13
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure); 
  
  //LED1-3(D39-41):PF6,8,10 output push-pull 2Mhz 0010
  GPIOF->CRL &= 0xF2FFFFFF;
  GPIOF->CRL |= 0x02000000;
  GPIOF->CRH &= 0xFFFFF2F2;
  GPIOF->CRH |= 0x00000202;
  GPIOF->CRH &= 0xFFF2FFFF;
  GPIOF->CRH |= 0x00020000;
  
  //I2C1
  //Configure I2C1 pins: SCL and SDA
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //PB13
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

//Config NVIC
void NVIC_Configuration()
{
  //Set the Vector Table base location at 0x20000000  
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
  
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the USARTy Interrupt */
  /*NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);*/

  /* Configure and enable I2C1 event interrupt -------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
}

//Config EXTI
void EXTI_Configuration()
{
  //EXTI6: PD6
  AFIO->EXTICR[1] &= 0xFFFFF3FF;
  AFIO->EXTICR[1] |= 0x00000300;
  //EXTI12: PF12
  AFIO->EXTICR[3] &= 0xFFFFFFF5;
  AFIO->EXTICR[3] |= 0x00000005;
  //EXTI13: PF13
  AFIO->EXTICR[0] &= 0xFFFF5FFF;
  AFIO->EXTICR[0] |= 0x00005000;
  
  EXTI_InitTypeDef EXTI_InitStructure;
  //Configure EXTI line 6 12 13 
  EXTI_InitStructure.EXTI_Line = EXTI_Line6 | EXTI_Line12 | EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //why not event? 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

//Config TIM
void TIM_Configuration()
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
  //APB1_Prescalor = 2: APB1_clock of TIM3 = APB1_CLK x 2
  /* TIM_frequence = APB1_clock / (TIM_Prescaler + 1)=72M/90=800K */
  TIM_TimeBaseStructure.TIM_Prescaler =89;   
  TIM_TimeBaseStructure.TIM_ClockDivision =0;//TIM_CKD_DIV4;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  
  //TIM_UpdateRequestConfig(TIM3, TIM_UpdateSource_Global);

  /* TIM enable counter */
  TIM_Cmd(TIM2, DISABLE);
  TIM_Cmd(TIM4, ENABLE);
  TIM_Cmd(TIM5, ENABLE);
}

//Config USART
void USART_Configuration()
{
  //USART1 configured as follow:
  //      - BaudRate = 115200 baud  
  //      - Word Length = 8 Bits
  //      - One Stop Bit
  //      - No parity
  //      - Hardware flow control disabled (RTS and CTS signals)
  //      - Receive and transmit enabled
  USART_InitTypeDef USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure);
  
  //Enable USART1 Receive and Transmit interrupts
  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
  
  //Enable the USART1
  USART_Cmd(USART1, ENABLE);
}

//Config DMA
void DMA_Configuration()
{
  //DMA1 channel 7 configuration: I2C Rx
  DMA_DeInit(DMA1_Channel7);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2C1_Buffer_Rx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = BufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel7, &DMA_InitStructure);

  DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
  
  //DMA1 channel 6 configuration: I2C Tx
  DMA_DeInit(DMA1_Channel6);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)I2C1_Buffer_Tx;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);
  //DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE); 
 
  DMA_Cmd(DMA1_Channel7, DISABLE);
  DMA_Cmd(DMA1_Channel6, DISABLE);
}

//Config I2C
void I2C_Configuration()
{
  //check whether bus is busy or not
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    I2C_SoftwareResetCmd(I2C1, ENABLE);
  }
  
  I2C_InitTypeDef   I2C_InitStructure;
  //I2C1 configuration
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
  I2C_Init(I2C1, &I2C_InitStructure); 
  
  I2C_Cmd(I2C1, ENABLE);
  /* Enable I2C1 event and buffer interrupts */
  I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
}  



