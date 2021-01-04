/*
 * TFT_400x240_OTM4001A_16bit.c
 *
 *  Created on: 2014年8月26日
 *      Author: Sam
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/epi.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "inc/tm4c1294ncpdt.h"
#include "TFT_400x240_OTM4001A_16bit.h"
#include "font.h"
#include "EPIinit/EPIinit.h"
#include "picture.h"


//
// LCD control line GPIO definitions.
//

#define LCD_CS_PERIPH              SYSCTL_PERIPH_GPIOE
#define LCD_CS_BASE                GPIO_PORTE_BASE
#define LCD_CS_PIN                 GPIO_PIN_1
#define LCD_CS_PIN_REG               0x40024008
#define LCD_RESET_PERIPH           SYSCTL_PERIPH_GPIOF
#define LCD_RESET_BASE             GPIO_PORTF_BASE
#define LCD_RESET_PIN              GPIO_PIN_4
#define LCD_RESET_PIN_REG            0x40025040
#define LCD_RS_PERIPH              SYSCTL_PERIPH_GPIOP
#define LCD_RS_BASE                GPIO_PORTP_BASE
#define LCD_RS_PIN                 GPIO_PIN_5
#define LCD_RS_PIN_REG               0x40065080
#define LCD_IM0_PERIPH             SYSCTL_PERIPH_GPIOL
#define LCD_IM0_BASE               GPIO_PORTL_BASE
#define LCD_IM0_PIN                GPIO_PIN_5
#define LCD_IM0_PIN_REG              0x40062080
#define LCD_WR_PERIPH              SYSCTL_PERIPH_GPIOE
#define LCD_WR_BASE                GPIO_PORTE_BASE
#define LCD_WR_PIN                 GPIO_PIN_2
#define LCD_WR_PIN_REG               0x40024010
#define LCD_RD_PERIPH              SYSCTL_PERIPH_GPIOE
#define LCD_RD_BASE                GPIO_PORTE_BASE
#define LCD_RD_PIN                 GPIO_PIN_0
#define LCD_RD_PIN_REG               0x40024004

//*****************************************************************************
//
// define TFTLCD_ADDR to use EPI HB16 mode to driver TFTLCD
//
#define TFTLCD_ADDR	  *(unsigned short *)0xA0000000

#define LCD_VERTICAL_MAX 400
#define LCD_HORIZONTAL_MAX 240


//*****************************************************************************
//
// Function pointer types for low level LCD controller access functions.
//
//*****************************************************************************
typedef void (*pfnWriteData)(uint16_t ui16Data);
typedef void (*pfnWriteCommand)(uint16_t ui8Data);

//*****************************************************************************
//
// Function pointers for low level LCD controller access functions.
//
//*****************************************************************************

static void WriteDataGPIO(uint16_t ui16Data);
static void WriteCommandGPIO(uint16_t ui16Data);

pfnWriteData WriteData = WriteDataGPIO;
pfnWriteCommand WriteCommand = WriteCommandGPIO;

//*****************************************************************************
//
// Writes a data word to the SSD1306.  This function implements the basic GPIO
// interface to the LCD display.
//
//*****************************************************************************

static void
WriteDataGPIO(uint16_t ui16Data)
{
    //
    // Write the most significant byte of the data to the bus.
    //
	TFTLCD_ADDR = 0;
//    SysCtlDelay(3);
    //
    // Pull RS High.
    //
    HWREG(LCD_RS_PIN_REG) = 0xFF;
    SysCtlDelay(1);
    //
    // Pull CS Low.
    //
    HWREG(LCD_CS_PIN_REG) = 0;
    //
    // Pull RD High.
    //
//    HWREG(LCD_RD_PIN_REG) = 0xFF;

//    SysCtlDelay(3);

    //
    // Assert the write enable signal.
    // Delay for at least 60nS (at 120 Mhz) to meet Display timing.
    //
    HWREG(LCD_WR_PIN_REG) = 0;
    //
    // Write the least significant byte of the data to the bus.
    //
    TFTLCD_ADDR = ui16Data;
//    SysCtlDelay(4);

    SysCtlDelay(3);
    //
    // Deassert the write enable signal.
    //
    HWREG(LCD_WR_PIN_REG) = 0xFF;
//    SysCtlDelay(1);

    //
    // Deassert the chip select pin.
    //
    HWREG(LCD_CS_PIN_REG) = 0xFF;
    SysCtlDelay(3);
}

//*****************************************************************************
//
// Writes a command to the SSD2119.  This function implements the basic GPIO
// interface to the LCD display.
//
//*****************************************************************************

static void
WriteCommandGPIO(uint16_t ui16Data)
{
    //
    // Write the most significant byte of the data to the bus. This is always
    // 0 since commands are no more than 8 bits currently.
    //
//	TFTLCD_ADDR = 0;
//    SysCtlDelay(3);
    //
    // Pull RS Low.
    //
    HWREG(LCD_RS_PIN_REG) = 0;
        SysCtlDelay(1);
    //
    // Pull CS Low.
    //
    HWREG(LCD_CS_PIN_REG) = 0;
    //
    // Pull RD High.
    //
//    HWREG(LCD_RD_PIN_REG) = 0xFF;
    // Assert the write enable signal.
    // Delay for at least 60nS (at 120 Mhz) to meet Display timing.
    //
    HWREG(LCD_WR_PIN_REG) = 0;

    //
    // Write the least significant byte of the data to the bus.
    //
	TFTLCD_ADDR = ui16Data;

//    SysCtlDelay(3);

    SysCtlDelay(3);
    //
    // Deassert the write enable signals.  Make sure we add padding here
    // too since some compilers inline this function.
    //
    HWREG(LCD_WR_PIN_REG) = 0xFF;
//    SysCtlDelay(1);

    HWREG(LCD_CS_PIN_REG) = 0xFF;
    SysCtlDelay(3);
}







//*****************************************************************************
//
// Initializes the pins required for the GPIO-based LCD interface.
//
// This function configures the GPIO pins used to control the LCD display
// when the basic GPIO interface is in use.  On exit, the LCD controller
// has been reset and is ready to receive command and data writes.
//
// \return None.
//
//*****************************************************************************
void
InitGPIOLCDInterface(uint32_t ui32ClockMS)
{
    //
    // Configure the pins that connect to the LCD as GPIO outputs.
    //
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOB);//
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOC);//
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOE);//
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOF);//
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOK);//
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOM);//
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPION);//
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOP);//
    SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOL);//

    GPIOPinTypeGPIOOutput(LCD_CS_BASE, LCD_CS_PIN);
    GPIOPinTypeGPIOOutput(LCD_RESET_BASE, LCD_RESET_PIN);
    GPIOPinTypeGPIOOutput(LCD_RS_BASE, LCD_RS_PIN);
    GPIOPinTypeGPIOOutput(LCD_IM0_BASE, LCD_IM0_PIN);
    GPIOPinTypeGPIOOutput(LCD_WR_BASE, LCD_WR_PIN);
    GPIOPinTypeGPIOOutput(LCD_RD_BASE, LCD_RD_PIN);

    //
    // Set the LCD control pins to their default values.
    //
    GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, LCD_CS_PIN);
    GPIOPinWrite(LCD_RS_BASE, LCD_RS_PIN, 0x00);
    GPIOPinWrite(LCD_RD_BASE, LCD_RD_PIN, LCD_RD_PIN);
    GPIOPinWrite(LCD_WR_BASE, LCD_WR_PIN, LCD_WR_PIN);
    GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, 0);
    GPIOPinWrite(LCD_IM0_BASE, LCD_IM0_PIN, 0x00);
    //
    // Delay for 1ms.
    //
    SysCtlDelay(ui32ClockMS);

    //
    // Deassert the LCD reset signal.
    //
    GPIOPinWrite(LCD_RESET_BASE, LCD_RESET_PIN, LCD_RESET_PIN);

    //
    // Delay for 1ms while the LCD comes out of reset.
    //
    SysCtlDelay(ui32ClockMS);
}


void TFTLCD_EnterSleep()
{

	WriteCommand(0x07F0); WriteData(0x5420);
	WriteCommand(0x07DE); WriteData(0x0C00);
	WriteCommand(0x07F0); WriteData(0x0000);
	WriteCommand(0x0007); WriteData(0x0000);
	SysCtlDelay(200000);
	WriteCommand(0x0100); WriteData(0x0002);
	SysCtlDelay(200000);
}

void TFTLCD_ExitSleep()
{

	WriteCommand(0x07F0); WriteData(0x5420);
	WriteCommand(0x07DE); WriteData(0x0400);
	WriteCommand(0x07F0); WriteData(0x0000);
	WriteCommand(0x0100); WriteData(0x11B0);
	SysCtlDelay(200000);
	WriteCommand(0x0007); WriteData(0x0173);
	SysCtlDelay(200000);

}




//*****************************************************************************
//
//! Initializes the display driver.
//!
//! \param ui32SysClockSpeed is the system clock speed of the MCU.
//!
//! This function initializes the TFTLCD display controller on the panel,
//! preparing it to display data.
//!
//! \return None.
//
//*****************************************************************************
void
TFT_400x240_OTM4001Ainit(uint32_t ui32SysClockSpeed)
{
    uint32_t ui32ClockMS;

    //
    // Get the current processor clock frequency.
    //
    ui32ClockMS = ui32SysClockSpeed / (3 * 1000);

    //
    // Enable the GPIO peripherals used to interface to the OTM4001A.
    //
    SysCtlPeripheralEnable(LCD_RS_PERIPH);
    SysCtlPeripheralEnable(LCD_RD_PERIPH);
    SysCtlPeripheralEnable(LCD_WR_PERIPH);
    SysCtlPeripheralEnable(LCD_CS_PERIPH);
    SysCtlPeripheralEnable(LCD_RESET_PERIPH);
    SysCtlPeripheralEnable(LCD_IM0_PERIPH);

    //
    // Perform low level interface initialization depending upon how the LCD
    // is connected to the Tiva C Series microcontroller.  This varies
    // depending upon the daughter board connected it is possible that a
    // daughter board can drive the LCD directly rather than via the basic GPIO
    // interface.
    //
    {
        //
        // Initialize the GPIOs used to interface to the LCD controller.
        //
        InitGPIOLCDInterface(ui32ClockMS);
    }

    HWREG(LCD_RESET_PIN_REG) = 0xFF;
    SysCtlDelay(200000);   //Under 120MHz system clock, SysCtlDelay(1000) is about 25us, SysCtlDelay(1) for about 25ns.
    HWREG(LCD_RESET_PIN_REG) = 0;
    SysCtlDelay(400000);
    HWREG(LCD_RESET_PIN_REG) = 0xFF;
    SysCtlDelay(4000000);
    // sleep
//    TFTLCD_EnterSleep();
//======== Power ON sequence============//
    WriteCommand(0x0606);WriteData(0x0000);//Set the order of receiving data when using i80 interface.1st to 2nd
    SysCtlDelay(4000000);
    WriteCommand(0x0007);WriteData(0x0001);//Display Control 1
    SysCtlDelay(200000);
    WriteCommand(0x0110);WriteData(0x0001);
    SysCtlDelay(200000);
    WriteCommand(0x0100);WriteData(0x17B0);
    WriteCommand(0x0101);WriteData(0x0147);
    WriteCommand(0x0102);WriteData(0x019D);
    WriteCommand(0x0103);WriteData(0x3600);
    WriteCommand(0x0281);WriteData(0x0010);
    SysCtlDelay(200000);
    WriteCommand(0x0102);WriteData(0x01BD);
    SysCtlDelay(200000);
//==========================================//
    WriteCommand(0x0000);WriteData(0x0000);
    WriteCommand(0x0001);WriteData(0x0100);

    WriteCommand(0x0002);WriteData(0x0100);
    WriteCommand(0x0003);WriteData(0x00A0);

    WriteCommand(0x0006);WriteData(0x0000);
//    WriteCommand(0x0008);WriteData(0x0503);
    WriteCommand(0x0008);WriteData(0x0202);//Display Control 2

    WriteCommand(0x0009);WriteData(0x0001);//Display Control 3
//    WriteCommand(0x000B);WriteData(0x0010);//Low Power Control
    WriteCommand(0x000B);WriteData(0x0000);//Sam test

    WriteCommand(0x000C);WriteData(0x0000);//External Display Interface Control 1
     	 	 	 	 	 	 	 	 	   //RM = 0,System interface/VSYNC interface
    WriteCommand(0x000F);WriteData(0x0000);//External Display Interface Control 2
    WriteCommand(0x0007);WriteData(0x0001);

                                           //Panel Interface Control 1
    WriteCommand(0x0010);WriteData(0x0013);//Set the clock cycle per line  19

    WriteCommand(0x0011);WriteData(0x0202);// NOWI [2:0]: Set the adjacent gate driver output non-overlap
//    WriteCommand(0x0011);WriteData(0x0301);// SDTI2-0: Set the delay of source output in every line.

    WriteCommand(0x0012);WriteData(0x0300);
    WriteCommand(0x0020);WriteData(0x021E);
    WriteCommand(0x0021);WriteData(0x0202);

    WriteCommand(0x0022);WriteData(0x0100);
    WriteCommand(0x0090);WriteData(0x8000);//Set the position of the frame marker. 0 < FMP < BP + NL + FP
    WriteCommand(0x0092);WriteData(0x8000);
    WriteCommand(0x0100);WriteData(0x16B0);
//    WriteCommand(0x0100);WriteData(0x12B0);
//    WriteCommand(0x0100);WriteData(0x11B0);
    SysCtlDelay(200000);
    WriteCommand(0x0101);WriteData(0x0147);
//    WriteCommand(0x0101);WriteData(0x0027);//Set DC/DC charge pump circuit,Set the voltage of VCI1. VCI1
    WriteCommand(0x0102);WriteData(0x01BE);
//    WriteCommand(0x0102);WriteData(0x018A);//Set the voltage level of VREG1OUT
    SysCtlDelay(200000);
//    WriteCommand(0x0103);WriteData(0x0600);
    WriteCommand(0x0103);WriteData(0x1600);
    SysCtlDelay(200000);
    WriteCommand(0x0107);WriteData(0x0000);
    SysCtlDelay(200000);
    WriteCommand(0x0110);WriteData(0x0001);
    SysCtlDelay(200000);
    WriteCommand(0x0210);WriteData(0x0000);
    WriteCommand(0x0211);WriteData(0x00EF);
    WriteCommand(0x0212);WriteData(0x0000);
   //Window Vertical RAM Address End
    WriteCommand(0x0213);WriteData(0x018F);
    WriteCommand(0x0200);WriteData(0x0000);//tb

    WriteCommand(0x0201);WriteData(0x0000);//tb
    WriteCommand(0x0280);WriteData(0x0000);
//    WriteCommand(0x0281);WriteData(0x0000);
    WriteCommand(0x0281);WriteData(0x0007);

    WriteCommand(0x0282);WriteData(0x0000);
    SysCtlDelay(200000);
    WriteCommand(0x0300);WriteData(0x0002);
    WriteCommand(0x0301);WriteData(0x081D);
    WriteCommand(0x0302);WriteData(0x1621);
    WriteCommand(0x0303);WriteData(0x3D12);
    WriteCommand(0x0304);WriteData(0x3C0B);
    WriteCommand(0x0305);WriteData(0x1004);
    WriteCommand(0x0306);WriteData(0x0A06);
    WriteCommand(0x0307);WriteData(0x0612);
    WriteCommand(0x0308);WriteData(0x0105);
    WriteCommand(0x0309);WriteData(0x0004);
    WriteCommand(0x030A);WriteData(0x0F05);
    WriteCommand(0x030B);WriteData(0x0F00);
    WriteCommand(0x030C);WriteData(0x000F);
    WriteCommand(0x030D);WriteData(0x050F);
    WriteCommand(0x030E);WriteData(0x0106);
    WriteCommand(0x030F);WriteData(0x0406);

    WriteCommand(0x0400);WriteData(0x3500);
    WriteCommand(0x0401);WriteData(0x0001);
    WriteCommand(0x0404);WriteData(0x0000);
    WriteCommand(0x0500);WriteData(0x0000);
    WriteCommand(0x0501);WriteData(0x0000);
    WriteCommand(0x0502);WriteData(0x0000);
    WriteCommand(0x0503);WriteData(0x0000);
    WriteCommand(0x0504);WriteData(0x0000);
    WriteCommand(0x0505);WriteData(0x0000);
    WriteCommand(0x0600);WriteData(0x0000);
    WriteCommand(0x0606);WriteData(0x0000);
    WriteCommand(0x06F0);WriteData(0x0000);
    WriteCommand(0x07F0);WriteData(0x5420);

    WriteCommand(0x07F2);WriteData(0x00DF);
    WriteCommand(0x07F3);WriteData(0x288A);
    WriteCommand(0x07F4);WriteData(0x0022);
    WriteCommand(0x07F5);WriteData(0x0041);
    WriteCommand(0x07F0);WriteData(0x0000);
    WriteCommand(0x0007);WriteData(0x0173);
    SysCtlDelay(200000);
    WriteCommand(0x0007);WriteData(0x0061);
    SysCtlDelay(200000);
    WriteCommand(0x0007);WriteData(0x0173);
    SysCtlDelay(2000000);
    WriteCommand(0x0202);

    // Exit sleep
//    TFTLCD_ExitSleep();
    TFTLCD_CLEAR(0);
}

void TFTLCD_REFRESH_GRAM(uint16_t ui16Data)
{
	WriteData(ui16Data);
}

//================================================================================================
//	实现功能：	设置坐标
//  输入参数：  x0，y0 起始坐标
//         x1，y1 结束坐标
//================================================================================================
void TFTLCD_SetPos(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1)
{
    WriteCommand(0x0210);WriteData(x0);
    WriteCommand(0x0211);WriteData(x1);
    WriteCommand(0x0212);WriteData(y0);
    WriteCommand(0x0213);WriteData(y1);
    WriteCommand(0x0200);WriteData(x0);
    WriteCommand(0x0201);WriteData(y0);
    WriteCommand(0x0202);
}

//================================================================================================
//	实现功能：	画点
//  输入参数：  x，y 需要画点坐标
//         color 点的颜色
//================================================================================================
void TFTLCD_DrawPoint(unsigned int x,unsigned int y,unsigned int color)
{
	TFTLCD_SetPos(x,x,y,y);        //设置待画点坐标
	WriteData(color);         //在指定点写入颜色数据
}


//================================================================================================
//	实现功能：	显示Ascii字符
//  输入参数：  x 横坐标
//              y 纵坐标
//		        c 需要显示的字符
//		        fColor 字符颜色
//		        bColor 字符背景颜色
//================================================================================================
void TFTLCD_ShowChar(uint32_t x, uint32_t y, uint32_t chr, uint32_t fColor, uint32_t bColor)
{
	uint32_t i,j;
	uint8_t m;

	chr=chr-' ';//得到偏移后的值
	TFTLCD_SetPos(x,x+8-1,y,y+16-1);                    //设置字符显示位置
	for(i=0; i<16;i++) {                             //循环写入16字节，一个字符为16字节
	  m=asc2_1608[chr][i];  //提取c字符的第i个字节
		for(j=0;j<8;j++) {                        //循环写入8位，一个字节为8位
			if((m&0x80)==0x80) {                  //判断最高位是否为1
				WriteData(fColor);           //最高位为1，写入字符颜色
				}
			else {
				WriteData(bColor);           //最高位为0，写入背景颜色
				;
				}
			m<<=1;                                //左移1位，准备写下一位
			}
		}
}



//显示字符串
//x,y:起点坐标
//*p:字符串起始地址
//用16字体
#define MAX_CHAR_POSX 230
#define MAX_CHAR_POSY 390
void TFTLCD_ShowString(uint32_t x,uint32_t y,char *p,uint32_t fColor, uint32_t bColor)
{

    while(*p!='\0')
    {
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;TFTLCD_CLEAR(bColor);}
        TFTLCD_ShowChar(x,y,*p,fColor,bColor);
        x+=8;
        p++;
    }
}

//================================================================================================
//	实现功能：	TFT清屏
//	输入参数：	bColor 清屏所使用的背景色
//================================================================================================
void TFTLCD_CLEAR(uint32_t bColor)
{
 uint32_t i,j;
 TFTLCD_SetPos(0,240,0,400);//400x240
 for (i=0;i<400;i++)
	{
	   for (j=0;j<240;j++)
		   WriteData(bColor);
	}
}

//================================================================================================
//	实现功能：	用指定颜色填充指定区域
//	输入参数：	x0:横坐标起点,y0:纵坐标起点,x1:横坐标终点,y1:纵坐标终点,color:指定颜色
//================================================================================================
void TFTLCD_FillBlock(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1, uint32_t Color)
{
	 uint32_t i,j;
	TFTLCD_SetPos(x0,x1,y0,y1);
	 for (i=y0;i<y1;i++)
		{
		   for (j=x0;j<x1;j++)
			   WriteData(Color);
		}


}

//================================================================================================
//  实现功能：显示一个变量数据 i32Data  -9999<=i32Data<=9999
//  输入参数：变量数字i32Data，    x,y:起点坐标       ,color:指定颜色
//================================================================================================
void TFTLCD_ShowData(uint32_t x,uint32_t y,int32_t i32Data,uint32_t fColor, uint32_t bColor)
{
	char Data[6];
	if(i32Data>9999||i32Data<-9999)
	{
		TFTLCD_ShowString(x,y,"Error:Out of range!",RED,WHITE);
		return;
	}
	if(i32Data < 0)
	{
		Data[0] = '-';
		i32Data = ~i32Data+1;
	}
	else
	{
		Data[0] = ' ';
	}
	Data[1] = i32Data / 1000;
	Data[2] = (i32Data-Data[1]*1000) / 100;
	Data[3] = (i32Data-Data[1]*1000-Data[2]*100) / 10;
	Data[4] = (i32Data-Data[1]*1000-Data[2]*100-Data[3]*10);
	Data[1] += '0';
	Data[2] += '0';
	Data[3] += '0';
	Data[4] += '0';
	Data[5] = '\0';
	TFTLCD_ShowString(x,y,Data,fColor,bColor);
	;
}



//显示一副图片
void TFTLCD_ShowPicture(uint32_t x,uint32_t y,uint8_t gImage[])
{
	uint32_t temp,i,j,m;
	for(i=0;i<120;i++)
	{
		for(j=0;j<240;j++)
		{
			WriteData(0x00);
		}
	}
	for(i=0;i<160;i++)
	{
		for(j=0;j<56;j++)
		{
			WriteData(0x00);
		}
		for(j=0;j<128;j++)
		{
			temp=gImage[m];
			temp |= gImage[m+1]<<8;
			WriteData(temp);
			m++;
			m++;
			}
		for(j=0;j<56;j++)
			{
			WriteData(0x00);
			}
	}
	for(i=0;i<120;i++)
	{
		for(j=0;j<240;j++)
		{
			WriteData(0x00);
		}
	}

}

//================================================================================================
//	实现功能：	画一条水平线
//	输入参数：	x0:横坐标起点,x1:横坐标终点,y:纵坐标,color:指定颜色
//================================================================================================
void TFTLCD_DrawHorizontalLine(uint32_t x0,uint32_t x1,uint32_t y,uint32_t color)
{
	uint32_t i=0,PointNum = 0;
	PointNum = x1-x0;
	for(i=0;i<PointNum;i++)
	{
		TFTLCD_DrawPoint(x0+i,y,color);
	}

}


//================================================================================================
//	实现功能：	画一条垂直线
//	输入参数：	y0:纵坐标起点,y1:纵坐标终点,x:横坐标,color:指定颜色
//================================================================================================
void TFTLCD_DrawVerticalLine(uint32_t y0,uint32_t y1,uint32_t x,uint32_t color)
{
	uint32_t i=0,PointNum = 0;
	PointNum = y1-y0;
	for(i=0;i<PointNum;i++)
	{
		TFTLCD_DrawPoint(x,y0+i,color);
	}

}

//====================================================================================
// 实现功能：   画一条任意的直线
//  输入参数：   y0:纵坐标起点,y1:纵坐标终点,x0:横坐标起点，x1：横坐标起点,color:指定颜色
//=====================================================================================
void TFTLCD_DrawLine(int x1,int y1,int x2,int y2,int color)
{
    int dx,dy,e;
    dx=x2-x1;
    dy=y2-y1;

    if(dx>=0)
    {
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 1/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                    TFTLCD_DrawPoint(x1,y1,color);
                    if(e>0){y1+=1;e-=dx;}
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 2/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                    TFTLCD_DrawPoint(x1,y1,color);
                    if(e>0){x1+=1;e-=dy;}
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)


            if(dx>=dy) // 8/8 octant
            {
                e=dy-dx/2;
                while(x1<=x2)
                {
                    TFTLCD_DrawPoint(x1,y1,color);
                    if(e>0){y1-=1;e-=dx;}
                    x1+=1;
                    e+=dy;
                }
            }
            else        // 7/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                    TFTLCD_DrawPoint(x1,y1,color);
                    if(e>0){x1+=1;e-=dy;}
                    y1-=1;
                    e+=dx;
                }
            }
        }
    }
    else //dx<0
    {
        dx=-dx;     //dx=abs(dx)
        if(dy >= 0) // dy>=0
        {
            if(dx>=dy) // 4/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                    TFTLCD_DrawPoint(x1,y1,color);
                    if(e>0){y1+=1;e-=dx;}
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 3/8 octant
            {
                e=dx-dy/2;
                while(y1<=y2)
                {
                    TFTLCD_DrawPoint(x1,y1,color);
                    if(e>0){x1-=1;e-=dy;}
                    y1+=1;
                    e+=dx;
                }
            }
        }
        else           // dy<0
        {
            dy=-dy;   // dy=abs(dy)


            if(dx>=dy) // 5/8 octant
            {
                e=dy-dx/2;
                while(x1>=x2)
                {
                    TFTLCD_DrawPoint(x1,y1,color);
                    if(e>0){y1-=1;e-=dx;}
                    x1-=1;
                    e+=dy;
                }
            }
            else        // 6/8 octant
            {
                e=dx-dy/2;
                while(y1>=y2)
                {
                    TFTLCD_DrawPoint(x1,y1,color);
                    if(e>0){x1-=1;e-=dy;}
                    y1-=1;
                    e+=dx;
                }
            }
        }
    }
}

//==============================================================
/*
 * 画三角
 */
