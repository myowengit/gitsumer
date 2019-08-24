#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "iic.h"

void eeprom_init(void);
uint8_t eeprom_read_one_byte(uint16_t addr);
void eeprom_write_one_byte(uint16_t addr,uint8_t dat);
void eeprom_write_num(uint16_t addr,uint32_t dat,uint8_t len);
uint32_t eeprom_read_num(uint16_t addr,uint8_t len);
uint8_t eeprom_chack(void);
void eeprom_read(uint16_t addr,uint8_t *p_buf,uint8_t num);
void eeprom_write(uint16_t addr,uint8_t *p_buf,uint16_t num);

#endif /*__EEPROM_H__*/


