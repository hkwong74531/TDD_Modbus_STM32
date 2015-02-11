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

TEST(ModbusProtocol, crcCorrectCheck)
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
	
	BYTES_EQUAL(0xFC, ret);
	
	ret = eMBPoll();	
	
	BYTES_EQUAL(0xFF, ret);
}