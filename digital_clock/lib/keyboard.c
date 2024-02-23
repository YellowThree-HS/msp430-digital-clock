/*
 * keyboard.c
 *
 *  Created on: 2022年6月22日
 *      Author: Administrator
 */

#include <msp430.h>
#include "keyboard.h"

void init_key(void)
{
    //先设置输入输出。按键按下就会出低电平，不按为高电平
    //行为输出 P1.3 P1.2 P1.1 P1.0
    P1DIR |= BIT0+BIT1+BIT2+BIT3;
    //列为输入 P1.4 P1.5 P1.6 P1.7。上拉
    P1DIR &= ~(BIT4+BIT5+BIT6+BIT7);
    P1REN |= (BIT4+BIT5+BIT6+BIT7);
    P1OUT |= (BIT4+BIT5+BIT6+BIT7);
}

char s_key() //无消抖
{

    //扫描第一行，第一行置为低电平
    P1OUT |= BIT5+BIT6+BIT7;
    P1OUT &= ~BIT4;
    if(P1IN & BIT3==0)
        return '1';
    if(P1IN & BIT2==0)
        return '2';
    if(P1IN & BIT1==0)
        return '3';
    if(P1IN & BIT0==0)
        return 'A';


    //扫描第二行，第二行置为低电平
    P1OUT |= BIT4+BIT6+BIT7;
    P1OUT &= ~BIT5;
    if(P1IN & BIT3==0)
        return '4';
    if(P1IN & BIT2==0)
        return '5';
    if(P1IN & BIT1==0)
        return '6';
    if(P1IN & BIT0==0)
        return 'B';

    //扫描第三行，第三行置为低电平
    P1OUT |= BIT5+BIT4+BIT7;
    P1OUT &= ~BIT6;
    if(P1IN & BIT3==0)
        return '7';
    if(P1IN & BIT2==0)
        return '8';
    if(P1IN & BIT1==0)
        return '9';
    if(P1IN & BIT0==0)
        return 'C';

    //扫描第四行，第四行置为低电平
    P1OUT |= BIT5+BIT6+BIT4;
    P1OUT &= ~BIT7;
    if(P1IN & BIT3==0)
        return '*';
    if(P1IN & BIT2==0)
        return '0';
    if(P1IN & BIT1==0)
        return '#';
    if(P1IN & BIT0==0)
        return 'D';

    return '\0';
}

char key()  //有消抖
{
    char key0=key();
    _delay_cycles(3000000);
    if(key0==key())
        return key0;
}

char getkey()   //接收一个非空字符
{
    char key0;
    do{
        key0=key();
        if(key0!='\0') {OLED_AllClear();OLED_ShowChar(0,0,key0);}
    }while(key0=='\0');
    return key0;
}








