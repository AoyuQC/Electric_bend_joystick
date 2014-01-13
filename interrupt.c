/**
  ******************************************************************************
  * @file    EXTI/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stdio.h"
#include "buffer.h"
#include "sch.h"
#include "Joystick.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup EXTI_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t error = 0;
uint8_t DMA_sig = 0;
uint8_t i = 0;
extern sTask SCH_Tasks_G[SCH_MAX_TASKS];
extern __IO uint8_t Tx1_Idx, Rx1_Idx, Direction;
extern uint8_t I2C1_Buffer_Tx[], I2C1_Buffer_Rx[];
extern volatile FIFO_TypeDef NunchuckRx;
extern DMA_InitTypeDef DMA_InitStructure;
extern uint8_t nunchuck_data[]; 
//debug
extern uint8_t mode;
uint8_t sm_count[100];
uint8_t data_count[100];
uint8_t r_data_count[100];
uint8_t num = 0;
uint8_t over_pos = 0;
uint32_t laststatus = 0;
uint8_t data_frame = 0;
uint8_t r_data_frame = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}


/**
  * @brief 这是调度器的中断服务程序，初始化函数中的定时器设置决定了它了调用频率
  * 这个版本由SysTick定时器中断触发，定时器每1ms触发一次
  */

/***********************************************************************************/
void SysTick_Handler(void)
{
	tByte Index;
        
	// SysTick定时器无须清除中断标志； 
	for(Index = 0;Index < SCH_MAX_TASKS;Index++)
	{
		if(SCH_Tasks_G[Index].pTask)     //表示确实有分配一个任务指针？
		{
			if(SCH_Tasks_G[Index].Delay == 0)
			{
				//任务需要运行
				SCH_Tasks_G[Index].RunMe +=1;	//“RunMe”标志加1
				if(SCH_Tasks_G[Index].Period)
				{
					//调度周期性的任务再次运行
					SCH_Tasks_G[Index].Delay = SCH_Tasks_G[Index].Period-1;//为什么减1：为了第period毫秒runme设为1，立刻执行
				}
			}				
			else
			{	
				//还没准备好允许，延迟减1
				SCH_Tasks_G[Index].Delay -= 1;
			}
		}
	}
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles External lines 9 to 5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
}

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{

}



/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
  /*if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  { 
    remote_data[indexx] = (USART_ReceiveData(USART1) & 0x7F);

    if(U1Rx.count < USARTBUFFSIZE)
    {
      if(remote_data[indexx] == '$')
      {
        //printf("run to here");
        start_put = 1;
        BufferPut(&U1Rx, remote_data[indexx++]);
      }
      else if(start_put == 1)
      {
        BufferPut(&U1Rx, remote_data[indexx++]);
        if(remote_data[--indexx]  == '*')
        {
          //printf("finish one receive");
          start_put = 0;
          indexx = 0;
          dir_count++;
        }
     }
    }
    else
    {
      error = 1;//printf("BufferGet fail : run of U1Rx \n");
    }
    
    //printf("\n get new value %d",INSTR); debug purpose
    //USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }*/
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {   
    USART_SendData(USART1, nunchuck_data[i++]);

    if(i == 4)
    {
      i = 0;
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }    
  }
}

/**
  * @brief  This function handles I2C1 Event interrupt request.
  * @param  None
  * @retval None
  */
