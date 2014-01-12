/**
  ******************************************************************************
  * @file    main
  * @author  zay
  * @version V1.0
  * @date    12/30/2013
  * @brief   Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "joystick.h"

/* Private variables ---------------------------------------------------------*/

//debug

int main(void)
{
  //initial hardware configuraiton
  Hardware_Configuration();
  
  System_Initialization();
  
  while(1)
  {
    SCH_Dispatch_Tasks();
  }    
}

