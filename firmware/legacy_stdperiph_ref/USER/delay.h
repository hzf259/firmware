#ifndef __DELAY_H
#define __DELAY_H 			   


#define  DelayMs  delay_ms
#define  DelayUs  delay_us


void DelayInit(void);
void DelayMs(unsigned int nms);
void DelayUs(unsigned long nus);
void DelayS(unsigned int ns);

#endif
