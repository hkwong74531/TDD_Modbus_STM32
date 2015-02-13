/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbrtu.c,v 1.18 2007/09/12 10:15:56 wolti Exp $
 */
/* ----------------------- Platform includes --------------------------------*/
#include "ModBusRTU.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */


/* ----------------------- Static variables ---------------------------------*/
volatile eMBSndState eSndState;
volatile eMBRcvState eRcvState;

static volatile uint8_t  ucRTUBuf[MB_SER_PDU_SIZE_MAX];

static volatile uint8_t *pucSndBufferCur;
static volatile uint16_t usSndBufferCount;

static volatile uint16_t usRcvBufferPos;

static uint8_t    ucMBAddress;



/* 定义功很码与功能处理函数列表 */
static xMBFunctionHandler xFuncHandlers[MB_FUNC_HANDLERS_MAX] = {
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
    {MB_FUNC_OTHER_REPORT_SLAVEID, eMBFuncReportSlaveID},
#endif
#if MB_FUNC_READ_INPUT_ENABLED > 0
    {MB_FUNC_READ_INPUT_REGISTER, eMBFuncReadInputRegister},
#endif
#if MB_FUNC_READ_HOLDING_ENABLED > 0
    {MB_FUNC_READ_HOLDING_REGISTER, eMBFuncReadHoldingRegister},
#endif
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_REGISTERS, eMBFuncWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
    {MB_FUNC_WRITE_REGISTER, eMBFuncWriteHoldingRegister},
#endif
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
    {MB_FUNC_READWRITE_MULTIPLE_REGISTERS, eMBFuncReadWriteMultipleHoldingRegister},
#endif
#if MB_FUNC_READ_COILS_ENABLED > 0
    {MB_FUNC_READ_COILS, eMBFuncReadCoils},
#endif
#if MB_FUNC_WRITE_COIL_ENABLED > 0
    {MB_FUNC_WRITE_SINGLE_COIL, eMBFuncWriteCoil},
#endif
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
    {MB_FUNC_WRITE_MULTIPLE_COILS, eMBFuncWriteMultipleCoils},
#endif
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
    {MB_FUNC_READ_DISCRETE_INPUTS, eMBFuncReadDiscreteInputs},
#endif
};

static const uint8_t aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const uint8_t aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

static eMBEventType eQueuedEvent;
static bool     xEventInQueue;

