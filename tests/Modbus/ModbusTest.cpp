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
	uint8_t function;
	
	void timeout()
	{
		ret = eMBPoll(&function);
		ret = eMBPoll(&function);
		ret = eMBPoll(&function);	
		TIME_IQR_HANDLER();	
	}
	
    void setup()
    {
		test = 0;
		
		usRegHoldingAddr[0] = 0xAA55;
		usRegHoldingAddr[1] = 0x1111;
		usRegHoldingAddr[2] = 0x2222;
		usRegHoldingAddr[3] = 0x3333;
		usRegHoldingAddr[4] = 0x4444;
		usRegHoldingAddr[5] = 0x0707;

		usRegInputAddr[0] = 0x8000;		
		usRegInputAddr[1] = 0x8001;
		usRegInputAddr[2] = 0x8002;
		usRegInputAddr[3] = 0x8003;
		usRegInputAddr[4] = 0x8004;
		usRegInputAddr[5] = 0x8005;		
		
		eMBInit(1, 9600);
		
		ucMBSetInitState(1);
		
		timeout();
		ret = eMBPoll(&function);	
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
	
	ret = eMBPoll(&function);
	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, timeoutCheck)
{
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();

	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFD, ret);	// 最小桢判断
	
	timeout();
	ret = eMBPoll(&function);	

	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, crcErrorCheck)
{
	ret = eMBPoll(&function);	
	
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

	ret = eMBPoll(&function);	
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
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFC, ret);
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
#endif	
}

IGNORE_TEST(ModbusProtocol, crcCorrectCheck)
{
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
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
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, successReadHolding)
{
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
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
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x03, UART_DR);
	
	ret = eMBPoll(&function);	

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
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, failureReadHolding2)
{
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x56;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x01;	// register address out of bound
	xMBRTUReceiveFSM();	

	UART_DR = 0x64;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x1A;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(2, ret);	// exceptionCode = 2
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x83, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x02, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xC0, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xF1, UART_DR);
	
	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, failureReadHolding3)
{
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x03;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
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
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(3, ret);	// exceptionCode = 3
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x83, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x03, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x31, UART_DR);
	
	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, successWriteHolding)
{
	LONGS_EQUAL(0xAA55, usRegHoldingAddr[0]);	
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x06;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
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
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	LONGS_EQUAL(0xBB66, usRegHoldingAddr[0]);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x06, UART_DR);
	
	ret = eMBPoll(&function);	

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
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, failureWriteHolding2)
{

	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x06;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x56;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0xBB;
	xMBRTUReceiveFSM();

	UART_DR = 0x66;
	xMBRTUReceiveFSM();	

	UART_DR = 0x9B;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x00;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(2, ret);	// exceptionCode = 2
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x86, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x02, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xC3, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xA1, UART_DR);

	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, failureWriteHolding3)
{

	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x06;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);	
#if 0	
	UART_DR = 0x00;	// intentionally add one more byte
	xMBRTUReceiveFSM();	

	UART_DR = 0xBB;
	xMBRTUReceiveFSM();

	UART_DR = 0x66;
	xMBRTUReceiveFSM();	
	
	UART_DR = 0xF9;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x7C;	// correct CRC
	xMBRTUReceiveFSM();	
#else
	UART_DR = 0xBB;
	xMBRTUReceiveFSM();

	UART_DR = 0x66;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;	// intentionally add one more byte
	xMBRTUReceiveFSM();	
	
	UART_DR = 0x50;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x23;	// correct CRC
	xMBRTUReceiveFSM();	
#endif	
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(3, ret);	// exceptionCode = 2
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x86, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x03, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x02, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x61, UART_DR);

	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, successWriteHoldingMultiple)
{
	LONGS_EQUAL(0xAA55, usRegHoldingAddr[0]);	
	LONGS_EQUAL(0x1111, usRegHoldingAddr[1]);	
	LONGS_EQUAL(0x2222, usRegHoldingAddr[2]);	
	LONGS_EQUAL(0x3333, usRegHoldingAddr[3]);	
	LONGS_EQUAL(0x4444, usRegHoldingAddr[4]);	
	LONGS_EQUAL(0x0707, usRegHoldingAddr[5]);		
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x10;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x06;
	xMBRTUReceiveFSM();	

	UART_DR = 12;
	xMBRTUReceiveFSM();	
	
	UART_DR = 0x88;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x99;
	xMBRTUReceiveFSM();	

	UART_DR = 0xAA;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0xBB;
	xMBRTUReceiveFSM();	

	UART_DR = 0xCC;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0xDD;
	xMBRTUReceiveFSM();	
	
	UART_DR = 0xBE;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x35;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	LONGS_EQUAL(0x8800, usRegHoldingAddr[0]);
	LONGS_EQUAL(0x0099, usRegHoldingAddr[1]);
	LONGS_EQUAL(0xAA00, usRegHoldingAddr[2]);
	LONGS_EQUAL(0x00BB, usRegHoldingAddr[3]);
	LONGS_EQUAL(0xCC00, usRegHoldingAddr[4]);
	LONGS_EQUAL(0x00DD, usRegHoldingAddr[5]);	
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x10, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x06, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x40, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x0B, UART_DR);

	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

