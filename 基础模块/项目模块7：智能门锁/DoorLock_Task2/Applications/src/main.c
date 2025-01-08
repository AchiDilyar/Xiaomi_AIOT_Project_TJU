#include "main.h"
#include "gd32f4xx.h"
#include "ht16k33.h"
#include "i2c.h"
#include "systick.h"

#include "stdio.h"
#include "string.h"
#include "e4.h"
#include "s5.h"
#include "ring_voice.h"
#include "gd32f330.h"
#include "gd32f450z_eval.h"
i2c_addr_def key_addr; // Key address struct
i2c_addr_def LED_addr; // LED address struct
// i2c_addr_def NFC_addr; // NFC address struct
i2c_addr_def SMG_addr; // SMG
i2c_addr_def e3_curtain_addr;
i2c_addr_def s5_addr;
uint8_t keyvalue[6]; // Storage of key status
uint8_t keyvalue_h[6];
uint8_t curtain_run_sts;
int NFCLOCK_count = 0;
uint8_t ucArray_ID[6];
uint8_t DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t wd_buffer[20];
uint8_t back_stat    = 0;
uint8_t stat         = 0;
uint8_t cardth       = 0;
uint16_t delay_count = 0; // 延时变量
uint16_t delay_inc   = 0;
uint16_t time_count  = 0;         // 1s定时变量
unsigned short playdata[PLAYCNT]; // 语音数据缓存
unsigned long playcnt  = 0;       // 音频文件索引
unsigned char playflag = 0;       // 放音标志
uint8_t start_flag     = 0;       // 录音标志
uint8_t record_flag    = 0;       // 录音完成标志
uint8_t inc_flag       = 0;
//==================================================I2C====================================================================================================
void i2c_addr_init(void)
{
    uint8_t i;
//---------------------------------------------------LED----------------------------------------------------
#if E1
    // E1 RGB LED I2C address init
    // poll I2C0,Horizontal
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, PCA9685_ADDRESS_E1 + i * 2)) {
            LED_addr.periph = I2C0;
            LED_addr.addr   = PCA9685_ADDRESS_E1 + i * 2;
            LED_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (LED_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, PCA9685_ADDRESS_E1 + i * 2)) {
                LED_addr.periph = I2C1;
                LED_addr.addr   = PCA9685_ADDRESS_E1 + i * 2;
                LED_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (LED_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization success!\n");
        if (LED_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", LED_addr.addr);
        printf("\n");
#endif
        // init E1 rgb led
        i2c_byte_write(LED_addr.periph, LED_addr.addr, PCA9685_MODE1, 0x0);
        setPWM_off(LED_addr.periph, LED_addr.addr);
    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization failed	!\n");
#endif
    }

    //------------------------------------------------------SMG------------------------------------------------------------

    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, HT16K33_ADDRESS_E1 + i * 2)) {
            SMG_addr.periph = I2C0;
            SMG_addr.addr   = HT16K33_ADDRESS_E1 + i * 2;
            SMG_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (SMG_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, HT16K33_ADDRESS_E1 + i * 2)) {
                SMG_addr.periph = I2C1;
                SMG_addr.addr   = HT16K33_ADDRESS_E1 + i * 2;
                SMG_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (SMG_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("E1-SMG is initialization success!\n");
        if (SMG_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", SMG_addr.addr);
        printf("\n");
#endif
        ht16k33_init(SMG_addr.periph, SMG_addr.addr);
    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("E1-SMG is initialization failed	!\n");
#endif
    }

#endif

#if E3
    // E3 I2C address init
    // poll I2C0,Horizontal
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, HR8833_ADDRESS_E3 + i * 2)) {
            e3_curtain_addr.periph = I2C0;
            e3_curtain_addr.addr   = HR8833_ADDRESS_E3 + i * 2;
            e3_curtain_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (e3_curtain_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, HR8833_ADDRESS_E3 + i * 2)) {
                e3_curtain_addr.periph = I2C1;
                e3_curtain_addr.addr   = HR8833_ADDRESS_E3 + i * 2;
                e3_curtain_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (e3_curtain_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization success!\n");
        if (e3_curtain_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", e3_curtain_addr.addr);
        printf("\n");
#endif
        // init E3 rgb led
        i2c_cmd_write(e3_curtain_addr.periph, e3_curtain_addr.addr, STU_REG_E3);
    }
    // if(s2_ss_addr.flag){
    // 	//i2c_byte_write(sensor_addr.periph,sensor_addr.addr,0x30,0xA2);
    // 	bh1750_return(s2_ss_addr.periph,s2_ss_addr.addr);
    // }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization failed	!\n");
#endif
    }
#endif

//---------------------------------------------------KEY----------------------------------------------
#if S1
    // S1 KEY I2C address init
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, HT16K33_ADDRESS_S1 + i * 2)) {
            key_addr.periph = I2C0;
            key_addr.addr   = HT16K33_ADDRESS_S1 + i * 2;
            key_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (key_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, HT16K33_ADDRESS_S1 + i * 2)) {
                key_addr.periph = I2C1;
                key_addr.addr   = HT16K33_ADDRESS_S1 + i * 2;
                key_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (key_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("S1-KEY is initialization success!\n");
        if (key_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", key_addr.addr);
        printf("\n");
#endif
        i2c_cmd_write(key_addr.periph, key_addr.addr, SYSTEM_ON);
    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("S1-KEY is initialization failed	!\n");
#endif
    }
#endif
//-------------------------------------------------------NFC----------------------------------------------
#if S5
    // S1 KEY I2C address init
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, MS523_ADDRESS_S5 + i * 2)) {
            NFC_addr.periph = I2C0;
            NFC_addr.addr   = MS523_ADDRESS_S5 + i * 2;
            NFC_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (NFC_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, MS523_ADDRESS_S5 + i * 2)) {
                NFC_addr.periph = I2C1;
                NFC_addr.addr   = MS523_ADDRESS_S5 + i * 2;
                NFC_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (NFC_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("S5-NFC is initialization success!\n");
        if (NFC_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", NFC_addr.addr);
        printf("\n");
#endif
        // i2c_cmd_write(key_addr.periph, key_addr.addr, SYSTEM_ON);

    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("S1-KEY is initialization failed	!\n");
#endif
    }
#endif
}

//==================================================C2I====================================================================================================
void SPI2_IRQHandler(void)
{
    if (SET == spi_i2s_interrupt_flag_get(SPI2, SPI_I2S_INT_TBE)) {
        if (playflag) {
            spi_i2s_data_transmit(SPI2, playdata[playcnt++]); // 放音
            if (playcnt >= PLAYCNT) {
                playflag   = 0;
                playcnt    = 0;
                start_flag = 1;
            }
        } else {
            spi_i2s_data_transmit(SPI2, 0x0000);
        }
    }

    if (SET == spi_i2s_interrupt_flag_get(I2S2_ADD, SPI_I2S_INT_RBNE)) {
        // playdata[playcnt++] = spi_i2s_data_receive(I2S2_ADD); // 录音
        playcnt = 80000;
        if (playcnt >= PLAYCNT) {
            spi_i2s_interrupt_disable(I2S2_ADD, SPI_I2S_INT_RBNE);
            record_flag = 1;
            playcnt     = 0;
        }
    }
}
void setPWM(uint32_t i2c_periph, uint8_t i2c_addr, uint8_t num, uint16_t on, uint16_t off)
{ // Set signal PWM
    i2c_byte_write(i2c_periph, i2c_addr, LED0_ON_L + 4 * num, on);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_ON_H + 4 * num, on >> 8);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_OFF_L + 4 * num, off);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_OFF_H + 4 * num, off >> 8);
}
void setPWM_off(uint32_t i2c_periph, uint8_t i2c_addr)
{ // turn off LED
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_ON_L, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_ON_H, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_OFF_L, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_OFF_H, 0);
}
void fill_data()
{
    for (int len = 0; len < 30000; len++) {
        playdata[len] = ring_voice[len];
    }
}

