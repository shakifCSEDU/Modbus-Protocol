#include "rs485_config.h"


void rs485_init(void){
	RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA-> MODER |= (1 << 0); 
}
void rs485_tx_enable(void){
	GPIOA-> BSRR |= (1U << 0);
}
void rs485_tx_disable(void){
	GPIOA-> BSRR |= (1U << 16); 
}
