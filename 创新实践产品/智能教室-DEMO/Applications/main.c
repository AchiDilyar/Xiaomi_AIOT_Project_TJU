#include "gd32f4xx.h"
#include "main.h"
#include "i2c.h"
#include "systick.h"
#include "ht16k33.h"

i2c_addr_def s1_key_addr;
i2c_addr_def s2_ss_addr;
i2c_addr_def e1_led_addr;
i2c_addr_def e1_num_addr;
i2c_addr_def e2_fan_addr;

struct s2_para s2_data;
int status = 0;//Open or close

struct s2_para{
	float temperature;
	float humidity;
};

uint8_t keyvalue[6];

void setPWM(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t num, uint16_t on, uint16_t off){//Set signal PWM
    i2c_byte_write(i2c_periph,i2c_addr,LED0_ON_L+4*num,on);
    i2c_byte_write(i2c_periph,i2c_addr,LED0_ON_H+4*num,on>>8);
    i2c_byte_write(i2c_periph,i2c_addr,LED0_OFF_L+4*num,off);
    i2c_byte_write(i2c_periph,i2c_addr,LED0_OFF_H+4*num,off>>8);
}
void setPWM_off(uint32_t i2c_periph,uint8_t i2c_addr){//turn off LED
    i2c_byte_write(i2c_periph,i2c_addr,ALLLED_ON_L,0);
    i2c_byte_write(i2c_periph,i2c_addr,ALLLED_ON_H,0);
    i2c_byte_write(i2c_periph,i2c_addr,ALLLED_OFF_L,0);
    i2c_byte_write(i2c_periph,i2c_addr,ALLLED_OFF_H,0);
}

void pca9685_motor_control(uint32_t i2c_periph, uint8_t i2c_addr, uint8_t num)
{
    i2c_byte_write(i2c_periph, i2c_addr, PCA9685_MODE1, 0x0);

    switch (num)
    {
        // 关闭
        case 0:
            setPWM(i2c_periph, i2c_addr, 0x0, 0x0199, 0x1000);
            break;
        // 30%
        case 1:
            setPWM(i2c_periph, i2c_addr, 0x0, 0x0199, 0x0665);
            break;
        // 60%
        case 2:
            setPWM(i2c_periph, i2c_addr, 0x0, 0x0199, 0x0B32);
            break;
        // 90%
        case 3:
            setPWM(i2c_periph, i2c_addr, 0x0, 0x0199, 0x0FFF);
            break;
        // 关闭
        default:
            setPWM(i2c_periph, i2c_addr, 0x0, 0x0199, 0x1000);
            break;
    }
}

