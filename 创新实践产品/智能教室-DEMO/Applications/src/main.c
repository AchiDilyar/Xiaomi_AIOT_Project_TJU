#include "gd32f4xx.h"
#include "gd32f450z_eval.h"
#include "main.h"
#include "i2c.h"
#include "stdio.h"
#include "string.h"
#include "systick.h"
#include "ht16k33.h"
#include "s5.h"
#include "e4.h"
#include "ring_voice.h"
#include "bh1750.h"

i2c_addr_def s1_key_addr;     // 按键
i2c_addr_def s2_ss_addr;      // 温湿度计传感器
i2c_addr_def s2_light_addr;   // 光照传感器
i2c_addr_def s5_addr;         // NFC
i2c_addr_def e1_led_addr;     // RGB灯泡
i2c_addr_def e1_num_addr;     // 数码管
i2c_addr_def e2_fan_addr;     // 风扇
i2c_addr_def e3_curtain_addr; // 窗帘机
i2c_addr_def SMG_addr;        // SMG
uint8_t MODE_STATUS = 0;      // 默认在菜单模式
uint8_t MODE_LIST   = 1;      // 默认在菜单模式
int S1_MODE_1       = 0;      // 门锁密码模式
int S1_MODE_2       = 0;      // 门锁NFC模式
int S1_MODE_3       = 0;      // 风扇NFC模式
int S1_MODE_4       = 0;      // 风扇温湿度控制模式
int S1_MODE_5       = 0;      // 灯光手动模式
int S1_MODE_6       = 0;      // 灯光自动模式（紫外线）
int S1_MODE_7       = 0;      // 窗帘机手动模式
int S1_MODE_8       = 0;      // 窗帘机自动模式（光照）
int S1_MODE_9       = 0;      // 一键离校

int NFCFAN_count  = 1;                                        // NFC风扇刷卡次数
int NFCLOCK_count = 0;                                        // NFC门锁刷卡次数
int LIGHT_VALUE;                                              // 光照强度
unsigned short playdata[PLAYCNT];                             // 语音数据缓存
unsigned long playcnt  = 0;                                   // 音频文件索引
unsigned char playflag = 0;                                   // 播放音频标志
uint8_t FAN_STATUS     = 0;                                   // 风扇模块模式
uint8_t CURTAIN_STATUS = 2;                                   // 窗帘模块模式
uint32_t PASSWD        = 429;                                 // 开锁密码
uint32_t TEMP_PASSWD   = 0;                                   // 临时密码
int TEMP_PASSWD_LEN    = 0;                                   // 临时密码长度（比对用）
uint32_t NEW_PASSWD    = 0;                                   // 新密码
int NEW_PASSWD_LEN     = 0;                                   // 密码长度
int LOCK_STATUS        = -1;                                  // 门锁状态
uint8_t ucArray_ID[6];                                        // NFC数据
uint8_t DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // NFC数据
uint8_t keyvalue[24];                                         // 存放按键数据的数组
uint8_t keyvalue_h[6];                                        // 密码识别数组
uint8_t back_stat = 0;                                        // NFC识别状态
uint8_t stat      = 0;                                        // NFC识别状态

struct s2_para s2_data;

struct s2_para {
    float temperature;
    float humidity;
};

