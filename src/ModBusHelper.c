/********************************************************************************
  * @File   : ModbusPort.c
  * @Author : worldsing
  * @Version: V0.1
  * @Date   : 2013/05/05
  * @Brief  :
  *******************************************************************************
  * @Attention:
  */
#include "ModBusHelper.h"

uint32_t GPIOA, GPIOB, GPIOC, GPIOD;

uint8_t UART_DR;

#if 0
/* ------------------------------- 事件处理队列 -------------------------------*/
static eMBEventType eQueuedEvent;
static bool     xEventInQueue;

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
/* ------------------------------- 串品操作 -----------------------------------*/
//串口收发控制
void vMBPortSerialEnable( bool xRxEnable, bool xTxEnable ){
  
    ENTER_CRITICAL_SECTION();
    if( xRxEnable ){
        USART_RX_ENABLE();
        RS485SWITCH_TO_RECEIVE();
    }
    else{
       USART_RX_DISABLE();
       RS485SWITCH_TO_SEND();
    }
    if( xTxEnable ){
       USART_TX_ENABLE();
    }
    else{
       USART_TX_DISABLE();
    }
    EXIT_CRITICAL_SECTION();
}

//串初始化
bool xMBPortSerialInit(uint32_t ulBaudRate){
 	
	bool  bInitialized = true;
	GPIO_InitTypeDef GPIO_InitStruct; 
	USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  //UART IRQ
  NVIC_InitStructure.NVIC_IRQChannel = UART_IRQN_N;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //UART CLK
	RCC_APB2PeriphClockCmd(UART_GPIO_CLK        |      \
		                     RCC_APB2Periph_AFIO,        \
	                       ENABLE);
  if(UART == USART1)	
		RCC_APB2PeriphClockCmd(UART_CLK, ENABLE);
	else
		RCC_APB1PeriphClockCmd(UART_CLK, ENABLE);
	
  //UART GPIO	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = UART_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UART_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = UART_RX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UART_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = RS485PIN;
	GPIO_Init(RS485GPIO, &GPIO_InitStruct);
  
	//UART Config
	USART_InitStructure.USART_BaudRate = ulBaudRate;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART, &USART_InitStructure);
	USART_Cmd(UART, ENABLE);
	
	return bInitialized;
}

void UART_IQR_HANDLER(void){
	if(USART_GetITStatus(UART, USART_IT_TC)){
	  xMBRTUTransmitFSM();//发送中断子程序 
	}
	else if(USART_GetITStatus(UART, USART_IT_RXNE)){
		xMBRTUReceiveFSM();//接收中断子程序 
	}
}

/* ----------------------------- 定时器操作 -----------------------------------*/
//定时器初始化
void xMBPortTimersInit(uint16_t usTim1Timeout50us ){
  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIME_IRQN_N;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  if(TIEM == TIM1 || TIEM == TIM8 || TIEM == TIM9)
		RCC_APB2PeriphClockCmd(TIME_CLK, ENABLE);
	else		
		RCC_APB1PeriphClockCmd(TIME_CLK, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = CPU_CLK - 1;               //自动重装数据值 一个单位是1us
  TIM_TimeBaseStructure.TIM_Prescaler = usTim1Timeout50us * 50;	//预分频值       一个单位是50uS
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		    //时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	  //TIM向上计数模式
  TIM_TimeBaseInit(TIEM, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIEM, TIM_FLAG_Update);                         //清除溢出中断标志
}

//开定时器及中断
void vMBPortTimersEnable( ){
  TIM_ClearFlag(TIEM, TIM_FLAG_Update);                         //清除溢出中断标志
  TIM_SetCounter(TIEM,0x00);			                              //清零计数器值
  TIM_ITConfig(TIEM,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIEM,ENABLE);
}

//关定时器及中断
void vMBPortTimersDisable( ){
  TIM_ITConfig(TIEM,TIM_IT_Update,DISABLE);
  TIM_Cmd(TIEM,DISABLE);
}

//定时器溢出中断服务程序
void TIME_IQR_HANDLER( void ){
  
  if(eRcvState == STATE_RX_RCV){
    xMBPortEventPost( EV_FRAME_RECEIVED );
  }
  vMBPortTimersDisable();
  eRcvState = STATE_RX_IDLE;
}
#else
/* ------------------------------- 串品操作 -----------------------------------*/
//串口收发控制
void vMBPortSerialEnable( bool xRxEnable, bool xTxEnable ){
}	
/* ----------------------------- 定时器操作 -----------------------------------*/
//开定时器及中断
void vMBPortTimersEnable( ){
}
//关定时器及中断
void vMBPortTimersDisable( ){
}
#endif