void i2c_addr_init(void)
{
	uint8_t i;
#if	E2
	//E2 I2C address init
	//poll I2C0,Horizontal
	for(i=0;i<4;i++)
	{
		if(i2c_addr_poll(I2C0,PCA9685_ADDRESS_E2+i*2))
		{
			e2_fan_addr.periph = I2C0;
			e2_fan_addr.addr = PCA9685_ADDRESS_E2+i*2;
			e2_fan_addr.flag = 1;			
			break;
		}	
	}
	//address not read
	if(e2_fan_addr.flag != 1)
	{			
		//poll I2C1,Verti1
		for(i=0;i<4;i++)
		{
			if(i2c_addr_poll(I2C1,PCA9685_ADDRESS_E2+i*2))
			{
				e2_fan_addr.periph = I2C1;
				e2_fan_addr.addr = PCA9685_ADDRESS_E2+i*2;
				e2_fan_addr.flag = 1;
				break;
			}	
		}
	}
	//read successful
	if(e2_fan_addr.flag)
	{
		#ifdef PRINT_DEBUG_INFO
			printf("E1-RGB-LED is initialization success!\n");
			if(e2_fan_addr.periph == I2C0)
				printf("%s","I2C0");
			else
				printf("%s","I2C1");
			printf(":");
			printf("%X",e2_fan_addr.addr);
			printf("\n");
		#endif
		//init E1 rgb led
		i2c_byte_write(e2_fan_addr.periph,e2_fan_addr.addr,PCA9685_MODE1,0x0);
		setPWM_off(e2_fan_addr.periph,e2_fan_addr.addr);
	}
	//read failure
	else
	{
		#ifdef PRINT_DEBUG_INFO
			printf("E1-RGB-LED is initialization failed	!\n");
		#endif
	}
#endif
#if	E1
	//E1 RGB LED I2C address init
	//poll I2C0,Horizontal
	for(i=0;i<4;i++)
	{
		if(i2c_addr_poll(I2C0,PCA9685_ADDRESS_E1+i*2))
		{
			e1_led_addr.periph = I2C0;
			e1_led_addr.addr = PCA9685_ADDRESS_E1+i*2;
			e1_led_addr.flag = 1;			
			break;
		}	
	}
	//address not read
	if(e1_led_addr.flag != 1)
	{			
		//poll I2C1,Verti1
		for(i=0;i<4;i++)
		{
			if(i2c_addr_poll(I2C1,PCA9685_ADDRESS_E1+i*2))
			{
				e1_led_addr.periph = I2C1;
				e1_led_addr.addr = PCA9685_ADDRESS_E1+i*2;
				e1_led_addr.flag = 1;
				break;
			}	
		}
	}
	//read successful
	if(e1_led_addr.flag)
	{
		#ifdef PRINT_DEBUG_INFO
			printf("E1-RGB-LED is initialization success!\n");
			if(e1_led_addr.periph == I2C0)
				printf("%s","I2C0");
			else
				printf("%s","I2C1");
			printf(":");
			printf("%X",e1_led_addr.addr);
			printf("\n");
		#endif
		//init E1 rgb led
		i2c_byte_write(e1_led_addr.periph,e1_led_addr.addr,PCA9685_MODE1,0x0);
		setPWM_off(e1_led_addr.periph,e1_led_addr.addr);
	}
	//read failure
	else
	{
		#ifdef PRINT_DEBUG_INFO
			printf("E1-RGB-LED is initialization failed	!\n");
		#endif
	}

	//E1 num I2C address init
	//poll I2C0,Horizontal
	for(i=0;i<4;i++)
	{
		if(i2c_addr_poll(I2C0,HT16K33_ADDRESS_E1+i*2))
		{
			e1_num_addr.periph = I2C0;
			e1_num_addr.addr = HT16K33_ADDRESS_E1+i*2;
			e1_num_addr.flag = 1;			
			break;
		}	
	}
	//address not read
	if(e1_num_addr.flag != 1)
	{			
		//poll I2C1,Verti1
		for(i=0;i<4;i++)
		{
			if(i2c_addr_poll(I2C1,HT16K33_ADDRESS_E1+i*2))
			{
				e1_num_addr.periph = I2C1;
				e1_num_addr.addr = HT16K33_ADDRESS_E1+i*2;
				e1_num_addr.flag = 1;
				break;
			}	
		}
	}
	//read successful
	if(e1_num_addr.flag)
	{
		#ifdef PRINT_DEBUG_INFO
			printf("E1-RGB-LED is initialization success!\n");
			if(e1_num_addr.periph == I2C0)
				printf("%s","I2C0");
			else
				printf("%s","I2C1");
			printf(":");
			printf("%X",e1_num_addr.addr);
			printf("\n");
		#endif
		//init E1 rgb led
		i2c_cmd_write(e1_num_addr.periph,e1_num_addr.addr,SYSTEM_ON);
	}
	//read failure
	else
	{
		#ifdef PRINT_DEBUG_INFO
			printf("E1-RGB-LED is initialization failed	!\n");
		#endif
	}
#endif
#if	S1	
	//S1 KEY I2C address init
	for(i=0;i<4;i++)
	{
		if(i2c_addr_poll(I2C0,HT16K33_ADDRESS_S1+i*2)){
			s1_key_addr.periph = I2C0;
			s1_key_addr.addr = HT16K33_ADDRESS_S1+i*2;	
			s1_key_addr.flag = 1;
			break;
		}	
	}
	//address not read
	if(s1_key_addr.flag != 1){			
		//poll I2C1,Verti1
		for(i=0;i<4;i++){
			if(i2c_addr_poll(I2C1,HT16K33_ADDRESS_S1+i*2)){
				s1_key_addr.periph = I2C1;
				s1_key_addr.addr = HT16K33_ADDRESS_S1+i*2;	
				s1_key_addr.flag = 1;
				break;
			}	
		}
	}
	//read successful
	if(s1_key_addr.flag){
		#ifdef PRINT_DEBUG_INFO
			printf("S1-KEY is initialization success!\n");
			if(s1_key_addr.periph == I2C0)
				printf("%s","I2C0");
			else
				printf("%s","I2C1");
			printf(":");
			printf("%X",s1_key_addr.addr);
			printf("\n");
		#endif
		i2c_cmd_write(s1_key_addr.periph,s1_key_addr.addr,SYSTEM_ON);
	}
	//read failure
	else{
		#ifdef PRINT_DEBUG_INFO
			printf("S1-KEY is initialization failed	!\n");
		#endif
	}
#endif

#if	S2	
	//S2 I2C address init
	if(i2c_addr_poll(I2C0,SHT35_ADDRESS_S2_1)){
		s2_ss_addr.periph = I2C0;
		s2_ss_addr.addr = SHT35_ADDRESS_S2_1;	
		s2_ss_addr.flag = 1;
	}	
	//address not read
	else if(i2c_addr_poll(I2C0,SHT35_ADDRESS_S2_2)){			
		s2_ss_addr.periph = I2C1;
		s2_ss_addr.addr = SHT35_ADDRESS_S2_2;	
		s2_ss_addr.flag = 1;
	}
    if (s2_ss_addr.flag != 1) {
        if (i2c_addr_poll(I2C1, SHT35_ADDRESS_S2_1)) {
            s2_ss_addr.periph = I2C1;
            s2_ss_addr.addr   = SHT35_ADDRESS_S2_1;
            s2_ss_addr.flag   = 1;
        }
        // address not read
        else if (i2c_addr_poll(I2C1, SHT35_ADDRESS_S2_2)) {
            s2_ss_addr.periph = I2C1;
            s2_ss_addr.addr   = SHT35_ADDRESS_S2_2;
            s2_ss_addr.flag   = 1;
        }
    }
#endif
}

