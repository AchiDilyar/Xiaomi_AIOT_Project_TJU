#include "i2c.h"
#include "s5.h"
#include "stdio.h"
#include "string.h"

/***********************************************************************************************************
//s5 ms延时
************************************************************************************************************/
void delay_s5_ms(uint16_t count)
{
     uint16_t i;
     uint32_t decount;
	   for(i=0;i<count;i++)
     {
			    for(decount=0;decount<50000;decount++)
			    {
				}	
		 }	 
}


void MFRC_Delay(uint16_t Delay_Time)
{
  uint16_t i, j;
  for (i = 40; i > 0; i--)
  {
    for (j = Delay_Time; j > 0; j--);
  }
}
/*!
    \brief      clear register bit
		\param[in]  reg:register
		\param[in]  mask:bit mask
    \param[out] none
    \retval     none
*/
void ClearBitMask(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t reg, uint8_t mask)
{
  uint8_t tmp = 0x0;
  i2c_delay_read(i2c_periph,i2c_addr,reg,&tmp,1);
  i2c_delay_byte_write(i2c_periph,i2c_addr,reg, tmp & ~mask);  // clear bit mask
}

/*!
    \brief      set register bit
		\param[in]  reg:register
		\param[in]  mask:bit mask
    \param[out] none
    \retval     none
*/
void SetBitMask(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t reg, uint8_t mask)
{
  uint8_t tmp = 0x0;
  i2c_delay_read(i2c_periph,i2c_addr,reg,&tmp,1);
  i2c_delay_byte_write(i2c_periph,i2c_addr,reg, tmp | mask);  // set bit mask
}


/*!
    \brief      turn on the antenna
		\param[in]  none
    \param[out] none
    \retval     none
*/
void PcdAntennaOn(uint32_t i2c_periph,uint8_t i2c_addr)
{
  unsigned char i;
  i2c_delay_read(i2c_periph,i2c_addr,TxControlReg,&i,1);
  if (!(i & 0x03))
  {
    SetBitMask(i2c_periph,i2c_addr,TxControlReg, 0x03);
  }
}

/*!
    \brief      turn off the antenna
		\param[in]  none
    \param[out] none
    \retval     none
*/
void PcdAntennaOff(uint32_t i2c_periph,uint8_t i2c_addr)
{
  ClearBitMask(i2c_periph,i2c_addr,TxControlReg, 0x03);
}
/*!
    \brief      Communication with iso14443 card
		\param[in]  Command:Command
		\param[in]  pInData:send data		
		\param[in]  InLenByte:Send data length
		\param[out] pOutData:receive data
		\param[out] pOutLenBit:receive data bit lenth
    \retval     Communication status
*/

#define MAXRLEN                       18
int8_t PcdComMS523(uint32_t i2c_periph,uint8_t i2c_addr,
								 uint8_t Command, 
                 uint8_t *pInData, 
                 uint8_t InLenByte,
                 uint8_t *pOutData, 
                 uint16_t  *pOutLenBit)
{
  int8_t status = MI_ERR;
  uint8_t irqEn   = 0x00;
  uint8_t waitFor = 0x00;
  uint8_t lastBits;
  uint8_t n;
  uint16_t i;
  
  switch (Command)
  {
    case PCD_AUTHENT:
            irqEn = 0x12;
            waitFor = 0x10;
            break;
    case PCD_TRANSCEIVE:
            irqEn = 0x77;
            waitFor = 0x30;
            break;
    default:
            break;
  }
  
  i2c_delay_byte_write(i2c_periph,i2c_addr,ComIEnReg, irqEn | 0x80);
  ClearBitMask(i2c_periph,i2c_addr,ComIrqReg, 0x80);
  i2c_delay_byte_write(i2c_periph,i2c_addr,CommandReg, PCD_IDLE);
  SetBitMask(i2c_periph,i2c_addr,FIFOLevelReg, 0x80);  

  i2c_delay_write(i2c_periph,i2c_addr,FIFODataReg, pInData,InLenByte);

	
  i2c_delay_byte_write(i2c_periph,i2c_addr,CommandReg, Command);
  
  if (Command == PCD_TRANSCEIVE)
  {
    SetBitMask(i2c_periph,i2c_addr,BitFramingReg, 0x80);
  }
  
  i = 800; 
  do
  {
    i2c_delay_read(i2c_periph,i2c_addr,ComIrqReg,&n,1);
    i--;
  } while ((i != 0) && !(n & 0x01) && !(n & waitFor));
  ClearBitMask(i2c_periph,i2c_addr,BitFramingReg, 0x80);
  
  if (i != 0)
  {
		i2c_delay_read(i2c_periph,i2c_addr,ErrorReg,&n,1);
    if (!(n & 0x1B))
    {
      status = MI_OK;
      if (n & irqEn & 0x01)
      {
        status = MI_NOTAGERR;
      }
      if (Command == PCD_TRANSCEIVE)
      {
				i2c_delay_read(i2c_periph,i2c_addr,FIFOLevelReg,&n,1);
				i2c_delay_read(i2c_periph,i2c_addr,ControlReg,&lastBits,1);
        lastBits &= 0x07;
        if (lastBits)
        {
          *pOutLenBit = (n - 1) * 8 + lastBits;
        }
        else
        {
          *pOutLenBit = n * 8;
        }
        if (n == 0)
        {
          n = 1;
        }
        if (n > MAXRLEN)
        {
          n = MAXRLEN;
        }
				i2c_delay_read(i2c_periph,i2c_addr,FIFODataReg,pOutData,n);
      }
    }
    else
    {
      status = MI_ERR;
    }
  }
  SetBitMask(i2c_periph,i2c_addr,ControlReg, 0x80);           // stop timer now
  i2c_delay_byte_write(i2c_periph,i2c_addr,CommandReg, PCD_IDLE);
  return status;
}