void i2c_addr_init(void)
{
    uint8_t i;

//------------------------------------------------------S1按键初始化------------------------------------------------------------//
#if S1
    // S1 KEY I2C address init
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, HT16K33_ADDRESS_S1 + i * 2)) {
            s1_key_addr.periph = I2C0;
            s1_key_addr.addr   = HT16K33_ADDRESS_S1 + i * 2;
            s1_key_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (s1_key_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, HT16K33_ADDRESS_S1 + i * 2)) {
                s1_key_addr.periph = I2C1;
                s1_key_addr.addr   = HT16K33_ADDRESS_S1 + i * 2;
                s1_key_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (s1_key_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("S1-KEY is initialization success!\n");
        if (s1_key_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", s1_key_addr.addr);
        printf("\n");
#endif
        i2c_cmd_write(s1_key_addr.periph, s1_key_addr.addr, SYSTEM_ON);
    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("S1-KEY is initialization failed	!\n");
#endif
    }
#endif

//------------------------------------------------------S2温湿度+光照传感器初始化------------------------------------------------------------//
#if S2
    // S2 I2C address init
    if (i2c_addr_poll(I2C0, SHT35_ADDRESS_S2_1)) {
        s2_ss_addr.periph = I2C0;
        s2_ss_addr.addr   = SHT35_ADDRESS_S2_1;
        s2_ss_addr.flag   = 1;
    }
    // address not read
    else if (i2c_addr_poll(I2C0, SHT35_ADDRESS_S2_2)) {
        s2_ss_addr.periph = I2C1;
        s2_ss_addr.addr   = SHT35_ADDRESS_S2_2;
        s2_ss_addr.flag   = 1;
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

    // S2 Light address init
    if (i2c_addr_poll(I2C0, BH1750_ADDRESS_S2_1)) {
        s2_light_addr.periph = I2C0;
        s2_light_addr.addr   = BH1750_ADDRESS_S2_1;
        s2_light_addr.flag   = 1;

    } else if (i2c_addr_poll(I2C0, BH1750_ADDRESS_S2_2)) {
        s2_light_addr.periph = I2C0;
        s2_light_addr.addr   = BH1750_ADDRESS_S2_2;
        s2_light_addr.flag   = 1;
    }

    // address not read
    if (s2_light_addr.flag != 1) {
        // poll I2C1,Verti1

        if (i2c_addr_poll(I2C1, BH1750_ADDRESS_S2_1)) {
            s2_light_addr.periph = I2C1;
            s2_light_addr.addr   = BH1750_ADDRESS_S2_1;
            s2_light_addr.flag   = 1;
        } else if (i2c_addr_poll(I2C1, BH1750_ADDRESS_S2_2)) {
            s2_light_addr.periph = I2C1;
            s2_light_addr.addr   = BH1750_ADDRESS_S2_2;
            s2_light_addr.flag   = 1;
        }
    }

#endif

//------------------------------------------------------E1数码管+LED初始化------------------------------------------------------------//
#if E1
    //------------------------------------------------------RGB灯泡------------------------------------------------------------//
    // E1 RGB LED I2C address init
    // poll I2C0,Horizontal
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, PCA9685_ADDRESS_E1 + i * 2)) {
            e1_led_addr.periph = I2C0;
            e1_led_addr.addr   = PCA9685_ADDRESS_E1 + i * 2;
            e1_led_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (e1_led_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, PCA9685_ADDRESS_E1 + i * 2)) {
                e1_led_addr.periph = I2C1;
                e1_led_addr.addr   = PCA9685_ADDRESS_E1 + i * 2;
                e1_led_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (e1_led_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization success!\n");
        if (e1_led_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", e1_led_addr.addr);
        printf("\n");
#endif
        // init E1 rgb led
        i2c_byte_write(e1_led_addr.periph, e1_led_addr.addr, PCA9685_MODE1, 0x0);
        setPWM_off(e1_led_addr.periph, e1_led_addr.addr);
    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization failed	!\n");
#endif
    }
    //------------------------------------------------------数码管------------------------------------------------------------//
    // E1 num I2C address init
    // poll I2C0,Horizontal
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, HT16K33_ADDRESS_E1 + i * 2)) {
            e1_num_addr.periph = I2C0;
            e1_num_addr.addr   = HT16K33_ADDRESS_E1 + i * 2;
            e1_num_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (e1_num_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, HT16K33_ADDRESS_E1 + i * 2)) {
                e1_num_addr.periph = I2C1;
                e1_num_addr.addr   = HT16K33_ADDRESS_E1 + i * 2;
                e1_num_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (e1_num_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization success!\n");
        if (e1_num_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", e1_num_addr.addr);
        printf("\n");
#endif
        // init E1 rgb led
        i2c_cmd_write(e1_num_addr.periph, e1_num_addr.addr, SYSTEM_ON);
    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization failed	!\n");
#endif
    }
    //------------------------------------------------------SMG显示------------------------------------------------------------//
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

//------------------------------------------------------E2风扇地址初始化------------------------------------------------------------//
#if E2
    // E2 I2C address init
    // poll I2C0,Horizontal
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, PCA9685_ADDRESS_E2 + i * 2)) {
            e2_fan_addr.periph = I2C0;
            e2_fan_addr.addr   = PCA9685_ADDRESS_E2 + i * 2;
            e2_fan_addr.flag   = 1;
            break;
        }
    }
    // address not read
    if (e2_fan_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, PCA9685_ADDRESS_E2 + i * 2)) {
                e2_fan_addr.periph = I2C1;
                e2_fan_addr.addr   = PCA9685_ADDRESS_E2 + i * 2;
                e2_fan_addr.flag   = 1;
                break;
            }
        }
    }
    // read successful
    if (e2_fan_addr.flag) {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization success!\n");
        if (e2_fan_addr.periph == I2C0)
            printf("%s", "I2C0");
        else
            printf("%s", "I2C1");
        printf(":");
        printf("%X", e2_fan_addr.addr);
        printf("\n");
#endif
        // init E1 rgb led
        i2c_byte_write(e2_fan_addr.periph, e2_fan_addr.addr, PCA9685_MODE1, 0x0);
        setPWM_off(e2_fan_addr.periph, e2_fan_addr.addr);
    }
    // read failure
    else {
#ifdef PRINT_DEBUG_INFO
        printf("E1-RGB-LED is initialization failed	!\n");
#endif
    }
