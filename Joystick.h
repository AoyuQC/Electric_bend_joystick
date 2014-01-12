/*
 * joystick.h
 *
user header files of joystick project
 */
//#define I2C_DMA_ISR
//#define I2C_ISR
#define I2C_DMA
#define I2C_SM_DEBUG
//#define I2C_DATA_DEBUG
//#define I2C_DMA_EXPERIMENT 





//user define debug funcions
#include "func_debug.h"

//user configuraion of hardware
#include "hardware_config.h"

//I2C function
#include "i2c_func.h"

//user initialization of system
#include "system_config.h"

//header file of schedule
#include "sch.h"

//header file of r/w data of nunchuck
#include "joystick_data_rw.h"

//header file of processing data of nunchuck
#include "joystick_data_proc.h"

//header file of buffer function
#include "buffer.h"