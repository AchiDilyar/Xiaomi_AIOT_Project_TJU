#include "main.h"
#include "e4.h"
#include "gd32f4xx.h"
#include "ht16k33.h"
#include "i2c.h"
#include "string.h"
#include "systick.h"
i2c_addr_def key_addr; // Key address struct
i2c_addr_def LED_addr; // LED address struct
i2c_addr_def NFC_addr; // NFC address struct
i2c_addr_def SMG_addr; // SMG
i2c_addr_def s6_ult_addr;
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
            LED_addr.addr = PCA9685_ADDRESS_E1 + i * 2;
            LED_addr.flag = 1;
            break;
        }
    }
    // address not read
    if (LED_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, PCA9685_ADDRESS_E1 + i * 2)) {
                LED_addr.periph = I2C1;
                LED_addr.addr = PCA9685_ADDRESS_E1 + i * 2;
                LED_addr.flag = 1;
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
            SMG_addr.addr = HT16K33_ADDRESS_E1 + i * 2;
            SMG_addr.flag = 1;
            break;
        }
    }
    // address not read
    if (SMG_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, HT16K33_ADDRESS_E1 + i * 2)) {
                SMG_addr.periph = I2C1;
                SMG_addr.addr = HT16K33_ADDRESS_E1 + i * 2;
                SMG_addr.flag = 1;
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
//---------------------------------------------------KEY----------------------------------------------
#if S1
    // S1 KEY I2C address init
    for (i = 0; i < 4; i++) {
        if (i2c_addr_poll(I2C0, HT16K33_ADDRESS_S1 + i * 2)) {
            key_addr.periph = I2C0;
            key_addr.addr = HT16K33_ADDRESS_S1 + i * 2;
            key_addr.flag = 1;
            break;
        }
    }
    // address not read
    if (key_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, HT16K33_ADDRESS_S1 + i * 2)) {
                key_addr.periph = I2C1;
                key_addr.addr = HT16K33_ADDRESS_S1 + i * 2;
                key_addr.flag = 1;
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
            NFC_addr.addr = MS523_ADDRESS_S5 + i * 2;
            NFC_addr.flag = 1;
            break;
        }
    }
    // address not read
    if (NFC_addr.flag != 1) {
        // poll I2C1,Verti1
        for (i = 0; i < 4; i++) {
            if (i2c_addr_poll(I2C1, MS523_ADDRESS_S5 + i * 2)) {
                NFC_addr.periph = I2C1;
                NFC_addr.addr = MS523_ADDRESS_S5 + i * 2;
                NFC_addr.flag = 1;
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

    //-------------------------------------------------------CSB----------------------------------------------

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
        setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 4095);  // g
        setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 0);     // r
        setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 0);     // b
        break;
    case 2:
        setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 0);     // g
        setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 4095);  // r
        setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 0);     // b
        break;
    case 3:
        setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 0);     // g
        setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 0);     // r
        setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 4095);  // b
        break;
    case 4:
        setPWM(LED_addr.periph, LED_addr.addr, 0, 0, 4095); // g
        setPWM(LED_addr.periph, LED_addr.addr, 1, 0, 4095); // r
        setPWM(LED_addr.periph, LED_addr.addr, 2, 0, 4095); // b
        break;
    case 5:
        setPWM(LED_addr.periph, LED_addr.addr, 0, 0x0000, 0x0000);
        break;
    case 6:
        setPWM(LED_addr.periph, LED_addr.addr, 2, 0x0000, 0x0000);
        break;
    case 7:
        setPWM_off(LED_addr.periph, LED_addr.addr);
        break;
    default:
        setPWM_off(LED_addr.periph, LED_addr.addr);
        break;
    }
}
uint8_t keyvalue[6]; // Storage of key status
uint8_t keyvalue_h[6];
bit_status key[12]; // 状态
bit_status key_up[12]; // 状态上升沿
bit_status key_down[12]; // 下降沿
void key_init()
{
    for (int i = 0; i < 12; i++) {
        if (i < 6) {
            keyvalue[i] = 0;
            keyvalue_h[i] = 0;
        }

        key[i] = 0;
        key_up[i] = 0;
        key_down[i] = 0;
    }
}

// 0-8可用
void __key()
{
    i2c_read(key_addr.periph, key_addr.addr, KEYKS0, keyvalue, 6);
    for (int i = 0; i < 12; i++) {
        int x, y;
        x = 0, y = 0;
        if (i % 3 == 0)
            x = 0;
        if (i % 3 == 1)
            x = 2;
        if (i % 3 == 2)
            x = 4;
        y = i / 3;
        y = (1 << y);
        if ((keyvalue[x] & y) && (!(keyvalue_h[x] & y))) {
            key[i] ^= 1;
            key_up[i] = 1;
            key_down[i] = 0;
        } else if ((keyvalue_h[x] & y) && (!(keyvalue[x] & y))) {
            key_up[i] = 0;
            key_down[i] = 1;
        } else {
            key_up[i] = 0;
            key_down[i] = 0;
        }
    }

    for (int i = 0; i < 6; i++) {
        keyvalue_h[i] = keyvalue[i];
    }
}

