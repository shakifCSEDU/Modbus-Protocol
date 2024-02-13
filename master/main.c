#include<stm32f446xx.h>
#include<stdint.h>
#include "uart.h"
#include "crc.h"
#include "systick.h"
#include "test.h"
#include "modbus.h"
#include "rs485_config.h"

void init(void);
uint8_t read_cmd(void);
uint8_t read_str(void);
uint8_t clear_rdr(void);
void request(uint8_t dst, uint8_t fc);
void make_pkt(uint8_t dst, uint8_t fc);
//void send_pkt(void);
void rec_pkt(void);
void show(void);

uint8_t check_crc(void);

static struct modbus pkt1, pkt2;

static uint8_t* pkt1_ptr = (uint8_t*)&pkt1;
static uint8_t* pkt2_ptr = (uint8_t*)&pkt2;

static uint32_t pkt2_cnt = 0;

static uint8_t response_received = 0;
static uint8_t response_timeout = 0;

static uint8_t rx_tmp;

static uint8_t dest, fc;
static char dr[15];

static uint8_t slave_add;
static uint8_t response_flag = 1;
	




// Master

int main() {
	init();  
	//test();
	
	//read_holding_register(pkt1);
	sysTickTimer(1000);
	
	slave_add = 0x06;
	
	while(1) {	
		
	if(response_flag)
	{
		read_holding_register(pkt1,slave_add);
		response_flag = 0;
	}
	
	ms_delay(500);
	
}
}

/*
void request(uint8_t _dest, uint8_t _fc) {
	// if it is a braodcasting command 
	if (_dest == broadcast_addr) {
		make_pkt(_dest, _fc);
		send_pkt();
	}
	// else if it is a specific query command
	else {
		make_pkt(_dest, _fc);
		send_pkt();
		pkt2_cnt=0;
		response_received = 0;
		response_timeout = 0;
		
		uart6_rxne_interrupt_enable();
		
		sysTickTimer(1000);
		
		while(!response_received && !response_timeout) {}
		
		uart2_write_str("Response -> ");
			
		if (response_timeout) {
			uart2_write_str("Response timed out\n\n");
		}
		else {
			// response received
			uart2_write_str("Reponse from ");
			uart2_write_int(pkt2.src);
			uart2_write_str(":\n");
			if (check_crc() == 0) {
				uart2_write_str("CRC error\n\n");
			}
			else if (pkt2.fc == 10) {
				uart2_write_str("Request error\n\n");
			}
			else if (pkt2.fc == 0) {
				show();
			}
			else {
				uart2_write_str("Ok\n\n");
			}
		}
	}
}

// for each incoming valid byte this subroutine gets called
*/


void uart6_rx_callback() {
	// read byte
	//*(pkt2_ptr+pkt2_cnt++) = (uint8_t) USART6->DR;
	//if(pkt2_cnt == 1 && *pkt2_ptr != (0x05))pkt2_cnt--;
	rx_tmp = (uint8_t)USART6->DR;
	// slave add
	if(pkt2_cnt == 0 && ((rx_tmp == 0x05) ||(rx_tmp == 0x06)||(rx_tmp == 0x07) ) ){
		*(pkt2_ptr+pkt2_cnt) = rx_tmp;
		++pkt2_cnt;
		return;
	}
	// fc_code to point_count
	if(pkt2_cnt>=1 && pkt2_cnt<=7){
		*(pkt2_ptr+pkt2_cnt) = rx_tmp;
		++pkt2_cnt;
		return;
	}
	
	// data array
	if(pkt2_cnt>7 && pkt2_cnt<= 7+pkt2.byte_count){
		*(pkt2_ptr+pkt2_cnt) = rx_tmp;
		++pkt2_cnt;
		return;
	}
	
	//crc_lo
	if(pkt2_cnt == 7+pkt2.byte_count+1){
		*(pkt2_ptr+pkt2_cnt) = rx_tmp;
		++pkt2_cnt;
		return;
	}
	
	//crc_hi
	if(pkt2_cnt == 7+pkt2.byte_count+2){
		*(pkt2_ptr+pkt2_cnt) = rx_tmp;
		pkt2_cnt = 0;
		
		// slaveId
		uart2_write_str("Slave : ");
		uart2_write_int(pkt2.slave_add);
		
		// fCode
		uart2_write_str(" Fcode : ");
		uart2_write_int(pkt2.fcode);
		
		// regAddress
		uart2_write_str(" RegAddress : ");
		uart2_write_int(pkt2.reg_add);
		uart2_write_str("\n");
		response_flag = 1;
		return;
	}
	
	else{
		pkt2_cnt = 0;
	}
	
}




