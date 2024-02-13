#include<stm32f446xx.h>
#include "systick.h"

uint32_t m_ticks = 0;

void sysTickTimer() {
	m_ticks = 0;
	SysTick -> LOAD = 16000;
	SysTick -> VAL = 0;
	SysTick -> CTRL = 7;
	NVIC_SetPriority(SysTick_IRQn, 0);
}

void SysTick_Handler(void) {
	m_ticks++;
}

void sysTickDisable(void) {
	SysTick -> CTRL = 0;
	NVIC_ClearPendingIRQ(SysTick_IRQn);
}
void ms_delay(int delay){
	int cur_ms = m_ticks;
	
	while((cur_ms - m_ticks) < delay){}
}
