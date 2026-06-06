#ifndef __EEPROM_H
#define __EEPROM_H 

//功能：读取指定地址的半个字
//参数：addr表示要读的地址
//返回值：data为指定地址内的数据
u16 Read_HalfWord(u32 addr);


//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_ReadData(u32 Readaddr,u16 *pBuffer,u16 NumToRead);



//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);

//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite) ;
#endif