/*!
    \brief      reset ms523
		\param[in]  none
    \param[out] none
    \retval     none
*/
void PcdReset(uint32_t i2c_periph,uint8_t i2c_addr)
{

  i2c_delay_byte_write(i2c_periph,i2c_addr,CommandReg, PCD_RESETPHASE);
  MFRC_Delay(2000);
  
  i2c_delay_byte_write(i2c_periph,i2c_addr,ModeReg, 0x3D);            
  i2c_delay_byte_write(i2c_periph,i2c_addr,TReloadRegL, 30);             
  i2c_delay_byte_write(i2c_periph,i2c_addr,TReloadRegH, 0);
  i2c_delay_byte_write(i2c_periph,i2c_addr,TModeReg, 0x8D);
  i2c_delay_byte_write(i2c_periph,i2c_addr,TPrescalerReg, 0x3E);
  i2c_delay_byte_write(i2c_periph,i2c_addr,TxAutoReg, 0x40);
  
  ClearBitMask(i2c_periph,i2c_addr,TestPinEnReg, 0x80);         //off MX and DTRQ out
  i2c_delay_byte_write(i2c_periph,i2c_addr,TxAutoReg, 0x40);
  
}	

/*!
    \brief      Calulate CRC16
		\param[in]  pInData:send data		
		\param[in]  InLenByte:Send data length
		\param[out] pOutData:receive data
    \retval     none
*/
void CalulateCRC(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t *pIndata,uint8_t len,uint8_t *pOutData)
{
  uint8_t i,n;
      
  ClearBitMask(i2c_periph,i2c_addr,DivIrqReg,0x04);
  i2c_delay_byte_write(i2c_periph,i2c_addr,CommandReg,PCD_IDLE);
  SetBitMask(i2c_periph,i2c_addr,FIFOLevelReg,0x80);
 
  i2c_delay_write(i2c_periph,i2c_addr,FIFODataReg, pIndata,len); 	
  i2c_delay_byte_write(i2c_periph,i2c_addr,CommandReg, PCD_CALCCRC);
  i = 0xFF;
  do 
  {
    i2c_delay_read(i2c_periph,i2c_addr,DivIrqReg,&n,1);
    i--;
  }
  while ((i!=0) && !(n&0x04));
  i2c_delay_read(i2c_periph,i2c_addr,CRCResultRegL,&pOutData[0],1);
  i2c_delay_read(i2c_periph,i2c_addr,CRCResultRegM,&pOutData[1],1);
}