void I2C1_EV_IRQHandler(void) 
{
  laststatus = I2C_GetLastEvent(I2C1);
#ifdef I2C_ISR
  switch (laststatus)
  {
    case I2C_EVENT_MASTER_MODE_SELECT:                 /* EV5 */
      if (Direction == Transmitter)
      {
        /* Master Transmitter ----------------------------------------------*/
        /* Send slave Address for write */
        I2C_Send7bitAddress(I2C1, 0xA4, I2C_Direction_Transmitter);
      }
      else
      {
        /* Master Receiver -------------------------------------------------*/
        /* Send slave Address for read */
        I2C_Send7bitAddress(I2C1, 0xA4, I2C_Direction_Receiver);
      }
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 1;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 1;
      }
      #endif
      break;
        
    /* Master Transmitter --------------------------------------------------*/
    /* Test on I2C1 EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:  
      /* Send the first data */
      if (mode == 1)
      {
        I2C_SendData(I2C1, 0xF0);
        mode = 2;
      }
      else if (mode == 4)
      {
        I2C_SendData(I2C1, 0xFB);
        mode = 5;
      }
      else
        I2C_SendData(I2C1, 0x00);
     
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 2;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 2;
      }
      #endif 
      break;

    /* Test on I2C1 EV8 and clear it */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:  /* Without BTF, EV8 */ 
      if (mode == 2)
      {
        I2C_SendData(I2C1, 0x55);
        mode = 3;
      }
      else if (mode == 5)
      {
        I2C_SendData(I2C1, 0x00);
        mode = 6;
      }
      else if (mode == 7)
      {
        I2C_SendData(I2C1, 0xFA);
        mode = 8;
      }
      else
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
      }
      
      #ifdef I2C_SM_DEBUG  
      sm_count[num] = 3;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 3;
      }
      #endif
      break;

    case I2C_EVENT_MASTER_BYTE_TRANSMITTED: /* With BTF EV8-2 */
      if (mode == 3)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        I2C_GenerateSTART(I2C1, ENABLE);
        mode = 4;
      }
      else if (mode == 6)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        I2C_GenerateSTOP(I2C1, ENABLE);
        mode = 7;
      }
      else if (mode == 8)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        //I2C_SendData(I2C1, 0xFA);
        I2C_GenerateSTOP(I2C1, ENABLE);
        Direction = Receiver;
        I2C_GenerateSTART(I2C1, ENABLE);
        mode = 7;
      }

      #ifdef I2C_SM_DEBUG
      sm_count[num] = 4;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 4;
      }
      #endif
      break;

    /* Master Receiver -------------------------------------------------------*/
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
      if(Tx1BufferSize == 1)
      {
        /* Disable I2C1 acknowledgement */
        I2C_AcknowledgeConfig(I2C1, DISABLE);
        /* Send I2C1 STOP Condition */
        I2C_GenerateSTOP(I2C1, ENABLE);
      }
      
      //I2C_DMACmd(I2C1, ENABLE);
      //DMA_Cmd(DMA1_Channel7, ENABLE);
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 5;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 5;
      }
      #endif
      break;

   /* Test on I2C1 EV7 and clear it */
   case I2C_EVENT_MASTER_BYTE_RECEIVED:
     /* Store I2C1 received data */
     //I2C1_Buffer_Rx[Rx1_Idx++] = I2C_ReceiveData (I2C1);
     if (NunchuckRx.count < NUNCHUCKBUFSIZE)
     {
       if (Rx1_Idx == 0)
       {
         //indicate a start of nunchuck data
         BufferPut(&NunchuckRx, '$');
       }
       
       BufferPut(&NunchuckRx, I2C_ReceiveData(I2C1));
       //BufferPut(&NunchuckRx, I2C1_Buffer_Rx[Rx1_Idx-1]);
       Rx1_Idx++;
       
       if (Rx1_Idx == (Tx1BufferSize -1))
       {
         /* Disable I2C1 acknowledgement */
         I2C_AcknowledgeConfig(I2C1, DISABLE);
         /* Send I2C1 STOP Condition */
         I2C_GenerateSTOP(I2C1, ENABLE);
       }   
       
       if (Rx1_Idx == Tx1BufferSize)
       {
         //indicate an end of nunchuck data
         BufferPut(&NunchuckRx, '*');
         I2C_AcknowledgeConfig(I2C1, ENABLE);
       }
     }
     else
     {
       error = 1;
     }
     
     #ifdef I2C_SM_DEBUG
     sm_count[num] = 6;
     num++;
     if (num >= 100)
     {
       num = 99;
       over_pos = 6;
     }
     #endif
     break;

    default:
      break;
  }