// after one second of issuing a query command this subroutine gets called 
/*
void systick_interrupt() {
	sysTickDisable();
	uart6_rxne_interrupt_disable();
	NVIC_ClearPendingIRQ(USART6_IRQn);
	response_timeout = 1;
}

	




uint8_t check_crc() {
	if (CRC16(pkt2_ptr, sizeof(pkt2)-2) == pkt2.crc) return 1;
	return 0;
}
*/
/*
void make_pkt(uint8_t _dest, uint8_t _fc){
	//pkt1.start = start_seq;
	//pkt1.src = 0;
	//pkt1.dest = _dest;
 // pkt1.fc = _fc;
	for(uint32_t i = 0; i < sizeof(pkt1)-6; i++) {
			pkt1.data[i] = (uint8_t) i;
	}
	pkt1.crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
}

// blocking send packet function
void send_pkt(){
	ttl_tx_enable();
	uint8_t* c = (uint8_t*) &pkt1;
	int itr = sizeof(pkt1);
	int i = 0;
	while(i < itr) {
		uart6_write(*(c+i));
		++i;
	}
	while(!(USART6->SR & (1 << 6))) {}
	ttl_tx_disable();
}

void show() {
	uint32_t tmp = 0;
	for (int i = 0; i < 4; ++i) {
		tmp += (uint32_t)(pkt2.data[0+i]<<(8*i));
	}
	uart2_write_str("Packet count: ");
	uart2_write_int(tmp);
	uart2_write_str("\n");
	
	tmp = 0;
	for (int i = 0; i < 4; ++i) {
		tmp += (uint32_t)(pkt2.data[4+i]<<(8*i));
	}
	uart2_write_str("Error packet count: ");
	uart2_write_int(tmp);
	uart2_write_str("\n");
	
	tmp = 0;
	for (int i = 0; i < 4; ++i) {
		tmp += (uint32_t)(pkt2.data[8+i]<<(8*i));
	}
	uart2_write_str("Flushed packet count: ");
	uart2_write_int(tmp);
	uart2_write_str("\n");
	
	tmp = 0;
	for (int i = 0; i < 4; ++i) {
		tmp += (uint32_t)(pkt2.data[12+i]<<(8*i));
	}
	uart2_write_str("Error byte count: ");
	uart2_write_int(tmp);
	uart2_write_str("\n");
	
}

// blocking receive packet function
void rec_pkt() {
	uint8_t* c = (uint8_t*) &pkt2;
	int itr = sizeof(pkt2);
	int i = 0;
	while(i < itr) {
		*(c+i) = uart6_read();
		++i;
	}
}
**/
// this function reads two integers from terminal
uint8_t read_cmd(){
	uint8_t len = read_str()-1;
	uint8_t cnt = 0;
	dest = 0;
	while(cnt < len && dr[cnt] >= '0' && dr[cnt] <= '9') {
		dest *= 10;
		dest += dr[cnt++]-'0';
	}
	if (cnt == 0) return 0;
	if (cnt == len) return 0;
	if (dr[cnt] != ' ') return 0;
	++cnt;
	if (cnt == len) return 0;
	fc = 0;
	while(cnt < len && dr[cnt] >= '0' && dr[cnt] <= '9') {
		fc *= 10;
		fc += dr[cnt++]-'0';
	}
	if (cnt < len) return 0;
	return 1;
}

// this function read a string from uart2 rx
uint8_t read_str() {
	  
	uint8_t cnt = 0;
  uint8_t c = 0;
	
	while(cnt != 20 && c != '\n' && c != ".") {
		c = uart2_read();
		dr[cnt++] = c;
	}
	
	if (cnt == 20) dr[cnt++] = '\n';
	return cnt;
}


// using PA0 as ttl DE/RE controller
// PA0 high -> DE high, RE low -> TX enabled
// PA0 low -> DE low, RE high -> RX enabled


uint8_t clear_rdr() {
		// clear RDR if it has a byte
		if((USART6->SR & (1 << 5))) {
			uint8_t c = (uint8_t)USART6->DR;
			//uart2_write(c);
			return 1;
		}
		return 0;                                   
}


void init() {
	uart2_tx_rx_init();
	uart6_tx_rx_init();
	uart6_rxne_interrupt_disable();
	rs485_init();
}

/*
void test() {
	// wait for some time
	for (int i = 0; i < 1000000; ++i) {}
	// Mass broadcast for testing.
	uint32_t message_cnt = 0;
	while(1) {
		// something in uart2_rx available then scan
		if ((USART2->SR & (1 << 5))) {
			uint8_t now = (uint8_t)USART2->DR;
			if (now == '\n') {
				break;
			}
		}
		request(broadcast_addr, 0);
		++message_cnt;
	}
	uart2_write_str("Broadcasted messsage cnt: ");
	uart2_write_int(message_cnt);
	uart2_write_str("\n\n");
	// 0 is a query function code to retrive debug info i.e. pkt_cnt, byte_cnt, err_cnt etc.
	for(uint8_t i = 1; i <= 9; i++) {
		uart2_write_str("Request -> Destination: ");
		uart2_write_int(i);
		uart2_write_str(", Function code: ");
		uart2_write_int(0);
		uart2_write_str("\n");
		request(i, 0);
		uart2_write_str("\n\n");
	}
}
***/
