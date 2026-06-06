#ifndef __HX711_H
#define __HX711_H 
 

//IO方向设置

////IO操作函数											   
#define	HX711_SCK PCout(14) //数据端口	PA0
#define	HX711_DOUT  PCin(13)  //数据端口	PA0 



void Init_HX711pin(void);
u32 HX711_Read(void);  
void Get_Maopi(void);
void Get_Weight(void);

extern u32 Weight_Maopi;
extern s32 Weight_Shiwu;


#endif











