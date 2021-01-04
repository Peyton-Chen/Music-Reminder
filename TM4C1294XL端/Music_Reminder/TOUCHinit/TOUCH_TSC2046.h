/*
 * TOUCH_TSC2046.h
 *
 *  Created on: 2014Äê8ÔÂ28ÈÕ
 *      Author: Sam
 */

#ifndef TOUCH_TSC2046_H_
#define TOUCH_TSC2046_H_

uint32_t GetData[6];

extern uint32_t TouchXData[6];
extern uint32_t TouchYData[6];

extern void TOUCH_TSC2046init(uint32_t ui32SysClockSpeed);
extern void SoftSPI_PutData(uint8_t ui8Data);
extern uint16_t SoftSPI_GetData();
extern uint16_t touch_gety();
extern uint16_t touch_getx();
extern void TOUCH_PointAdjust(uint32_t *TouchXData, uint32_t *TouchYData);
#endif /* TOUCH_TSC2046_H_ */