#endif

//------------------------------------------------------E3窗帘机地址初始化------------------------------------------------------------//
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
}

// PWM地址处理
void setPWM(uint32_t i2c_periph, uint8_t i2c_addr, uint8_t num, uint16_t on, uint16_t off)
{ // Set signal PWM
    i2c_byte_write(i2c_periph, i2c_addr, LED0_ON_L + 4 * num, on);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_ON_H + 4 * num, on >> 8);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_OFF_L + 4 * num, off);
    i2c_byte_write(i2c_periph, i2c_addr, LED0_OFF_H + 4 * num, off >> 8);
}

// PWM地址处理
void setPWM_off(uint32_t i2c_periph, uint8_t i2c_addr)
{ // turn off LED
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_ON_L, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_ON_H, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_OFF_L, 0);
    i2c_byte_write(i2c_periph, i2c_addr, ALLLED_OFF_H, 0);
}

// 音频相关
void SPI2_IRQHandler(void)
{
    if (SET == spi_i2s_interrupt_flag_get(SPI2, SPI_I2S_INT_TBE)) {
        if (playflag) {
            spi_i2s_data_transmit(SPI2, playdata[playcnt++]); // 放音
            if (playcnt >= PLAYCNT) {
                playflag = 0;
                playcnt  = 0;
            }
        } else {
            spi_i2s_data_transmit(SPI2, 0x0000);
        }
    }
}

// 读入音频数据
void fill_data()
{
    for (int len = 0; len < 30000; len++) {
        playdata[len] = ring_voice[len];
    }
}

// 播放音频
void voice()
{
    led_control(LED_BLUE_RED_MID);
    fill_data();
    e4_playback_config();
    playflag = 1;
    return;
}

// RGB灯泡颜色
void led_control(uint8_t color)
{
    switch (color) {
        case 0:
            setPWM_off(e1_led_addr.periph, e1_led_addr.addr);
            break;
        case 1:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 4095); // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 0);    // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 0);    // b
            break;
        case 2:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 0);    // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 4095); // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 0);    // b
            break;
        case 3:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 0);    // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 0);    // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 4095); // b
            break;
        case 4:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 4095); // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 2047); // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 0);    // b
            break;
        case 5:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 0);    // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 4095); // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 2047); // b
            break;
        case 6:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 2047); // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 0);    // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 4095); // b
            break;
        case 7:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 4095); // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 4095); // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 0);    // b
            break;
        case 8:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 0);    // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 4095); // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 4095); // b
            break;
        case 9:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 4095); // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 0);    // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 4095); // b
            break;
        case 10:
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 1, 0, 4095); // r
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 0, 0, 4095); // g
            setPWM(e1_led_addr.periph, e1_led_addr.addr, 2, 0, 4095); // b
            break;
        default:
            setPWM_off(e1_led_addr.periph, e1_led_addr.addr);
            break;
    }
}