#elif defined I2C_DMA_ISR
  switch (laststatus)
  {
    case I2C_EVENT_MASTER_MODE_SELECT:                 /* EV5 */
      if (Direction == Transmitter)
      {
        /* Master Transmitter ----------------------------------------------*/
        /* Send slave Address for write */
        I2C_Send7bitAddress(I2C1, 0xA4, I2C_Direction_Transmitter);
      }
      else
      {
        /* Master Receiver -------------------------------------------------*/
        /* Send slave Address for read */
        I2C_Send7bitAddress(I2C1, 0xA4, I2C_Direction_Receiver);
      }
      
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 1;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 1;
      }
      #endif
      break;
        
    /* Master Transmitter --------------------------------------------------*/
    /* Test on I2C1 EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:  
      /* Send the first data */
      if (mode == 1)
      {
        I2C_SendData(I2C1, 0xF0);
        mode = 2;
      }
      else if (mode == 4)
      {
        I2C_SendData(I2C1, 0xFB);
        mode = 5;
      }
      else
        I2C_SendData(I2C1, 0x00);
     
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 2;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 2;
      }
      #endif 
      break;

    /* Test on I2C1 EV8 and clear it */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:  /* Without BTF, EV8 */ 
      if (mode == 2)
      {
        I2C_SendData(I2C1, 0x55);
        mode = 3;
      }
      else if (mode == 5)
      {
        I2C_SendData(I2C1, 0x00);
        mode = 6;
      }
      else if (mode == 7)
      {
        I2C_SendData(I2C1, 0xFA);
        mode = 8;
      }
      else
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
      }
      
      #ifdef I2C_SM_DEBUG  
      sm_count[num] = 3;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 3;
      }
      #endif
      break;

    case I2C_EVENT_MASTER_BYTE_TRANSMITTED: /* With BTF EV8-2 */
      if (mode == 3)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        I2C_GenerateSTART(I2C1, ENABLE);
        mode = 4;
      }
      else if (mode == 6)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        I2C_GenerateSTOP(I2C1, ENABLE);
        mode = 7;
      }
      else if (mode == 8)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        //I2C_SendData(I2C1, 0xFA);
        I2C_GenerateSTOP(I2C1, ENABLE);
        Direction = Receiver;
        I2C_GenerateSTART(I2C1, ENABLE);
        mode = 7;
      }

      #ifdef I2C_SM_DEBUG
      sm_count[num] = 4;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 4;
      }
      #endif
      break;

    /* Master Receiver -------------------------------------------------------*/
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
      if(Tx1BufferSize == 1)
      {
        /* Disable I2C1 acknowledgement */
        I2C_AcknowledgeConfig(I2C1, DISABLE);
        /* Send I2C1 STOP Condition */
        I2C_GenerateSTOP(I2C1, ENABLE);
      }
      
      I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
      I2C_DMACmd(I2C1, ENABLE);
      DMA_Cmd(DMA1_Channel7, ENABLE);
      
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 5;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 5;
      }
      #endif
      break;

   /* Test on I2C1 EV7 and clear it */
   case I2C_EVENT_MASTER_BYTE_RECEIVED:
     /* Store I2C1 received data */
     //I2C1_Buffer_Rx[Rx1_Idx++] = I2C_ReceiveData (I2C1);
     if (NunchuckRx.count < NUNCHUCKBUFSIZE)
     {
       if (Rx1_Idx == 0)
       {
         //indicate a start of nunchuck data
         BufferPut(&NunchuckRx, '$');
       }
       
       BufferPut(&NunchuckRx, I2C_ReceiveData(I2C1));
       //BufferPut(&NunchuckRx, I2C1_Buffer_Rx[Rx1_Idx-1]);
       Rx1_Idx++;
       
       if (Rx1_Idx == (Tx1BufferSize -1))
       {
         /* Disable I2C1 acknowledgement */
         I2C_AcknowledgeConfig(I2C1, DISABLE);
         /* Send I2C1 STOP Condition */
         I2C_GenerateSTOP(I2C1, ENABLE);
       }   
       
       if (Rx1_Idx == Tx1BufferSize)
       {
         //indicate an end of nunchuck data
         BufferPut(&NunchuckRx, '*');
         I2C_AcknowledgeConfig(I2C1, ENABLE);
       }
     }
     else
     {
       error = 1;
     }
     
     #ifdef I2C_SM_DEBUG
     sm_count[num] = 6;
     num++;
     if (num >= 100)
     {
       num = 99;
       over_pos = 6;
     }
     #endif
     break;

    default:
      break;
  }
