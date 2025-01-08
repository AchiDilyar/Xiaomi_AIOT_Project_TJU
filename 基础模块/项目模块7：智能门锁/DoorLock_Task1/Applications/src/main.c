#include "main.h"
#include "gd32f4xx.h"
#include "ht16k33.h"
#include "i2c.h"
#include "systick.h"
i2c_addr_def key_addr; // Key address struct
i2c_addr_def LED_addr; // LED address struct
// i2c_addr_def NFC_addr; // NFC address struct
i2c_addr_def SMG_addr; // SMG
i2c_addr_def e3_curtain_addr;
uint8_t keyvalue[6]; // Storage of key status
uint8_t keyvalue_h[6];
uint8_t curtain_run_sts;
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
    } else if (lock_state == 0) {
        led_color(3);
        SMG_show(SMG_addr.periph, SMG_addr.addr, temp_password, temp_password_len);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
    } else if (lock_state == 1) {
        led_color(2);
        SMG_show(SMG_addr.periph, SMG_addr.addr, password, 4);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
    } else if (lock_state == 2) {
        led_color(4);
        SMG_show(SMG_addr.periph, SMG_addr.addr, new_password, new_password_len);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
    } else if (lock_state == 3) {
        led_color(1);
        SMG_show(SMG_addr.periph, SMG_addr.addr, 0, 0);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
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

    while (1) {
        i2c_read(key_addr.periph, key_addr.addr, KEYKS0, keyvalue, 6); // get key status
        key_control();                                                 // take action
        // SMG_show(SMG_addr.periph, SMG_addr.addr, key_down(),4);
        // key_down();
        // i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 50);
        // SMG_show(SMG_addr.periph, SMG_addr.addr, 1211,4);
        // empty();
        delay_1ms(50);
    }
}
