/*
 * main.c
 *  Created on: 2020��12��21��
 *      Author: cpt
 */

// �⵼��
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_epi.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/epi.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/fpu.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "grlib/grlib.h"
#include "utils/uartstdio.h"
#include "TFTinit/TFT_400x240_OTM4001A_16bit.h"
#include "TOUCHinit/TOUCH_TSC2046.h"
#include "EPIinit/EPIinit.h"
#include "I2C/i2c.h"
#include "Tool/KalmanFilter.h"
#include "ADC/ADC.h"

#define M_PI 3.14159265358979323846F	// ����pi��ֵ
#define TICKS_PER_SECOND 1000			// ���򻯼�����
//*****************************************************************************
uint32_t g_ui32SysClock;		// ϵͳʱ��
uint32_t ulADC0_Value;			// ���ڴ�����ֵ�ѹֵ

int cur_music = 1;				// ��ǰ�������ֵı��
volatile unsigned int count = 0;	// �����ж�ͼƬѡ��Ƕ�
volatile unsigned int Timernum = 0;		// �������ֹܼ���
bool pauseflag = false; // ��¼ֹͣ���ǿ���
extern uint32_t GetData[6];


bool Lock = true;	// �ж��Ƿ�����
uint32_t Finger=0;	// ��Ƭ�����ص���Ϣ

uint32_t TouchXData[6];		// ��Ŵ���λ�ã�����ֵ�˲�
uint32_t TouchYData[6];

//=============================================================
/*
 * ��ˮ��
 */
//=============================================================
void liushui(void){

	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1); //PF1����ߣ�����LED0
	SysCtlDelay(1000*(10000000/3000));         //��ʱn*1ms
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0); //PF1����ͣ��ر�LED0

	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0xff); //PF2����ߣ�����LED1
	SysCtlDelay(1000*(10000000/3000));         //��ʱn*1ms
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0); //PF2����ͣ�����LED1

	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);
	SysCtlDelay(1000*(10000000/3000));         //��ʱn*1ms
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);

	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,GPIO_PIN_0);
	SysCtlDelay(1000*(10000000/3000));         //��ʱn*1ms
	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_0,0);

	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,GPIO_PIN_1);
	SysCtlDelay(1000*(10000000/3000));         //��ʱn*1ms
	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_1,0);

	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,GPIO_PIN_2);
	SysCtlDelay(1000*(10000000/3000));         //��ʱn*1ms
	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_2,0);

	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,GPIO_PIN_3);
	SysCtlDelay(1000*(10000000/3000));         //��ʱn*1ms
	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_3,0);

	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_4,GPIO_PIN_4);
	SysCtlDelay(1000*(10000000/3000));         //��ʱn*1ms
	GPIOPinWrite(GPIO_PORTL_BASE,GPIO_PIN_4,0);
}

//*****************************************************************************
//
// ϵͳUART��ʼ��
//
//*****************************************************************************
void ConfigureUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTStdioConfig(0, 115200, g_ui32SysClock);
}


//=============================================================
/*
 * MP3ģ���UART��ʼ������
 */
//=============================================================
void UART_initial(void){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

	GPIOPinConfigure(GPIO_PP0_U6RX);
	GPIOPinConfigure(GPIO_PP1_U6TX);

	GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART6_BASE, g_ui32SysClock, 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}

//============================================================
/*
 * ������ʼ������
 */
//============================================================
void GPIOInitial(void){
	// GPIOD�ܽų�ʼ������
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);

	// GPIOD�ж�����
//	GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_LOW_LEVEL);
//	GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_0);

	// GPION�ܽų�ʼ������
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPION);
	GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_2 | GPIO_PIN_3);

	// GPIOP�ܽų�ʼ������
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOP);
	GPIOPinTypeGPIOInput(GPIO_PORTP_BASE, GPIO_PIN_2);

	// GPIOK�ܽų�ʼ������
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
//	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOK);
//	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
//	GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_3);

	// GPIOM�ܽų�ʼ������
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOM);
//	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_5);
	GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_5);


	// GPION�ж�����
//	GPIOIntTypeSet(GPIO_PORTN_BASE, GPIO_PIN_2|GPIO_PIN_3, GPIO_LOW_LEVEL);
//	GPIOIntEnable(GPIO_PORTN_BASE, GPIO_INT_PIN_2 | GPIO_INT_PIN_3);

	// ��ˮ��GPIO��ʼ��
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);//ʹ��GPIOF
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);//ʹ��GPIOL
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);//����PF1��PF2��PF3Ϊ���
	GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);//����PL0��PL1��PL2��PL3��PL4Ϊ���

}

