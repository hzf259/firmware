#include "main.h"

//声明一下引脚编号对应上数码管段位
#define GPIO_Pin_A  GPIO_Pin_0  
#define GPIO_Pin_B  GPIO_Pin_1 
#define GPIO_Pin_C  GPIO_Pin_2 
#define GPIO_Pin_D  GPIO_Pin_3 
#define GPIO_Pin_E  GPIO_Pin_4 
#define GPIO_Pin_F  GPIO_Pin_5 
#define GPIO_Pin_G  GPIO_Pin_6 
#define GPIO_Pin_H  GPIO_Pin_7 

#define GPIO_Pin_DQ  GPIO_Pin_0
#define GPIO_Pin_DB  GPIO_Pin_14
#define GPIO_Pin_DS  GPIO_Pin_13
#define GPIO_Pin_DG  GPIO_Pin_15

//延时程序
void SM_DelayMs(unsigned int i)
{
	unsigned int j,k;

	for(j=i; j>0; j--)
		for(k=114; k>0; k--);
}

//数码管引脚定义程序
void SM_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 						 
//=============================================================================
//段码引脚设定
//=============================================================================			 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_A|GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_D|GPIO_Pin_E|GPIO_Pin_F|GPIO_Pin_G|GPIO_Pin_H;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 						 
//=============================================================================
//位码引脚设定
//=============================================================================		
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
//应为使用的共阴数码管  先把位的几个IO口设置为高电平	
	GPIO_SetBits(GPIOC,GPIO_Pin_DG);
	GPIO_SetBits(GPIOC,GPIO_Pin_DS);
	GPIO_SetBits(GPIOC,GPIO_Pin_DB);
	GPIO_SetBits(GPIOB,GPIO_Pin_DQ);
	
}

//void S_display(int dat,u8 bit)
//{
//	if(dat>=0) SM_display(dat,bit);
//	else
//	{
//		
//	}
//}

//四位数码管显示   dat代表 显示的数据   bit 代表这个数是小数点要在第几位0-3
void SM_display(int dat,u8 bit)
{
 int eng;
 if(dat<0)
 {
		eng = 1;
		dat = dat*(-1);
	 if(dat>999) {dat = dat/10;if(bit>0)bit--;}
 }
 if (dat < 9999)//因为是四位最多只能显示9999
 { 
	int i;
  int s;
  s = dat;
  i=0;
  for (i=0;i<4;i++)//循环四次
  {
//==================先把第一个显示位开启=============================
   switch (i)
    {
    case 0:
     GPIO_ResetBits(GPIOC,GPIO_Pin_DG);
     break;
    case 1:
     GPIO_ResetBits(GPIOC,GPIO_Pin_DS);
     break;
    case 2:
     GPIO_ResetBits(GPIOC,GPIO_Pin_DB);
    break;
    case 3:
     GPIO_ResetBits(GPIOB,GPIO_Pin_DQ);
    break; 
    } 
		
	//==================================这里相当于段码 ====================== 	
   switch (s % 10)
   {
    case 0:
     GPIO_ResetBits(GPIOA,GPIO_Pin_G);
		 GPIO_SetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_D|GPIO_Pin_E|GPIO_Pin_F);
    break;
    case 1:
     GPIO_ResetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_D|GPIO_Pin_E|GPIO_Pin_F|GPIO_Pin_G);
		 GPIO_SetBits(GPIOA,GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_H);
    break;
    case 2:
     GPIO_ResetBits(GPIOA,GPIO_Pin_C|GPIO_Pin_F);
		 GPIO_SetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_B|GPIO_Pin_D|GPIO_Pin_E|GPIO_Pin_G);
    break;
    case 3:
     GPIO_ResetBits(GPIOA,GPIO_Pin_E|GPIO_Pin_F);
		 GPIO_SetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_D|GPIO_Pin_G);
    break;
    case 4:
     GPIO_ResetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_D|GPIO_Pin_E);
		 GPIO_SetBits(GPIOA,GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_F|GPIO_Pin_G|GPIO_Pin_H);
    break;
    case 5:
     GPIO_ResetBits(GPIOA,GPIO_Pin_B|GPIO_Pin_E);
		 GPIO_SetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_C|GPIO_Pin_D|GPIO_Pin_F|GPIO_Pin_G);
    break;
    case 6:
     GPIO_ResetBits(GPIOA,GPIO_Pin_B);
		 GPIO_SetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_C|GPIO_Pin_D|GPIO_Pin_E|GPIO_Pin_F|GPIO_Pin_G);
    break;
    case 7:
     GPIO_ResetBits(GPIOA,GPIO_Pin_D|GPIO_Pin_E|GPIO_Pin_F|GPIO_Pin_G);
		 GPIO_SetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_H);
    break;
    case 8:
			  GPIO_SetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_D|GPIO_Pin_E|GPIO_Pin_F|GPIO_Pin_G);
        break;
    case 9:
     GPIO_ResetBits(GPIOA,GPIO_Pin_E);
		 GPIO_SetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_D|GPIO_Pin_F|GPIO_Pin_G);
    break;
   }
	 // =============根据小数点要求显示小数点 =========================
	 if(i == bit)	GPIO_SetBits(GPIOA,GPIO_Pin_H);
				else GPIO_ResetBits(GPIOA,GPIO_Pin_H);
	 SM_DelayMs(15);
	 
	 //===================显示好了把位关闭避免出现显示问题
	  switch (i)
    {
    case 0:
     GPIO_SetBits(GPIOC,GPIO_Pin_DG);
     break;
    case 1:
     GPIO_SetBits(GPIOC,GPIO_Pin_DS);
     break;
    case 2:
     GPIO_SetBits(GPIOC,GPIO_Pin_DB);
    break;
    case 3:
     GPIO_SetBits(GPIOB,GPIO_Pin_DQ);
    break; 
    } 
		GPIO_ResetBits(GPIOA,GPIO_Pin_H);
		SM_DelayMs(5);
		//======================判断这个数是不是完了  完了就 结束函数
   s = s / 10; 
   if (s==0)
	 {
			if(eng == 1)
			{
				i++;
				switch (i)
				{
				case 0:
				 GPIO_ResetBits(GPIOC,GPIO_Pin_DG);
				 break;
				case 1:
				 GPIO_ResetBits(GPIOC,GPIO_Pin_DS);
				 break;
				case 2:
				 GPIO_ResetBits(GPIOC,GPIO_Pin_DB);
				break;
				case 3:
				 GPIO_ResetBits(GPIOB,GPIO_Pin_DQ);
				break; 
				} 
				GPIO_SetBits(GPIOA,GPIO_Pin_G);
				GPIO_ResetBits(GPIOA,GPIO_Pin_A|GPIO_Pin_B|GPIO_Pin_C|GPIO_Pin_D|GPIO_Pin_E|GPIO_Pin_F);
				SM_DelayMs(15);
					switch (i)
					{
					case 0:
					 GPIO_SetBits(GPIOC,GPIO_Pin_DG);
					 break;
					case 1:
					 GPIO_SetBits(GPIOC,GPIO_Pin_DS);
					 break;
					case 2:
					 GPIO_SetBits(GPIOC,GPIO_Pin_DB);
					break;
					case 3:
					 GPIO_SetBits(GPIOB,GPIO_Pin_DQ);
					break; 
					} 
					GPIO_ResetBits(GPIOA,GPIO_Pin_H);
					SM_DelayMs(5);				
			}
		
			i=10;
			
	 }
    
  }  
 }
}


