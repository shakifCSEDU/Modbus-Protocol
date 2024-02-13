#include "modbus.h"
#include "uart.h"

uint8_t TxData[100];

void send_pkt(struct modbus pkt){
	rs485_tx_enable();
	
	uint8_t* c = (uint8_t*) &pkt;
	
	int itr = pkt.byte_count+8;
	
	int i = 0;
	
	while(i < itr){
		uart6_write(*(c+i));
		++i;
	}
	// now transmit crc_hi,crc_lo 
	uart6_write(*(c+i+(SIZE-pkt.byte_count)));
	++i;
	
	uart6_write(*(c+i+(SIZE-pkt.byte_count)));
	++i;
	

	
	
	while(!(USART6->SR & (1 << 6))) {}
	rs485_tx_disable();
		
	uint8_t tmp = 0xFF & USART6->DR;
	uart6_rxne_interrupt_enable();
}

void read_holding_register(struct modbus pkt1,uint8_t slave_add){
	
	//Prepare for data
	//SId|Fcode|RAdd(H)|RAdd(L)|#of wants(H)|#of wants(L)|Crc(L)|Crc(h)
	pkt1.slave_add = slave_add;// slave address
	pkt1.fcode = 0x03+slave_add; // Function code for Read Holding Registers

  
  pkt1.reg_add = 0x04;//The Register address will be 00000000 00000100 = 4 + 40001 = 40005

  pkt1.byte_count = 0x05;// no of registers to read will be 00000000 00000101 = 5 Registers = 10 Bytes
  
	pkt1.points_count = 0x02;
	
  for(int i = 0 ; i < pkt1.byte_count ; i++){
		pkt1.data[i] = (0xFF & (i+1));
	}
	
	uint16_t crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
	pkt1.crc_lo = crc&0xFF;
	pkt1.crc_hi = (crc>>8)&0xFF;
	send_pkt(pkt1);
}





void read_input_register(struct modbus pkt1){
	pkt1.slave_add = 0x05;  // slave address
  pkt1.fcode = 0x04;  // Function code for Reading inputs

  pkt1.reg_add = 0x01;//The Register address will be 00000000 00000001 = 1 +30001 = 30002

  pkt1.byte_count = 0x05;
  // no of inputs to be read will be 00000000 00001010 = 10 bits = 2 Bytes

  uint16_t crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
	pkt1.crc_lo = crc&0xFF;
	pkt1.crc_hi = (crc>>8)&0xFF;
  send_pkt(pkt1);
	
}


void read_coil(struct modbus pkt1){
	 pkt1.slave_add = 0x05;  // slave address
	 pkt1.fcode = 0x01;  // Function code for Read coils

  
  // The function code 0x01 means we are reading coils
  // The coil address ranges from 1 - 10000
  // The address we input can range from 0-9999 (0x00-0x270F)
   // Here 0 corresponds to the Address 1 and 9999 corresponds to 10000
  // Although we can only read 2000 coils sequentially at once

   pkt1.reg_add = 0x01;
  //The coil address will be 00000000 00000000 = 0 + 1 = 1

  pkt1.byte_count = 8;
  // no of coils/inputs to read will be 00000000 00001000 = 8 coils = 2 Byte

  uint16_t crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
	pkt1.crc_lo = crc&0xFF;
	pkt1.crc_hi = (crc>>8)&0xFF;
  send_pkt(pkt1);
}


void read_discrete_inputs(struct modbus pkt1){
	pkt1.slave_add = 0x05;  // slave address
  pkt1.fcode = 0x02;  // Function code for Reading inputs


  
   // The function code 0x02 means we are reading inputs
   // The input address ranges from 10001 - 20000
   // The address master provides can range from 0-9999 (0x00-0x270F)
   // Here 0 corresponds to the Address 10001 and 9999 corresponds to 20000
   // Although we can only read 2000 inputs sequentially at once
  pkt1.reg_add =  0x01;
  //The input address will be 00000000 00000000 = 1 + 10001 = 10002

  pkt1.byte_count =  10;
  // no of inputs to be read will be 00000000 00001010 = 10 bits = 2 Bytes
	uint16_t crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
	pkt1.crc_lo = crc&0xFF;
	pkt1.crc_hi = (crc>>8)&0xFF;
	
  send_pkt(pkt1);
}



void write_single_coil(struct modbus pkt1){
	
	pkt1.slave_add = 0x05;  // slave address
	pkt1.fcode = 0x05;  // Force single coil

	pkt1.reg_add = 0;  // coil address low
//The coil address will be 00000000 00000000 = 0 + 1 = 1

	pkt1.data[0] = 0xFF;  // force data high

	uint16_t crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
	pkt1.crc_lo = crc&0xFF;
	pkt1.crc_hi = (crc>>8)&0xFF;
	
	send_pkt(pkt1);
}



void write_single_register(struct modbus pkt1){
	pkt1.slave_add = 0x05;  // slave address
	pkt1.fcode = 0x06;  // Preset single Register

	pkt1.reg_add =  2;  // Register address 
//The coil address will be 00000000 00000101 = 2 + 40001 = 40003

	pkt1.data[0] = 0x12;  // Preset data high
	pkt1.data[1] = 0x34;  // Preset data low

  uint16_t crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
	pkt1.crc_lo = crc&0xFF;
	pkt1.crc_hi = (crc>>8)&0xFF;
	
	send_pkt(pkt1);
}



void write_multiple_coils(struct modbus pkt1){
	pkt1.slave_add = 0x05;  // slave address
	pkt1.fcode = 0x0F;  // Force Multiple coils

	pkt1.reg_add = 3;  // coil address low
	//The coil address will be 00000000 00000011 = 3 + 1 = 4

	pkt1.points_count = 0x0F;  // no. of coils low
	// Total no. of coils = 00000000 00001111 = 15 coils

	pkt1.byte_count = 0x02; // Byte count (15 coils would need 2 bytes)

	pkt1.data[0] = 0x35;  // Data for first 8 coils
	pkt1.data[1] = 0x74;  // Data for next 8 coils (actually 7 coils)

	uint16_t crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
	pkt1.crc_lo = crc&0xFF;
	pkt1.crc_hi = (crc>>8)&0xFF;
	
	send_pkt(pkt1);
}    



void write_multiple_registers(struct modbus pkt1){
	pkt1.slave_add = 0x05;  // slave address
	pkt1.fcode = 0x10;  // Preset multiple Registers

	pkt1.reg_add = 0;  // Register address low
//The coil address will be 00000000 00000000 = 0 + 40001 = 40001

	
	 pkt1.points_count = 0x03;  // no. of Registers low
	// Total no. of Registers = 00000000 00000011 = 3 Registers

	pkt1.byte_count = 6; // Byte count (3 Registers would need 6 bytes (2 bytes per register))

	pkt1.data[0] = 0x12;  // Data High for first Register
	pkt1.data[1] = 0x34;  // Data low for first Register

	pkt1.data[2] = 0x56;  // Data High for 2nd Register
	pkt1.data[3] = 0x78;  // Data low for 2nd Register

	pkt1.data[4] = 0xab;  // Data High for 3rd Register
	pkt1.data[5] = 0xcd;  // Data low for 3rd Register

	
	uint16_t crc = CRC16((uint8_t*)&pkt1, sizeof(pkt1)-2);
	pkt1.crc_lo = crc&0xFF;
	pkt1.crc_hi = (crc>>8)&0xFF;
	send_pkt(pkt1);
}