IGNORE_TEST(ModbusProtocol, failureWriteHoldingMultiple2)
{
	LONGS_EQUAL(0xAA55, usRegHoldingAddr[0]);	
	LONGS_EQUAL(0x1111, usRegHoldingAddr[1]);	
	LONGS_EQUAL(0x2222, usRegHoldingAddr[2]);	
	LONGS_EQUAL(0x3333, usRegHoldingAddr[3]);	
	LONGS_EQUAL(0x4444, usRegHoldingAddr[4]);	
	LONGS_EQUAL(0x0707, usRegHoldingAddr[5]);		
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x10;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x07;
	xMBRTUReceiveFSM();	

	UART_DR = 14;
	xMBRTUReceiveFSM();	
	
	UART_DR = 0x88;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x99;
	xMBRTUReceiveFSM();	

	UART_DR = 0xAA;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0xBB;
	xMBRTUReceiveFSM();	

	UART_DR = 0xCC;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0xDD;
	xMBRTUReceiveFSM();	
	
	UART_DR = 0xEE;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0xBF;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x86;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(2, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	LONGS_EQUAL(0xAA55, usRegHoldingAddr[0]);	
	LONGS_EQUAL(0x1111, usRegHoldingAddr[1]);	
	LONGS_EQUAL(0x2222, usRegHoldingAddr[2]);	
	LONGS_EQUAL(0x3333, usRegHoldingAddr[3]);	
	LONGS_EQUAL(0x4444, usRegHoldingAddr[4]);	
	LONGS_EQUAL(0x0707, usRegHoldingAddr[5]);		
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x90, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x02, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xCD, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xC1, UART_DR);
	
	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, failureWriteHoldingMultiple3)
{
	LONGS_EQUAL(0xAA55, usRegHoldingAddr[0]);	
	LONGS_EQUAL(0x1111, usRegHoldingAddr[1]);	
	LONGS_EQUAL(0x2222, usRegHoldingAddr[2]);	
	LONGS_EQUAL(0x3333, usRegHoldingAddr[3]);	
	LONGS_EQUAL(0x4444, usRegHoldingAddr[4]);	
	LONGS_EQUAL(0x0707, usRegHoldingAddr[5]);		
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x10;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x06;
	xMBRTUReceiveFSM();	

	UART_DR = 12;
	xMBRTUReceiveFSM();	
	
	UART_DR = 0x88;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x99;
	xMBRTUReceiveFSM();	

	UART_DR = 0xAA;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0xBB;
	xMBRTUReceiveFSM();	

	UART_DR = 0xCC;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0xDD;
	xMBRTUReceiveFSM();	
	
	UART_DR = 0xEE;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();	

	UART_DR = 0xFC;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0x17;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(3, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	LONGS_EQUAL(0xAA55, usRegHoldingAddr[0]);	
	LONGS_EQUAL(0x1111, usRegHoldingAddr[1]);	
	LONGS_EQUAL(0x2222, usRegHoldingAddr[2]);	
	LONGS_EQUAL(0x3333, usRegHoldingAddr[3]);	
	LONGS_EQUAL(0x4444, usRegHoldingAddr[4]);	
	LONGS_EQUAL(0x0707, usRegHoldingAddr[5]);		
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x90, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x03, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x0C, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, successReadInput)
{
	ucMBSetInitState(1);
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x04;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x07;//06
	xMBRTUReceiveFSM();	

	UART_DR = 0xB1;	//70// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0xC8;	//08// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x04, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(14, UART_DR);//12

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x80, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x80, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x80, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x02, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x80, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x03, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x80, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x04, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x80, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x05, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x19, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x0C, UART_DR);

	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	BYTES_EQUAL(0x04, ucADUReadRequestByte(0));
}

TEST(ModbusProtocol, writeHoldingFunction)
{
	LONGS_EQUAL(0xAA55, usRegHoldingAddr[0]);	
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x06;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
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
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	BYTES_EQUAL(6, function);
	
	LONGS_EQUAL(0, test);
	modBusWriteFunctionHandler(function);
	LONGS_EQUAL(0xBB66, test);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	LONGS_EQUAL(0xBB66, usRegHoldingAddr[0]);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x06, UART_DR);
	
	ret = eMBPoll(&function);	

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
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
}

TEST(ModbusProtocol, readInputInitState)
{
	ucMBSetInitState(1);
	
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0xFF, ret);
	
	UART_DR = 0x01;
	xMBRTUReceiveFSM();
	
	UART_DR = 0x04;
	xMBRTUReceiveFSM();

	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x06;
	xMBRTUReceiveFSM();

	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);	
	
	UART_DR = 0x00;
	xMBRTUReceiveFSM();

	UART_DR = 0x01;
	xMBRTUReceiveFSM();	

	UART_DR = 0xD1;	// correct CRC
	xMBRTUReceiveFSM();

	UART_DR = 0xCB;	// correct CRC
	xMBRTUReceiveFSM();	
	
	timeout();
	ret = eMBPoll(&function);	
	
	BYTES_EQUAL(0, ret);	// no error
	BYTES_EQUAL(STATE_TX_XMIT, eSndState);
	
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x04, UART_DR);
	
	ret = eMBPoll(&function);	

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x02, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x00, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x01, UART_DR);
	
	xMBRTUTransmitFSM();
	BYTES_EQUAL(0x78, UART_DR);

	xMBRTUTransmitFSM();
	BYTES_EQUAL(0xF0, UART_DR);

	xMBRTUTransmitFSM();	// interrupt after last byte sent, and transmit idle again
	BYTES_EQUAL(STATE_TX_IDLE, eSndState);	
	
	timeout();
	ret = eMBPoll(&function);	
	BYTES_EQUAL(0xFF, ret);
//	BYTES_EQUAL(0x04, ucADUReadRequestByte(0));
}
