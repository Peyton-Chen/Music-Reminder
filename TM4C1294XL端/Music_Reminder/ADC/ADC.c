#include "adc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
//=============================================================
/*
 * 配置ADC模块
 */
//=============================================================
void ADCConfig(void){

	// 初始化UART
//	InitConsole();

	// 初始化ADC0/PE3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_7);

	// 配置ADC采集序列
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH4 | ADC_CTL_END | ADC_CTL_IE);
	// 使能ADC采集序列
	ADCSequenceEnable(ADC0_BASE, 3);

	ADCIntClear(ADC0_BASE, 3);
}