//==============================================================
void TFTLCD_DrawTriangle(){
	TFTLCD_DrawHorizontalLine(70,200,128,GREEN);
	TFTLCD_DrawHorizontalLine(70,200,193,GREEN);
	TFTLCD_DrawVerticalLine(129,193,71,GREEN);
	TFTLCD_DrawVerticalLine(129,193,199,GREEN);
}

//==============================================================
/*
 * 画背景
 */
//==============================================================
void DrawBackground(void){
	uint32_t temp, i, j, m;
	m = 0;
	for(i=0; i<400; i++){
		for(j=0; j<240; j++){
			temp = gImage_1[m];
			temp |= gImage_1[m+1] << 8;
			TFTLCD_DrawPoint(j, i, temp);
			m++;
			m++;
		}
	}
}

//==============================================================
/*
 * 画首页背景
 */
//==============================================================
void DrawFirestBackground(void){
	uint32_t temp, i, j, m;
	m = 0;
	for(i=0; i<400; i++){
		for(j=0; j<240; j++){
			temp = shouye[m];
			temp |= shouye[m+1] << 8;
			TFTLCD_DrawPoint(j, i, temp);
			m++;
			m++;
		}
	}
}
//==============================================================
/*
 * 画图
 */
//==============================================================
void DrawPict(uint32_t x, uint32_t y,int p){
	uint32_t temp, i, j, m;
	m = 0;

	const unsigned char * image;
	switch(p){
		case 5:image=ka1;break;
		case 6:image=ka2;break;
		case 7:image=ka3;break;
		case 8:image=ka4;break;
		case 1:image=jay1;break;
		case 2:image=jay2;break;
		case 3:image=jay3;break;
		case 4:image=jay4;break;
	}
	for(i=y; i<y+100; i++){
		for(j=x; j<x+100; j++){
			temp = image[m];
			temp |= image[m+1] << 8;
			TFTLCD_DrawPoint(j, i, temp);
			m++;
			m++;
		}
	}
}