uint16_t usMBCRC16( uint8_t * pucFrame, uint16_t usLen )
{
    uint8_t           ucCRCHi = 0xFF;
    uint8_t           ucCRCLo = 0xFF;
    uint16_t          iIndex;

    while( usLen-- ){
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( uint8_t )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( uint16_t )( ucCRCHi << 8 | ucCRCLo );
}

/* ------------------------------- 事件处理队列 -------------------------------*/
//对列初始化
bool xMBPortEventInit( void ) {
    xEventInQueue = false;
    return true;
}

//进入队列
bool xMBPortEventPost( eMBEventType eEvent ){
    xEventInQueue = true;
    eQueuedEvent = eEvent;
    return true;
}

//出队列
bool xMBPortEventGet( eMBEventType * eEvent ){
  
    bool xEventHappened = false;
    if( xEventInQueue ){
        *eEvent = eQueuedEvent;
        xEventInQueue = false;
        xEventHappened = true;
    }
    return xEventHappened;
}

//接收状态机由接收中断调用
void xMBRTUReceiveFSM( void )
{
    uint8_t           ucByte;
    assert( eSndState == STATE_TX_IDLE );

    UsartReceive(&ucByte);

    switch ( eRcvState ){
    case STATE_RX_INIT:
    case STATE_RX_ERROR:
        vMBPortTimersEnable(  );
        break;
    case STATE_RX_IDLE:
        usRcvBufferPos = 0;
        ucRTUBuf[usRcvBufferPos++] = ucByte;
        eRcvState = STATE_RX_RCV;
        vMBPortTimersEnable(  );
        break;
    case STATE_RX_RCV:
        if( usRcvBufferPos < MB_SER_PDU_SIZE_MAX ){
            ucRTUBuf[usRcvBufferPos++] = ucByte;
        }
        else{
            eRcvState = STATE_RX_ERROR;
        }
        vMBPortTimersEnable(  );
        break;    
	default:
		break;
    }
}

//发送状态机由发送中断调用
void xMBRTUTransmitFSM( void )
{
    assert( eRcvState == STATE_RX_IDLE );
    switch ( eSndState )
    {
    case STATE_TX_IDLE:
        vMBPortSerialEnable( true, false );
        break;
    case STATE_TX_XMIT:
        if( usSndBufferCount != 0 ){
            UsartSend((uint8_t)*pucSndBufferCur);
            pucSndBufferCur++;  
            usSndBufferCount--;
        }
        else{
            xMBPortEventPost( EV_FRAME_SENT );
            vMBPortSerialEnable( true, false );
            eSndState = STATE_TX_IDLE;
        }
        break;
	default:
		break;
    }
}


void eMBInit(uint8_t ucSlaveAddress, uint32_t ulBaudRate)
{
//    uint16_t           usTimerT35_50us;     //一个单位是50uS
    
    if( ( ucSlaveAddress == MB_ADDRESS_BROADCAST ) ||
        ( ucSlaveAddress < MB_ADDRESS_MIN        ) || 
        ( ucSlaveAddress > MB_ADDRESS_MAX        ) ){
      return;
    }
    ucMBAddress = ucSlaveAddress;
#if 0	
    xMBPortSerialInit(ulBaudRate);
    
    if( ulBaudRate > 19200 ){
        usTimerT35_50us = 35;    
    }
    else{
        usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
    }
    xMBPortTimersInit( usTimerT35_50us);
#endif    
    xMBPortEventInit();
    eRcvState = STATE_RX_INIT;
#if 0	
    vMBPortSerialEnable(true, false);
    vMBPortTimersEnable();
#endif
}

uint8_t eMBPoll( void ){
  
  static uint8_t   *ucMBFrame;
  static uint8_t    ucFunctionCode;
  static uint16_t   usLength;
  static eMBException eException;
  eMBEventType    eEvent;
  uint8_t i;
  uint16_t usCRC16;
  if(xMBPortEventGet( &eEvent) == true ){                             //桢事件判断
    if(eEvent == EV_FRAME_RECEIVED){    
      if(usRcvBufferPos < MB_SER_PDU_SIZE_MIN)                        //最小桢判断
        return 0xFD;
      if(usMBCRC16((uint8_t *)ucRTUBuf, usRcvBufferPos ) != 0)          //CRC判断
        return 0xFC;
      if(IS_VALID_ADD){                                               //地址	
        ucMBFrame = (uint8_t *) &ucRTUBuf[MB_SER_PDU_PDU_OFF];	
        usLength = (uint16_t)( usRcvBufferPos - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_CRC);
        ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];
        eException = MB_EX_ILLEGAL_FUNCTION;
        for(i = 0; i < MB_FUNC_HANDLERS_MAX; i++ ){                  //执行功能码
          if( xFuncHandlers[i].ucFunctionCode == 0 ){
            return MB_EX_ILLEGAL_FUNCTION;
          }
          else if( xFuncHandlers[i].ucFunctionCode == ucFunctionCode ){
            eException = xFuncHandlers[i].pxHandler( ucMBFrame, &usLength );
            break;                               
          }
      }
      if(IS_NOT_BROADCAST){                                        //回复主机
        if( eException != MB_EX_NONE ){                            //错误码         
          usLength = 0;
          ucMBFrame[usLength++] = ( uint8_t )( ucFunctionCode | MB_FUNC_ERROR );
          ucMBFrame[usLength++] = eException;
        } 
        if(eRcvState == STATE_RX_IDLE){                            //发送
          pucSndBufferCur = ( uint8_t * ) ucMBFrame - 1;
          pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucMBAddress;
          usSndBufferCount = usLength + 1;      
          usCRC16 = usMBCRC16( ( uint8_t * ) pucSndBufferCur, usSndBufferCount );
          ucRTUBuf[usSndBufferCount++] = ( uint8_t )( usCRC16 & 0xFF );
          ucRTUBuf[usSndBufferCount++] = ( uint8_t )( usCRC16 >> 8 );    
          eSndState = STATE_TX_XMIT;
          vMBPortSerialEnable( false, true );
          }//发送结束
        }//回复结束
		return eException;
      }//地址判断
	  return 0xFE;
    }//桢事件判断
  }
  return 0xFF;
}
//定时器溢出中断服务程序
void TIME_IQR_HANDLER( void ){
  
  if(eRcvState == STATE_RX_RCV){
    xMBPortEventPost( EV_FRAME_RECEIVED );
  }
  vMBPortTimersDisable();
  eRcvState = STATE_RX_IDLE;
}

uint8_t ucADUReadRequestByte(uint8_t n)
{
	return ucRTUBuf[n];
}

