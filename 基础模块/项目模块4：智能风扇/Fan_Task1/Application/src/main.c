/*!
    \file    main.c
    \brief   main routine		comb2

    \version 2021-08, V0.1
*/

#include "gd32f4xx.h"
#include "main.h"
#include "systick.h"

#include "pca9685.h"
#include "i2c.h"
#include "ht16k33.h"
#include "i2c_test.h"
#include <math.h>

uint8_t MODE = 0;
uint8_t motorstate = 0;

i2c_addr_def e1_nixie_tube_addr;
i2c_addr_def e1_rgb_led_addr;
i2c_addr_def e2_fan_addr;
i2c_addr_def e3_curtain_addr;
i2c_addr_def s1_key_addr;
i2c_addr_def s5_nfc_addr;
i2c_addr_def s6_ult_addr;
i2c_addr_def s7_ir_addr;
i2c_addr_def s2_sensor_addr;



int key_trans(void){//Translation from Array to Key number
	if(keyvalue[0]&0x01){
		return 1;
	}
	else if(keyvalue[2]&0x01){
		return 2;
	}
	else if(keyvalue[4]&0x01){
		return 3;
	}
	else if(keyvalue[0]&0x02){
		return 4;
	}
	else if(keyvalue[2]&0x02){
		return 5;
	}
	else if(keyvalue[4]&0x02){
		return 6;
	}
	return 0;
}

struct s2_para{
	float temperature;
	float humidity;
} s2_data;
void s2_read_sht3x(void){
	uint8_t th_value[6];
	uint16_t tmp;
  i2c_byte_write(s2_sensor_addr.periph,s2_sensor_addr.addr,0x2C,0x06);
	delay_1ms(100);
	i2c_read(s2_sensor_addr.periph,s2_sensor_addr.addr,0x21,th_value,6);
	
	tmp=(th_value[0]<<8)+ th_value[1];
	s2_data.temperature=(float)tmp*175/(65536-1)-45; 
	
  tmp=(th_value[3]<<8) + th_value[4];
	s2_data.humidity=(float)tmp*100/(65536-1); 
	
}



void i2c_init(void){
	i2c0_gpio_config();
		/* configure I2C */
    i2c0_config(); 			
		/* configure I2C GPIO */
    i2c1_gpio_config();
		/* configure I2C */
    i2c1_config(); 	
		systick_config();
		i2c_addr_init();
	
}


int main(void)
{


	i2c_init();
	//0 - 手动模式
	//1 - 自动模式

	while(1){
		//手动模式
		if(MODE==0){
			i2c_read(s1_key_addr.periph,s1_key_addr.addr,KEYKS0,keyvalue,6);//get key status
			switch(key_trans()){
				case 1:
				{
					pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,0);
					motorstate=0;
					digital_control(motorstate);
					break;
				}
				case 2: 
				{
					pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,1);
					motorstate=1;
					digital_control(motorstate);
					break;
				}
				case 3:
				{
					pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,2);
					motorstate=2;
					digital_control(motorstate);
					break;
				}
				case 4:
				{
					pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,3);
					motorstate=3;
					digital_control(motorstate);
					break;
				}
				case 5:
				{
					MODE = 1;
					break;
				}
				default:
					pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,0);
					motorstate=0;
					digital_control(motorstate);
			}
			delay_1ms(100);
		}
		if(MODE==1)
		{
			i2c_read(s1_key_addr.periph,s1_key_addr.addr,KEYKS0,keyvalue,6);//get key status
			// int temperture=(int)(1.07*s2_data.temperature-2.7+exp((17.27*s2_data.temperature)/(237.27+s2_data.temperature)));
			s2_read_sht3x();
			int temperture = (int)s2_data.temperature;
			digital_control(temperture);

			if(temperture<25){
				pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,0);
			}

			if(temperture>=25&&temperture<=27){
				pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,1);
			}

			if(temperture>27&&temperture<=30){
				pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,2);
			}

			if(temperture>30){
				pc9685_motor_control(e2_fan_addr.periph,e2_fan_addr.addr,3);
			}
			if(key_trans()==5)
			{
    		    MODE = 0;
    		}
    		delay_1ms(100);
        }
        
    }
}
