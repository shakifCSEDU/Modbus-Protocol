#include<stm32f446xx.h>


extern uint8_t addr;
extern uint8_t data;
extern uint8_t broad_addr;

extern struct packet pkt;

extern void ttl_enable_init(void);
extern void ttl_tx_enable(void);
extern void ttl_tx_disable(void);
extern void send_response(void);
extern void send_error_response(void);
extern uint8_t check_crc(void);
void fill_debug_data(void);
void reset_debug_counters(void);

void led_on(void);
void led_off(void);
void uart2_tx_rx_init(void);
void uart2_write(char c);
void uart2_write_int(uint32_t a);
char uart2_read(void);
void uart2_read_str(void);
void uart2_write_str(char*);
void USART2_IRQHandler(void);
void uart2_tx_callback(void);
void uart2_rx_callback(void);
void uart2_txe_rxne_interrupt_enable(void);
void uart2_txe_interrupt_enable(void);
void uart2_txe_interrupt_disable(void);
void uart2_rxne_interrupt_enable(void);
void uart2_rxne_interrupt_disable(void);

void uart6_tx_rx_init(void);
void uart6_write(char c);
char uart6_read(void);
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