#elif defined I2C_DMA
  switch (laststatus)
  {
    case I2C_EVENT_MASTER_MODE_SELECT:                 /* EV5 */
      if (Direction == Transmitter)
      {
        /* Master Transmitter ----------------------------------------------*/
        /* Send slave Address for write */
        I2C_Send7bitAddress(I2C1, 0xA4, I2C_Direction_Transmitter);
      }
      else
      {
        /* Master Receiver -------------------------------------------------*/
        /* Send slave Address for read */
        I2C_Send7bitAddress(I2C1, 0xA4, I2C_Direction_Receiver);
      }
      
      I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
      
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 1;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 1;
      }
      #endif
      break;
        
    /* Master Transmitter --------------------------------------------------*/
    /* Test on I2C1 EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
      I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);  
      /* Send the first data */
      if (mode == 1)
      {
        I2C1_Buffer_Tx[0] = 0xF0;
        I2C1_Buffer_Tx[1] = 0x55;
        I2C_DMACmd(I2C1, ENABLE);
        DMA_Cmd(DMA1_Channel6, ENABLE);
      }
      else if (mode == 2)
      {
        I2C1_Buffer_Tx[0] = 0xFB;
        I2C1_Buffer_Tx[1] = 0x00;
        I2C_DMACmd(I2C1, ENABLE);
        DMA_Cmd(DMA1_Channel6, ENABLE);
      }
      else if (mode == 3)
      {
        I2C1_Buffer_Tx[0] = 0x00;
        I2C1_Buffer_Tx[1] = 0xFA;
        I2C_DMACmd(I2C1, ENABLE);
        DMA_Cmd(DMA1_Channel6, ENABLE);
        mode = 4;
      }
     
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 2;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 2;
      }
      #endif 
      break;

    /* Test on I2C1 EV8 and clear it */
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:  /* Without BTF, EV8 */ 
      /*if (mode == 2)
      {
        I2C_SendData(I2C1, 0x55);
        I2C_DMACmd(I2C1, ENABLE);
        DMA_Cmd(DMA1_Channel7, ENABLE);
        mode = 3;
      }
      else if (mode == 5)
      {
        I2C_SendData(I2C1, 0x00);
        mode = 6;
      }
      else if (mode == 7)
      {
        I2C_SendData(I2C1, 0xFA);
        mode = 8;
      }
      else
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
      }*/
      
      #ifdef I2C_SM_DEBUG  
      sm_count[num] = 3;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 3;
      }
      #endif
      break;

    case I2C_EVENT_MASTER_BYTE_TRANSMITTED: /* With BTF EV8-2 */
      if (mode == 3)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        I2C_GenerateSTART(I2C1, ENABLE);
        mode = 4;
      }
      else if (mode == 6)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        I2C_GenerateSTOP(I2C1, ENABLE);
        mode = 7;
      }
      else if (mode == 8)
      {
        I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        //I2C_SendData(I2C1, 0xFA);
        I2C_GenerateSTOP(I2C1, ENABLE);
        Direction = Receiver;
        I2C_GenerateSTART(I2C1, ENABLE);
        mode = 7;
      }

      #ifdef I2C_SM_DEBUG
      sm_count[num] = 4;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 4;
      }
      #endif
      break;

    /* Master Receiver -------------------------------------------------------*/
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
      if(Tx1BufferSize == 1)
      {
        /* Disable I2C1 acknowledgement */
        I2C_AcknowledgeConfig(I2C1, DISABLE);
        /* Send I2C1 STOP Condition */
        I2C_GenerateSTOP(I2C1, ENABLE);
      }
      
      I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
      I2C_DMACmd(I2C1, ENABLE);
      DMA_Cmd(DMA1_Channel7, ENABLE);
      
      #ifdef I2C_SM_DEBUG
      sm_count[num] = 5;
      num++;
      if (num >= 100)
      {
        num = 99;
        over_pos = 5;
      }
      #endif
      break;

   /* Test on I2C1 EV7 and clear it */
   case I2C_EVENT_MASTER_BYTE_RECEIVED:
     /* Store I2C1 received data */
     //I2C1_Buffer_Rx[Rx1_Idx++] = I2C_ReceiveData (I2C1);
     if (NunchuckRx.count < NUNCHUCKBUFSIZE)
     {
       if (Rx1_Idx == 0)
       {
         //indicate a start of nunchuck data
         BufferPut(&NunchuckRx, '$');
       }
       
       BufferPut(&NunchuckRx, I2C_ReceiveData(I2C1));
       //BufferPut(&NunchuckRx, I2C1_Buffer_Rx[Rx1_Idx-1]);
       Rx1_Idx++;
       
       if (Rx1_Idx == (Tx1BufferSize -1))
       {
         /* Disable I2C1 acknowledgement */
         I2C_AcknowledgeConfig(I2C1, DISABLE);
         /* Send I2C1 STOP Condition */
         I2C_GenerateSTOP(I2C1, ENABLE);
       }   
       
       if (Rx1_Idx == Tx1BufferSize)
       {
         //indicate an end of nunchuck data
         BufferPut(&NunchuckRx, '*');
         I2C_AcknowledgeConfig(I2C1, ENABLE);
       }
     }
     else
     {
       error = 1;
     }
     
     #ifdef I2C_SM_DEBUG
     sm_count[num] = 6;
     num++;
     if (num >= 100)
     {
       num = 99;
       over_pos = 6;
     }
     #endif
     break;

    default:
      break;
  }