void voice()
{
    while (1) {
        if (start_flag == 0) {
            start_flag  = 1;
            record_flag = 0;
            playcnt     = 0;
            e4_sound_recording_config();
            nvic_irq_enable(SPI2_IRQn, 0, 0); // 打开中断,可以录音
            return;
            //LED_OFF;
        } else {
            if (record_flag) {
                fill_data();
                record_flag = 0;
                e4_playback_config();
                playflag = 1;
                break;
                //LED_ON;
            }
        }
    }
}

void led_control(uint8_t color)
{ // Control over target LED
    switch (color) {
        case 0:
            setPWM_off(LED_addr.periph, LED_addr.addr);
            break;
        case 1:
            setPWM(LED_addr.periph, LED_addr.addr, 1, 0x0199, 0x0C00); // r
            break;
        case 2:
            setPWM(LED_addr.periph, LED_addr.addr, 0, 0x0199, 0x0C00); // g
            break;
        case 3:
            setPWM(LED_addr.periph, LED_addr.addr, 2, 0x0199, 0x0C00); // b
            break;
            // case 4:
            //     setPWM(LED_addr.periph, LED_addr.addr, 1, 0x0000, 0x0000);
            //     break;
            // case 5:
            //     setPWM(LED_addr.periph, LED_addr.addr, 0, 0x0000, 0x0000);
            //     break;
            // case 6:
            //     setPWM(LED_addr.periph, LED_addr.addr, 2, 0x0000, 0x0000);
            //     break;
    }
}
void led_color(uint8_t color)
{
    switch (color) {
        case 0:
            led_control(0);
            break;
        case 1: // r
            led_control(0);
            led_control(1);
            break;
        case 2: // g
            led_control(0);
            led_control(2);
            break;
        case 3: // b
            led_control(0);
            led_control(3);
            break;
        case 4:
            led_control(0);
            led_control(1);
            led_control(2);
            break;
        case 5:
            led_control(0);
            led_control(1);
            led_control(3);
            break;
        case 6:
            led_control(0);
            led_control(2);
            led_control(3);
            break;
        case 7:
            led_control(1);
            led_control(2);
            led_control(3);
            break;
    }
}
bit_status key[6];

