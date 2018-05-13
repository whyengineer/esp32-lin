#include "bsp.h"

#include "wm8978.h"
#include "hal_i2c.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_i2s.h"


#define bit0  0x001
#define bit1  0x002
#define bit2  0x004 
#define bit3  0x008 
#define bit4  0x010 
#define bit5  0x020 
#define bit6  0x040 
#define bit7  0x080 
#define bit8  0x100
 	

static uint16_t WM8978_REGVAL_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 




uint8_t WM8978_Write_Reg(uint8_t reg,uint16_t val)
{ 
	uint8_t buf[2];
	buf[0]=((val&0x0100)>>8)|(reg<<1);
	buf[1]=(uint8_t)(val&0xff);
	hal_i2c_master_mem_write(0,WM8978_ADDR,buf[0],buf+1,1);
	WM8978_REGVAL_TBL[reg]=val;
	return 0;	
}  

uint16_t WM8978_Read_Reg(uint8_t reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 

