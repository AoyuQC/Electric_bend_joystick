/**
  ******************************************************************************
  * @file    joystick_data_proc.c 
  * @author  zay
  * @version V1.0
  * @date    
  * @brief   1.Proc_stick_data;
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
//overall header file of STM32F10x
#include "stm32f10x.h"
#include "joystick.h"
    
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
uint8_t nunchuck_data[4] = {'S','0','0','*'};
uint8_t start_get = 0;
uint8_t get_data[128];
extern uint8_t I2C1_Buffer_Rx[];
extern volatile FIFO_TypeDef NunchuckRx; 
extern int cnt;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Data_printf(uint8_t *rx_data, uint8_t frame_count);
void Data_uart(uint8_t *rx_data, uint8_t frame_count);
    
void Proc_stick_data()
{
  uint8_t i;
  uint8_t count;
  uint8_t index = 0;
  uint8_t ch;
  uint8_t frame_count = 0;
  
  count = NunchuckRx.count;
  for (i = 1; i <= count; i++)
  {
    //printf("the i is %d \n",i);
    if (BufferGet(&NunchuckRx, &ch) == SUCCESS)
    {
      if (ch == '$')
      {
        start_get = 1;
      }
      else if (ch == '*')
      {
        start_get = 0;
        frame_count++;
      }
      else if(start_get == 1)
      {
        get_data[index] = ch;
        index++;
      }  
    }
    else
    {
      printf("BufferGet fail : run of U1Rx \n");
    }
  }
  
  //printf received data
  Data_printf(get_data, frame_count);
  
  //transfer position data
  //Data_uart(get_data, frame_count);
}

//transfer data through UART
void Data_uart(uint8_t *rx_data, uint8_t frame_count)
{
  while (frame_count--)
  {
    nunchuck_data[1] = rx_data[0 + frame_count * 6];
    nunchuck_data[2] = rx_data[1 + frame_count * 6];
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  }
}
    

  
//printf received data
void Data_printf(uint8_t *rx_data, uint8_t frame_count)
{
  while (frame_count--)
  {
    unsigned int  btn_z=0, btn_c=0;
    int joy_x_axis   = rx_data[0 + frame_count * 6];
    int joy_y_axis   = rx_data[1 + frame_count * 6];
    int accel_x_axis = rx_data[2 + frame_count * 6]; 
    int accel_y_axis = rx_data[3 + frame_count * 6];
    int accel_z_axis = rx_data[4 + frame_count * 6];
    //int accel_x_axis = rx_data[2 + frame_count * 6] * 2 * 2; 
    //int accel_y_axis = rx_data[3 + frame_count * 6] * 2 * 2;
    //int accel_z_axis = rx_data[4 + frame_count * 6] * 2 * 2;
    
    if((rx_data[5 + frame_count * 6]) >> 0 & 1)
    {
      btn_z = 1;
    }
    if((rx_data[5 + frame_count * 6]) >> 1 & 1)
    {
      btn_c = 1;
    }
    /*if((rx_data[5 + frame_count * 6]) >> 2 & 1)
    {
      accel_x_axis += 2;
    }
     if((rx_data[5 + frame_count * 6]) >> 3 & 1)
    {
      accel_x_axis += 1;
    }
    
    if((rx_data[5 + frame_count * 6]) >> 4 & 1)
    {
      accel_y_axis += 2;
    }
    if((rx_data[5 + frame_count * 6]) >> 5 & 1)
    {
      accel_y_axis += 1;
    }
    if((rx_data[5 + frame_count * 6]) >> 6 & 1)
    {
      accel_z_axis += 2;
    }
     if((rx_data[5 + frame_count * 6]) >> 7 & 1)
    {
      accel_z_axis += 1;
    }*/
    printf ("%d",joy_x_axis);
    printf ("\t");
    printf ("%d",joy_y_axis);
    printf ("\t");
    printf("%d",accel_x_axis);
    printf ("\t");
    printf("%d",accel_y_axis);
    printf ("\t");
    printf("%d",accel_z_axis);
    printf ("\t");
    printf("%d",btn_z);
    printf ("\t");
    printf("%d",btn_c);
    printf ("\t");
    printf("%d",frame_count);
    printf ("\r\n");
  }
} 