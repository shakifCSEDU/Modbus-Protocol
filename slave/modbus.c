#include "modbus.h"
#include<stdint.h>
#include "uart.h"
uint8_t Tx[100];

void send_pkt(struct modbus packet)
{
	uint8_t* ptr2 = (uint8_t*) &packet;
	ttl_tx_enable();
	
	// send reponse packet
	uint32_t itr =sizeof(packet);
	uint32_t i = 0;
	
	uart6_write(*(ptr2+i));
	++i;
	
	uart6_write(*(ptr2+i));
	++i;
	
	uart6_write(*(ptr2+i));
	++i;
	
	itr-=3;
	
	while(i < itr) {
			uart6_write(*(ptr2+i));
			++i;
	}
	// waiting for TC for last byte
	while(!(USART6->SR & (1 << 6))) {}
	ttl_tx_disable();
		
	uint8_t tmp = USART6->DR & 0xFF;
	uart6_rxne_interrupt_enable();
}

void read_holding_register(struct modbus packet)
{
	  uart2_write_str("Slave : ");
		uart2_write_int(packet.slave_add);
		
		uart2_write_str(" Function code: ");
		uart2_write_int(packet.fcode);
		
		uart2_write_str("Register address : ");
		uart2_write_int(packet.reg_add);
	
	
	uint16_t startAddr = packet.reg_add;  // start Register Address

	uint16_t numRegs = packet.byte_count;   // number to registers master has requested
	/*
	if ((numRegs<1)||(numRegs>125))  // maximum no. of Registers as per the PDF
	{
		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
		return 0;
	}
*/
	uint16_t endAddr = startAddr+numRegs-1;  // end Register
	
	/*
	if (endAddr>49)  // end Register can not be more than 49 as we only have record of 50 Registers in total
	{
		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
		return 0;
	}
	*/
	
	 // Prepare TxData buffer

 //| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
 //| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |

 int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer

 for (int i=0; i<numRegs; i++)   // Load the actual data into TxData buffer
 {
   packet.data[i] = (Holding_Registers_Database[startAddr]>>8)&0xFF;  // extract the higher byte
   packet.data[i+1] = (Holding_Registers_Database[startAddr])&0xFF;   // extract the lower byte
   startAddr++;  // increment the register address
 }
  uint16_t crc = CRC16((uint8_t*)&packet, sizeof(packet)-2);
	packet.crc_lo = crc&0xFF;
	packet.crc_hi = (crc>>8)&0xFF;
	
 send_pkt(packet);  // send data... CRC will be calculated in the function itself
 
}





/*

void modbusException (uint8_t exceptioncode)
{
	//| SLAVE_ID | FUNCTION_CODE | Exception code | CRC     |
	//| 1 BYTE   |  1 BYTE       |    1 BYTE      | 2 BYTES |

	TxData[0] = RxData[0];       // slave ID
	TxData[1] = RxData[1]|0x80;  // adding 1 to the MSB of the function code
	TxData[2] = exceptioncode;   // Load the Exception code
	//sendData(TxData, 3);         // send Data... CRC will be calculated in the function
}




uint8_t readCoils (void)
{
	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Coil Address

	uint16_t numCoils = ((RxData[4]<<8)| RxData[5]);   // number to coils master has requested
	if ((numCoils<1)||(numCoils>2000))  // maximum no. of coils as per the PDF
	{
		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
		return 0;
	}

	uint16_t endAddr = startAddr+numCoils-1;  // Last coils address
	if (endAddr>199)  // end coil can not be more than 199 as we only have record of 200 (0-199) coils in total
	{
		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
		return 0;
	}


	//reset TxData buffer
	memset (TxData, '\0', 256);

	// Prepare TxData buffer

	//| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
	//| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |

	TxData[0] = SLAVE_ID;  // slave ID
	TxData[1] = RxData[1];  // function code
	TxData[2] = (numCoils/8) + ((numCoils%8)>0 ? 1:0);  // Byte count
	int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer

	/* The approach is simple. We will read 1 bit at a time and store them in the Txdata buffer.
	 * First find the offset in the first byte we read from, for eg- if the start coil is 13,
	 * we will read from database[1] with an offset of 5. This bit will be stored in the TxData[0] at 0th position.
	 * Then we will keep shifting the database[1] to the right and read the bits.
	 * Once the bitposition has crossed the value 7, we will increment the startbyte
	 * When the indxposition exceeds 7, we increment the indx variable, so to copy into the next byte of the TxData
	 * This keeps going until the number of coils required have been copied
	 */
