#include "gd32f4xx.h"
#include "main.h"
#include "i2c.h"
#include "systick.h"
#include <stdio.h>

i2c_addr_def key_addr; // Key address struct
i2c_addr_def LED_addr; // LED address struct
i2c_addr_def s6_ult_addr;
uint8_t	AUTO_MODE	= 0; // init mode

uint8_t keyvalue[24]; // Storage of key status

void setPWM(uint32_t i2c_periph, uint8_t i2c_addr, uint8_t num, uint16_t on, uint16_t off) {
    i2c_byte_write(i2c_periph, i2c_addr, LED0_ON_L + 4 * num, on);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_ON_H + 4 * num, on >> 8);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_OFF_L + 4 * num, off);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_OFF_H + 4 * num, off >> 8);
}

void setPWM_off(uint32_t i2c_periph, uint8_t i2c_addr) {
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_ON_L, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_ON_H, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_OFF_L, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_OFF_H, 0);
}

void i2c_addr_init(void) {
    uint8_t i;
#if E1
    for (i = 0; i < 8; i++) {
        if (i2c_addr_poll(I2C0, PCA9685_ADDRESS_E1 + i * 2)) {
            LED_addr.periph = I2C0;
            LED_addr.addr = PCA9685_ADDRESS_E1 + i * 2;
            LED_addr.flag = 1;
            break;
        }
    }
    if (LED_addr.flag != 1) {
        for (i = 0; i < 8; i++) {
            if (i2c_addr_poll(I2C1, PCA9685_ADDRESS_E1 + i * 2)) {
                LED_addr.periph = I2C1;
                LED_addr.addr = PCA9685_ADDRESS_E1 + i * 2;
                LED_addr.flag = 1;
                break;
            }
        }
    }
    if (LED_addr.flag) {
        i2c_byte_write(LED_addr.periph, LED_addr.addr, PCA9685_MODE1, 0x0);
        setPWM_off(LED_addr.periph, LED_addr.addr);
    }
#endif
#if S1
    for (i = 0; i < 8; i++) {
        if (i2c_addr_poll(I2C0, HT16K33_ADDRESS_S1 + i * 2)) {
            key_addr.periph = I2C0;
            key_addr.addr = HT16K33_ADDRESS_S1 + i * 2;
            key_addr.flag = 1;
            break;
        }
    }
    if (key_addr.flag != 1) {
        for (i = 0; i < 8; i++) {
            if (i2c_addr_poll(I2C1, HT16K33_ADDRESS_S1 + i * 2)) {
                key_addr.periph = I2C1;
                key_addr.addr = HT16K33_ADDRESS_S1 + i * 2;
                key_addr.flag = 1;
                break;
            }
        }
    }
    if (key_addr.flag) {
        i2c_cmd_write(key_addr.periph, key_addr.addr, SYSTEM_ON);
    }
#endif
#if S6
    // S6 KEY I2C address init
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, GD32F330_ADDRESS_S6 + i * 2)) {
            s6_ult_addr.periph = I2C0;
            s6_ult_addr.addr = GD32F330_ADDRESS_S6 + i * 2;
            s6_ult_addr.flag = 1;
            break;
        }
    }
    // address not read
    if (s6_ult_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, GD32F330_ADDRESS_S6 + i * 2)) {
                s6_ult_addr.periph = I2C1;
                s6_ult_addr.addr = GD32F330_ADDRESS_S6 + i * 2;
                s6_ult_addr.flag = 1;
                break;
            }
        }
    }
    // read successful
    if (s6_ult_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("S1-KEY is initialization success!\n");
        if (s6_ult_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", s6_ult_addr.addr);
        printf("\n");
#endif
        i2c_cmd_write(s6_ult_addr.periph, s6_ult_addr.addr, SYSTEM_ON);
    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("S1-KEY is initialization failed	!\n");
#endif
    }
#endif
}

