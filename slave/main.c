#include<stm32f446xx.h>
#include "uart.h"
//#include "systick.h"
#include "modbus.h"
uint8_t addr = 2;
uint8_t broad_addr = 10;
uint8_t data = 0;

void init(void);
void ttl_init(void);
void ttl_tx_enable(void);
void ttl_tx_disable(void);
void crc_init(void);

// Slave

int main() {
	init();
	while(1) {
		
	}
}

void init() {
	uart2_tx_rx_init();
	uart6_tx_rx_init();
	uint8_t tmp = 0xFF & USART6->DR;
	uart6_rxne_interrupt_enable();
	
	ttl_init();
	ttl_tx_disable();
	
}

// using PA0 as ttl DE/RE controller
// PA0 high -> DE high, RE low -> TX enabled
// PA0 low -> DE low, RE high -> RX enabled
void ttl_init() {
	RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA-> MODER |= (1 << 0); 
}
void ttl_tx_enable() {
	GPIOA-> BSRR |= (1U << 0);
}
void ttl_tx_disable() {
	GPIOA-> BSRR |= (1U << 16); 
}