//==============================================================
/*
 * 画暂停键
 */
//==============================================================
void DrawPauseKey(uint32_t x, uint32_t y){
	uint32_t i;
	for(i=0; i<30; i++){
//		TFTLCD_DrawLine(y+i, x+i, y+50-i, x+i, WHITE);
		TFTLCD_DrawLine(x+i/2, y+i/2, x+i/2, y+30-i/2, WHITE);
	}
}

//==============================================================
/*
 * 画上一首键
 */
//==============================================================
void DrawLastKey(uint32_t x, uint32_t y){
	uint32_t i;
	for(i=0; i<24; i++){
		TFTLCD_DrawLine(x-i/2, y+i/2, x-i/2, y+24-i/2, WHITE);
	}
	TFTLCD_DrawLine(x-(i+1)/2, y, x-(i+1)/2, y+24, WHITE);
}

//==============================================================
/*
 * 画下一首键
 */
//==============================================================
void DrawNextKey(uint32_t x, uint32_t y){
	uint32_t i;
	for(i=0; i<24; i++){
		TFTLCD_DrawLine(x+i/2, y+i/2, x+i/2, y+24-i/2, WHITE);
	}
	TFTLCD_DrawLine(x+(i+1)/2, y, x+(i+1)/2, y+24, WHITE);
}

