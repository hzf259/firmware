#ifndef _FLASH_H
#define _FLASH_H
 
/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_flash.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
#if !defined (STM32_DK_128K) &&  !defined (STM32_EK)
 //#define USE_STM3210B_EVAL
 #define STM32_EK
#endif
 
/* Define the STM32F10x hardware depending on the used evaluation board */
#ifdef STM32_DK_128K
  #define FLASH_PAGE_SIZE    ((u16)0x400) //1024  1K
#elif defined STM32_EK
  #define FLASH_PAGE_SIZE    ((u16)0x800) //2048   2K
#endif /* USE_STM3210B_EVAL */
/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
 
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 
void Writeflash(u8 Erasenumber ,u32 *p,u8 start,u8 end);
void Readflash(u32 *p,u8 start,u8 end) ;
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
 
//Writeflash(0,Var_Compensate,0,6);
//Readflash(Var_Compensate,0,6)

#endif /* __PLATFORM_CONFIG_H */


