/*
 * EPIinit.h
 *
 *  Created on: 2014-8-4
 *      Author: Sam
 */

#ifndef EPIINIT_H_
#define EPIINIT_H_
//*****************************************************************************
//
// Use the following to specify the GPIO pins used by the HB16 Mode EPI bus.
//
//*****************************************************************************
#define EPI_PORTA_PINS (GPIO_PIN_7 | GPIO_PIN_6)
#define EPI_PORTB_PINS (GPIO_PIN_3)
#define EPI_PORTC_PINS (GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4)
#define EPI_PORTG_PINS (GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTK_PINS (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTL_PINS (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTM_PINS (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTN_PINS (GPIO_PIN_3 | GPIO_PIN_2)


void EPIGPIOinit(void);


#endif /* EPIINIT_H_ */
