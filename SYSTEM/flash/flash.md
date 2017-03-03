### FLASH读写
#### 库函数讲解
1. 锁定解锁函数

		void FLASH_Unlock(void);  	//写flash之前解锁
		void FLASH_Lock(void);  	//写完flash之后锁定

2. 写操作函数

		FLASH_Status FLASH_ProgramDoubleWord(uint32_t Address, uint64_t Data);
		FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
		FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
		FLASH_Status FLASH_ProgramByte(uint32_t Address, uint8_t Data);

3. 擦除函数

		FLASH_Status FLASH_EraseSector(uint32_t FLASH_Sector, uint8_t VoltageRange);
		FLASH_Status FLASH_EraseAllSectors(uint8_t VoltageRange);

	对于这些函数的第二个参数，我们这里电源电压范围是 3.3V，所以选择 VoltageRange_3 即可。

4. 	取获取FLASH状态

		FLASH_Status FLASH_GetStatus(void);
		//返回值是枚举类型的
		typedef enum
		{
			FLASH_BUSY = 1,//操作忙
			FLASH_ERROR_RD,//读保护错误
			FLASH_ERROR_PGS,//编程顺序错误
			FLASH_ERROR_PGP,//编程并行位数错误
			FLASH_ERROR_PGA,//编程对齐错误
			FLASH_ERROR_WRP,//写保护错误
			FLASH_ERROR_PROGRAM,//编程错误
			FLASH_ERROR_OPERATION,//操作错误
			FLASH_COMPLETE//操作结束
		}FLASH_Status;

5. 等待操作完成函数

		FLASH_Status FLASH_WaitForLastOperation(void);

	写操作库函数中本身带有等待。
	
6. 读取flash中的数据

		u32 STMFLASH_ReadWord_Inc(u32* faddr)
		{
			u32 temp = *(vu32*)(* faddr);
			faddr += 4;
			return temp;
		}

		u16 STMFLASH_ReadHalfWord_Inc(u32* faddr)
		{
			u16 temp = *(vu32*)(* faddr);
			faddr += 2;
			return temp;
		}

		u8 STMFLASH_ReadByte_Inc(u32* faddr)
		{
			u8 temp = *(vu32*)(* faddr);
			faddr += 1;
			return temp;
		}

		float STMFLASH_ReadFloat_Inc(u32* faddr)
		{
			float temp = *(vu32*)(* faddr);
			faddr += sizeof(float);
			return temp;
		}

	此部分是由我编写，加入了地址自增，减小了出错的可能性
	若不希望地址自增，则使用下面的函数

		u32 STMFLASH_ReadWord(u32 faddr)
		{
			return *(vu32*)faddr;
		}
		
		u16 STMFLASH_ReadHalfWord(u32 faddr)
		{
			return *(vu16*)faddr;
		}

		u8 STMFLASH_ReadByte(u32 faddr)
		{
			return *(vu8*)faddr;
		}

		float STMFLASH_ReadFloat(u32 faddr)
		{
			return *(float*)faddr;
		}

*** 
#### 读写操作
##### 读
  STM23F4的FLASH读取是很简单的。例如，我们要从地址 addr，读取一个字（字节为 8位，半字为 16 位，字为32位），可以通过如下的语句读取：
	data=*(vu32*)addr;
  上面我自己写了一些读取的函数，包括字，双字，半字，一Byte，float 以及自增
##### 写
  由于写flash时要确保该位置是已擦除状态，所以一般情况下，是一整片扇区擦除，然后重新写数据，步骤如下：

	FLASH_Unlock();  //解锁FLASH，写数据
    FLASH_DataCacheCmd(DISABLE);
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
    								FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);  //写FLASH之前，先清除标志
    if(FLASH_EraseSector(FLASH_Sector_x, VoltageRange_3) != FLASH_COMPLETE){ 
        FLASH_DataCacheCmd(ENABLE); 
        FLASH_Lock();//上锁
        return -1;   //擦写失败，，退出
    }
	 
    //写
	 
    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();  //  写保护
    if(param_addr > PARAM_FLASH_ADDR_END){   //如果超过FLASH的存储空间，则报错返回
        return -2;
    }
