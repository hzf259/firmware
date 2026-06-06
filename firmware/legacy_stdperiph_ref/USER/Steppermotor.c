/* 该程序为步进电机驱动程序*/
 
 #include "main.h"

#define Motor_in1 PBout(12)  
#define Motor_in2 PBout(13) 
#define Motor_in3 PBout(14) 
#define Motor_in4 PBout(15) 

#define Step_angle  5.3;//步进角度
int pulse_num;//脉冲数
//char Motor_out_temp;

const unsigned char F_Rotation[4]={0x01,0x02,0x04,0x08};//正转表格
const unsigned char B_Rotation[4]={0x08,0x04,0x02,0x01};//反转表格

//=============================================================================
//文件名称：Key_Init
//功能概要：GPIO初始化
//参数说明：无
//函数返回：无
//=============================================================================
void motor_io_Init(void)  //引脚定义
{
//将PB0/1/10/11 设置为上拉电阻输入因为我们使用低电平信号触发
	GPIO_InitTypeDef gpio_initstruct;
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB , ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;							
	gpio_initstruct.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;			
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;							
	GPIO_Init(GPIOB, &gpio_initstruct);												
	
}



void Motorcw(char speed)
{

				
	Motor_in4 = 1;
	Motor_in3 = 0;
	Motor_in2 = 0;
	Motor_in1 = 0;
	DelayMs(speed);
	Motor_in4 = 0;
	Motor_in3 = 1;
	Motor_in2 = 0;
	Motor_in1 = 0;
	DelayMs(speed);
	Motor_in4 = 0;
	Motor_in3 = 0;
	Motor_in2 = 1;
	Motor_in1 = 0;
	DelayMs(speed);
	Motor_in4 = 0;
	Motor_in3 = 0;
	Motor_in2 = 0;
	Motor_in1 = 1;
	DelayMs(speed);
        
}


void Motor_io_out(char speed)
{
	Motor_in4 = 0;
	Motor_in3 = 0;
	Motor_in2 = 0;
	Motor_in1 = 1;
	DelayMs(speed);
	Motor_in4 = 0;
	Motor_in3 = 0;
	Motor_in2 = 1;
	Motor_in1 = 0;
	DelayMs(speed);
	Motor_in4 = 0;
	Motor_in3 = 1;
	Motor_in2 = 0;
	Motor_in1 = 0;
	DelayMs(speed);
	Motor_in4 = 1;
	Motor_in3 = 0;
	Motor_in2 = 0;
	Motor_in1 = 0;
	DelayMs(speed);

}
void Motor(int Motor_speed,int Motor_angle,char Motor_dir)//转速圈/秒  角度 
{
	pulse_num = Motor_angle/Step_angle;
	for(;pulse_num>0;pulse_num--)
	{
		if(Motor_dir==0)//反转
		{
				Motorcw(Motor_speed);
		 }
		else//正转
		{
			Motor_io_out(Motor_speed);
		}
	}
	Motor_in4 = 0;
	Motor_in3 = 0;
	Motor_in2 = 0;
	Motor_in1 = 0;	
}