/*!
    \brief      Set the mode of ms523
		\param[in]  type:mode
		\param[out] none
    \retval     status
*/
int8_t M500PcdConfigISOType(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t type)
{
  if('A' == type)
  {
    i2c_delay_byte_write(i2c_periph,i2c_addr,Status2Reg,0x08);
    i2c_delay_byte_write(i2c_periph,i2c_addr,ModeReg,0x3D);
    i2c_delay_byte_write(i2c_periph,i2c_addr,RxSelReg,0x86);
    i2c_delay_byte_write(i2c_periph,i2c_addr,RFCfgReg,0x7F);   
    i2c_delay_byte_write(i2c_periph,i2c_addr,TReloadRegL,30);
    i2c_delay_byte_write(i2c_periph,i2c_addr,TReloadRegH,0);
    i2c_delay_byte_write(i2c_periph,i2c_addr,TModeReg,0x8D);
    i2c_delay_byte_write(i2c_periph,i2c_addr,TPrescalerReg,0x3E);
    MFRC_Delay(10000);
    PcdAntennaOn(i2c_periph,i2c_addr);
  }
  else
  {
    return -1;
  }
	
  return MI_OK;
}

/*!
    \brief      Initialization ms523
		\param[in]  none
		\param[out] none
    \retval     none
*/

void MFRC522_Init(uint32_t i2c_periph,uint8_t i2c_addr)
{
  PcdReset(i2c_periph,i2c_addr);
  PcdAntennaOff(i2c_periph,i2c_addr);
  MFRC_Delay(2000);
  PcdAntennaOn(i2c_periph,i2c_addr);
  M500PcdConfigISOType(i2c_periph,i2c_addr,'A');
}

/*!
    \brief      Card search
		\param[in]  req_code:Card searching mode
		\param[in]  pTagType:Card type
								0x4400 = Mifare_UltraLight
                0x0400 = Mifare_One(S50)
                0x0200 = Mifare_One(S70)
                0x0800 = Mifare_Pro(X)
                0x4403 = Mifare_DESFire
		\param[out] none
    \retval     status
*/

int8_t PcdRequest(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t req_code, uint8_t *pTagType)
{
  int8_t status;  
  uint16_t  unLen;
  uint8_t ucComMS523Buf[MAXRLEN]; 
  
  ClearBitMask(i2c_periph,i2c_addr,Status2Reg, 0x08);
  i2c_delay_byte_write(i2c_periph,i2c_addr,BitFramingReg, 0x07);
  SetBitMask(i2c_periph,i2c_addr,TxControlReg, 0x03);
  
  ucComMS523Buf[0] = req_code;
  
  status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE, ucComMS523Buf, 1, ucComMS523Buf,
                  &unLen);
  if ((status == MI_OK) && (unLen == 0x10))
  {
    *pTagType = ucComMS523Buf[0];
    *(pTagType + 1) = ucComMS523Buf[1];
  }
  else
  {
    status = MI_ERR;
  }
  
  return status;
}

/*!
    \brief      card anti-collision
		\param[out]  pSnr:serial number,4bytes
		\param[in] none
    \retval     status
*/

int8_t PcdAnticoll(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t *pSnr)
{
  int8_t status;
  uint8_t i,snr_check=0;
  uint16_t  unLen;
  uint8_t ucComMS523Buf[MAXRLEN]; 
  

  ClearBitMask(i2c_periph,i2c_addr,Status2Reg,0x08);
  i2c_delay_byte_write(i2c_periph,i2c_addr,BitFramingReg,0x00);
  ClearBitMask(i2c_periph,i2c_addr,CollReg,0x80);

  ucComMS523Buf[0] = PICC_ANTICOLL1;
  ucComMS523Buf[1] = 0x20;

  status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,2,ucComMS523Buf,&unLen);

  if (status == MI_OK)
  {
   for (i=0; i<4; i++)
   {   
     *(pSnr+i)  = ucComMS523Buf[i];
     snr_check ^= ucComMS523Buf[i];
   }
   if (snr_check != ucComMS523Buf[i])
   {   
     status = MI_ERR;    
   }
  }
  
  SetBitMask(i2c_periph,i2c_addr,CollReg,0x80);
  return status;
}
/*!
    \brief      Selected card
		\param[in]  pSnr:serial number,4bytes
		\param[out] none
    \retval     status
*/
int8_t PcdSelect(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t *pSnr)
{
  int8_t status;
  uint8_t i;
  uint16_t  unLen;
  uint8_t ucComMS523Buf[MAXRLEN]; 
  
  ucComMS523Buf[0] = PICC_ANTICOLL1;
  ucComMS523Buf[1] = 0x70;
  ucComMS523Buf[6] = 0;
  for (i=0; i<4; i++)
  {
    ucComMS523Buf[i+2] = *(pSnr+i);
    ucComMS523Buf[6]  ^= *(pSnr+i);
  }
  CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,7,&ucComMS523Buf[7]);

  ClearBitMask(i2c_periph,i2c_addr,Status2Reg,0x08);

  status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,9,ucComMS523Buf,&unLen);
  
  if ((status == MI_OK) && (unLen == 0x18))
  {   
    status = MI_OK;  
  }
  else
  {   
    status = MI_ERR;    
  }

  return status;
}
/*!
    \brief      Enter sleep mode
		\param[in]  none
		\param[out] none
    \retval     status
*/

