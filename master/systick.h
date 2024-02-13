#ifndef __SYSTICK_H
#define __SYSTICK_H

#include<stm32f446xx.h>

void sysTickTimer(uint32_t millis);
void SysTick_Handler(void);
void sysTickDisable(void);
void systick_interrupt(void);
void ms_delay(int delay);
#endif
