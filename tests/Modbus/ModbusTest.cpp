//- ------------------------------------------------------------------
//-    Copyright (c) James W. Grenning -- All Rights Reserved         
//-    For use by owners of Test-Driven Development for Embedded C,   
//-    and attendees of Renaissance Software Consulting, Co. training 
//-    classes.                                                       
//-                                                                   
//-    Available at http://pragprog.com/titles/jgade/                 
//-        ISBN 1-934356-62-X, ISBN13 978-1-934356-62-3               
//-                                                                   
//-    Authorized users may use this source code in your own          
//-    projects, however the source code may not be used to           
//-    create training material, courses, books, articles, and        
//-    the like. We make no guarantees that this source code is       
//-    fit for any purpose.                                           
//-                                                                   
//-    www.renaissancesoftware.net james@renaissancesoftware.net      
//- ------------------------------------------------------------------

extern "C"
{
#include "ModbusRTU.h"
#include "ModbusHelper.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(ModbusProtocol)
{
	uint8_t ret;
	
	void timeout()
	{
		ret = eMBPoll();
		ret = eMBPoll();
		ret = eMBPoll();	
		TIME_IQR_HANDLER();	
	}
	
    void setup()
    {
		usRegHoldingAddr[0] = 0xAA55;
		eMBInit(1, 9600);
		
		timeout();
		ret = eMBPoll();	
    }

    void teardown()
    {
    }
};

IGNORE_TEST(ModbusProtocol, initialCheck)
{
	LONGS_EQUAL(STATE_RX_INIT, eRcvState);
	
	TIME_IQR_HANDLER();

	LONGS_EQUAL(STATE_RX_IDLE, eRcvState);	
	
	ret = eMBPoll();
	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, timeoutCheck)
{
	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();

	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFD, ret);	// 最小桢判断
	
	timeout();
	ret = eMBPoll();	

	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, crcErrorCheck)
{
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
	
#if 1
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x0A;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;	// wrong CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x00;	// wrong CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFC, ret);
	
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
#endif	
}

IGNORE_TEST(ModbusProtocol, crcCorrectCheck)
{
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x01;
	xMBRTUReceiveFSM();	

	UART_DR = 0x84;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x0A;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, successReadHolding)
{
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x01;
	xMBRTUReceiveFSM();	

	UART_DR = 0x84;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x0A;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x03, UART_DR);
	
	ret = eMBPoll();	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x02, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xAA, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x55, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x06, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xDB, UART_DR);
	
	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, failureReadHolding2)
{
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x06;
	xMBRTUReceiveFSM();

	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x01;	// register address out of bound
	xMBRTUReceiveFSM();	

	UART_DR = 0x64;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x0B;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(2, ret);	// exceptionCode = 2
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x83, UART_DR);
	
	ret = eMBPoll();	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x02, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xC0, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xF1, UART_DR);
	
	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, failureReadHolding3)
{
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;	// intentionally add one more byte
	xMBRTUReceiveFSM();	

	UART_DR = 0x0B;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0xF3;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(3, ret);	// exceptionCode = 3
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x83, UART_DR);
	
	ret = eMBPoll();	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x03, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x31, UART_DR);
	
	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, successWriteHolding)
{
	LONGS_EQUAL(0xAA55, usRegHoldingAddr[0]);	
	
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x06;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0xBB;
	xMBRTUReceiveFSM();

	UART_DR = 0x66;
	xMBRTUReceiveFSM();	

	UART_DR = 0x7B;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x10;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
	
	LONGS_EQUAL(0xBB66, usRegHoldingAddr[0]);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x06, UART_DR);
	
	ret = eMBPoll();	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xBB, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x66, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x7B, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x10, UART_DR);

	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, failureWriteHolding2)
{

	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x06;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x07;
	xMBRTUReceiveFSM();

	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0xBB;
	xMBRTUReceiveFSM();

	UART_DR = 0x66;
	xMBRTUReceiveFSM();	

	UART_DR = 0xCA;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0xD1;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll();	
	
	BYTES_EQUAL(2, ret);	// exceptionCode = 2
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
	
	LONGS_EQUAL(0xBB66, usRegHoldingAddr[0]);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x06, UART_DR);
	
	ret = eMBPoll();	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xBB, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x66, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x7B, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x10, UART_DR);

	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll();	
	BYTES_EQUAL(0xFF, ret);
}