/*	 
	int startByte = startAddr/8;  // which byte we have to start extracting the data from
	uint16_t bitPosition = startAddr%8;  // The shift position in the first byte
	int indxPosition = 0;  // The shift position in the current indx of the TxData buffer

	// Load the actual data into TxData buffer
	for (int i=0; i<numCoils; i++)
	{
		TxData[indx] |= ((Coils_Database[startByte] >> bitPosition) &0x01) << indxPosition;
		indxPosition++; bitPosition++;
		if (indxPosition>7)  // if the indxposition exceeds 7, we have to copy the data into the next byte position
		{
			indxPosition = 0;
			indx++;
		}
		if (bitPosition>7)  // if the bitposition exceeds 7, we have to increment the startbyte
		{
			bitPosition=0;
			startByte++;
		}
	}

	if (numCoils%8 != 0)indx++;  // increment the indx variable, only if the numcoils is not a multiple of 8
	//sendData(TxData, indx);  // send data... CRC will be calculated in the function itself
	return 1;   // success
}

uint8_t readInputs (void)
{
	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address

	uint16_t numCoils = ((RxData[4]<<8)|RxData[5]);   // number to coils master has requested
	if ((numCoils<1)||(numCoils>2000))  // maximum no. of coils as per the PDF
	{
		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
		return 0;
	}

	uint16_t endAddr = startAddr+numCoils-1;  // Last coils address
	if (endAddr>199)  // end coil can not be more than 199 as we only have record of 200 (0-199) coils in total
	{
		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
		return 0;
	}


	//reset TxData buffer
	memset (TxData, '\0', 256);

	// Prepare TxData buffer

	//| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
	//| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |

	//TxData[0] = SLAVE_ID;  // slave ID
	TxData[1] = RxData[1];  // function code
	TxData[2] = (numCoils/8) + ((numCoils%8)>0 ? 1:0);  // Byte count
	int indx = 3;  // we need to keep track of how many bytes has been stored in TxData Buffer

	/* The approach is simple. We will read 1 bit at a time and store them in the Txdata buffer.
	 * First find the offset in the first byte we read from, for eg- if the start coil is 13,
	 * we will read from database[1] with an offset of 5. This bit will be stored in the TxData[0] at 0th position.
	 * Then we will keep shifting the database[1] to the right and read the bits.
	 * Once the bitposition has crossed the value 7, we will increment the startbyte
	 * When the indxposition exceeds 7, we increment the indx variable, so to copy into the next byte of the TxData
	 * This keeps going until the number of coils required have been copied
	 */
	/*
	int startByte = startAddr/8;  // which byte we have to start extracting the data from
	uint16_t bitPosition = startAddr%8;  // The shift position in the first byte
	int indxPosition = 0;  // The shift position in the current indx of the TxData buffer

	// Load the actual data into TxData buffer
	for (int i=0; i<numCoils; i++)
	{
		TxData[indx] |= ((Inputs_Database[startByte] >> bitPosition) &0x01) << indxPosition;
		indxPosition++; bitPosition++;
		if (indxPosition>7)  // if the indxposition exceeds 7, we have to copy the data into the next byte position
		{
			indxPosition = 0;
			indx++;
		}
		if (bitPosition>7)  // if the bitposition exceeds 7, we have to increment the startbyte
		{
			bitPosition=0;
			startByte++;
		}
	}

	if (numCoils%8 != 0)indx++;  // increment the indx variable, only if the numcoils is not a multiple of 8
	//sendData(TxData, indx);  // send data... CRC will be calculated in the function itself
	return 1;   // success
}
uint8_t writeSingleReg (void)
{
	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address

	if (startAddr>49)  // The Register Address can not be more than 49 as we only have record of 50 Registers in total
	{
		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
		return 0;
	}

	Holding_Registers_Database[startAddr] = (RxData[4]<<8)|RxData[5];

	// Prepare Response

	//TxData[0] = SLAVE_ID;    // slave ID
	TxData[1] = RxData[1];   // function code
	TxData[2] = RxData[2];   // Start Addr HIGH Byte
	TxData[3] = RxData[3];   // Start Addr LOW Byte
	TxData[4] = RxData[4];   // Reg Data HIGH Byte
	TxData[5] = RxData[5];   // Reg Data LOW  Byte

	//sendData(TxData, 6);  // send data... CRC will be calculated in the function itself
	return 1;   // success
}
uint8_t writeHoldingRegs (void)
{
	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Register Address

	uint16_t numRegs = ((RxData[4]<<8)|RxData[5]);   // number to registers master has requested
	if ((numRegs<1)||(numRegs>123))  // maximum no. of Registers as per the PDF
	{
		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
		return 0;
	}

	uint16_t endAddr = startAddr+numRegs-1;  // end Register
	if (endAddr>49)  // end Register can not be more than 49 as we only have record of 50 Registers in total
	{
		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
		return 0;
	}
	int indx = 7;  // we need to keep track of index in RxData
	for (int i=0; i<numRegs; i++)
	{
		Holding_Registers_Database[startAddr++] = (RxData[indx++]<<8)|RxData[indx++];
	}

	// Prepare Response

	//| SLAVE_ID | FUNCTION_CODE | Start Addr | num of Regs    | CRC     |
	//| 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES      | 2 BYTES |

	//TxData[0] = SLAVE_ID;    // slave ID
	TxData[1] = RxData[1];   // function code
	TxData[2] = RxData[2];   // Start Addr HIGH Byte
	TxData[3] = RxData[3];   // Start Addr LOW Byte
	TxData[4] = RxData[4];   // num of Regs HIGH Byte
	TxData[5] = RxData[5];   // num of Regs LOW Byte

	//sendData(TxData, 6);  // send data... CRC will be calculated in the function itself
	return 1;   // success
}



uint8_t writeSingleCoil (void)
{
	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Coil Address

	if (startAddr>199)  // The Coil Address can not be more than 199 as we only have record of 200 Coils in total
	{
		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
		return 0;
	}

	/* Calculation for the bit in the database, where the modification will be done */
