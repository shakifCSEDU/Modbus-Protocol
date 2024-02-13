#include<stm32f446xx.h>
#include "systick.h"

static uint32_t m_ticks = 0;
static uint32_t delay_millis = 0;
//static uint8_t toggle = 0;

void sysTickTimer(uint32_t millis) {
	m_ticks = 0;
	delay_millis = millis;
	SysTick -> LOAD = 16000;
	SysTick -> VAL = 0;
	SysTick -> CTRL = 7;
	NVIC_SetPriority(SysTick_IRQn, 0);
}

void SysTick_Handler(void) {
	m_ticks++;
	//if (m_ticks == delay_millis) {
		//systick_interrupt();
	//}
}

void sysTickDisable(void) {
	SysTick -> CTRL = 0;
	NVIC_ClearPendingIRQ(SysTick_IRQn);
}

void ms_delay(int delay){
	int cur_ms = (int)m_ticks;
	while((m_ticks - cur_ms) < delay){}
}