//==============================================================
/*
 * 画播放键
 */
//==============================================================
void DrawStartKey(uint32_t x, uint32_t y){
	uint32_t i;
	for(i=0; i<3; i++){
		TFTLCD_DrawLine(x+i, y, x+i, y+30, WHITE);
		TFTLCD_DrawLine(x+i+15, y, x+i+15, y+30, WHITE);
	}
}

//==============================================================
/*
 * 画播放键背景
 */
//==============================================================
void DrawKeyBack(void){
	int i,j;

	for(i=40;i<200;i++){
		for(j=290;j<340;j++){
			TFTLCD_DrawPoint(i,j,GRAYBLUE);
		}
	}
}
//==============================================================
/*
 * 清除播放键
 */
//==============================================================
void ClearStartKey(uint32_t x, uint32_t y){
	uint32_t i;
	for(i=0; i<3; i++){
		TFTLCD_DrawLine(x+i, y, x+i, y+30, WHITE);
		TFTLCD_DrawLine(x+i+15, y, x+i+15, y+30, GRAYBLUE);
	}
}

//==============================================================
/*
 * 清除暂停键
 */
//==============================================================
void ClearPauseKey(uint32_t x, uint32_t y){
	uint32_t i;
	for(i=0; i<30; i++){
//		TFTLCD_DrawLine(y+i, x+i, y+50-i, x+i, WHITE);
		TFTLCD_DrawLine(x+i/2, y+i/2, x+i/2, y+30-i/2, GRAYBLUE);
	}
}
void TFTLCD_ShowChar_Chinese(uint32_t x, uint32_t y, uint32_t index, uint32_t fColor, uint32_t bColor)
{
	uint32_t i,j;
	uint8_t m;

	TFTLCD_SetPos(x,x+16-1,y,y+16-1);                    //设置字符显示位置
	for(i=0; i<32;i++) {                             //循环写入16字节，一个字符为16字节
	  m=chinese[index][i];  //提取c字符的第i个字节
		for(j=0;j<8;j++) {                        //循环写入8位，一个字节为8位
			if((m&0x80)==0x80) {                  //判断最高位是否为1
				WriteData(fColor);           //最高位为1，写入字符颜色
				}
			else {
				WriteData(bColor);           //最高位为0，写入背景颜色
				;
				}
			m<<=1;                                //左移1位，准备写下一位
			}
		}
}