//=============================================================
/*
 * ��ʼ���������ж�
 */
//=============================================================
void TimerIntInitial(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_PIOSC);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR  | TIMER_CFG_B_PERIODIC | TIMER_CFG_A_PERIODIC);//

	TimerLoadSet(TIMER0_BASE, TIMER_B, g_ui32SysClock/TICKS_PER_SECOND ); // set period 1/SysClock * SysClock/1000 = 1/1000s=1ms
	TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock/5000 );

	TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

//=============================================================
/*
 * �������жϺ���--�������ֹ���ʾ
 */
//=============================================================
void Timer0BIntHandler(void){
//	if(Lock==false){
	unsigned long Status;
	TimerDisable(TIMER0_BASE, TIMER_B);
	Status = TimerIntStatus(TIMER0_BASE, true);
	// ���һλ
	if(Status == TIMER_TIMB_TIMEOUT){

		Timernum++;
    	I2C0TubeSelSet(~0x10);I2C0TubeLowSet(0x82);I2C0TubeHighSet(0x00);SysCtlDelay(2000);
    	I2C0TubeSelSet(0xff);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		switch((Timernum/100) % 10){
		case 1: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x22);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 2: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x2c);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 3: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x26);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 4: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x60);I2C0TubeHighSet(0x32);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 5: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x16);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 6: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x1e);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 7: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x26);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 8: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x3e);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 9: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x36);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		default: I2C0TubeSelSet(0x36);I2C0TubeLowSet(0x10);I2C0TubeHighSet(0x3e);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		}
		// �����ڶ�λ
		switch((Timernum/1000) % 6){
		case 1: I2C0TubeSelSet(0x3a);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x22);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 2: I2C0TubeSelSet(0x3a);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x2c);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 3: I2C0TubeSelSet(0x3a);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x26);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 4: I2C0TubeSelSet(0x3a);I2C0TubeLowSet(0x60);I2C0TubeHighSet(0x32);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 5: I2C0TubeSelSet(0x3a);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x16);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		default: I2C0TubeSelSet(0x3a);I2C0TubeLowSet(0x10);I2C0TubeHighSet(0x3e);SysCtlDelay(20000);
				 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		}
		// �ڶ�λ
		switch((Timernum/6000) % 10){
		case 1: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x22);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 2: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x2c);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 3: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x26);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 4: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x60);I2C0TubeHighSet(0x32);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 5: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x16);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 6: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x1e);SysCtlDelay(20000);
				I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 7: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x26);SysCtlDelay(20000);
		 	 	I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 8: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x3e);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 9: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x36);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		default: I2C0TubeSelSet(0x3c);I2C0TubeLowSet(0x10);I2C0TubeHighSet(0x3e);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		}
		// ��һλ
		switch((Timernum/60000) % 6){
		case 1: I2C0TubeSelSet(0x1f);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x22);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 2: I2C0TubeSelSet(0x1f);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x2c);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 3: I2C0TubeSelSet(0x1f);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x26);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 4: I2C0TubeSelSet(0x1f);I2C0TubeLowSet(0x60);I2C0TubeHighSet(0x32);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		case 5: I2C0TubeSelSet(0x1f);I2C0TubeLowSet(0x70);I2C0TubeHighSet(0x16);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		break;
		default: I2C0TubeSelSet(0x1f);I2C0TubeLowSet(0x10);I2C0TubeHighSet(0x3e);SysCtlDelay(20000);
		 I2C0TubeSelSet(0xFF);I2C0TubeLowSet(0x00);I2C0TubeHighSet(0x00);
		}
		if(Timernum == 360000){
			Timernum = 0;
		}
	}

	TimerIntClear(TIMER0_BASE, Status);
	TimerLoadSet(TIMER0_BASE, TIMER_B, g_ui32SysClock/TICKS_PER_SECOND);
	TimerEnable(TIMER0_BASE, TIMER_B);
//	}
}


//===============================================================
/*
 * �жϿ���ͼƬ��ת
 */
