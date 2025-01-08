/*!
    \file  main.h
    \brief the header file of main 
		\version 2021-08, V0.1
*/
#include "gd32f4xx.h"

//#define PRINT_DEBUG_INFO
	


#define LED_RED 1
#define LED_GREEN 2	
#define LED_BLUE 3

#define LED_RED_GREEN_MID 4
#define LED_GREEN_BLUE_MID 5
#define LED_BLUE_RED_MID 6

#define LED_YELLOW 7
#define LED_SKYBLUE 8
#define LED_PURPLE 9

#define LED_WHITE 10
#define LED_OFF 11



#define ETH				0
#define	USB				0
#define	I2S				0

#define	E1				1
#define	E2				1
#define	E3				1

#define	S1				1
#define	S5				1
#define	S6				1
#define	S7				1



#define	KEYKS0				0x40

#define HT16K33_ADDRESS_S1    0xE8
#define PCA9685_ADDRESS_E1    0xC0			//0xC0¡¢0xC2¡¢0xC4¡¢0xC6
#define GD32F330_ADDRESS_S6 0x58
#define GD32F330_READ_CMD 0XAA

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define PCA9685_MODE1 0x0

#define	SYSTEM_ON				0x21

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

////I2C address struct
//typedef struct{
//	uint8_t flag;
//	uint32_t periph;
//	uint8_t addr;
//} i2c_addr_def;

void setPWM(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t num, uint16_t on, uint16_t off);//Set signal PWM
void setPWM_off(uint32_t i2c_periph,uint8_t i2c_addr);
void i2c_addr_init(void);
void led_control(uint8_t color);//Control over target LED
void key_control(int key);//Control over 
int key_trans(void);//Translation from Array to Key number