int8_t PcdHalt(uint32_t i2c_periph,uint8_t i2c_addr)
{
  uint16_t  unLen;
  uint8_t ucComMS523Buf[MAXRLEN]; 

  ucComMS523Buf[0] = PICC_HALT;
  ucComMS523Buf[1] = 0;
  CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,2,&ucComMS523Buf[2]);

  PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,4,ucComMS523Buf,&unLen);

  return MI_OK;
}
/*!
    \brief      Verify card password
		\param[in]  auth_mode:Password authentication mode
		\param[in]  addr:Block address
		\param[in]  pKey:Password
		\param[in]  pSnr:serial number,4bytes
		\param[out] none
    \retval     status
*/            
int8_t PcdAuthState(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr)
{
  int8_t status;
  uint16_t  unLen;
  uint8_t i,ucComMS523Buf[MAXRLEN]; 
	uint8_t tmp;

  ucComMS523Buf[0] = auth_mode;
  ucComMS523Buf[1] = addr;
  for (i=0; i<6; i++)
  {    
    ucComMS523Buf[i+2] = *(pKey+i);   
  }
  for (i=0; i<4; i++)
  {    
    ucComMS523Buf[i+8] = *(pSnr+i);   
  }

  
  status = PcdComMS523(i2c_periph,i2c_addr,PCD_AUTHENT,ucComMS523Buf,12,ucComMS523Buf,&unLen);
	i2c_delay_read(i2c_periph,i2c_addr,Status2Reg,&tmp,1);
  if ((status != MI_OK) || (!(tmp & 0x08)))
  {   
    status = MI_ERR;   
  }
  
  return status;
}
/*!
    \brief      read M1 card data of block
		\param[in]  addr:Block address
		\param[out] pData: data
    \retval     status
*/    