// 控制风扇风速
void pca9685_motor_control(uint32_t i2c_periph, uint8_t i2c_addr, uint8_t num)
{
    i2c_byte_write(i2c_periph, i2c_addr, PCA9685_MODE1, 0x0);

    switch (num) {
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

// 读取温湿度计数据
struct s2_para s2_read_sht3x(uint32_t i2c_periph, uint8_t i2c_addr)
{
    uint8_t th_value[6];
    struct s2_para sht_para;
    uint16_t tmp;
    i2c_byte_write(i2c_periph, i2c_addr, 0x2C, 0x06);
    delay_1ms(100);
    i2c_read(i2c_periph, i2c_addr, 0x21, th_value, 6);

    tmp                  = (th_value[0] << 8) + th_value[1];
    sht_para.temperature = (float)tmp * 175 / (65536 - 1) - 45;

    tmp               = (th_value[3] << 8) + th_value[4];
    sht_para.humidity = (float)tmp * 100 / (65536 - 1);

    return sht_para;
}

// 数码管显示
void digital_control(int num)
{
    ht16k33_init(e1_num_addr.periph, e1_num_addr.addr);
    for (int i = 4; i > 0; i--) {
        ht16k33_display_data(e1_num_addr.periph, e1_num_addr.addr, i, num % 10);
        num /= 10;
    }
}

// 按键扫描
int key_trans(void)
{
    if (keyvalue[0] & 0x01) {
        return 1;
    } else if (keyvalue[2] & 0x01) {
        return 2;
    } else if (keyvalue[4] & 0x01) {
        return 3;
    } else if (keyvalue[0] & 0x02) {
        return 4;
    } else if (keyvalue[2] & 0x02) {
        return 5;
    } else if (keyvalue[4] & 0x02) {
        return 6;
    } else if (keyvalue[0] & 0x04) {
        return 7;
    } else if (keyvalue[2] & 0x04) {
        return 8;
    } else if (keyvalue[4] & 0x04) {
        return 9;
    } else if (keyvalue[0] & 0x08) {
        return 10;
    } else if (keyvalue[2] & 0x08) {
        return 11;
    } else if (keyvalue[4] & 0x08) {
        return 12;
    }
    return 0;
}

// 功能菜单：按键控制
void MODE_LIST_key_control(int key)
{
    led_control(LED_WHITE);
    digital_control(0);
    switch (key) {
        case 1:
            // digital_control(1);
            MODE_STATUS = 1;
            break;
        case 2:
            MODE_STATUS = 2;
            digital_control(2);
            break;
        case 3:
            digital_control(3);
            MODE_STATUS = 3;
            break;
        case 4:
            digital_control(4);
            MODE_STATUS = 4;
            break;
        case 5:
            digital_control(5);
            MODE_STATUS = 5;
            break;
        case 6:
            digital_control(6);
            MODE_STATUS = 6;
            break;
        case 7:
            digital_control(7);
            MODE_STATUS = 7;
            break;
        case 8:
            // digital_control(8);
            MODE_STATUS = 8;
            break;
        case 9:
            digital_control(9);
            MODE_STATUS = 9;
            break;
        default:
            return;
    }
}

// 智能门锁-密码模式：密码识别
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
    for (int i = 0; i < 6; i++) {
        keyvalue_h[i] = keyvalue[i];
    }
    return key_;
}

// 智能门锁-密码模式
void LOCK_MODE_0()
{
    int key_ = key_down();
    switch (key_) {
        case 0:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 1;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 1;
                NEW_PASSWD_LEN++;
            }
            break;
        case 1:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 2;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 2;
                NEW_PASSWD_LEN++;
            }
            break;
        case 2:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 3;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 3;
                NEW_PASSWD_LEN++;
            }
            break;
        case 3:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 4;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 4;
                NEW_PASSWD_LEN++;
            }
            break;
        case 4:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 5;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 5;
                NEW_PASSWD_LEN++;
            }
            break;
        case 5:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 6;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 6;
                NEW_PASSWD_LEN++;
            }
            break;
        case 6:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 7;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 7;
                NEW_PASSWD_LEN++;
            }
            break;
        case 7:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 8;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 8;
                NEW_PASSWD_LEN++;
            }
            break;
        case 8:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10 + 9;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10 + 9;
                NEW_PASSWD_LEN++;
            }
            break;
        case 9:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD / 10;
                TEMP_PASSWD_LEN--;
            } else if (LOCK_STATUS == 1) {
                NEW_PASSWD     = 0;
                NEW_PASSWD_LEN = 0;
                LOCK_STATUS    = 2;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD / 10;
                NEW_PASSWD_LEN--;
            }
            break;
        case 10:
            if (LOCK_STATUS == 0) {
                TEMP_PASSWD = TEMP_PASSWD * 10;
                TEMP_PASSWD_LEN++;
            } else if (LOCK_STATUS == 2) {
                NEW_PASSWD = NEW_PASSWD * 10;
                NEW_PASSWD_LEN++;
            }
            break;
        case 11:
            if (LOCK_STATUS == -1 || LOCK_STATUS == 3) {
                TEMP_PASSWD     = 0;
                TEMP_PASSWD_LEN = 0;
                LOCK_STATUS     = 0;
            } else if (LOCK_STATUS == 0) {
                if (TEMP_PASSWD == PASSWD) {
                    LOCK_STATUS = 1;
                } else {
                    LOCK_STATUS = 3;
                }
            } else if (LOCK_STATUS == 1) {
                TEMP_PASSWD     = 0;
                TEMP_PASSWD_LEN = 0;
                LOCK_STATUS     = 0;
            } else if (LOCK_STATUS == 2) {
                PASSWD      = NEW_PASSWD;
                LOCK_STATUS = 1;
            }
            break;
        default:
            // return;
            break;
    }

    // 识别到输入特殊密码"1895"：回到主功能菜单
    if (TEMP_PASSWD == 1895) {
        MODE_STATUS = 0;
        TEMP_PASSWD = 0;
        LOCK_STATUS = -1;
        return;
    }

    // 密码比对
    TEMP_PASSWD %= 10000;
    NEW_PASSWD %= 10000;
    if (TEMP_PASSWD_LEN > 4)
        TEMP_PASSWD_LEN = 4;
    else if (TEMP_PASSWD_LEN < 0)
        TEMP_PASSWD_LEN = 0;
    if (NEW_PASSWD_LEN > 4)
        NEW_PASSWD_LEN = 4;
    else if (NEW_PASSWD_LEN < 0)
        NEW_PASSWD_LEN = 0;
    if (LOCK_STATUS == -1) {
        led_control(LED_SKYBLUE);
        SMG_show(SMG_addr.periph, SMG_addr.addr, 0, 0);
    } else if (LOCK_STATUS == 0) {
        led_control(LED_BLUE);
        SMG_show(SMG_addr.periph, SMG_addr.addr, TEMP_PASSWD, TEMP_PASSWD_LEN);
        // i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
    } else if (LOCK_STATUS == 1) {
        led_control(LED_GREEN);
        SMG_show(SMG_addr.periph, SMG_addr.addr, PASSWD, 4);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
    } else if (LOCK_STATUS == 2) {
        led_control(LED_YELLOW);
        SMG_show(SMG_addr.periph, SMG_addr.addr, NEW_PASSWD, NEW_PASSWD_LEN);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
    } else if (LOCK_STATUS == 3) {
        led_control(LED_RED);
        SMG_show(SMG_addr.periph, SMG_addr.addr, 0, 0);
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
    }
};

