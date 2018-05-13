#include "bsp.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



#define bit0  0x001
#define bit1  0x002
#define bit2  0x004 
#define bit3  0x008 
#define bit4  0x010 
#define bit5  0x020 
#define bit6  0x040 
#define bit7  0x080 
#define bit8  0x100

#define WM8978_ADDR	 0X1A

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

#define SET_BIT(x,y) (x|=y)
#define CLEAR_BIT(x,y) (x&=~y)
#define CONFIG_BIT(x,y,z) (if(z){SET_BIT(x,y);}else{CLEAR_BIT(x,y);})


static uint8_t wm8978_write_reg(uint8_t index)
{ 
	uint8_t buf[2];
	val=WM8978_REGVAL_TBL[index];
	buf[0]=((val&0x0100)>>8)|(index<<1);
	buf[1]=(uint8_t)(val&0xff);
	hal_i2c_master_mem_write(0,WM8978_ADDR,buf[0],buf+1,1);
	return 0;	
}  

static uint16_t wm8978_read_reg(uint8_t reg)
{  
	return WM8978_REGVAL_TBL[reg];	
} 

typedef struct{
	uint16_t L_PGA_EN:1;
	uint16_t R_PGA_EN:1;
	uint16_t LIP_PGA:1;
	uint16_t LIN_PGA:1;
	uitn16_t RIP_PGA:1;
	uint16_t RIN_PGA:1;
	uint16_t R2_PGA:1;
	uint16_t L2_PGA:1;
    uint16_t ALC:2;
	uint16_t L_BOOST_EN:1;
	uint16_t R_BOOST_EN:1;

}InputCfgDef;

void wm8978_pga_vol(uin8_t left,uint8_t right){
	WM8978_REGVAL_TBL[45]&=~(0x3f);
	WM8978_REGVAL_TBL[45]|=left;

	WM8978_REGVAL_TBL[46]&=~(0x3f);
	WM8978_REGVAL_TBL[46]|=right;
	wm8978_write_reg(45);
	wm8978_write_reg(46);
}
void wm8978_pag_mute(uint8_t left,uint8_t right){
	
}

void wm8978_input_cfg(){
	InputPathCfgDef input_cfg;
	//pga enable
	CONFIG_BIT(WM8978_REGVAL_TBL[0x02],bit2,input_cfg->L_PGA_EN);
	CONFIG_BIT(WM8978_REGVAL_TBL[0x02],bit3,input_cfg->R_PGA_EN);
	//input control
	CONFIG_BIT(WM8978_REGVAL_TBL[44],bit0,input_cfg->LIP_PGA);
	CONFIG_BIT(WM8978_REGVAL_TBL[44],bit1,input_cfg->LIN_PGA);
	CONFIG_BIT(WM8978_REGVAL_TBL[44],bit2,input_cfg->L2_PGA);

	CONFIG_BIT(WM8978_REGVAL_TBL[44],bit4,input_cfg->RIP_PGA);
	CONFIG_BIT(WM8978_REGVAL_TBL[44],bit5,input_cfg->RIN_PGA);
	CONFIG_BIT(WM8978_REGVAL_TBL[44],bit6,input_cfg->R2_PGA);
	//alc control
	WM8978_REGVAL_TBL[32]&=~(0x3<<7);
	WM8978_REGVAL_TBL[32]|=(input_cfg->ALC<<7);
}