int8_t PcdRead(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t addr,uint8_t *pData)
{
  int8_t status;
  uint16_t  unLen;
  uint8_t i,ucComMS523Buf[MAXRLEN]; 

  ucComMS523Buf[0] = PICC_READ;
  ucComMS523Buf[1] = addr;
  CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,2,&ucComMS523Buf[2]);
 
  status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,4,ucComMS523Buf,&unLen);
  if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(pData, ucComMS523Buf, 16);   }
  {
    for (i=0; i<16; i++)
    {    
      *(pData+i) = ucComMS523Buf[i];   
    }
  }
  else
  {   
    status = MI_ERR;   
  }
  
  return status;
}
/*!
    \brief      write data of block to M1 card
		\param[in]  addr:Block address
		\param[in] pData: data
    \retval     status
*/                
int8_t PcdWrite(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t addr,uint8_t *pData)
{
  int8_t status;
  uint16_t  unLen;
  uint8_t i,ucComMS523Buf[MAXRLEN]; 
  
  ucComMS523Buf[0] = PICC_WRITE;
  ucComMS523Buf[1] = addr;
  CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,2,&ucComMS523Buf[2]);

  status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,4,ucComMS523Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMS523Buf[0] & 0x0F) != 0x0A))
  {   
    status = MI_ERR;   
  }
      
  if (status == MI_OK)
  {
    //memcpy(ucComMS523Buf, pData, 16);
    for (i=0; i<16; i++)
    {    
      ucComMS523Buf[i] = *(pData+i);   
    }
    CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,16,&ucComMS523Buf[16]);

    status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,18,ucComMS523Buf,&unLen);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMS523Buf[0] & 0x0F) != 0x0A))
    {   
      status = MI_ERR;   
    }
  }
  
  return status;
}
/*!
    \brief      Deduction and recharge
		\param[in]  dd_mode:command
		\param[in]  addr: Wallet address
		\param[in]  pValue: data,4-byte increase (decrease) low order data in front
    \retval     status
*/             
int8_t PcdValue(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t dd_mode,uint8_t addr,uint8_t *pValue)
{
  int8_t status;
  uint16_t  unLen;
  uint8_t ucComMS523Buf[MAXRLEN]; 
  
  ucComMS523Buf[0] = dd_mode;
  ucComMS523Buf[1] = addr;
  CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,2,&ucComMS523Buf[2]);
  
  status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,4,ucComMS523Buf,&unLen);
  
  if ((status != MI_OK) || (unLen != 4) || ((ucComMS523Buf[0] & 0x0F) != 0x0A))
  {   
    status = MI_ERR;   
  }
      
  if (status == MI_OK)
  {
    memcpy(ucComMS523Buf, pValue, 4);
 //       for (i=0; i<16; i++)
 //       {    ucComMS523Buf[i] = *(pValue+i);   }
    CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,4,&ucComMS523Buf[4]);
    unLen = 0;
    status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,6,ucComMS523Buf,&unLen);
    if (status != MI_ERR)
    {    
      status = MI_OK;    
    }
  }
  
  if (status == MI_OK)
  {
    ucComMS523Buf[0] = PICC_TRANSFER;
    ucComMS523Buf[1] = addr;
    CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,2,&ucComMS523Buf[2]); 

    status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,4,ucComMS523Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMS523Buf[0] & 0x0F) != 0x0A))
    {   
      status = MI_ERR;   
    }
  }
  return status;
}
/*!
    \brief      Backup Wallet
		\param[in]  dd_mode:command
		\param[in]  sourceaddr: source address
		\param[in]  goaladdr: Destination address
    \retval     status
*/  
int8_t PcdBakValue(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t sourceaddr, uint8_t goaladdr)
{
  int8_t status;
  uint16_t  unLen;
  uint8_t ucComMS523Buf[MAXRLEN]; 

  ucComMS523Buf[0] = PICC_RESTORE;
  ucComMS523Buf[1] = sourceaddr;
  CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,2,&ucComMS523Buf[2]);

  status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,4,ucComMS523Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMS523Buf[0] & 0x0F) != 0x0A))
  {   
    status = MI_ERR;   
  }
  
  if (status == MI_OK)
  {
	ucComMS523Buf[0] = 0;
	ucComMS523Buf[1] = 0;
	ucComMS523Buf[2] = 0;
	ucComMS523Buf[3] = 0;
	CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,4,&ucComMS523Buf[4]);

	status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,6,ucComMS523Buf,&unLen);
	if (status != MI_ERR)
	{    
	  status = MI_OK;    
	}
  }
  
  if (status != MI_OK)
  {    
    return MI_ERR;   
  }
  
  ucComMS523Buf[0] = PICC_TRANSFER;
  ucComMS523Buf[1] = goaladdr;

  CalulateCRC(i2c_periph,i2c_addr,ucComMS523Buf,2,&ucComMS523Buf[2]);

  status = PcdComMS523(i2c_periph,i2c_addr,PCD_TRANSCEIVE,ucComMS523Buf,4,ucComMS523Buf,&unLen);

  if ((status != MI_OK) || (unLen != 4) || ((ucComMS523Buf[0] & 0x0F) != 0x0A))
  {   
    status = MI_ERR;   
  }

  return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Init_MFRC522(uint32_t i2c_periph,uint8_t i2c_addr)
{
	PcdReset(i2c_periph,i2c_addr);
	PcdAntennaOff(i2c_periph,i2c_addr);
	delay_s5_ms(500);
	PcdAntennaOn(i2c_periph,i2c_addr);
	M500PcdConfigISOType(i2c_periph,i2c_addr,'A');
}



/*********************************************************************************************************
函数名:     s5_init
入口参数:   i2c初始地址 address 
出口参数:   无 
返回值:     i2c_addr_def定义结构体
作者:       zzz
日期:       2023/4/3
调用描述:   得到s5 i2c地址,若器件不存在则结构体flag值为0,若存在则初始化芯片置结构体flag值为1
**********************************************************************************************************/
i2c_addr_def s5_init(uint8_t address)
{
	uint8_t i;
	i2c_addr_def e_address;

	e_address.flag = 0;

	for(i=0;i<4;i++)
	{
		e_address = get_board_address(address + i*2);
		if(e_address.flag)
		{
			Init_MFRC522(e_address.periph,e_address.addr);
			break;
		}						
	}

	return e_address;
}