// 智能门锁-NFC模式：按键控制
void LOCK_MODE_1_key_control(int key)
{
    switch (key) {
        case 12:
            MODE_STATUS = 0;
            break;
        default:
            return;
    }
};

// 智能门锁-NFC模式
void LOCK_MODE_1()
{
    if (NFCLOCK_count == 1) {
        i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
        led_control(LED_RED);
        NFCLOCK_count = 0;
    }

    if (s5_detect(s5_addr.periph, s5_addr.addr, ucArray_ID)) {

        if (s5_verify(s5_addr.periph, s5_addr.addr, PICC_AUTHENT1A, 16, DefaultKey, ucArray_ID) == 0) {
            stat = 1;
            if (stat != back_stat) {
                i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
                led_control(LED_GREEN);
                NFCLOCK_count++;
            }
        }
    } else {
        stat = 0;
    }

    back_stat = stat;
    s5_sleep(s5_addr.periph, s5_addr.addr);
    delay_1ms(100);
};

// 智能风扇-NFC模式：按键控制
void FAN_MODE_0_key_control(int key)
{
    switch (key) {
        case 12:
            MODE_STATUS = 0;
            break;
        default:
            return;
    }
}

// 智能风扇-NFC模式
void FAN_MODE_0()
{
    if (s5_detect(s5_addr.periph, s5_addr.addr, ucArray_ID)) {
        if (s5_verify(s5_addr.periph, s5_addr.addr, PICC_AUTHENT1A, 16, DefaultKey, ucArray_ID) == 0) {
            stat = 1;
            if (stat != back_stat) {
                if (NFCFAN_count == 4) {
                    NFCFAN_count = 0;
                    pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 0);
                }

                pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, NFCFAN_count); // 根据刷卡次数：NFCFAN_count控制风扇转速

                switch (NFCFAN_count) {
                    case 0:
                        led_control(LED_OFF);
                        digital_control(0000);
                        break;
                    case 1:
                        led_control(LED_GREEN);
                        digital_control(1111);
                        break;
                    case 2:
                        led_control(LED_YELLOW);
                        digital_control(2222);
                        break;
                    case 3:
                        led_control(LED_RED);
                        digital_control(3333);
                        break;
                    default:
                        break;
                }
                NFCFAN_count++;
            }
        }
    } else {
        stat = 0;
    }
    back_stat = stat;
    s5_sleep(s5_addr.periph, s5_addr.addr);
};

