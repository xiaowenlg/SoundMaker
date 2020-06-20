#ifndef _STMFLASH_H
#define _STMFLASH_H
#include "stdint.h"
#include "main.h"
#include "MainConfig.h"
void STMFLASH_Write_NoCheck(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite);
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);
void STMFLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);
void Eeprom_write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t datlen);
#endif // !_STMFLASH_H
