#include "StmFlash.h"
#include "stm32f1xx_hal_flash.h"
 
uint16_t STMFLASH_BUF[STM_SECTOR_SIZE / 2];//最多2K
//************************************
// 函数:    STMFLASH_Write_NoCheck
// 函数全名:  STMFLASH_Write_NoCheck
// 函数类型:    public 
// 返回值:   void
// Qualifier:
// 参数: uint32_t WriteAddr 起始地址
// 参数: uint16_t * pBuffer 写入的数据
// 参数: uint16_t NumToWrite 数据长度
//************************************
void STMFLASH_Write_NoCheck(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
	uint16_t i;
	for (i = 0; i < NumToWrite; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, WriteAddr, pBuffer[i]); //对Flash进行烧写，FLASH_TYPEPROGRAM_HALFWORD 声明操作的Flash地址的16位的，此外还有32位跟64位的操作，自行翻查HAL库的定义即可
		WriteAddr += 2;//地址增加2
	}
}

void STMFLASH_Write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
	uint32_t secpos;	   
	uint16_t secoff;	  
	uint16_t secremain;    
	uint16_t i;
	uint32_t offaddr;  
	FLASH_EraseInitTypeDef My_Flash;  //声明 FLASH_EraseInitTypeDef 结构体为 My_Flash
	if (WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + STM32_FLASH_SIZE)))return;
	HAL_FLASH_Unlock();

	My_Flash.TypeErase = FLASH_TYPEERASE_PAGES;  //标明Flash执行页面只做擦除操作
	My_Flash.NbPages = 1;                        //说明要擦除的页数，此参数必须是Min_Data = 1和Max_Data =(最大页数-初始页的值)之间的值
	uint32_t PageError = 0;                    //设置PageError,如果出现错误这个变量会被设置为出错的FLASH地址

	offaddr = WriteAddr - STM32_FLASH_BASE;		
	secpos = offaddr / STM_SECTOR_SIZE;			
	secoff = (offaddr%STM_SECTOR_SIZE) / 2;		
	secremain = STM_SECTOR_SIZE / 2 - secoff;		
	if (NumToWrite <= secremain)secremain = NumToWrite;
	while (1)
	{
		STMFLASH_Read(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);
		for (i = 0; i < secremain; i++)
		{
			if (STMFLASH_BUF[secoff + i] != 0XFFFF)break; 
		}
		if (i < secremain)
		{
			My_Flash.PageAddress = secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE;  //声明要擦除的地址
			HAL_FLASHEx_Erase(&My_Flash, &PageError);  //调用擦除函数擦除
			for (i = 0; i < secremain; i++)
			{
				STMFLASH_BUF[i + secoff] = pBuffer[i];
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);
		}
		else STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);			   
		if (NumToWrite == secremain)break;
		else
		{
			secpos++;				
			secoff = 0;				 
			pBuffer += secremain;  	
			WriteAddr += secremain;  
			NumToWrite -= secremain;	
			if (NumToWrite > (STM_SECTOR_SIZE / 2))secremain = STM_SECTOR_SIZE / 2;
			else secremain = NumToWrite;
		}
	};
	HAL_FLASH_Lock();
}
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return  *(__IO uint16_t*)faddr;
}
void STMFLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead)
{
	uint16_t i;
	for (i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);
		ReadAddr += 2;
	}
}
//读半字节
void Eeprom_write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t datlen) {
	uint16_t i = 0;
	// uint32_t Robot_Num_Flash_Add = EEPROM_BEGIN_ADRR;
	FLASH_EraseInitTypeDef My_Flash;  //声明 FLASH_EraseInitTypeDef 结构体为 My_Flash
	HAL_FLASH_Unlock();               //解锁Flash
	My_Flash.TypeErase = FLASH_TYPEERASE_PAGES;  //标明Flash执行页面只做擦除操作
	My_Flash.PageAddress = WriteAddr;  //声明要擦除的地址
	My_Flash.NbPages = 1;                        //说明要擦除的页数，此参数必须是Min_Data = 1和Max_Data =(最大页数-初始页的值)之间的值
	uint32_t PageError = 0;                    //设置PageError,如果出现错误这个变量会被设置为出错的FLASH地址
	HAL_FLASHEx_Erase(&My_Flash, &PageError);  //调用擦除函数擦除
	for (i = 0; i < datlen; i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, WriteAddr, pBuffer[i]);
		WriteAddr += 2;//地址增加2
	}

	HAL_FLASH_Lock(); //锁住Flash
}

void readd() {
	uint16_t ID_Num;
	uint32_t Robot_Num_Flash_Add = EEPROM_BEGIN_ADRR;
	ID_Num = *(__IO
		uint16_t*)(Robot_Num_Flash_Add); //*(__IO uint16_t *)是读取该地址的参数值,其值为16位数据,一次读取两个字节，*(__IO uint32_t *)就一次读4个字节
	//Uart_printf(&huart1,"dat:%x",ID_Num);
	// printf("ID_num:0x%x\r\n", ID_Num);
}