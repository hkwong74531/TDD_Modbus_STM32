/********************************************************************************
  * @File   : Led.c
  * @Author : worldsing
  * @Version: V0.1
  * @Date   : 2013/05/05
  * @Brief  :
  *******************************************************************************
  * @Attention:
  */

#ifndef _HELPER_H
#define _HELPER_H

#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include "stdbool.h"

#include "Bit.h"

extern uint32_t GPIOA, GPIOB, GPIOC, GPIOD;
extern uint16_t usRegHoldingAddr[];
extern uint16_t usRegInputAddr[];

//CPU  寄存器类型指针定义
typedef volatile uint32_t * CPURegistertype;

#define UART_USE                                 2       //串口号
                 
#define TIM_USE                                  3       //定时器号
                 
#define CPU_CLK                                  72      //72MHz             

#define REG_HOLDING_NREGS                        6       //保持寄存器数量
#define REG_INPUT_NREGS                          6       //输入寄存器数量

#define MBCOIL_00	{&GPIOB, 7}
#define MBCOIL_01	{&GPIOB, 8}
#define MBCOIL_02	{&GPIOB, 12}
#define MBCOIL_03	{&GPIOB, 14}
#define MBCOIL_04	{&GPIOD, 0}
#define MBCOIL_05	{&GPIOD, 1}
#define MBCOIL_06	{&GPIOD, 13}
#define MBCOIL_07	{&GPIOD, 14}

#define MBDISCRETE_00	{&GPIOC, 3} 
#define MBDISCRETE_01	{&GPIOC, 4}
#define MBDISCRETE_02	{&GPIOC, 8}
#define MBDISCRETE_03	{&GPIOC, 11}
#define MBDISCRETE_04	{&GPIOA, 2}
#define MBDISCRETE_05	{&GPIOA, 8}
#define MBDISCRETE_06	{&GPIOA, 12}
#define MBDISCRETE_07	{&GPIOA, 15}

#define REGHOLDING_00	(usRegHoldingAddr)
#define REGHOLDING_01	(usRegHoldingAddr + 1)
#define REGHOLDING_02	(usRegHoldingAddr + 2)
#define REGHOLDING_03	(usRegHoldingAddr + 3)
#define REGHOLDING_04	(usRegHoldingAddr + 4)
#define REGHOLDING_05	(usRegHoldingAddr + 5)

#define REGINPUT_00	(usRegInputAddr)
#define	REGINPUT_01	(usRegInputAddr + 1)
#define	REGINPUT_02	(usRegInputAddr + 2)
#define	REGINPUT_03	(usRegInputAddr + 3)
#define	REGINPUT_04	(usRegInputAddr + 4)
#define	REGINPUT_05	(usRegInputAddr + 5)
	
#define ENTER_CRITICAL_SECTION( )        //关总中断
#define EXIT_CRITICAL_SECTION( )         //开总中断

bool xMBPortSerialInit(uint32_t ulBaudRate);
void vMBPortSerialEnable( bool xRxEnable, bool xTxEnable );

void xMBPortTimersInit( uint16_t usTimeOut50us );
void vMBPortTimersEnable( void );
void vMBPortTimersDisable( void );

void TIME_IQR_HANDLER( void );

#define assert(expr)
#if 0
#if (UART_USE == 1)
  #define UART                  USART1
	#define UART_GPIO             GPIOA  
	#define UART_CLK              RCC_APB2Periph_USART1 
	#define UART_GPIO_CLK         RCC_APB2Periph_GPIOA  
	#define UART_TX_PIN           GPIO_Pin_9
	#define UART_RX_PIN           GPIO_Pin_10
	#define UART_IQR_HANDLER      USART1_IRQHandler
	#define UART_IRQN_N           USART1_IRQn
#endif
#if (UART_USE == 2)
  #define UART                  USART2
	#define UART_GPIO             GPIOA  
	#define UART_CLK              RCC_APB1Periph_USART2 
	#define UART_GPIO_CLK         RCC_APB2Periph_GPIOA  
	#define UART_TX_PIN           GPIO_Pin_2
	#define UART_RX_PIN           GPIO_Pin_3
	#define UART_IQR_HANDLER      USART2_IRQHandler
	#define UART_IRQN_N           USART2_IRQn
#endif
#if (UART_USE == 3)
  #define UART                  USART3
	#define UART_GPIO             GPIOB  
	#define UART_CLK              RCC_APB1Periph_USART3 
	#define UART_GPIO_CLK         RCC_APB2Periph_GPIOB  
	#define UART_TX_PIN           GPIO_Pin_10
	#define UART_RX_PIN           GPIO_Pin_11
	#define UART_IQR_HANDLER      USART3_IRQHandler
	#define UART_IRQN_N           USART3_IRQn
#endif

#if (TIM_USE == 2)
#define TIEM                    TIM2
#define TIME_IRQN_N             TIM2_IRQn
#define TIME_IQR_HANDLER        TIM2_IRQHandler
#define TIME_CLK                RCC_APB1Periph_TIM2
#endif  
  
#if (TIM_USE == 3)  
#define TIEM                    TIM3
#define TIME_IRQN_N             TIM3_IRQn
#define TIME_IQR_HANDLER        TIM3_IRQHandler
#define TIME_CLK                RCC_APB1Periph_TIM3
#endif  
  
#if (TIM_USE == 4)  
#define TIEM                    TIM4
#define TIME_IRQN_N             TIM4_IRQn
#define TIME_IQR_HANDLER        TIM4_IRQHandler
#define TIME_CLK                RCC_APB1Periph_TIM4
#endif

#define UART_DR                  (UART->DR)                               
#define USART_RX_ENABLE()        USART_ITConfig(UART, USART_IT_RXNE, ENABLE)
#define USART_RX_DISABLE()       USART_ITConfig(UART, USART_IT_RXNE, DISABLE)
#define USART_TX_ENABLE()        USART_ITConfig(UART, USART_IT_TC, ENABLE);
#define USART_TX_DISABLE()       USART_ITConfig(UART, USART_IT_TC, DISABLE);
     
#define RS485PIN                 GPIO_Pin_1
#define RS485SWITCH_TO_SEND()    GPIO_SetBits(RS485GPIO, RS485PIN)
#define RS485SWITCH_TO_RECEIVE() GPIO_ResetBits(RS485GPIO, RS485PIN)
#define RS485GPIO                GPIOA  
#define RS485GPIO_CLK            RCC_APB2Periph_GPIOA
#else
extern uint8_t UART_DR;
#endif
#define UsartSend(ucByte)        ((UART_DR) = (ucByte))        
#define UsartReceive(ucByte)     (*(ucByte) = (uint8_t)(UART_DR))

#endif