void led_control(uint8_t color) {
    switch (color) {
        case 0:
            setPWM_off(LED_addr.periph, LED_addr.addr);
            break;
        case 1:
					  setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 4095);   // r
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 0); 			// g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 0);     	// b
            break;
        case 2:
            setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 0);  		// r
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 4095);   // g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 0);     	// b
            break;
        case 3:
					  setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 0);     	// r
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 0);     	// g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 4095);  	// b
            break;
        case 4:
            setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 4095); // r
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 2047); // g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 0);    // b
            break;
        case 5:
						setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 0); 		// r	  
						setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 4095); // g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 2047); // b
            break;
        case 6:
						setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 2047);	// r
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 0); 		// g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 4095); // b
            break;
				case 7:
            setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 4095); // r
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 4095); // g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 0);    // b
            break;
				case 8:
						setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 0); 		// r	  
						setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 4095); // g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 4095); // b
            break;
				case 9:
						setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 4095);	// r
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 0); 		// g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 4095); // b
            break;
				case 10: //white
					  setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 4095); // r
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 4095); // g
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 4095); // b
            break;
				case 11:
						setPWM_off(LED_addr.periph, LED_addr.addr);
            break;
        default:
            setPWM_off(LED_addr.periph, LED_addr.addr);
            break;
    }
}

void key_control(int key) {
    switch (key) {
        case 1:
            led_control(LED_RED);
            break;
        case 2:
            led_control(LED_GREEN);
            break;
        case 3:
            led_control(LED_BLUE);
            break;
        case 4:
            led_control(LED_RED_GREEN_MID);
            break;
        case 5:
            led_control(LED_GREEN_BLUE_MID);
            break;
        case 6:
            led_control(LED_BLUE_RED_MID);
            break;
				case 7:
            led_control(LED_YELLOW);
            break;
				case 8:
            led_control(LED_SKYBLUE);
            break;
				case 9:
            led_control(LED_PURPLE);
            break;
				case 10:
            led_control(LED_WHITE);
            break;
				case 11:
						led_control(LED_OFF);
            break;
				case 12:
						led_control(LED_OFF);
						if(AUTO_MODE == 1)
							AUTO_MODE = 0;
						else
							AUTO_MODE = 1;
            break;
        default:
            return;
    }
}

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
	else if(keyvalue[0]&0x04){
		return 7;
	}
	else if(keyvalue[2]&0x04){
		return 8;
	}
	else if(keyvalue[4]&0x04){
		return 9;
	}
	else if(keyvalue[0]&0x08){
		return 10;
	}
	else if(keyvalue[2]&0x08){
		return 11;
	}
	else if(keyvalue[4]&0x08){
		return 12;
	}
	return 0;
}

// auto mode
int key_trans_auto_mode(void){
	if(keyvalue[4]&0x08){
		return 12;
	}
	return 0;
}

uint8_t near_flag;

void light_dis()
{
    uint8_t recvdata[2];
    uint16_t display;
    uint8_t sts;

    // read the distance
    sts = i2c_read(s6_ult_addr.periph, s6_ult_addr.addr, GD32F330_READ_CMD, recvdata, 2);
    if (sts) {
        display = (recvdata[0] << 8) | recvdata[1];
        // People enter, turn on the fan
        // < 1m, low speed
        if (display < 600) {
            near_flag = 1;
            // < 1.5m, medium speed
        } else {
            near_flag = 0;
        }
    } else {
        display = 0;
        near_flag = 0;
    }
}


void auto_mode(){
	light_dis();
	if (near_flag) {
			led_control(LED_WHITE);
  } else {
			led_control(LED_OFF);
	}
}

int main(void) {
    i2c0_gpio_config();
    i2c0_config();
    i2c1_gpio_config();
    i2c1_config();
    i2c_addr_init();
    systick_config();

	
    while (1) {
        i2c_read(key_addr.periph, key_addr.addr, KEYKS0, keyvalue, 24);
			
				if(AUTO_MODE == 1){
					key_control(key_trans_auto_mode());
					delay_1ms(100);
					auto_mode();
				}
				else
				{	
					key_control(key_trans());
				  delay_1ms(100);
				}			  
    }
}
