#include "NFC.h"
#include "i2c.h"
#include "systick.h"
#include <stdio.h>
void ms532_init(uint32_t i2c_periph, uint8_t i2c_addr)
{
    i2c_byte_write(i2c_periph, i2c_addr, 0x01, 0x0F);
    i2c_byte_write(i2c_periph, i2c_addr, 0x11, 0x3D);
    i2c_byte_write(i2c_periph, i2c_addr, 0x2D, 0x1E);
    i2c_byte_write(i2c_periph, i2c_addr, 0x2C, 0x00);
    i2c_byte_write(i2c_periph, i2c_addr, 0x2A, 0x8D);
    i2c_byte_write(i2c_periph, i2c_addr, 0x2B, 0x3E);

    i2c_byte_write(i2c_periph, i2c_addr, 0x15, 0x40);
    i2c_byte_write(i2c_periph, i2c_addr, 0x80, 0x10);
    i2c_byte_write(i2c_periph, i2c_addr, 0x00, 0x10);
    i2c_byte_write(i2c_periph, i2c_addr, 0x15, 0x40);
    {
        uint8_t* t;
        i2c_read(i2c_periph, i2c_addr, 0x14, t, 8);
        //t &= 0b11111100;
        i2c_byte_write(i2c_periph, i2c_addr, 0x14, *t);
        delay_1ms(200);
    }
    i2c_byte_write(i2c_periph, i2c_addr, 0x08, 0x08);
    i2c_byte_write(i2c_periph, i2c_addr, 0x11, 0x3D);
    i2c_byte_write(i2c_periph, i2c_addr, 0x17, 0x86);
    i2c_byte_write(i2c_periph, i2c_addr, 0x26, 0x7F);
    i2c_byte_write(i2c_periph, i2c_addr, 0x2D, 0x30);
    i2c_byte_write(i2c_periph, i2c_addr, 0x2C, 0x00);
    i2c_byte_write(i2c_periph, i2c_addr, 0x2A, 0x8D);
    i2c_byte_write(i2c_periph, i2c_addr, 0x2B, 0x3E);
}