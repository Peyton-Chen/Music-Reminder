#include "i2c.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"


//IIC 接受数据临时缓冲区
unsigned char I2C_RECV_DATA[] =
				{
					0x00,
					0x00,
					0x00,
					0x00,
					0x00,
					0x00
				};

/*******************************************
		拉高 SDA 信号
********************************************/
void I2C_Set_sda_high( void )
{
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,GPIO_PIN_3);  //拉高PB3
    _NOP();
    _NOP();
    return;
}

/*******************************************
		拉低SDA 信号
********************************************/
void I2C_Set_sda_low ( void )
{
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3,0X00000000);  //拉低PB3
    _NOP();
    _NOP();
    return;
}

/*******************************************
		拉高SCL 信号
********************************************/
void I2C_Set_scl_high( void )
{
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,GPIO_PIN_2);  //拉高PB2
    _NOP();
    _NOP();
    return;
}

/*******************************************
		拉低SCL 信号
********************************************/
void I2C_Set_scl_low ( void )
{
	GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,0X00000000);  //拉低PB2
    _NOP();
    _NOP();
    return;
}

/*******************************************
		IIC 信号结束信号函数
********************************************/
void I2C_STOP(void)
{
    int i;
    I2C_Set_sda_low();
    for(i = NUM;i > 0;i--);
    I2C_Set_scl_low();
    for(i = NUM;i > 0;i--);
    I2C_Set_scl_high();
    for(i = NUM;i > 0;i--);
    I2C_Set_sda_high();
    for(i = NUM+1;i > 0;i--);
    return;
}


/*******************************************
		IIC 信号初始化
********************************************/
void I2C_Initial( void )
{
    I2C_Set_scl_low();
    I2C_STOP();
    return;
}


/*******************************************
		IIC 信号起始信号函数
********************************************/
void I2C_START(void)
{
    int i;

    I2C_Set_sda_high();
    for(i = NUM;i > 0;i--);
    I2C_Set_scl_high();
    for(i = NUM;i > 0;i--);
    I2C_Set_sda_low();
    for(i = NUM;i > 0;i--);
    I2C_Set_scl_low();
    return;
}

/*******************************************
		IIC 获取应答函数
********************************************/
int  I2C_GetACK(void)
{
    int j;
    _NOP();
    _NOP();
    I2C_Set_scl_low();
    for(j = NUM;j> 0;j--);
    I2C_Set_scl_high();
    for(j = NUM;j> 0;j--);
    I2C_Set_sda_low();
    for(j = NUM;j > 0;j--);
    I2C_Set_scl_low();
    return 1;
}

/*******************************************
		IIC 设置应答函数
********************************************/
void I2C_SetNAk(void)
{
    I2C_Set_scl_low();
    I2C_Set_sda_high();
    I2C_Set_scl_high();
    I2C_Set_scl_low();
    return;
}

/*******************************************
		IIC 发送字节函数
		参数 	1：要发送字节值
		return ：无返回
********************************************/
void I2C_TxByte(unsigned char nValue)
{
    int i;
    int j;
    for(i = 0;i < 8;i++)
    {
    	if(nValue & 0x80)
    	    I2C_Set_sda_high();
    	else
    	    I2C_Set_sda_low();
    	for(j = NUM;j > 0;j--);
    	I2C_Set_scl_high();
    	nValue <<= 1;
    	for(j = NUM;j > 0;j--);
    	I2C_Set_scl_low();
    }

    return;
}


/*******************************************
		IIC 发送数组函数
	参数  	1 num : 发送字节数
	    2 device_addr : iic目标地址
	    3 *data	：发送数组地址
	return ：无返回
********************************************/
void i2c_write(int num, unsigned char device_addr,unsigned char *data)
{
    int i = 0;
    int count = num;
    unsigned char *send_data = data;
    unsigned char write_addr = device_addr;

    I2C_Set_scl_high();
    for(i = NUM;i > 0;i--);
    I2C_Set_sda_high();
    for(i = NUM;i > 0;i--);

    for(i = 0;i < count;i++)
    {
      I2C_START();           //模拟I2C写数据的时序
      I2C_TxByte(write_addr);
      I2C_GetACK();
      I2C_TxByte(send_data[i]);
      I2C_GetACK();
      i++;
      I2C_TxByte(send_data[i]);
      I2C_GetACK();
      I2C_STOP();
    }

}

//*********************************************************************
//******配置I2C0模块的IO引脚，**********************************************
void I2C0GPIOBEnable(void)
{	// Enable GPIO portB containing the I2C pins (PB2&PB3)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2|GPIO_PIN_3);

}

//******配置PCA9557芯片中连接米字管的各引脚为输出***********************************
void I2C0DeviceInit(void)
{
	unsigned char dataBuf[2] = {PCA9557_REG_CONFIG, 0x00};
	i2c_write(2,I2C0_ADDR_TUBE_SEL,dataBuf);
	i2c_write(2,I2C0_ADDR_TUBE_SEG_LOW,dataBuf);
	i2c_write(2,I2C0_ADDR_TUBE_SEG_HIGH,dataBuf);

}


//*******设置米字管的管选信号**************************************************
void I2C0TubeSelSet(char data)
{   //选择1、2、3、4、5哪个米字管亮
	unsigned char dataBuf[2] = {PCA9557_REG_OUTPUT, data};
	i2c_write(2,I2C0_ADDR_TUBE_SEL,dataBuf);
}
//*******点亮米字管的相应码段**************************************************
void I2C0TubeLowSet(char data)
{  //点亮7-14管脚对应的码段
	unsigned char dataBuf[2] = {PCA9557_REG_OUTPUT, data};
	i2c_write(2,I2C0_ADDR_TUBE_SEG_LOW,dataBuf);
}
void I2C0TubeHighSet(char data)
{  //点亮15-18管脚对应的码段
	unsigned char dataBuf[2] = {PCA9557_REG_OUTPUT, data};
	i2c_write(2,I2C0_ADDR_TUBE_SEG_HIGH,dataBuf);
}
