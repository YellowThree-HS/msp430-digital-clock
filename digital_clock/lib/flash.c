/*
 * flash.c
 *
 *  Created on: 2022年1月18日
 *      Author: Administrator
 */
#include <msp430.h>
void write_flash_int(unsigned long *addr,int x)
{
    FCTL1=FWKEY+ERASE;
    FCTL3=FWKEY;
    _DINT();
    *addr=0;
    FCTL1=FWKEY+WRT;
    while(FCTL3 & BUSY);                      // 等待擦除操作完成
    FCTL1 = FWKEY + BLKWRT;                     // 使能长字写入操作
    *addr= x;                     // 将长字写入目的Flash段
    while(FCTL3 & BUSY);                      // 等待写入操作完成
    _EINT();                                    // 启动全局中断
    FCTL1=FWKEY;
    FCTL3=FWKEY+LOCK;
}
int read_flash_int(unsigned long *addr)
{
    int x=*addr;
    return x;
}



