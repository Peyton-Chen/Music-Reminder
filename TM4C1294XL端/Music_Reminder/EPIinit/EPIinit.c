/*
 * EPIinit.c
 *
 *  Created on: 2014-8-4
 *      Author: Sam
 */

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_epi.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/epi.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "EPIinit.h"

//! - EPI0 peripheral
//! - GPIO Port A peripheral (for EPI0 pins)
//! - GPIO Port B peripheral (for EPI0 pins)
//! - GPIO Port C peripheral (for EPI0 pins)
//! - GPIO Port G peripheral (for EPI0 pins)
//! - GPIO Port K peripheral (for EPI0 pins)
//! - GPIO Port L peripheral (for EPI0 pins)
//! - GPIO Port M peripheral (for EPI0 pins)
//! - GPIO Port N peripheral (for EPI0 pins)
//! - EPI0S0  - PK0
//! - EPI0S1  - PK1
//! - EPI0S2  - PK2
//! - EPI0S3  - PK3
//! - EPI0S4  - PC7
//! - EPI0S5  - PC6
//! - EPI0S6  - PC5
//! - EPI0S7  - PC4
//! - EPI0S8  - PA6
//! - EPI0S9  - PA7
//! - EPI0S10 - PG1
//! - EPI0S11 - PG0
//! - EPI0S12 - PM3
//! - EPI0S13 - PM2
//! - EPI0S14 - PM1
//! - EPI0S15 - PM0
//! - EPI0S16 - PL0
//! - EPI0S17 - PL1
//! - EPI0S18 - PL2
//! - EPI0S19 - PL3
//! - EPI0S28 - PB3
//! - EPI0S29 - PN2
//! - EPI0S30 - PN3
//! - EPI0S31 - PK5

#define EPI_PORTA_PINS (GPIO_PIN_7 | GPIO_PIN_6)
#define EPI_PORTB_PINS (GPIO_PIN_3)
#define EPI_PORTC_PINS (GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4)
#define EPI_PORTG_PINS (GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTK_PINS (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTL_PINS (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTM_PINS (GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0)
#define EPI_PORTN_PINS (GPIO_PIN_3 | GPIO_PIN_2)



void EPIGPIOinit()
{
	uint32_t ui32Val;
    //
    // The EPI0 peripheral must be enabled for use.
    //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EPI0);

    //
    // For this example EPI0 is used with multiple pins on PortA,C,G,K,M
    // The actual port and pins used may be different on your
    // part, consult the data sheet for more information.
    // TODO: Update based upon the EPI pin assignment on your target part.
    //

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);

    //
    // This step configures the internal pin muxes to set the EPI pins for use
    // with EPI.  Please refer to the datasheet for more information about pin
    // muxing.
    // implementation.
    // TODO: Update this section based upon the EPI pin assignment on your
    // target part.
    //

    //
    // EPI0S4 ~ EPI0S7: C4 ~ 7
    //
    ui32Val = HWREG(GPIO_PORTC_BASE + GPIO_O_PCTL);
    ui32Val &= 0x0000FFFF;
    ui32Val |= 0xFFFF0000;
    HWREG(GPIO_PORTC_BASE + GPIO_O_PCTL) = ui32Val;

    //
    // EPI0S00 ~ EPI0S03: K0 ~ 3
    //
    ui32Val = HWREG(GPIO_PORTK_BASE + GPIO_O_PCTL);
    ui32Val &= 0xFFFF0000;
    ui32Val |= 0x0000FFFF;
    HWREG(GPIO_PORTK_BASE + GPIO_O_PCTL) = ui32Val;

    //
    // EPI0S8 ~ EPI0S9: A6 ~ 7
    //
    ui32Val = HWREG(GPIO_PORTA_BASE + GPIO_O_PCTL);
    ui32Val &= 0x00FFFFFF;
    ui32Val |= 0xFF000000;
    HWREG(GPIO_PORTA_BASE + GPIO_O_PCTL) = ui32Val;

    //
    // EPI0S10 ~ EPI0S11: G0 ~ 1
    //
    ui32Val = HWREG(GPIO_PORTG_BASE + GPIO_O_PCTL);
    ui32Val &= 0xFFFFFF00;
    ui32Val |= 0x000000FF;
    HWREG(GPIO_PORTG_BASE + GPIO_O_PCTL) = ui32Val;

    //
    // EPI0S12 ~ EPI0S15: M0 ~ 3
    //
    ui32Val = HWREG(GPIO_PORTM_BASE + GPIO_O_PCTL);
    ui32Val &= 0xFFFF0000;
    ui32Val |= 0x0000FFFF;
    HWREG(GPIO_PORTM_BASE + GPIO_O_PCTL) = ui32Val;


    //
    // Configure the GPIO pins for EPI mode.  All the EPI pins require 8mA
    // drive strength in push-pull operation.  This step also gives control of
    // pins to the EPI module.
    //
    GPIOPinTypeEPI(GPIO_PORTA_BASE, EPI_PORTA_PINS);
    GPIOPinTypeEPI(GPIO_PORTC_BASE, EPI_PORTC_PINS);
    GPIOPinTypeEPI(GPIO_PORTG_BASE, EPI_PORTG_PINS);
    GPIOPinTypeEPI(GPIO_PORTK_BASE, EPI_PORTK_PINS);
    GPIOPinTypeEPI(GPIO_PORTM_BASE, EPI_PORTM_PINS);

    //
    // Sets the usage mode of the EPI module.  For this example we will use
    // the HB16 mode.
    //
    EPIModeSet(EPI0_BASE, EPI_MODE_HB16);

    //
    // Configure the HB16 mode. We will use the normal (or full power) operating
    // state which means we will not use the low power self-refresh state.
    //
    EPIConfigHB16Set(EPI0_BASE,EPI_HB16_MODE_ADDEMUX | EPI_HB16_RDWAIT_2 | EPI_HB16_WRWAIT_2 | EPI_HB16_CSCFG_CS,0);
    EPIAddressMapSet(EPI0_BASE, EPI_ADDR_PER_SIZE_256B | EPI_ADDR_PER_BASE_A);
}