int key_down()
{
    int key_ = 12;
    if ((keyvalue[0] & 0x01) && (!(keyvalue_h[0] & 0x01))) {
        key_ = 0;
    }
    if ((keyvalue[2] & 0x01) && (!(keyvalue_h[2] & 0x01))) {
        key_ = 1;
    }
    if ((keyvalue[4] & 0x01) && (!(keyvalue_h[4] & 0x01))) {
        key_ = 2;
    }
    if ((keyvalue[0] & 0x02) && (!(keyvalue_h[0] & 0x02))) {
        key_ = 3;
    }
    if ((keyvalue[2] & 0x02) && (!(keyvalue_h[2] & 0x02))) {
        key_ = 4;
    }
    if ((keyvalue[4] & 0x02) && (!(keyvalue_h[4] & 0x02))) {
        key_ = 5;
    }
    if ((keyvalue[0] & 0x04) && (!(keyvalue_h[0] & 0x04))) {
        key_ = 6;
    }
    if ((keyvalue[2] & 0x04) && (!(keyvalue_h[2] & 0x04))) {
        key_ = 7;
    }
    if ((keyvalue[4] & 0x04) && (!(keyvalue_h[4] & 0x04))) {
        key_ = 8;
    }
    if ((keyvalue[0] & 0x08) && (!(keyvalue_h[0] & 0x08))) {
        key_ = 9;
    }
    if ((keyvalue[2] & 0x08) && (!(keyvalue_h[2] & 0x08))) {
        key_ = 10;
    }
    if ((keyvalue[4] & 0x08) && (!(keyvalue_h[4] & 0x08))) {
        key_ = 11;
    }
    // if(key_!=12) SMG_show(SMG_addr.periph, SMG_addr.addr, key_,4);
    for (int i = 0; i < 6; i++) {
        keyvalue_h[i] = keyvalue[i];
    }
    return key_;
}
// i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
// SMG_show(SMG_addr.periph, SMG_addr.addr, temp_password);
uint32_t password      = 1234;
uint32_t temp_password = 0;
int temp_password_len  = 0;
uint32_t new_password  = 0;
int new_password_len   = 0;
int lock_state         = -1;
void key_control()
{

    int key_ = key_down();
    switch (key_) {
        case 0:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 1;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 1;
                new_password_len++;
            }
            break;
        case 1:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 2;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 2;
                new_password_len++;
            }
            break;
        case 2:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 3;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 3;
                new_password_len++;
            }
            break;
        case 3:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 4;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 4;
                new_password_len++;
            }
            break;
        case 4:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 5;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 5;
                new_password_len++;
            }
            break;
        case 5:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 6;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 6;
                new_password_len++;
            }
            break;
        case 6:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 7;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 7;
                new_password_len++;
            }
            break;
        case 7:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 8;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 8;
                new_password_len++;
            }
            break;
        case 8:
            if (lock_state == 0) {
                temp_password = temp_password * 10 + 9;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10 + 9;
                new_password_len++;
            }
            break;
        case 9:
            if (lock_state == 0) {
                temp_password = temp_password / 10;
                temp_password_len--;
            } else if (lock_state == 1) {
                new_password     = 0;
                new_password_len = 0;
                lock_state       = 2;
            } else if (lock_state == 2) {
                new_password = new_password / 10;
                new_password_len--;
            }
            break;
        case 10:
            if (lock_state == 0) {
                temp_password = temp_password * 10;
                temp_password_len++;
            } else if (lock_state == 2) {
                new_password = new_password * 10;
                new_password_len++;
            }
            break;
        case 11:
            if (lock_state == -1 || lock_state == 3) {
                temp_password     = 0;
                temp_password_len = 0;
                lock_state        = 0;
            } else if (lock_state == 0) {
                if (temp_password == password) {
                    lock_state = 1;
                } else {
                    lock_state = 3;
                }
            } else if (lock_state == 1) {
                temp_password     = 0;
                temp_password_len = 0;
                lock_state        = 0;
            } else if (lock_state == 2) {
                password   = new_password;
                lock_state = 1;
            }
            break;
        default:
            // return;
            break;
    }
    temp_password %= 10000;
    new_password %= 10000;
    if (temp_password_len > 4)
        temp_password_len = 4;
    else if (temp_password_len < 0)
        temp_password_len = 0;
    if (new_password_len > 4)
        new_password_len = 4;
    else if (new_password_len < 0)
        new_password_len = 0;
    if (lock_state == -1) {
        led_color(7);
        SMG_show(SMG_addr.periph, SMG_addr.addr, 0, 0);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
        // voice();
    } else if (lock_state == 0) {
        led_color(3); // 3
        SMG_show(SMG_addr.periph, SMG_addr.addr, temp_password, temp_password_len);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
        // voice();
    } else if (lock_state == 1) {
        led_color(2);
        SMG_show(SMG_addr.periph, SMG_addr.addr, password, 4);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
        voice();
    } else if (lock_state == 2) {
        led_color(4);
        SMG_show(SMG_addr.periph, SMG_addr.addr, new_password, new_password_len);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
        // voice();
    } else if (lock_state == 3) {
        led_color(1);
        SMG_show(SMG_addr.periph, SMG_addr.addr, 0, 0);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
        //  voice();
    }
}
int main(void)
{
    /* configure I2C GPIO */
    i2c0_gpio_config();
    /* configure I2C */
    i2c0_config();
    /* configure I2C GPIO */
    i2c1_gpio_config();
    /* configure I2C */
    i2c1_config();
    i2c_addr_init();  // initializing i2c address of key and LED
    systick_config(); // system tick configuration
    gd_eval_com_init(EVAL_COM0);
    s5_addr = s5_init(MS523_ADDRESS_S5);
    e4_init();
    voice();
    while (1) {
        i2c_read(key_addr.periph, key_addr.addr, KEYKS0, keyvalue, 6); // get key status

        if (NFCLOCK_count == 1) {
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
            led_color(1);
            NFCLOCK_count = 0;
        }

        if (s5_detect(s5_addr.periph, s5_addr.addr, ucArray_ID)) {
            if (s5_verify(s5_addr.periph, s5_addr.addr, PICC_AUTHENT1A, 16, DefaultKey, ucArray_ID) == 0) {
                stat = 1;
                if (stat != back_stat) {
                    cardth ^= 1;
                    memset(wd_buffer, 0, 20);
                    // strncpy((char*)wd_buffer,"123456789ABCDE\r\n",16);
                    //  wd_buffer[0]=255;
                    //  wd_buffer[1]=255;
                    //  wd_buffer[2]=255;
                    //  wd_buffer[3]=255;

                    // wd_buffer[4]=255;
                    // wd_buffer[5]=255;
                    // wd_buffer[6]=255;
                    // wd_buffer[7]=7;

                    // wd_buffer[8]=128;
                    // wd_buffer[9]=105;
                    // wd_buffer[10]=255;
                    // wd_buffer[11]=255;

                    // wd_buffer[12]=255;
                    // wd_buffer[13]=255;
                    // wd_buffer[14]=255;
                    // wd_buffer[15]=255;
                    // wd_buffer[0]=1;
                    // if(s5_write_data(s5_addr.periph,s5_addr.addr,16,wd_buffer) == 0)
                    // {
                    // 	memset(wd_buffer,0,20);
                    // 	//wd_buffer[2]=0;
                    // 	digital_control(4444);
                    // 	if(s5_read_data(s5_addr.periph,s5_addr.addr,16,wd_buffer) == 0)
                    // 	{
                    // 		//debug_printf(EVAL_COM0,(char *)wd_buffer);
                    // 	}
                    // 	digital_control(wd_buffer[2]);
                    // 	//digital_control(3333);
                    // }
                    memset(wd_buffer, 0, 20);
                    if (s5_read_data(s5_addr.periph, s5_addr.addr, 16, wd_buffer) == 0) {
                        // debug_printf(EVAL_COM0,(char *)wd_buffer);
                    }
                    if (cardth) {
                        // pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr,3);
                        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
                        led_color(2);
                        // voice();
                        NFCLOCK_count++;
                    } //, wd_buffer[0]);// 读取wd_buffer[0]的值控制风扇转速
                    // else pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 0);
                }
                // digital_control(3333);
            }
            // digital_control(1111);
        } else {
            stat = 0;
            // digital_control(2222);
        }
        back_stat = stat;
        s5_sleep(s5_addr.periph, s5_addr.addr);

        // printf("%d\n",keyvalue);

        // key_control(); // take action

        // SMG_show(SMG_addr.periph, SMG_addr.addr, key_down(),4);
        // key_down();
        // i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 50);
        // SMG_show(SMG_addr.periph, SMG_addr.addr, 1211,4);
        // empty();
        delay_1ms(50);
    }
}
