#include "eeprom.h"

/*
*函数功能:eeprom初始化
*/
void eeprom_init(void)
{
	IIC_init();
}

/*
*函数功能:从eeprom中读取一个字节数据
*函数参数:addr 读取地址
*函数返回值:读取到的字节
*/
uint8_t eeprom_read_one_byte(uint16_t addr)
{
	uint8_t temp = 0;
	IIC_Start();
	IIC_Send_Byte(0xA0 + ((addr/256) << 1));
	IIC_Wait_Ack();
	IIC_Send_Byte(addr % 256);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	temp = IIC_Read_Byte(0);
	IIC_Stop();
	return temp;
}

/*
*函数功能:向eeprom中写入一个字节的数据
*函数参数:	addr写入地址 dat 要写入的数据
*函数返回值:无
*/
void eeprom_write_one_byte(uint16_t addr,uint8_t dat)
{
	IIC_Start();
	IIC_Send_Byte(0xA0 + ((addr / 256) << 1));
	IIC_Wait_Ack();
	IIC_Send_Byte(addr % 256);
	IIC_Wait_Ack();
	IIC_Send_Byte(dat);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_ms(10);
}

/*
*函数功能:向eeprom中写入一定bit长度的数据
*函数参数:addr 写入地址 dat写入的数据 len写入长度
*函数返回值:无
*/
void eeprom_write_num(uint16_t addr,uint32_t dat,uint8_t len)
{
	uint8_t t;
	for(t = 0;t < len;t++)
	{
		eeprom_write_one_byte(addr + t, (dat >> (8*t)) & 0xFF);
	}
}

/*
*函数功能:从eeprom中读取一定bit位的数据
*函数参数:addr 读取地址         len读取地址
*函数返回值:读取到的数据
*/
uint32_t eeprom_read_num(uint16_t addr,uint8_t len)
{
	uint8_t t;
	uint32_t temp = 0;
	for(t = 0;t < len;t++)
	{
		temp <<= 8;
		temp += eeprom_read_one_byte(addr+len-t-1);
	}
	return temp;
}

/*
*函数功能:检测eeprom是否损坏
*函数参数:无
*函数返回值:0 正常         1异常
*/
uint8_t eeprom_chack(void)
{
	uint8_t temp;
	temp = eeprom_read_one_byte(255);
	if(temp == 0x55) return 0;
	else
	{
		eeprom_write_one_byte(255, 0x55);
		temp = eeprom_read_one_byte(255);
		if(temp == 0x55) return 0;
	}
	return 1;
}

/*
*函数功能:从eeprom中读取一定长度字节的数据
*函数参数:addr 读取地址          *p_buf接收缓冲区首地址        	num读取数量
*函数返回值:无
*/
void eeprom_read(uint16_t addr,uint8_t *p_buf,uint8_t num)
{
	while(num)
	{
		*p_buf++ = eeprom_read_one_byte(addr++);
		num--;
	}
}

/*
*函数功能:向eeprom中写入指定长度字节数的数据
*函数参数:addr 写入地址 *p_buf 要写入的数据的首地址  num写入长度
*函数返回值:无
*/
void eeprom_write(uint16_t addr,uint8_t *p_buf,uint16_t num)
{
	while(num--)
	{
		eeprom_write_one_byte(addr, *p_buf);
		addr++;
		p_buf++;
	}
}





