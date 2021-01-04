/*
 * TFT_400x240_OTM4001A_16bit.h
 *
 *  Created on: 2014年8月26日
 *      Author: Sam
 */

#ifndef TFT_400X240_OTM4001A_16BIT_H_
#define TFT_400X240_OTM4001A_16BIT_H_

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000
#define RED           	 0x001F
#define BLUE         	 0xF800
#define GREEN         	 0x07E0
#define GBLUE			 0X07FF
#define PURPLE			 0XF01F //紫色
#define MAGENTA       	 0xF81F
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL)
#define LGRAYBLUE        0XA651 //浅灰蓝色
#define LBBLUE           0X2B12 //浅棕蓝色


extern void InitGPIOLCDInterface(uint32_t ui32ClockMS);
extern void TFT_400x240_OTM4001Ainit(uint32_t ui32SysClockSpeed);
extern void TFTLCD_REFRESH_GRAM(uint16_t ui16Data);
extern void TFTLCD_SET_COM(uint16_t ui16Data);
extern void TFTLCD_DrawPoint(unsigned int x,unsigned int y,unsigned int color);
extern void TFTLCD_ShowString(uint32_t x,uint32_t y,char *p,uint32_t fColor, uint32_t bColor);
extern void TFTLCD_CLEAR(uint32_t bColor);
extern void TFTLCD_EnterSleep();
extern void TFTLCD_ExitSleep();
extern void TFTLCD_FillBlock(uint32_t x0, uint32_t x1, uint32_t y0, uint32_t y1, uint32_t Color);
extern void TFTLCD_DrawHorizontalLine(uint32_t x0,uint32_t x1,uint32_t y,uint32_t color);
extern void TFTLCD_DrawVerticalLine(uint32_t y0,uint32_t y1,uint32_t x,uint32_t color);
extern void TFTLCD_ShowData(uint32_t x,uint32_t y,int32_t i32Data,uint32_t fColor, uint32_t bColor);
extern void TFTLCD_ShowPicture(uint32_t x,uint32_t y,uint8_t gImage[]);
extern void TFTLCD_DrawTriangle();
extern void DrawBackground(void);
extern void DrawFirestBackground(void);
extern void DrawPict(uint32_t x, uint32_t y,int p);
extern void DrawPauseKey(uint32_t x, uint32_t y);
extern void DrawLastKey(uint32_t x, uint32_t y);
extern void DrawNextKey(uint32_t x, uint32_t y);
extern void DrawStartKey(uint32_t x, uint32_t y);
extern void ClearStartKey(uint32_t x, uint32_t y);
extern void ClearPauseKey(uint32_t x, uint32_t y);
extern void DrawKeyBack(void);
extern void TFTLCD_ShowChar_Chinese(uint32_t x, uint32_t y, uint32_t index, uint32_t fColor, uint32_t bColor);
#endif /* TFT_400X240_OTM4001A_16BIT_H_ */