/*********************************************************************************************************
函数名:     s5_all_init
入口参数:   s5_address结构体指针  s5_addr:s5 i2c起始地址 
出口参数:   s5_address结构体指针
返回值:     无
作者:       zzz
日期:       2023/4/4
调用描述:   查找并初始化所有s5子板,并得到相应的地址放在结构体指针s5_address
**********************************************************************************************************/
void s5_all_init(out s5_addr_def *s5_address,uint8_t s5_addr)
{
	uint8_t i; 

	for(i=0;i<4;i++)
	{
		s5_address->nfc_addr[i] = get_board_address(s5_addr + i*2);
		if(s5_address->nfc_addr[i].flag) 
		{
			Init_MFRC522( s5_address->nfc_addr[i].periph, s5_address->nfc_addr[i].addr);
		}						
	}	
}


/*********************************************************************************************************
函数名:     s5_detect
入口参数:   uint32_t i2c_periph,uint8_t i2c_addr Cardnum:数据指针
出口参数:   Cardnum数据指针
返回值:     成功返回1 失败返回0
作者:       zzz
日期:       2023/4/3
调用描述:   检测MFC卡是否存在,检测到成功返回1,否则返回0
**********************************************************************************************************/
uint8_t s5_detect(uint32_t i2c_periph,uint8_t i2c_addr,out uint8_t *Cardnum)
{
	uint8_t sts = 0;
	uint8_t Type[2];


	if(PcdRequest(i2c_periph,i2c_addr,PICC_REQALL,Type) == MI_OK)
	{
		if(PcdAnticoll(i2c_periph,i2c_addr,Cardnum) == MI_OK)
		{
			if(PcdSelect(i2c_periph,i2c_addr,Cardnum) == MI_OK)
			{
				sts = 1; 
			}
		}
	}
	return sts;
}



/*********************************************************************************************************
函数名:     s5_verify
入口参数:   i2c_periph:I2C口 i2c_addr:i2c地址 addr:块地址(0-63) pKey:密码指针 pSnr:数据指针
出口参数:   无 
返回值:     成功返回0 失败返回2
作者:       zzz
日期:       2023/4/3
调用描述:   块密码验证
**********************************************************************************************************/
uint8_t s5_verify(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr)
{
	uint8_t rt = 2;
	if(addr <= 63)
	{	 
		rt = PcdAuthState(i2c_periph,i2c_addr,auth_mode,addr,pKey,pSnr);  
	}	 
	return rt;
}

/*********************************************************************************************************
函数名:     s5_write_data
入口参数:   i2c_periph:I2C口 i2c_addr:i2c地址 addr:块地址 *pData:待写入数据指针 块地址0-63
出口参数:   无 
返回值:     成功返回0 失败返回2
作者:       zzz
日期:       2023/4/3
调用描述:   往MFC块地址写入不超过16个字节的数据
**********************************************************************************************************/
uint8_t s5_write_data(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t addr,uint8_t *pData)
{
	 uint8_t rt = 2;
     if(addr <= 63)
     {
          rt = PcdWrite(i2c_periph,i2c_addr,addr,pData);
     }
		 
	 return rt;
}



/*********************************************************************************************************
函数名:     s5_read_data
入口参数:   i2c_periph:I2C口 i2c_addr:i2c地址 addr:块地址 *pData:读入数据指针 块地址0-63
出口参数:   无 
返回值:     成功返回0 失败返回2
作者:       zzz
日期:       2023/4/3
调用描述:   往pData指针中读入不超过16个字节的数据
**********************************************************************************************************/
uint8_t s5_read_data(uint32_t i2c_periph,uint8_t i2c_addr,uint8_t addr,uint8_t *pData)
{
     uint8_t rt = 2;
     if(addr <= 63)
     {
          rt = PcdRead(i2c_periph,i2c_addr,addr,pData);
     }
		 
	 return rt;
}	
	

/*********************************************************************************************************
函数名:     s5_sleep
入口参数:   i2c_periph:I2C口 i2c_addr:i2c地址 
出口参数:   无 
返回值:     无
作者:       zzz
日期:       2023/4/4
调用描述:   进入休眠
**********************************************************************************************************/
void s5_sleep(uint32_t i2c_periph,uint8_t i2c_addr)
{
     PcdHalt(i2c_periph,i2c_addr);
}

