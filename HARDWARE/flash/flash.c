#include "flash.h"

/**
  * @brief  读取一个字的数据，并将地址自增，减少错误的可能，方便连续读数据
  *     
  * @note	请传入地址的指针，以实现自增的效果
  * @param  faddr: 数据的地址指针  
  *          
  * @retval u32: 地址对应的flash中的数据
  */
u32 STMFLASH_ReadWord_Inc(u32* faddr)
{
    u32 temp = *(vu32*)(* faddr);
    *faddr += 4;
    return temp;
}

/**
  * @brief  读取半字的数据，并将地址自增，减少错误的可能，方便连续读数据
  *     
  * @note	请传入地址的指针，以实现自增的效果
  * @param  faddr: 数据的地址指针  
  *          
  * @retval u16: 地址对应的flash中的数据
  */
u16 STMFLASH_ReadHalfWord_Inc(u32* faddr)
{
    u16 temp = *(vu32*)(* faddr);
    *faddr += 2;
    return temp;
}

/**
  * @brief  读取一个字节的数据，并将地址自增，减少错误的可能，方便连续读数据
  *     
  * @note	请传入地址的指针，以实现自增的效果
  * @param  faddr: 数据的地址指针  
  *          
  * @retval u16: 地址对应的flash中的数据
  */
u8 STMFLASH_ReadByte_Inc(u32* faddr)
{
    u8 temp = *(vu32*)(* faddr);
    *faddr += 1;
    return temp;
}

/**
  * @brief  读取一个float的数据，并将地址自增，减少错误的可能，方便连续读数据
  *     
  * @note	请传入地址的指针，以实现自增的效果
  *
  * @param  faddr: 数据的地址指针  
  *          
  * @retval float: 地址对应的flash中的数据
  */
float STMFLASH_ReadFloat_Inc(u32* faddr)
{
    float temp = *(float*)(* faddr);
    *faddr += sizeof(float);
    return temp;
}

/**
  * @brief  读取一个字的数据
  *     
  * @param  faddr: 数据的地址 
  *          
  * @retval u32: 地址对应的flash中的数据
  */
u32 STMFLASH_ReadWord(u32 faddr)
{
    return *(vu32*)faddr;
}

/**
  * @brief  读取一个半字的数据
  *     
  * @param  faddr: 数据的地址 
  *          
  * @retval u16: 地址对应的flash中的数据
  */
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr;
}

/**
  * @brief  读取一个字节的数据
  *     
  * @param  faddr: 数据的地址 
  *          
  * @retval u8: 地址对应的flash中的数据
  */
u8 STMFLASH_ReadByte(u32 faddr)
{
    return *(vu8*)faddr;
}

/**
  * @brief  读取一个float的数据
  *     
  * @param  faddr: 数据的地址 
  *          
  * @retval float: 地址对应的flash中的数据
  */
float STMFLASH_ReadFloat(u32 faddr)
{
    return *(float*)faddr;
}

/**
  * @brief  在相应的位置上写入一个float型的数据，需要把他当作一个u32型的数据存入
  *			
  * @note	当读取时请使用STMFLASH_ReadFloat或STMFLASH_ReadFloat_Inc
  *
  * @param  uint32_t Address: 数据的地址  
  *          
  * @param  float Data: 数据
  *
  * @retval FLASH_Status: flash的状态
  */
FLASH_Status FLASH_ProgramFloat(uint32_t Address, float Data)
{
	u32 * ptr = (u32 *) &Data;
	return FLASH_ProgramWord(Address, *ptr);
}
