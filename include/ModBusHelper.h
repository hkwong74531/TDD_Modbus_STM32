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

extern uint16_t ucMBInitState;
extern uint32_t GPIOA, GPIOB, GPIOC, GPIOD;
extern uint16_t usRegHoldingAddr[];
extern uint16_t usRegInputAddr[];

extern uint16_t test;

//CPU  寄存器类型指针定义
typedef volatile uint32_t * CPURegistertype;

#define UART_USE                                 2       //串口号
                 
#define TIM_USE                                  3       //定时器号
                 
#define CPU_CLK                                  72      //72MHz             

#define REG_HOLDING_NREGS                        0x55       //保持寄存器数量
#define REG_INPUT_NREGS                          7       //输入寄存器数量

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
#define REGHOLDING_06	(usRegHoldingAddr + 6)
#define REGHOLDING_07	(usRegHoldingAddr + 7)
#define REGHOLDING_08	(usRegHoldingAddr + 8)
#define REGHOLDING_09	(usRegHoldingAddr + 9)
#define REGHOLDING_10	(usRegHoldingAddr + 10)
#define REGHOLDING_11	(usRegHoldingAddr + 11)
#define REGHOLDING_12	(usRegHoldingAddr + 12)
#define REGHOLDING_13	(usRegHoldingAddr + 13)
#define REGHOLDING_14	(usRegHoldingAddr + 14)
#define REGHOLDING_15	(usRegHoldingAddr + 15)
#define REGHOLDING_16	(usRegHoldingAddr + 16)
#define REGHOLDING_17	(usRegHoldingAddr + 17)
#define REGHOLDING_18	(usRegHoldingAddr + 18)
#define REGHOLDING_19	(usRegHoldingAddr + 19)
#define REGHOLDING_20	(usRegHoldingAddr + 20)
#define REGHOLDING_21	(usRegHoldingAddr + 21)
#define REGHOLDING_22	(usRegHoldingAddr + 22)
#define REGHOLDING_23	(usRegHoldingAddr + 23)
#define REGHOLDING_24	(usRegHoldingAddr + 24)
#define REGHOLDING_25	(usRegHoldingAddr + 25)
#define REGHOLDING_26	(usRegHoldingAddr + 26)
#define REGHOLDING_27	(usRegHoldingAddr + 27)
#define REGHOLDING_28	(usRegHoldingAddr + 28)
#define REGHOLDING_29	(usRegHoldingAddr + 29)
#define REGHOLDING_30	(usRegHoldingAddr + 30)
#define REGHOLDING_31	(usRegHoldingAddr + 31)
#define REGHOLDING_32	(usRegHoldingAddr + 32)
#define REGHOLDING_33	(usRegHoldingAddr + 33)
#define REGHOLDING_34	(usRegHoldingAddr + 34)
#define REGHOLDING_35	(usRegHoldingAddr + 35)
#define REGHOLDING_36	(usRegHoldingAddr + 36)
#define REGHOLDING_37	(usRegHoldingAddr + 37)
#define REGHOLDING_38	(usRegHoldingAddr + 38)
#define REGHOLDING_39	(usRegHoldingAddr + 39)
#define REGHOLDING_40	(usRegHoldingAddr + 40)
#define REGHOLDING_41	(usRegHoldingAddr + 41)
#define REGHOLDING_42	(usRegHoldingAddr + 42)
#define REGHOLDING_43	(usRegHoldingAddr + 43)
#define REGHOLDING_44	(usRegHoldingAddr + 44)
#define REGHOLDING_45	(usRegHoldingAddr + 45)
#define REGHOLDING_46	(usRegHoldingAddr + 46)
#define REGHOLDING_47	(usRegHoldingAddr + 47)
#define REGHOLDING_48	(usRegHoldingAddr + 48)
#define REGHOLDING_49	(usRegHoldingAddr + 49)
#define REGHOLDING_50	(usRegHoldingAddr + 50)
#define REGHOLDING_51	(usRegHoldingAddr + 51)
#define REGHOLDING_52	(usRegHoldingAddr + 52)
#define REGHOLDING_53	(usRegHoldingAddr + 53)
#define REGHOLDING_54	(usRegHoldingAddr + 54)
#define REGHOLDING_55	(usRegHoldingAddr + 55)
#define REGHOLDING_56	(usRegHoldingAddr + 56)
#define REGHOLDING_57	(usRegHoldingAddr + 57)
#define REGHOLDING_58	(usRegHoldingAddr + 58)
#define REGHOLDING_59	(usRegHoldingAddr + 59)
#define REGHOLDING_60	(usRegHoldingAddr + 60)
#define REGHOLDING_61	(usRegHoldingAddr + 61)
#define REGHOLDING_62	(usRegHoldingAddr + 62)
#define REGHOLDING_63	(usRegHoldingAddr + 63)
#define REGHOLDING_64	(usRegHoldingAddr + 64)
#define REGHOLDING_65	(usRegHoldingAddr + 65)
#define REGHOLDING_66	(usRegHoldingAddr + 66)
#define REGHOLDING_67	(usRegHoldingAddr + 67)
#define REGHOLDING_68	(usRegHoldingAddr + 68)
#define REGHOLDING_69	(usRegHoldingAddr + 69)
#define REGHOLDING_70	(usRegHoldingAddr + 70)
#define REGHOLDING_71	(usRegHoldingAddr + 71)
#define REGHOLDING_72	(usRegHoldingAddr + 72)
#define REGHOLDING_73	(usRegHoldingAddr + 73)
#define REGHOLDING_74	(usRegHoldingAddr + 74)
#define REGHOLDING_75	(usRegHoldingAddr + 75)
#define REGHOLDING_76	(usRegHoldingAddr + 76)
#define REGHOLDING_77	(usRegHoldingAddr + 77)
#define REGHOLDING_78	(usRegHoldingAddr + 78)
#define REGHOLDING_79	(usRegHoldingAddr + 79)
#define REGHOLDING_80	(usRegHoldingAddr + 80)
#define REGHOLDING_81	(usRegHoldingAddr + 81)
#define REGHOLDING_82	(usRegHoldingAddr + 82)
#define REGHOLDING_83	(usRegHoldingAddr + 83)
#define REGHOLDING_84	(usRegHoldingAddr + 84)