//==============================================================录音===================================================================

#define MAXPLAYCNT 80000 // for S3 and E4=80000,other=10000

unsigned long recordplaycnt = 0;

uint16_t delay_count = 0; // 延时变量
uint16_t time_count = 0; // 1s定时变量

unsigned short playdata[MAXPLAYCNT]; // 语音数据缓存

unsigned long playcnt = 0; // 音频文件索引
unsigned char playflag = 0; // 放音标志

 uint8_t start_record_flag = 0; // 录音标志
uint8_t record_finish_flag = 1; // 录音完成标志

uint8_t ringing_flag = 0; // 扬声器一直响

/***********************************************************************************************************
//timer3 init 1ms定时
************************************************************************************************************/
void timer3_init(void)
{
    timer_parameter_struct timer_init_struct;

    rcu_periph_clock_enable(RCU_TIMER3);

    timer_deinit(TIMER3);
    timer_init_struct.prescaler = 4199;
    timer_init_struct.period = 20;
    timer_init_struct.alignedmode = TIMER_COUNTER_EDGE;
    timer_init_struct.counterdirection = TIMER_COUNTER_UP;
    timer_init_struct.clockdivision = TIMER_CKDIV_DIV1;
    timer_init_struct.repetitioncounter = 0;
    timer_init(TIMER3, &timer_init_struct);

    nvic_irq_enable(TIMER3_IRQn, 1, 1);
    timer_interrupt_enable(TIMER3, TIMER_INT_UP);
    timer_enable(TIMER3);
}

/********************************************************************************************************
//初始化led
*********************************************************************************************************/
void dis_led_init(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* configure led GPIO port */
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    GPIO_BC(GPIOF) = GPIO_PIN_4;
}

/********************************************************************************************************
//初始化串口
*********************************************************************************************************/
void uart_init(uint32_t usart_periph)
{
    if (usart_periph == USART0) {
        nvic_irq_enable(USART0_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_GPIOB); /* enable GPIO clock */
        rcu_periph_clock_enable(RCU_USART0); /* enable USART clock */
        gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_6); /* connect port to USARTx_Tx */
        gpio_af_set(GPIOB, GPIO_AF_7, GPIO_PIN_7); /* connect port to USARTx_Rx */
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6); /* configure USART Tx as alternate function push-pull */
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
        gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7); /* configure USART Rx as alternate function push-pull */
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
        /* USART configure */
        usart_deinit(USART0);
        usart_baudrate_set(USART0, 115200U);
        usart_receive_config(USART0, USART_RECEIVE_ENABLE);
        usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
        usart_enable(USART0);
        usart_interrupt_enable(USART0, USART_INT_RBNE);
    } else if (usart_periph == USART2) {
        nvic_irq_enable(USART2_IRQn, 0, 0);
        rcu_periph_clock_enable(RCU_GPIOC); /* enable GPIO clock */
        rcu_periph_clock_enable(RCU_USART2); /* enable USART clock */
        gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_10); /* connect port to USARTx_Tx */
        gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_11); /* connect port to USARTx_Rx */
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10); /* configure USART Tx as alternate function push-pull */
        gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11); /* configure USART Rx as alternate function push-pull */
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
        /* USART configure */
        usart_deinit(USART2);
        usart_baudrate_set(USART2, 115200U);
        usart_receive_config(USART2, USART_RECEIVE_ENABLE);
        usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
        usart_enable(USART2);
        usart_interrupt_enable(USART2, USART_INT_RBNE);
    }
}

/********************************************************************************************************
//串口发送数据
*********************************************************************************************************/
uint16_t uart_print(uint32_t usart_periph, uint8_t* data, uint16_t len)
{
    uint8_t i;
    for (i = 0; i < len; i++) {
        while (usart_flag_get(usart_periph, USART_FLAG_TC) == RESET)
            ;
        usart_data_transmit(usart_periph, data[i]);
    }
    while (usart_flag_get(usart_periph, USART_FLAG_TC) == RESET)
        ;
    return len;
}

/********************************************************************************************************
//调试串口发送数据
*********************************************************************************************************/
void debug_printf(uint32_t usart_periph, char* string)
{
    uint8_t buffer[100];
    uint16_t len;

    len = strlen(string);
    strncpy((char*)buffer, string, len);
    uart_print(usart_periph, buffer, len);
}
/********************************************************************************************************
//1ms定时中断服务程序
*********************************************************************************************************/
void TIMER3_IRQHandler(void)
{
    if (timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_UP)) {
        timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);

        if (delay_count > 0)
            delay_count--;

        if (time_count++ >= 300) {
            time_count = 0;
        }
    }
}

