#include "uart.h"

void uart2_tx_rx_init() {
	// enable clock to GPIOA
	RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	// PA2, PA3 mode to alternate functions, 
	GPIOA-> MODER |= (0x2 << 4) | (0x2 << 6);
	// alternate function type to AF7 = 0111 for PA2 and PA3 
	GPIOA-> AFR[0] |= (0x7 << 8) | (0x7 << 12);
	
	// enable clock to usart2
	RCC-> APB1ENR |= RCC_APB1ENR_USART2EN;
	// Buadrate 9600 -> 0x683, 115200 -> 0x8B when periphClk = 16 MHz
	USART2-> BRR |= 0x8B;
	// usart2 enable, tx enable, rx enable
	USART2-> CR1 |= (1<<13) | (1<<3) | (1<<2);
	// enable uart2 interrupt in NVIC
	NVIC_EnableIRQ(USART2_IRQn);
}

void uart6_tx_rx_init() {
	// enable clock to GPIOC
	RCC-> AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	// PA6, PA7 mode to alternate functions, 
	GPIOC-> MODER |= (0x2 << 12) | (0x2 << 14);
	// alternate function type to AF8 = 1000 for PA6 and PA7 
	GPIOC-> AFR[0] |= (0x8U << 24) | (0x8U << 28);
	
	// enable clock access to USART6
	RCC-> APB2ENR |= RCC_APB2ENR_USART6EN;
	// baudrate 115200 -> BRR = 0x8B, 9600 -> BRR = 0x683 
	USART6-> BRR |= 0x8B;
	// uart enable, rx enable, tx enable
	USART6-> CR1 |= (1<<3) | (1<<2) | (1<<13);
	// enable uart6 interrupt in NVIC
	NVIC_EnableIRQ(USART6_IRQn);
}


// ISR for UART2 TXE and RXNE interrupt
/*
void USART2_IRQHandler() {
	// if interrupt is for TXE and TXEIE is enabled 
	if ((USART2-> SR & 1<<7) && (USART2-> CR1 & 1<<7)) {
		uart2_tx_callback();
	}
	// else if interrupt is for RXNE
	if (USART2-> SR & 1<<5) {
		uart2_rx_callback();
	}
}
*/

// ISR for UART6 TXE and RXNE interrupt 
void USART6_IRQHandler() {
	if ((USART6->SR & (1 << 1)) || (USART6->SR & (1 << 2)) || (USART6->SR & (1 << 3))) {
		
	if(USART6->SR & (1<<3)){
			uart2_write_str("**** OVER RUN *****\n");
		}
		
		uint8_t tmp = (USART6->DR & 0xFF);
	}
	/*
	else if (USART6-> SR & 1<<7) {
		uart6_tx_callback();
	}
	*/
	// else if interrupt is for RXNE
	else if (USART6-> SR & 1<<5) {
		uart6_rx_callback();
	}
}


// synchronous byte transfer(without interrupt)

void uart2_write(uint8_t c) {		
	while(!(USART2->SR & (1 << 7))) {}
	USART2-> DR = c;
}
uint8_t uart2_read() {
	while(!(USART2->SR & (1 << 5))) {}
	return (uint8_t) USART2-> DR;
}
void uart2_write_str(char *s) {
	while(*s != '\0') {
		uart2_write(*s);
		++s;
	}
}
void uart2_write_int(uint32_t a) {
	if (a == 0) {
		uart2_write('0');
		return;
	}
	char tmp[15];
	uint8_t _cnt = 0;
	while (a != 0) {
		tmp[_cnt++] = a%10;
		a/=10;
	} 
	while (_cnt != 0) {
		uart2_write('0'+tmp[_cnt-1]);
		--_cnt;
	}
}

void uart6_write(uint8_t c) {
	while(!(USART6-> SR & (1 << 7))) {}
	USART6-> DR = c;
}
uint8_t uart6_read() {
	while(!(USART6->SR & (1 << 5))) {}
	return (uint8_t) USART6-> DR;
}

void uart2_txe_rxne_interrupt_enable() {
	// usart2 TXE and RXNE interrupt enable
	USART2-> CR1 |= (1 << 7) | (1 << 5);
}
void uart2_txe_interrupt_enable() {
	// usart2 TXE interrupt enable
	USART2-> CR1 |= (1 << 7);
}
void uart2_txe_interrupt_disable() {
	// usart2 TXE interrupt disable 
	USART2-> CR1 &= ~(1U << 7);
}
void uart2_rxne_interrupt_enable() {
	// usart2 RXNE interrupt enable
	USART2-> CR1 |= (1 << 5);
}
void uart2_rxne_interrupt_disable() {
	// usart2 RXNE interrupt disable 
	USART2-> CR1 &= ~(1U << 5);
}
void uart6_txe_rxne_interrupt_enable() {
	// usart6 TXE and RXNE interrupt enable
	USART6-> CR1 |= (1 << 7) | (1 << 5);
}
void uart6_txe_interrupt_enable() {
	// usart6 TXE interrupt enable
	USART6-> CR1 |= (1 << 7);
}
void uart6_txe_interrupt_disable() {
	// usart6 TXE interrupt disable 
	USART6-> CR1 &= ~(1U << 7);
}
void uart6_rxne_interrupt_enable() {
	// usart6 RXNE interrupt enable
	USART6-> CR1 |= (1 << 5);
}
void uart6_rxne_interrupt_disable() {
	// usart6 RXNE interrupt disable 
	USART6-> CR1 &= ~(1U << 5);
}


