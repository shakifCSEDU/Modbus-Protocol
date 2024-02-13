#include<stdint.h>
#include "rs485_config.h"
#include "uart.h"
#define SIZE 100




//SId|Fcode|RAdd(H)|RAdd(L)|#of wants(H)|#of wants(L)|Crc(L)|Crc(h)




struct modbus{
	uint8_t slave_add;
	uint8_t fcode;
	
	uint16_t reg_add;
 
	uint16_t byte_count;
	uint16_t points_count;
	
	
	uint8_t data[SIZE];
	
	
	uint8_t crc_lo;
	uint8_t crc_hi

};

void read_holding_register(struct modbus pkt1,uint8_t slave_add);

void read_input_register(struct modbus pkt1);

void read_coil(struct modbus pkt1);
void read_discrete_inputs(struct modbus pkt1);

void write_single_coil(struct modbus pkt1);
void write_single_register(struct modbus pkt1);
void write_multiple_coils(struct modbus pkt1);
void write_multiple_registers(struct modbus pkt1);

void send_pkt(struct modbus pkt);