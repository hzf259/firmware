#include "main.h"
u32 StartAddr = 0x0801F800;  //要写入Flash的数据的首地址--FLASH起始地址
u32 EndAddr  =  0x0801FFFF;  //要写入Flash的数据的末地址--FLASH结束地址
u32 FlashAddress=0x00;//Flash的内部地址	 
vu32 NbrOfPage = 0x00; //要擦除的页面数量
u32 data = 0;
//u32 *p=(u32 *)0x08008000; //定义指针指向要传送的数据的地址
volatile FLASH_Status FLASHStatus;
volatile TestStatus MemoryProgramStatus;
ErrorStatus HSEStartUpStatus;
 
/*******************************************************************************
* Function Name  : Writeflash
* Description    : 写函数 把数据从CPU写到FLASH中
*                
* Input          : u8 Erasenumber,u32 *Data，输入要擦除的页面和要写入的数据的地址
* Output         : None
* Return         : None
*******************************************************************************/
void Writeflash(u8 Erasenumber ,u32 *p,u8 start,u8 end)
{
	int i = start;
	FLASHStatus = FLASH_COMPLETE;
   MemoryProgramStatus = PASSED;
 
    /* Unlock the Flash Program Erase controller */
    FLASH_Unlock();	//FLASH解锁
 
    /* Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//清标志位
	
//擦除后写数据		
//*******************************************************************************  
//一次擦除1页
      /* Define the number of page to be erased *///定义要擦出的页面的数量
      NbrOfPage = (EndAddr - StartAddr) / FLASH_PAGE_SIZE;
	 /* Erase the FLASH pages *///页面擦除子程序
      FLASHStatus = FLASH_ErasePage(StartAddr + (FLASH_PAGE_SIZE * NbrOfPage));
	 //写数据
	  FlashAddress = StartAddr+4*start;
	 
      while((FlashAddress < EndAddr) && (FLASHStatus == FLASH_COMPLETE) && i<end)
      {
	     FLASHStatus = FLASH_ProgramWord(FlashAddress, *(p+i));//*p);
		 i++;
	     FlashAddress = FlashAddress + 4;
      }
}
 
/*******************************************************************************
* Function Name  : Readflash
* Description    : 读数据，从FLASH中读出需要的数据
*                
* Input          : None
* Output         : Data输出要取出的数据
* Return         : None
*******************************************************************************/
 
void Readflash(u32 *p,u8 start,u8 end) //
{
   	int j = start;
    FlashAddress = StartAddr+4*start;
	//读数据
 
	while(j<end)
	{
		*(p+j) = *(u32*)(FlashAddress+4*j);
		j++;
	}
}