//===============================================================
void Timer0AIntHandler(void){
//	if(Lock==false){
	unsigned long Status;
	TimerDisable(TIMER0_BASE, TIMER_A);
	Status = TimerIntStatus(TIMER0_BASE, true);

	if(Status == TIMER_TIMA_TIMEOUT){

	    switch(cur_music){
	    case 1:
	        switch(count / 3){
            case 0: DrawPict(70, 80, 1);break;
            case 1: DrawPict(70, 80, 2);break;
            case 2: DrawPict(70, 80, 3);break;
            case 3: DrawPict(70, 80, 4);break;
            default:
	        }
	        break;
	    case 2:
	        switch(count / 3){
	        case 0: DrawPict(70, 80, 5);break;
	        case 1: DrawPict(70, 80, 6);break;
	        case 2: DrawPict(70, 80, 7);break;
	        case 3: DrawPict(70, 80, 8);break;
	        default:
	        }
	        break;
	    default: break;
	    }

		count++;
		if(count == 12){
			count = 0;
		}
	}
	TimerIntClear(TIMER0_BASE, Status);
	TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock/5000);
	TimerEnable(TIMER0_BASE, TIMER_A);

	TOUCH_PointAdjust(&TouchXData[5], &TouchYData[5]);
	TOUCH_PressKey(&TouchXData[5], &TouchYData[5]);

	identify_key();
//	}
}

//==============================================================
/*
 * ������Ӧ�ļ�ʱִ�в�ͬ�Ĺ���
 */
//==============================================================
void TOUCH_PressKey(uint32_t* TouchXData, uint32_t* TouchYData){

	if(*TouchYData >= 290 && *TouchYData <= 350){
		// ����ͣ��
		if(*TouchXData >= 100 && *TouchXData <= 150){
			if(pauseflag){
				TimerEnable(TIMER0_BASE, TIMER_B);
				TimerEnable(TIMER0_BASE, TIMER_A);
				// ��������
				GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x20);
				SysCtlDelay(100000);
				GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x00);

				play_music(1);
				ClearPauseKey(115, 300);
				DrawStartKey(115, 300);
//				liushui();
			}
			else{
				TimerDisable(TIMER0_BASE, TIMER_B);
				TimerDisable(TIMER0_BASE, TIMER_A);
				// ��������
				GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x20);
				SysCtlDelay(200000);
				GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x00);

//				stop_music();
				play_music(2);
				ClearStartKey(115, 300);
				DrawPauseKey(115,300);
//				liushui();
			}
			pauseflag = !pauseflag;
			return;
		}
		// ��ǰһ�׸�İ�ť
		if(*TouchXData >= 40 && *TouchXData <= 85){
		    if(cur_music > 1){
		        cur_music --;
		        // ��������
		        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x20);
		        SysCtlDelay(200000);
                GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x00);

                Timernum = 0;
                play_music(3);
                liushui();
		    }

			return;
		}
		// ����һ�׸�İ�ť
		if(*TouchXData >= 165 && *TouchXData <= 180){
		    if(cur_music < 2){
		        cur_music ++;
		        // ��������
                GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x20);
                SysCtlDelay(200000);
                GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x00);
                Timernum = 0;
                play_music(4);
                liushui();
		    }
		    return;
		}
	}
}

//=================================================================
/*
 * �������ж�
 */
//=================================================================
void identify_key(){

	// ��һ�ײ��ż� GPIO PN2��
	if((GPIOPinRead(GPIO_PORTP_BASE, GPIO_PIN_2) & 0x04) == 0x00){
		SysCtlDelay(20000);
		if((GPIOPinRead(GPIO_PORTP_BASE, GPIO_PIN_2) & 0x04) == 0x00){
		    if(cur_music > 1){
		        cur_music--;
		        Timernum = 0;
		        play_music(3);

		    	liushui();
		    }

			return ;
		}
	}
	// ��һ�ײ��ż�GPIO PD0��
	else if((GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) & 0x01) == 0x00){
		SysCtlDelay(20000);
		if((GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) & 0x01) == 0x00){
		    if(cur_music < 2){
		        cur_music++;
		        Timernum = 0;
		        play_music(4);

		    	liushui();
		    }
			return ;
		}
	}
	// ������
	if((GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_2) & 0x04) == 0x00){
		SysCtlDelay(20000);
		if((GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_2) & 0x04) == 0x00){

		    liushui();
		    UARTCharPut(UART6_BASE, 0x03);
			return ;
		}
	}
	// ������
	if((GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_3) & 0x08) == 0x00){
		SysCtlDelay(20000);
		if((GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_3) & 0x08) == 0x00){

			liushui();
			UARTCharPut(UART6_BASE, 0x04);
			return ;
		}
	}
}

//======================================================================
/*
 * ����MP3
 */
//======================================================================
void play_music(int number){

    int i;
	switch(number){
	case 1:
	    // ����
		UARTCharPut(UART6_BASE, 0x01);
		break;
	case 2:
	    // ��ͣ
		UARTCharPut(UART6_BASE, 0x02);
		break;
	case 3:
	    // ��һ��
		UARTCharPut(UART6_BASE, 0x06);
		break;
	case 4:
	    // ��һ��
		UARTCharPut(UART6_BASE, 0x05);
	    break;
	}
}