/*	
	int startByte = startAddr/8;  // which byte we have to start writing the data into
	uint16_t bitPosition = startAddr%8;  // The shift position in the first byte
/* The next 2 bytes in the RxData determines the state of the coil
	 * A value of FF 00 hex requests the coil to be ON.
	 * A value of 00 00 requests it to be OFF.
	 * All other values are illegal and will not affect the coil.
	 */

/*	if ((RxData[4] == 0xFF) && (RxData[5] == 0x00))
	{
		Coils_Database[startByte] |= 1<<bitPosition; // Replace that bit with 1
	}

	else if ((RxData[4] == 0x00) && (RxData[5] == 0x00))
	{
		Coils_Database[startByte] &= ~(1<<bitPosition); // Replace that bit with 0
	}
	
	
	// Prepare Response

	//| SLAVE_ID | FUNCTION_CODE | Start Addr | Data     | CRC     |
	//| 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES  | 2 BYTES |

	//TxData[0] = SLAVE_ID;    // slave ID
	TxData[1] = RxData[1];   // function code
	TxData[2] = RxData[2];   // Start Addr HIGH Byte
	TxData[3] = RxData[3];   // Start Addr LOW Byte
	TxData[4] = RxData[4];   // Coil Data HIGH Byte
	TxData[5] = RxData[5];   // Coil Data LOW  Byte

	//sendData(TxData, 6);  // send data... CRC will be calculated in the function itself

}
uint8_t writeMultiCoils (void)
{
	uint16_t startAddr = ((RxData[2]<<8)|RxData[3]);  // start Coil Address

	uint16_t numCoils = ((RxData[4]<<8)|RxData[5]);   // number to coils master has requested
	if ((numCoils<1)||(numCoils>1968))  // maximum no. of coils as per the PDF
	{
		modbusException (ILLEGAL_DATA_VALUE);  // send an exception
		return 0;
	}

	//uint16_t endAddr = startAddr+numCoils-1;  // Last coils address
	/*
	if (endAddr>199)  // end coil can not be more than 199 as we only have record of 200 (0-199) coils in total
	{
		modbusException(ILLEGAL_DATA_ADDRESS);   // send an exception
		return 0;
	}
	*/
	/* Calculation for the bit in the database, where the modification will be done */
	//int startByte = startAddr/8;  // which byte we have to start writing the data into
	/*uint16_t bitPosition = startAddr%8;  // The shift position in the first byte
	int indxPosition = 0;  // The shift position in the current indx of the RxData buffer

	int indx = 7;  // we need to keep track of index in RxData

	// Modify the bits as per the Byte received
	for (int i=0; i<numCoils; i++)
	{
		if (((RxData[indx]>>indxPosition)&0x01) == 1)
		{
			Coils_Database[startByte] |= 1<<bitPosition;  // replace that bit with 1
		}
		else
		{
			Coils_Database[startByte] &= ~(1<<bitPosition);  // replace that bit with 0
		}

		bitPosition++; indxPosition++;

		if (indxPosition>7)  // if the indxposition exceeds 7, we have to copy the data into the next byte position
		{
			indxPosition = 0;
			indx++;
		}
		if (bitPosition>7)  // if the bitposition exceeds 7, we have to increment the startbyte
		{
			bitPosition=0;
			startByte++;
		}
	}

// Prepare Response

	//| SLAVE_ID | FUNCTION_CODE | Start Addr | Data     | CRC     |
	//| 1 BYTE   |  1 BYTE       |  2 BYTE    | 2 BYTES  | 2 BYTES |

	//TxData[0] = SLAVE_ID;    // slave ID
	TxData[1] = RxData[1];   // function code
	TxData[2] = RxData[2];   // Start Addr HIGH Byte
	TxData[3] = RxData[3];   // Start Addr LOW Byte
	TxData[4] = RxData[4];   // num of coils HIGH Byte
	TxData[5] = RxData[5];   // num of coils LOW  Byte

	//sendData(TxData, 6);  // send data... CRC will be calculated in the function itself
	return 1;   // success
}
*/