#endif 
}

void DMA1_Channel7_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_IT_TC7) != RESET)
  {
    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA_ClearFlag(DMA1_IT_TC7);
    
    Rx1_Idx = 6;
    
    DMA_sig = 1;
    
    #ifdef I2C_DATA_DEBUG
    int i = 0;
    for (;i < 6; i++)
    {
      r_data_count[r_data_frame * 6 + i] = I2C1_Buffer_Rx[i];
    }
    r_data_frame++;
    #endif 
    
    #ifdef I2C_SM_DEBUG
    sm_count[num] = 7;
    num++;
    if (num >= 100)
    {
      num = 99;
      over_pos = 7;
    }
    #endif
    delay(10);
    //printf("DMA received one time! \n");
    I2C_AcknowledgeConfig(I2C1, ENABLE);
  }
  
  if (DMA_GetFlagStatus(DMA1_IT_HT7) != RESET)
  {
    DMA_ClearFlag(DMA1_IT_HT7);
    
    #ifdef I2C_DATA_DEBUG
    int i = 0;
    for (;i < 6; i++)
    {
      r_data_count[data_frame * 6 + i] = I2C1_Buffer_Rx[i];
    }
    r_data_frame++;
    #endif

    #ifdef I2C_SM_DEBUG
    sm_count[num] = 77;
    num++;
    if (num >= 100)
    {
      num = 99;
      over_pos = 77;
    }
    #endif
  }
}


void DMA1_Channel6_IRQHandler(void)
{
  if (DMA_GetFlagStatus(DMA1_IT_TC6) != RESET)
  {
    DMA_Cmd(DMA1_Channel6, DISABLE);
    DMA1_Channel6->CNDTR = 3;
    DMA_ClearFlag(DMA1_IT_TC6);
    switch (mode)
    {
      case 1:
        GPIO_SetBits(GPIOB, GPIO_Pin_13);
        //while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == SET);
        I2C_GenerateSTART(I2C1, ENABLE);
        GPIO_ResetBits(GPIOB, GPIO_Pin_13);
        mode = 2;
        break;
      case 2:
        I2C_GenerateSTOP(I2C1, ENABLE);
        mode = 3;
        break;
      case 4:
        I2C_GenerateSTOP(I2C1, ENABLE);
        Direction = Receiver;
        //I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
        I2C_GenerateSTART(I2C1, ENABLE);
        mode = 3;
        break;
      default :
        break;
    }
    
    #ifdef I2C_DATA_DEBUG
    int i = 0;
    for (;i < 2; i++)
    {
      data_count[data_frame * 2 + i] = I2C1_Buffer_Tx[i];
    }
    data_frame++;
    #endif
    
    #ifdef I2C_SM_DEBUG
    sm_count[num] = 8;
    num++;
    if (num >= 100)
    {
      num = 99;
      over_pos = 8;
    }
    #endif
  }
 
  if (DMA_GetFlagStatus(DMA1_IT_HT6) != RESET)
  {
    DMA_ClearFlag(DMA1_IT_HT6);
    
    #ifdef I2C_DATA_DEBUG
    int i = 0;
    for (;i < 2; i++)
    {
      data_count[data_frame * 2 + i] = I2C1_Buffer_Tx[i];
    }
    data_frame++;
    #endif
    
    #ifdef I2C_SM_DEBUG
    sm_count[num] = 88;
    num++;
    if (num >= 100)
    {
      num = 99;
      over_pos = 88;
    }
    #endif
  }
}
  




/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