// 智能风扇-自动模式：按键控制
void FAN_MODE_1_key_control(int key)
{
    if (FAN_STATUS == 1) {
        FAN_MODE_1();
    }

    if (FAN_STATUS == 2) {
        pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 0);
    }

    switch (key) {
        case 1:
            FAN_STATUS = 1;
            break;
        case 2:
            FAN_STATUS = 2;
            break;
        case 12:
            MODE_STATUS = 0;
            break;
        default:
            return;
    }
}

// 智能风扇-自动模式
void FAN_MODE_1()
{
    s2_data = s2_read_sht3x(s2_ss_addr.periph, s2_ss_addr.addr);

    int temperture = (int)s2_data.temperature;

    digital_control(temperture);

    if (temperture < 22) {
        pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 0);
    }

    if (temperture >= 22 && temperture <= 27) {
        pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 1);
    }

    if (temperture > 27 && temperture <= 30) {
        pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 2);
    }

    if (temperture > 30) {
        pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 3);
    }
};

// 智能灯光-手动模式：按键控制
void LIGHT_MODE_0_key_control(int key)
{
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
            MODE_STATUS = 0;
            break;
        default:
            return;
    }
};

// 智能灯光-自动模式：按键控制
void LIGHT_MODE_1_key_control(int key)
{
    switch (key) {
        case 1:
            led_control(LED_PURPLE);
            break;
        case 2:
            led_control(LED_OFF);
            break;
        case 12:
            MODE_STATUS = 0;
            break;
        default:
            return;
    }
};

// 智能窗帘-手动模式：按键控制
void CURTAIN_MODE_0_key_control(int key)
{
    led_control(LED_YELLOW);
    switch (key) {
        case 1:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
            digital_control(0);
            break;
        case 2:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 10);
            digital_control(1);
            break;
        case 3:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 20);
            digital_control(2);
            break;
        case 4:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 30);
            digital_control(3);
            break;
        case 5:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 40);
            digital_control(4);
            break;
        case 6:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 50);
            digital_control(5);
            break;
        case 7:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 60);
            digital_control(6);
            break;
        case 8:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 70);
            digital_control(7);
            break;
        case 9:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 80);
            digital_control(8);
            break;
        case 10:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 90);
            digital_control(9);
            break;
        case 11:
            i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 100);
            digital_control(10);
            break;
        case 12:
            MODE_STATUS = 0;
            break;
        default:
            return;
    }
};

