/*
 * TOUCH_TSC2046.c
 *
 *  Created on: 2014Äê8ÔÂ28ÈÕ
 *      Author: Sam
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "TFTinit/TFT_400x240_OTM4001A_16bit.h"
#include "TOUCHinit/TOUCH_TSC2046.h"

//
// TSC2046 control line GPIO definitions.
//
#define TOUCH_BUSY_PERIPH           	SYSCTL_PERIPH_GPIOB
#define TOUCH_BUSY_BASE            		GPIO_PORTB_BASE
#define TOUCH_BUSY_PIN              	GPIO_PIN_1
#define TOUCH_BUSY_PIN_REG           	0x40005008
#define TOUCH_PEN_PERIPH           		SYSCTL_PERIPH_GPIOB
#define TOUCH_PEN_BASE             		GPIO_PORTB_BASE
#define TOUCH_PEN_PIN                   GPIO_PIN_0
#define TOUCH_PEN_PIN_REG           	0x40005004

#define SOFTSPI_FSS_PERIPH		      	SYSCTL_PERIPH_GPIOA
#define SOFTSPI_FSS_BASE				GPIO_PORTA_BASE
#define SOFTSPI_FSS_PIN					GPIO_PIN_3
#define SOFTSPI_FSS_PIN_REG				0x40004020
#define SOFTSPI_CLK_PERIPH		      	SYSCTL_PERIPH_GPIOA
#define SOFTSPI_CLK_BASE				GPIO_PORTA_BASE
#define SOFTSPI_CLK_PIN					GPIO_PIN_2
#define SOFTSPI_CLK_PIN_REG				0x40004010
#define SOFTSPI_TX_PERIPH		      	SYSCTL_PERIPH_GPIOA
#define SOFTSPI_TX_BASE					GPIO_PORTA_BASE
#define SOFTSPI_TX_PIN					GPIO_PIN_4
#define SOFTSPI_TX_PIN_REG				0x40004040
#define SOFTSPI_RX_PERIPH		      	SYSCTL_PERIPH_GPIOA
#define SOFTSPI_RX_BASE					GPIO_PORTA_BASE
#define SOFTSPI_RX_PIN					GPIO_PIN_5
#define SOFTSPI_RX_PIN_REG				0x40004080

#define TOUCH_XMIN  261
#define TOUCH_XMAX  3820
#define TOUCH_YMIN  200
#define TOUCH_YMAX  3820

#define TFT_XSIZE   240
#define TFT_YSIZE   400


uint16_t temp = 0;

void SSIinit(uint32_t ui32SysClockSpeed)
{
	uint32_t g_pui32DataRx[3];
    //
    // The SSI0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    //
    // For this example SSI0 is used with PortA[5:2].  The actual port and pins
    // used may be different on your part, consult the data sheet for more
    // information.  GPIO port A needs to be enabled so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for SSI0 functions on port A2, A3, A4, and A5.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // The pins are assigned as follows:
    //      PA5 - SSI0Rx
    //      PA4 - SSI0Tx
    //      PA3 - SSI0Fss
    //      PA2 - SSI0CLK
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2);

    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI0,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
    SSIConfigSetExpClk(SSI0_BASE, ui32SysClockSpeed, SSI_FRF_NMW,
                       SSI_MODE_MASTER, 2000000, 12);

    //
    // Enable the SSI0 module.
    //
    SSIEnable(SSI0_BASE);

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //
    while(SSIDataGetNonBlocking(SSI0_BASE, &g_pui32DataRx[0]))
    {
    }

}


void SoftSPIinit()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //SPI
    GPIOPinTypeGPIOOutput(SOFTSPI_FSS_BASE, SOFTSPI_FSS_PIN);
    GPIOPinTypeGPIOOutput(SOFTSPI_CLK_BASE, SOFTSPI_CLK_PIN);
    GPIOPinTypeGPIOOutput(SOFTSPI_TX_BASE, SOFTSPI_TX_PIN);
//    GPIOPinTypeGPIOOutput(SOFTSPI_RX_BASE, SOFTSPI_RX_PIN);
    GPIOPinTypeGPIOInput(SOFTSPI_RX_BASE, SOFTSPI_RX_PIN);
}


void TouchInit()
{
	HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
	SysCtlDelay(8);
	HWREG(SOFTSPI_FSS_PIN_REG) = 0xFF;
	SysCtlDelay(8);
	HWREG(SOFTSPI_CLK_PIN_REG) = 0xFF;
	SysCtlDelay(8);
	HWREG(SOFTSPI_FSS_PIN_REG) = 0x00;
	SysCtlDelay(8);
}


// Put a data ui8Data out from SOFTSPI_TX_PIN.
void SoftSPI_PutData(uint8_t ui8Data)
{
	uint8_t i,buf=0;
	buf = ui8Data;
	HWREG(SOFTSPI_FSS_PIN_REG) = 0x00;
	SysCtlDelay(3);
	HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
	for(i=0;i<8;i++)
	{
		if(buf & 0x80)
		{
			HWREG(SOFTSPI_TX_PIN_REG) = 0xFF;
		}
		else
		{
			HWREG(SOFTSPI_TX_PIN_REG) = 0x00;
		}
		HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
		SysCtlDelay(8);
		HWREG(SOFTSPI_CLK_PIN_REG) = 0xFF;
		buf<<=1;
	}
	SysCtlDelay(3);
//	HWREG(SOFTSPI_FSS_PIN_REG) = 0xFF;
}

//Get a data ui8Data from SOFTSPI_RX_PIN.
uint16_t SoftSPI_GetData()
{
	uint16_t i,buf=0;
	HWREG(SOFTSPI_FSS_PIN_REG) = 0x00;
	SysCtlDelay(3);
	HWREG(SOFTSPI_CLK_PIN_REG) = 0xFF;
	SysCtlDelay(3);
	HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
	SysCtlDelay(3);
	for(i=0;i<12;i++)
	{
		if(GPIOPinRead(SOFTSPI_RX_BASE,SOFTSPI_RX_PIN) & 0x20) //Test if DOUT==1.Change 0x20 to different pins.
		{
			buf += 1;
		}
		HWREG(SOFTSPI_CLK_PIN_REG) = 0xFF;
		SysCtlDelay(10);
		HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
		buf<<=1;
	}
	SysCtlDelay(3);
	HWREG(SOFTSPI_FSS_PIN_REG) = 0xFF;
	return buf;

}

uint16_t touch_getx()
{
	uint16_t i,buf=0;
	TouchInit();
	SoftSPI_PutData(0x90);
	HWREG(SOFTSPI_FSS_PIN_REG) = 0;
	HWREG(SOFTSPI_CLK_PIN_REG) = 0xFF;
	SysCtlDelay(3);
	HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
	SysCtlDelay(3);
	for(i=0;i<12;i++)
	{
		if(GPIOPinRead(SOFTSPI_RX_BASE,SOFTSPI_RX_PIN) & 0x20) //Test if DOUT==1.Change 0x20 to different pins.
		{
			buf += 1;
		}
		HWREG(SOFTSPI_CLK_PIN_REG) = 0xFF;
		SysCtlDelay(8);
		HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
		buf<<=1;
	}
	HWREG(SOFTSPI_FSS_PIN_REG) = 0xFF;
	return buf;
}


uint16_t touch_gety()
{
	uint16_t i,buf=0;
	TouchInit();
	SoftSPI_PutData(0xd0);
	HWREG(SOFTSPI_FSS_PIN_REG) = 0;
	HWREG(SOFTSPI_CLK_PIN_REG) = 0xFF;
	SysCtlDelay(3);
	HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
	SysCtlDelay(3);
	for(i=0;i<12;i++)
	{
		if(GPIOPinRead(SOFTSPI_RX_BASE,SOFTSPI_RX_PIN) & 0x20) //Test if DOUT==1.Change 0x20 to different pins.
		{
			buf += 1;
		}
		HWREG(SOFTSPI_CLK_PIN_REG) = 0xFF;
		SysCtlDelay(8);
		HWREG(SOFTSPI_CLK_PIN_REG) = 0x00;
		buf<<=1;
	}
	HWREG(SOFTSPI_FSS_PIN_REG) = 0xFF;
	return buf;
}


// As TSC2046 DIN data are latched on the rising edge of DCLK
// while DOUT are shifted on the falling edge of DCLK
// we adopt software simulation SPI to transmit data.
void TOUCH_TSC2046init(uint32_t ui32SysClockSpeed)
{
    //
    // Get the current processor clock frequency.
    //
    //
    // Enable the GPIO peripherals used to interface to the TSC2046.
    //
    SysCtlPeripheralEnable(TOUCH_BUSY_PERIPH);
    SysCtlPeripheralEnable(TOUCH_PEN_PERIPH);
    GPIOPinTypeGPIOInput(TOUCH_BUSY_BASE,TOUCH_BUSY_PIN);
    GPIOPinTypeGPIOInput(TOUCH_PEN_BASE,TOUCH_PEN_PIN);
    //Enable soft SPI pins.
//    SoftSPIinit();
    SSIinit(ui32SysClockSpeed);

}



void TOUCH_PointAdjust(uint32_t *TouchXData, uint32_t *TouchYData)
{
	uint32_t x=0,y=0;
	if(*TouchXData<TOUCH_XMIN||*TouchXData>TOUCH_XMAX)
	{
		*TouchXData = 0xFFFF;
		return;
	}
	if(*TouchYData<TOUCH_YMIN||*TouchYData>TOUCH_YMAX)
	{
		*TouchYData = 0xFFFF;
		return;
	}
	x = *TouchXData;
	y = *TouchYData;
	*TouchXData = TFT_XSIZE*x/(TOUCH_XMAX-TOUCH_XMIN)-15;
	*TouchYData =TFT_YSIZE-(TFT_YSIZE*y/(TOUCH_YMAX-TOUCH_YMIN))+24;
//	*TouchXData = (TFT_XSIZE / 10 * x / 3559);
//	*TouchYData = TFT_YSIZE/10 -( TFT_YSIZE / 10 * y / 3620);
//	*TouchXData *= 10;
//	*TouchYData *= 10;
//	*TouchXData -= 15;
//	*TouchYData += 24;

}

void IntGPIOb()
{
	;
//	uint8_t i;
//	GetData = SoftSPI_GetData();
//	GetData[0] = touch_getx();
//	SoftSPI_PutData(0x90);
//	GetData[0] = SoftSPI_GetData();
//	SoftSPI_PutData(0xD0);
//	GetData[1] = SoftSPI_GetData();
//	TFTLCD_ShowData(100,300,GetData[0],BLACK,WHITE);
//	TFTLCD_ShowData(100,320,GetData[1],BLACK,WHITE);
/*
	IntDisable(INT_GPIOB);
	for(i=0;i<=5;i++)
	{
		SSIDataPut(SSI0_BASE,0xd0);
//		SSIDataGet(SSI0_BASE,&GetData[0]);
//		SSIDataGet(SSI0_BASE,&GetData[1]);
		SSIDataGet(SSI0_BASE,&TouchXData[i]);
//		GetData[2] = (GetData[0]<<8);
//		GetData[2] |= GetData[1];
//		GetData[2] >>= 3;
		SSIDataPut(SSI0_BASE,0x90);
//		SSIDataGet(SSI0_BASE,&GetData[3]);
//		SSIDataGet(SSI0_BASE,&GetData[4]);
		SSIDataGet(SSI0_BASE,&TouchYData[i]);
	}
	TouchXData[5] = (TouchXData[0]+TouchXData[1]+TouchXData[2]+TouchXData[3]+TouchXData[4])/5;
	TouchYData[5] = (TouchYData[0]+TouchYData[1]+TouchYData[2]+TouchYData[3]+TouchYData[4])/5;
	TOUCH_PointAdjust(&TouchXData[5], &TouchYData[5]);


	SysCtlDelay(3);
	IntEnable(INT_GPIOB);
*/
//	IntDisable(INT_GPIOB);
//	GetData[0] = touch_getx();
//	GetData[1] = touch_gety();
//	TOUCH_PointAdjust(&GetData[0], &GetData[1]);
//	if(temp >= 10){
//		if(GetData[0] >= 70 && GetData[0] <= 200){
//			if(GetData[1] >= 129 && GetData[1] <= 193){
//				TFTLCD_DrawCircle();
//			}
//		}
//	}
//
//	SysCtlDelay(3);
//	IntEnable(INT_GPIOB);
}







