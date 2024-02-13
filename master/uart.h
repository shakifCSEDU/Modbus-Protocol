#ifndef __UART_H
#define __UART_H
#include<stm32f446xx.h>


void uart2_tx_rx_init(void);
void uart2_write(uint8_t c);
uint8_t uart2_read(void);
void uart2_read_str(void);
void uart2_write_str(char *s);
void uart2_write_int(uint32_t a);
void USART2_IRQHandler(void);
void uart2_tx_callback(void);
void uart2_rx_callback(void);
void uart2_txe_rxne_interrupt_enable(void);
void uart2_txe_interrupt_enable(void);
void uart2_txe_interrupt_disable(void);
void uart2_rxne_interrupt_enable(void);
void uart2_rxne_interrupt_disable(void);

void uart6_tx_rx_init(void);
void uart6_write(uint8_t c);
uint8_t uart6_read(void);
void uart6_read_str(void);
void uart6_write_str(void);
void USART6_IRQHandler(void);
void uart6_tx_callback(void);
void uart6_rx_callback(void);
void uart6_txe_rxne_interrupt_enable(void);
void uart6_txe_interrupt_enable(void);
void uart6_txe_interrupt_disable(void);
void uart6_rxne_interrupt_enable(void);
void uart6_rxne_interrupt_disable(void);

#endif