// 智能窗帘-自动模式
void CURTAIN_MODE_1()
{
    LIGHT_VALUE = bh1750_get_light_value(s2_light_addr.periph, s2_light_addr.addr);
    digital_control(LIGHT_VALUE);
    i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, LIGHT_VALUE / 3);
};

// 智能窗帘-自动模式：按键控制
void CURTAIN_MODE_1_key_control(int key)
{

    if (CURTAIN_STATUS == 1) {
        CURTAIN_MODE_1();
    }
    if (CURTAIN_STATUS == 2) {
        digital_control(8);
    }
    switch (key) {
        case 1:
            CURTAIN_STATUS = 1;
            break;
        case 2:
            CURTAIN_STATUS = 2;
            break;
        case 12:
            MODE_STATUS    = 0;
            CURTAIN_STATUS = 2;
            break;
        default:
            return;
    }
};

// 主函数
int main(void)
{
    // 一些初始化
    init_i2c();
    i2c_addr_init();  // initializing i2c address of key and LED
    systick_config(); // system tick configuration
    ht16k33_init(e1_num_addr.periph, e1_num_addr.addr);
    bh1750_init(s2_light_addr.periph, s2_light_addr.addr);
    gd_eval_com_init(EVAL_COM0);
    s5_addr = s5_init(MS523_ADDRESS_S5);

    // 主循环
    while (1) {
        // 读取按键，并录入按键状态到keyvalue数组
        i2c_read(s1_key_addr.periph, s1_key_addr.addr, KEYKS0, keyvalue, 24);
        switch (MODE_STATUS) {
            case 0: // 功能菜单模式
                MODE_LIST_key_control(key_trans());
                break;
            case 1: // 智能门锁-密码模式
                LOCK_MODE_0();
                break;
            case 2: // 智能门锁-NFC模式
                LOCK_MODE_1();
                LOCK_MODE_1_key_control(key_trans());
                break;
            case 3: // 智能风扇-NFC模式
                FAN_MODE_0();
                FAN_MODE_0_key_control(key_trans());
                break;
            case 4: // 智能风扇-自动模式
                led_control(LED_RED);
                FAN_MODE_1_key_control(key_trans());
                break;
            case 5: // 智能灯光-手动模式
                LIGHT_MODE_0_key_control(key_trans());
                break;
            case 6: // 智能灯光-自动模式
                LIGHT_MODE_1_key_control(key_trans());
                break;
            case 7: // 智能窗帘-手动模式
                CURTAIN_MODE_0_key_control(key_trans());
                break;
            case 8: // 智能窗帘-自动模式
                led_control(LED_GREEN_BLUE_MID);
                CURTAIN_MODE_1_key_control(key_trans());
                break;
            case 9: // 一键离校模式
                // voice();
                pca9685_motor_control(e2_fan_addr.periph, e2_fan_addr.addr, 0);
                i2c_byte_write(e3_curtain_addr.periph, e3_curtain_addr.addr, CTL_REG_E3, 0);
                NFCFAN_count = 1;
                LOCK_STATUS  = 0;
                MODE_STATUS  = 0;
                break;
            default:
                break;
        }
        delay_1ms(100);
    }
}

// MODE_STATUS == 0                                        // 功能菜单模式
// MODE_STATUS == 1                                        // 智能门锁-密码模式
// MODE_STATUS == 2                                        // 智能门锁-NFC模式
// MODE_STATUS == 3                                        // 智能风扇-NFC模式
// MODE_STATUS == 4                                        // 智能风扇-模式
// MODE_STATUS == 5                                        // 智能灯光-手动模式
// MODE_STATUS == 6                                        // 智能灯光-自动模式
// MODE_STATUS == 7                                        // 智能窗帘-手动模式
// MODE_STATUS == 8                                        // 智能窗帘-自动模式
// MODE_STATUS == 9                                        // 一键离校模式