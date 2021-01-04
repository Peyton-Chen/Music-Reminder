#ifndef I2C_H
#define I2C_H
//*********************************************************************
//*********************************************************************
//=============================================================================
/*
 *  米字管的参数、函数、数据传输
 */
//*********************************************************************
#define _NOP() _nop()

//*********************************************************************
//*********************************************************************
#define I2C0_MASTER_BASE 0x40020000
#define I2C0_SLAVE_BASE 0x40020000

//*********************************************************************
// 地址、寄存器等定义部分
//*********************************************************************
//*********************************************************************
//
// 设定slave（从）模块的地址。这是一个7-bit的地址加上RS位，具体形式如下:
//                      [A6:A5:A4:A3:A2:A1:A0:RS]
// RS位是一个指示位，如果RS=0，则说明是主发送数据，从接收数据；RS=1说明是主接收数据，从发送数据
//
//*********************************************************************
//U21控制4个米字管和特殊管脚的亮灭
#define I2C0_ADDR_TUBE_SEL	      0x30  //00110000
//U22控制米字管7~14管脚对应的码段
#define I2C0_ADDR_TUBE_SEG_LOW    0x32  //00110010
//U23控制米字管15~18管脚对应的码段
#define I2C0_ADDR_TUBE_SEG_HIGH  0x34   //00110100
//U24控制LED光柱

//PCA9557内部寄存器，也称子地址
#define PCA9557_REG_INPUT	 0x00
#define PCA9557_REG_OUTPUT	 0x01
#define PCA9557_REG_PolInver 0x02
#define PCA9557_REG_CONFIG	 0x03

//*************************************************************************************
 #define NUM 0

void I2C_Set_sda_high( void );
void I2C_Set_sda_low ( void );
void I2C_Set_scl_high( void );
void I2C_Set_scl_low ( void );
void I2C_STOP(void);
void I2C_Initial( void );
void I2C_START(void);
int  I2C_GetACK(void);
void I2C_SetNAk(void);
void I2C_TxByte(unsigned char nValue);
void i2c_write(int num, unsigned char device_addr,unsigned char *data);
void I2C0GPIOBEnable(void);
void I2C0DeviceInit(void);
void I2C0TubeSelSet(char data);
void I2C0TubeLowSet(char data);
void I2C0TubeHighSet(char data);
#endif
