#ifndef S3_H
#define S3_H


#include "i2c.h"

#define  WM8978_ADDRESS           0x34	

#define	 PLAYCNT		              80000			// for S3 and E4=80000,other=10000

void WM8978_Init(void);
void WM8978_ADDA_Cfg(uint8_t dacen,uint8_t adcen);
void WM8978_Input_Cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen);
void WM8978_Output_Cfg(uint8_t dacen,uint8_t bpsen);
void WM8978_MIC_Gain(uint8_t gain);
void WM8978_LINEIN_Gain(uint8_t gain);
void WM8978_AUX_Gain(uint8_t gain);
void WM8978_Write_Reg(uint8_t reg,uint16_t val); 
uint16_t WM8978_Read_Reg(uint8_t reg);
void WM8978_HPvol_Set(uint8_t voll,uint8_t volr);
void WM8978_SPKvol_Set(uint8_t volx);
void WM8978_I2S_Cfg(uint8_t fmt,uint8_t len);
void WM8978_3D_Set(uint8_t depth);
void WM8978_EQ_3D_Dir(uint8_t dir); 
void WM8978_EQ1_Set(uint8_t cfreq,uint8_t gain); 
void WM8978_EQ2_Set(uint8_t cfreq,uint8_t gain);
void WM8978_EQ3_Set(uint8_t cfreq,uint8_t gain);
void WM8978_EQ4_Set(uint8_t cfreq,uint8_t gain);
void WM8978_EQ5_Set(uint8_t cfreq,uint8_t gain);


void i2s_config(void);
void e4_init(void);
void e4_sound_recording_config(void);
void e4_playback_config(void);


#endif