#define REGHOLDING_FUNC_00	writeHoldingDummy
#define REGHOLDING_FUNC_01	NULL
#define REGHOLDING_FUNC_02	NULL
#define REGHOLDING_FUNC_03	NULL
#define REGHOLDING_FUNC_04	NULL
#define REGHOLDING_FUNC_05	NULL
#define REGHOLDING_FUNC_06	NULL
#define REGHOLDING_FUNC_07	NULL
#define REGHOLDING_FUNC_08	NULL
#define REGHOLDING_FUNC_09	NULL
#define REGHOLDING_FUNC_10	NULL
#define REGHOLDING_FUNC_11	NULL
#define REGHOLDING_FUNC_12	NULL
#define REGHOLDING_FUNC_13	NULL
#define REGHOLDING_FUNC_14	NULL
#define REGHOLDING_FUNC_15	NULL
#define REGHOLDING_FUNC_16	NULL
#define REGHOLDING_FUNC_17	NULL
#define REGHOLDING_FUNC_18	NULL
#define REGHOLDING_FUNC_19	NULL
#define REGHOLDING_FUNC_20	NULL
#define REGHOLDING_FUNC_21	NULL	
#define REGHOLDING_FUNC_22	NULL
#define REGHOLDING_FUNC_23	NULL
#define REGHOLDING_FUNC_24	NULL
#define REGHOLDING_FUNC_25	NULL
#define REGHOLDING_FUNC_26	NULL
#define REGHOLDING_FUNC_27	NULL
#define REGHOLDING_FUNC_28	NULL
#define REGHOLDING_FUNC_29	NULL
#define REGHOLDING_FUNC_30	NULL
#define REGHOLDING_FUNC_31	NULL	
#define REGHOLDING_FUNC_32	NULL
#define REGHOLDING_FUNC_33	NULL
#define REGHOLDING_FUNC_34	NULL
#define REGHOLDING_FUNC_35	NULL
#define REGHOLDING_FUNC_36	NULL
#define REGHOLDING_FUNC_37	NULL
#define REGHOLDING_FUNC_38	NULL
#define REGHOLDING_FUNC_39	NULL
#define REGHOLDING_FUNC_40	NULL
#define REGHOLDING_FUNC_41	NULL	
#define REGHOLDING_FUNC_42	NULL
#define REGHOLDING_FUNC_43	NULL
#define REGHOLDING_FUNC_44	NULL
#define REGHOLDING_FUNC_45	NULL	
#define REGHOLDING_FUNC_46	NULL
#define REGHOLDING_FUNC_47	NULL
#define REGHOLDING_FUNC_48	NULL
#define REGHOLDING_FUNC_49	NULL
#define REGHOLDING_FUNC_50	NULL
#define REGHOLDING_FUNC_51	NULL	
#define REGHOLDING_FUNC_52	NULL
#define REGHOLDING_FUNC_53	NULL
#define REGHOLDING_FUNC_54	NULL
#define REGHOLDING_FUNC_55	NULL	
#define REGHOLDING_FUNC_56	NULL
#define REGHOLDING_FUNC_57	NULL
#define REGHOLDING_FUNC_58	NULL
#define REGHOLDING_FUNC_59	NULL
#define REGHOLDING_FUNC_60	NULL
#define REGHOLDING_FUNC_61	NULL	
#define REGHOLDING_FUNC_62	NULL
#define REGHOLDING_FUNC_63	NULL
#define REGHOLDING_FUNC_64	NULL
#define REGHOLDING_FUNC_65	NULL	
#define REGHOLDING_FUNC_66	NULL
#define REGHOLDING_FUNC_67	NULL
#define REGHOLDING_FUNC_68	NULL
#define REGHOLDING_FUNC_69	NULL
#define REGHOLDING_FUNC_70	NULL
#define REGHOLDING_FUNC_71	NULL	
#define REGHOLDING_FUNC_72	NULL
#define REGHOLDING_FUNC_73	NULL
#define REGHOLDING_FUNC_74	NULL
#define REGHOLDING_FUNC_75	NULL	
#define REGHOLDING_FUNC_76	NULL
#define REGHOLDING_FUNC_77	NULL
#define REGHOLDING_FUNC_78	NULL
#define REGHOLDING_FUNC_79	NULL
#define REGHOLDING_FUNC_80	NULL
#define REGHOLDING_FUNC_81	NULL	
#define REGHOLDING_FUNC_82	NULL
#define REGHOLDING_FUNC_83	NULL
#define REGHOLDING_FUNC_84	NULL

#define REGINPUT_00	(usRegInputAddr)
#define	REGINPUT_01	(usRegInputAddr + 1)
#define	REGINPUT_02	(usRegInputAddr + 2)
#define	REGINPUT_03	(usRegInputAddr + 3)
#define	REGINPUT_04	(usRegInputAddr + 4)
#define	REGINPUT_05	(usRegInputAddr + 5)
#define REGINPUT_06	&ucMBInitState
	
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

void writeHoldingDummy(uint16_t*);
void ucMBSetInitState(uint16_t);
uint16_t ucMBGetInitState(void);

#endif