/********************************************************************************************************
//spi2 i2s中断
*********************************************************************************************************/
void SPI2_IRQHandler(void)
{

    if (SET == spi_i2s_interrupt_flag_get(I2S2_ADD, SPI_I2S_INT_RBNE)) {
        playdata[playcnt++] = spi_i2s_data_receive(I2S2_ADD); // 录音
        if (playcnt >= MAXPLAYCNT || record_finish_flag == 1) {
            spi_i2s_interrupt_disable(I2S2_ADD, SPI_I2S_INT_RBNE);
            // record_flag = 1;
            record_finish_flag = 0;
            recordplaycnt = playcnt;
            playcnt = 0;
        }
    }

    if (SET == spi_i2s_interrupt_flag_get(SPI2, SPI_I2S_INT_TBE)) {
        if (playflag || ringing_flag) {
            spi_i2s_data_transmit(SPI2, playdata[playcnt++]); // 放音
            if (playcnt >= recordplaycnt) {
                playflag = 0;
                playcnt = 0;
                // start_flag = 0;
            }
        } else {
            spi_i2s_data_transmit(SPI2, 0x0000);
        }
    }
}

//==============================================================音录===================================================================

//===========================================================超声波测距=================================================================
bit_status near_flag;
void test_dis()
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
        if (display < 300) {
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

//===========================================================距测波声超=================================================================

int num = 0;
// key0开始暂停,key3重置闪烁0,key1千,key2百,key4十,key5个,key6录音,key7播放,key8停止发声;
int time = 0;
int time_h = 0;
void times()
{
    __key();

    if (!key[0]) // 时流
        num++;

    test_dis(); // 超声波测距
    if (key_up[6]) {
        start_record_flag = 1;
        record_finish_flag = 0;
        playcnt = 0;
        e4_sound_recording_config();
        nvic_irq_enable(SPI2_IRQn, 0, 0); // 打开中断,可以录音
    } else if (key_down[6]) {
        start_record_flag = 0;
        record_finish_flag = 1;
    }

    if(playflag == 1 || ringing_flag == 1) {
        led_control(LED_GREEN);
    } else if(start_record_flag == 1){
        led_control(LED_BLUE);
    } else {
        led_control(7);
    }

    if (key_up[7]) {
        e4_playback_config();
        playflag = 1;
    }
    if (near_flag) {
        ringing_flag = 0;
        playflag = 0;
        playcnt = 0;
    }

    if (key_up[8]) {
        ringing_flag = 0;
        playflag = 0;
        playcnt = 0;
    }

    if (key[3]) {
        if (key_up[3]) {
            time = 0;
        }
        if (key_up[1]) {
            int o = time / 1000;
            time -= o * 1000;
            time += ((o + 1) % 10) * 1000;
        }
        if (key_up[2]) {
            int o = (time % 1000) / 100;
            time -= o * 100;
            time += ((o + 1) % 10) * 100;
        }
        if (key_up[4]) {
            int o = (time % 100) / 10;
            time -= o * 10;
            time += ((o + 1) % 10) * 10;
        }
        if (key_up[5]) {
            int o = time % 10;
            time -= o;
            time += (o + 1) % 10;
        }
        
        if (num == 5) {
            ht16k33_display_off(SMG_addr.periph, SMG_addr.addr);
        }
        if (num == 10) {
            SMG_show(SMG_addr.periph, SMG_addr.addr, time);
        }
        if (num == 15) {
            ht16k33_display_off(SMG_addr.periph, SMG_addr.addr);
        }
        if (num == 20) {
            SMG_show(SMG_addr.periph, SMG_addr.addr, time);
            num = 0;
        }
        

    } else {
        if (num == 20) {
            if (time != 0) {
                time--;
                if (time == 0 && time_h != 0) {
                    ringing_flag = 1;
                }
            }
            num = 0;
        }
        SMG_show(SMG_addr.periph, SMG_addr.addr, time);
    }

    time_h = time;
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
    i2c_addr_init(); // initializing i2c address of key and LED
    systick_config(); // system tick configuration
    key_init();

    //////录音初始
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    dis_led_init();
    e4_init();
    //////

    while (1) {
        delay_1ms(50);
        times();
    }

    // while (1) {
    //     if (start_flag == 0) {
    //         start_flag = 1;
    //         record_flag = 0;
    //         playcnt = 0;
    //         e4_sound_recording_config();
    //         nvic_irq_enable(SPI2_IRQn, 0, 0); // 打开中断,可以录音
    //         LED_OFF;
    //     } else {
    //         if (record_flag) {
    //             record_flag = 0;
    //             e4_playback_config();
    //             playflag = 1;
    //             LED_ON;
    //         }
    //     }
    // }
}
