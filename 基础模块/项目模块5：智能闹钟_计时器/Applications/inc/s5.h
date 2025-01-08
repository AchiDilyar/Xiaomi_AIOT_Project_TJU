#ifndef S5_H
#define S5_H

#include "i2c.h"

#define     out
#define		MS523_ADDRESS_S5		 0x50	 //S5子板I2C地址


typedef struct
{
   i2c_addr_def nfc_addr[4];
}s5_addr_def;

enum _tag_eMFRC522_CMD
{
		PCD_IDLE       = 0x00,               //取消当前命令
		PCD_AUTHENT    = 0x0E,               //验证密钥
		PCD_RECEIVE    = 0x08,               //接收数据
		PCD_TRANSMIT   = 0x04,               //发送数据
		PCD_TRANSCEIVE = 0x0C,               //发送并接收数据
		PCD_RESETPHASE = 0x0F,               //复位
		PCD_CALCCRC    = 0x03,               //CRC计算
};

//Mifare_One card CMD
enum _tag_ePICC_CMD
{
		PICC_REQIDL    = 0x26,               //寻天线区内未进入休眠状态
		PICC_REQALL    = 0x52,               //寻天线区内全部卡
		PICC_ANTICOLL1 = 0x93,               //防冲撞
		PICC_ANTICOLL2 = 0x95,               //防冲撞
		PICC_AUTHENT1A = 0x60,               //验证A密钥
		PICC_AUTHENT1B = 0x61,               //验证B密钥
		PICC_READ      = 0x30,               //读块
		PICC_WRITE     = 0xA0,               //写块
		PICC_DECREMENT = 0xC0,               //扣款
		PICC_INCREMENT = 0xC1,               //充值
		PICC_RESTORE   = 0xC2,              //调块数据到缓冲区
		PICC_TRANSFER  = 0xB0,               //保存缓冲区中数据
		PICC_HALT      = 0x50,               //休眠
};

/////////////////////////////////////////////////////////////////////
//MS523 FIFO lenth
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH           64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MS523 register
/////////////////////////////////////////////////////////////////////
// PAGE 0
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		              0x3F

/////////////////////////////////////////////////////////////////////
//error code
/////////////////////////////////////////////////////////////////////
#define MI_OK                      0
#define MI_NOTAGERR                1//(-1)
#define MI_ERR                     2//(-2)



i2c_addr_def s5_init(uint8_t address);
void s5_all_init(s5_addr_def *s5_address,uint8_t s5_addr);
uint8_t s5_detect(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t *Cardnum);
uint8_t s5_verify(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr);
uint8_t s5_write_data(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t addr,uint8_t *pData);
uint8_t s5_read_data(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t addr,uint8_t *pData);
void s5_sleep(uint32_t i2c_periph,uint8_t i2c_addr);
#endif

