/*
 * flash.c
 *
 *  Created on: 2022��1��18��
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
    while(FCTL3 & BUSY);                      // �ȴ������������
    FCTL1 = FWKEY + BLKWRT;                     // ʹ�ܳ���д�����
    *addr= x;                     // ������д��Ŀ��Flash��
    while(FCTL3 & BUSY);                      // �ȴ�д��������
    _EINT();                                    // ����ȫ���ж�
    FCTL1=FWKEY;
    FCTL3=FWKEY+LOCK;
}
int read_flash_int(unsigned long *addr)
{
    int x=*addr;
    return x;
}



