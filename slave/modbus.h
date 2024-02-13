#include "crc.h"

#define  MAX_REQUEST_DATA_SIZE  256
#define  MAX_RESPONSE_DATA_SIZE 256


extern uint8_t RxData[MAX_REQUEST_DATA_SIZE];
extern uint8_t TxData[MAX_RESPONSE_DATA_SIZE];


#define ILLEGAL_FUNCTION       0x01
#define ILLEGAL_DATA_ADDRESS   0x02
#define ILLEGAL_DATA_VALUE     0x03

#define SIZE 100
#define SLAVE_ID 0x07

struct modbus{
	uint8_t slave_add;
	uint8_t fcode;
	
	uint16_t reg_add;
 
	uint16_t byte_count;
	uint16_t points_count;
	
	uint8_t data[SIZE];
	uint8_t crc_lo;
	uint8_t crc_hi;
	
};


static uint16_t Holding_Registers_Database[50]={
		0000,  1111,  2222,  3333,  4444,  5555,  6666,  7777,  8888,  9999,   // 0-9   40001-40010
		12345, 15432, 15535, 10234, 19876, 13579, 10293, 19827, 13456, 14567,  // 10-19 40011-40020
		21345, 22345, 24567, 25678, 26789, 24680, 20394, 29384, 26937, 27654,  // 20-29 40021-40030
		31245, 31456, 34567, 35678, 36789, 37890, 30948, 34958, 35867, 36092,  // 30-39 40031-40040
		45678, 46789, 47890, 41235, 42356, 43567, 40596, 49586, 48765, 41029,  // 40-49 40041-40050
};

static const uint16_t Input_Registers_Database[50]={
		0000,  1111,  2222,  3333,  4444,  5555,  6666,  7777,  8888,  9999,   // 0-9   40001-40010
		12345, 15432, 15535, 10234, 19876, 13579, 10293, 19827, 13456, 14567,  // 10-19 40011-40020
		21345, 22345, 24567, 25678, 26789, 24680, 20394, 29384, 26937, 27654,  // 20-29 40021-40030
		31245, 31456, 34567, 35678, 36789, 37890, 30948, 34958, 35867, 36092,  // 30-39 40031-40040
		45678, 46789, 47890, 41235, 42356, 43567, 40596, 49586, 48765, 41029,  // 40-49 40041-40050
};
static uint8_t Coils_Database[25]={
		0b01001001, 0b10011100, 0b10101010, 0b01010101, 0b11001100,    // 0-39    1-40
		0b10100011, 0b01100110, 0b10101111, 0b01100000, 0b10111100,    // 40-79   41-80
		0b11001100, 0b01101100, 0b01010011, 0b11111111, 0b00000000,    // 80-119  81-120
		0b01010101, 0b00111100, 0b00001111, 0b11110000, 0b10001111,    // 120-159 121-160
		0b01010100, 0b10011001, 0b11111000, 0b00001101, 0b00101010,    // 160-199 161-200
};
static const uint8_t Inputs_Database[25]={
		0b01001001, 0b10011100, 0b10101010, 0b01010101, 0b11001100,    // 0-39    10001-10040
		0b10100011, 0b01100110, 0b10101111, 0b01100000, 0b10111100,    // 40-79   10041-10080
		0b11001100, 0b01101100, 0b01010011, 0b11111111, 0b00000000,    // 80-119  10081-10120
		0b01010101, 0b00111100, 0b00001111, 0b11110000, 0b10001111,    // 120-159 10121-10160
		0b01010100, 0b10011001, 0b11111000, 0b00001101, 0b00101010,    // 160-199 10161-10200
};


void read_holding_register(struct modbus packet);
void send_pkt(struct modbus packet);

/*
uint8_t readInputRegs (void);

uint8_t readCoils (void);
uint8_t readInputs (void);
uint8_t writeSingleReg (void);
uint8_t writeHoldingRegs (void);
uint8_t writeSingleCoil (void);


void modbusException (uint8_t exceptioncode);
uint8_t writeMultiCoils (void);
*/

