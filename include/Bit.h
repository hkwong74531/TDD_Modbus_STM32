/********************************************************************************
  * @File   : Bit.h
  * @Author : worldsing
  * @Version: V0.1
  * @Date   : 2013/05/05
  * @Brief  :
  *******************************************************************************
  * @Attention:
  */
#ifndef __BIT_H__
#define __BII_H__

//下以操作前提是port可写入
#define RevBit(port, bitn)           (port ^=  (1<<(bitn)))                                 //翻转port口bitn位
#define SetBit(port, bitn)           (port |=  (1<<(bitn)))                                 //port口bitn位 = 1
#define ClrBit(port, bitn)           (port &= ~(1<<(bitn)))                                 //port口bitn位 = 0
// #define GetBit(port, bitn)           (port &   (1<<(bitn)))                                 //读port口bitn位值
#define GetBit(port, bitn)           ((port >> bitn) & 0x1)                                 //读port口bitn位值
#define OutBit(port, bitn, value)    ((value) ? (SetBit(port, bitn)) : (ClrBit(port, bitn)))//port口bitn位 = value

#endif

//end of file