void SysTickIntHandler(void){;}

void main()
{
    //====================================================
    // ��ʱ��������
	uint16_t ui32Loop = 0,temp=0;

	//====================================================
	// FPU����
    FPUEnable();
    FPULazyStackingEnable();

    //=====================================================
    // ����ʱ��
    g_ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                SYSCTL_CFG_VCO_480), 120000000);
    //======================================================
    // ���ֹܵ�����
	I2C0GPIOBEnable();//����I2C0ģ���IO����
	I2C0DeviceInit();//����PCA9557оƬ���������ֹܵĸ�����Ϊ���

	//======================================================
	// ��ʼ���ж�
	GPIOInitial();

    //======================================================
    // ���ö�ʱ���ж�
    SysTickPeriodSet(g_ui32SysClock / 10000);
    TimerIntInitial();

    //======================================================
    // ����UART�Լ�MP3��Ӧ��UARTģ��
    ConfigureUART();
    UART_initial();

    //=======================================================
    // ��ʾ����ʼ���Լ����ô���
    EPIGPIOinit();
    TOUCH_TSC2046init(g_ui32SysClock);

    GPIOIntEnable(GPIO_PORTB_BASE,GPIO_INT_PIN_0);
    GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_0,GPIO_FALLING_EDGE);

    TFT_400x240_OTM4001Ainit(g_ui32SysClock);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);

	SSIDataPut(SSI0_BASE,0xd0);

	//====================================================
	// ����ʾ���ϻ�����ҳ�Լ���ʾ����
	DrawFirestBackground();
	TFTLCD_ShowChar_Chinese(80,270,0,RED,WHITE);
	TFTLCD_ShowChar_Chinese(80+16,270,1,RED,WHITE);
	TFTLCD_ShowChar_Chinese(80+32,270,2,RED,WHITE);
	TFTLCD_ShowChar_Chinese(80+48,270,3,RED,WHITE);
	TFTLCD_ShowChar_Chinese(80+64,270,4,RED,WHITE);

    //======================================================
    while(1){
    	Finger = UARTCharGet(UART6_BASE);
    	if(Finger==1){
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x20);
			SysCtlDelay(100000);
			GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_5, 0x00);

    		SysCtlDelay(20000);
    	    TFTLCD_CLEAR(BLACK);
    		SysCtlDelay(20000);
    	    DrawBackground();


    		DrawKeyBack();
    		DrawStartKey(115, 300);
    		DrawLastKey(65, 303);
    		DrawNextKey(175, 303);

    		Lock = false;
    		break;
    	}
    }

    //======================================================
    // ��ʼ��������
    //======================================================
    // �����ж����ȼ��Լ�������ʱ���ж�
    IntPrioritySet(INT_TIMER0B, 0x80);
    IntPrioritySet(INT_TIMER0A, 0xe0);
    IntEnable(INT_TIMER0B);
    IntEnable(INT_TIMER0A);
    IntMasterEnable();
    SysTickIntEnable();
    TimerEnable(TIMER0_BASE, TIMER_B);
    TimerEnable(TIMER0_BASE, TIMER_A);

    play_music(1);
    while(1)
    {
    	temp++;	// ����
    	//=========================================================
    	// ��ȡ������λ��
    	for(ui32Loop=0;ui32Loop<=5;ui32Loop++)
    	{
			SSIDataPut(SSI0_BASE,0x90);
			SysCtlDelay(3);
			SSIDataGet(SSI0_BASE,&TouchXData[ui32Loop]);
			SysCtlDelay(3);
			SSIDataPut(SSI0_BASE,0xd0);
			SysCtlDelay(3);
			SSIDataGet(SSI0_BASE,&TouchYData[ui32Loop]);
			SysCtlDelay(3);
		}
		TouchXData[5] = (TouchXData[0]+TouchXData[1]+TouchXData[2]+TouchXData[3]+TouchXData[4])/5;
		TouchYData[5] = (TouchYData[0]+TouchYData[1]+TouchYData[2]+TouchYData[3]+TouchYData[4])/5;

		if(temp >= 150){
		    TOUCH_PointAdjust(&TouchXData[5], &TouchYData[5]);
		    TOUCH_PressKey(&TouchXData[5], &TouchYData[5]);
		}

		//============================================================
		// �жϴ���λ��
		if(temp >= 150){
		    identify_key();
		    temp = 0;
		}
    }
}