void led_control(uint8_t color){//Control over target LED
	switch(color)
	{
	case 0:
		setPWM_off(e1_led_addr.periph,e1_led_addr.addr);
		break;
	case 1:
		setPWM(e1_led_addr.periph,e1_led_addr.addr,0,0x0000,0x0010);
		setPWM(e1_led_addr.periph,e1_led_addr.addr,1,0x0199,0x0C00);
		setPWM(e1_led_addr.periph,e1_led_addr.addr,2,0x0000,0x0010);
		break;
	case 2:
		setPWM(e1_led_addr.periph,e1_led_addr.addr,0,0x0199,0x0800);
		setPWM(e1_led_addr.periph,e1_led_addr.addr,1,0x0199,0x0800);
		setPWM(e1_led_addr.periph,e1_led_addr.addr,2,0x0000,0x0010);
		break;		
	case 3:
		setPWM(e1_led_addr.periph,e1_led_addr.addr,0,0x0000,0x0010);
		setPWM(e1_led_addr.periph,e1_led_addr.addr,1,0x0000,0x0010);
		setPWM(e1_led_addr.periph,e1_led_addr.addr,2,0x0199,0x0C00);
		break;  
	default:
		setPWM_off(e1_led_addr.periph,e1_led_addr.addr);
		break;
	}
}

struct s2_para s2_read_sht3x(uint32_t i2c_periph, uint8_t i2c_addr)
{
    uint8_t th_value[6];
    struct s2_para sht_para;
    uint16_t tmp;
    i2c_byte_write(i2c_periph, i2c_addr, 0x2C, 0x06);
    delay_1ms(100);
    i2c_read(i2c_periph, i2c_addr, 0x21, th_value, 6);

    tmp = (th_value[0] << 8) + th_value[1];
    sht_para.temperature = (float)tmp * 175 / (65536 - 1) - 45;

    tmp = (th_value[3] << 8) + th_value[4];
    sht_para.humidity = (float)tmp * 100 / (65536 - 1);

    return sht_para;
}

void digital_control(int num){
	ht16k33_init(e1_num_addr.periph,e1_num_addr.addr);
	for(int i=4;i>0;i--){
		ht16k33_display_data(e1_num_addr.periph,e1_num_addr.addr,i,num%10);
		num/=10;
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
	return 0;
}

void key_control(int key){//Control over
	
    // if (status == 0 && key == 4) {
    //     digital_control(0);
    //     status = 1;
    //     return;
    // }
    switch (key) {
        case 1:
            led_control(LED_RED);
            digital_control(s2_data.temperature);
            break;
        case 2:
            led_control(LED_BLUE);
            digital_control(s2_data.humidity);
            break;
        case 3:
			digital_control(1111);
			s2_data=s2_read_sht3x(s2_ss_addr.periph,s2_ss_addr.addr);
			led_control(LED_YELLOW);
			digital_control(0);
			break;
		case 4:
			ht16k33_display_off(e1_num_addr.periph,e1_num_addr.addr);
			status=0;
			break;
        default:
            break;
    }
}

// uint32_t volatile msTicks; // Counter for millisecond Interval

// void SysTick_Handler(void) { // SysTick Interrupt Handler
//     msTicks++; // Increment Counter
// }

// int get_ticks() {
//     return msTicks;
// }

// void delay_ltick(void) {
//     uint32_t curTicks;
//     curTicks = msTicks; // Save Current SysTick Value
//     while (msTicks == curTicks) { // Wait for next SysTick Interrupt
//         __WFE(); // Power-Down until next Event/Interrupt
//     }
// }

// void delay_ms(int ms) {
//     int i;
//     for (i = 0; i < ms; i++) {
//         delay_ltick();
//     }
// }

void init_i2c(){
	i2c0_gpio_config();
		/* configure I2C */
	i2c0_config(); 
		/* configure I2C GPIO */
	i2c1_gpio_config();
		/* configure I2C */
	i2c1_config(); 	
}

int main(void){
	init_i2c();	
	i2c_addr_init();//initializing i2c address of key and LED
	systick_config();//system tick configuration
    while(1){
		i2c_read(s1_key_addr.periph,s1_key_addr.addr,KEYKS0,keyvalue,6);//get key status
		pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 3);
		key_control(key_trans());//take action
		delay_1ms(100);
    }
}
