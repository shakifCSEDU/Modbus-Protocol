#include "uart.h"
#include "crc.h"
//#include "systick.h"
#include "modbus.h"

static struct modbus packet,packet2;

static uint32_t pkt_size = sizeof(packet);
static uint8_t* ptr = (uint8_t*) &packet;
static uint8_t* ptr2 = (uint8_t*) &packet2;



static uint32_t cnt = 0;


// debug counters
static uint32_t pkt_cnt = 0;
static uint32_t error_pkt_cnt = 0;
static uint32_t flushed_pkt_cnt = 0;
static uint32_t error_byte_cnt = 0;

static uint8_t rx_tmp;


void uart6_rx_callback() {

	//*(ptr+cnt) = (uint8_t)USART6->DR;
	//++cnt;
	rx_tmp = (uint8_t)USART6->DR;
	
	// slave add
	if(cnt == 0 && rx_tmp == SLAVE_ID){
		*(ptr+cnt) = rx_tmp;
		++cnt;
	}
	
	// fc_code to point_count
	else if(cnt >=1 && cnt<=7){
		*(ptr+cnt) = rx_tmp;
		++cnt;
	}
	
	//data array
	else if(cnt>7 && cnt<=7+packet.byte_count){
		*(ptr+cnt) = rx_tmp;
		++cnt;
	}
	
	// crc lo
	else if(cnt == 7+packet.byte_count+1){
		*(ptr + cnt +(SIZE-packet.byte_count)) = rx_tmp;
		++cnt;
	}
	// crc_hi
	else if(cnt == 7+packet.byte_count+2){
		*(ptr + cnt +(SIZE-packet.byte_count)) = rx_tmp;
		cnt = 0;
		
		uart6_rxne_interrupt_disable();
		
		
		// slaveId
		uart2_write_str("Slave : ");
		uart2_write_int(packet.slave_add);
		
		// fCode
		uart2_write_str(" Fcode : ");
		uart2_write_int(packet.fcode);
		
		// regAddress
		uart2_write_str(" RegAddress : ");
		uart2_write_int(packet.reg_add);
		uart2_write_str("\n");
		
		send_response();
	}
	else{
		cnt = 0;
	}
	
	
}

//uint8_t check_crc() {
	//if (CRC16(ptr, sizeof(packet)-2) == packet.crc) return 1;
	//return 0;
//}

void send_response() {
	
	
	
	packet2.slave_add = SLAVE_ID;
	packet2.fcode = 0x03;
	packet2.reg_add = 0x04;
	
	packet2.byte_count = 0x05;
	packet2.points_count = 0x02;
	
	for(int i = 0 ; i<packet2.byte_count ; i++){
		packet2.data[i] = 0xFF & (i+1);
	}
	
	
	ttl_tx_enable();
	
	uint32_t itr = 8+packet2.byte_count;
	uint32_t i = 0 ;
	
	while(i<itr){
		uart6_write(*(ptr2+i));
		++i;
	}
	
	// crc_lo
	uart6_write(*(ptr2+i+(SIZE-packet2.byte_count)));
	++i;
	
	// crc hi
	uart6_write(*(ptr2+i+(SIZE-packet2.byte_count)));
	++i;
	
	// waiting for TC for last byte
	while(!(USART6->SR & (1 << 6))) {}
	ttl_tx_disable();
		
	uint8_t tmp = USART6->DR & 0xFF;
	uart6_rxne_interrupt_enable();
}
void send_error_response() {
	packet.fcode = 10;
	send_response();
}
void fill_debug_data() {
	// fill packet count in data[0:3];
	uint32_t tmp = pkt_cnt;
	for (uint8_t i = 0; i < 4; ++i) {
		packet.data[i] = tmp & 0xFF;
		tmp = tmp >> 8;
	}
	// fill error packet count in data[4:7];
	tmp = error_pkt_cnt;
	for (uint8_t i = 4; i < 8; ++i) {
		packet.data[i] = tmp & 0xFF;
		tmp = tmp >> 8;
	}
	// fill flushed packet count in data[8:11];
	tmp = flushed_pkt_cnt;
	for (uint8_t i = 8; i < 12; ++i) {
		packet.data[i] = tmp & 0xFF;
		tmp = tmp >> 8;
	}
	// fill error byte count in data[12:15];
	tmp = error_byte_cnt;
	for (uint8_t i = 12; i < 16; ++i) {
		packet.data[i] = tmp & 0xFF;
		tmp = tmp >> 8;
	}
}
void reset_debug_counters() {
	pkt_cnt = 0;
	error_pkt_cnt = 0;
	flushed_pkt_cnt = 0;
	error_byte_cnt = 0;
}

void led_on() {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA-> MODER |= (1<<10);
	GPIOA->BSRR |= (1<<5);
}
void led_off() {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA-> MODER |= (1<<10);
	GPIOA->BSRR |= (1<<21);
}

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
	//EIE bit set
	USART6-> CR3 |= (1 << 0);
	// uart enable, rx enable, tx enable
	USART6-> CR1 |= (1<<13) | (1<<3) | (1<<2);
	
	// enable uart6 interrupt in NVIC
	NVIC_EnableIRQ(USART6_IRQn);
}

/*
// ISR for UART2 TXE and RXNE interrupt
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
	// error
	if ((USART6->SR & (1 << 1)) || (USART6->SR & (1 << 2)) || (USART6->SR & (1 << 3))) {
		uint8_t tmp = (USART6->DR & 0xFF);
		error_byte_cnt++;
	}
	// if interrupt is for RXNE
	if (USART6-> SR & 1<<5) {
		uart6_rx_callback();
	}
}

// synchronous byte transfer(without interrupt)

void uart2_write(char c) {		
	while(!(USART2->SR & (1 << 7))) {}
	USART2-> DR = c;
}
char uart2_read() {
	while(!(USART2->SR & (1 << 5))) {}
	return (char) USART2-> DR;
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
void uart6_write(char c) {
	while(!(USART6-> SR & (1 << 7))) {}
	USART6-> DR = c;
}
char uart6_read() {
	while(!(USART6->SR & (1 << 5))) {}
	return (char) USART6-> DR;
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



