//代码功能：实现基础的时钟走时进位，修改时制，修改时间，设置闹钟，计时器，并有断电保存功能
//作者：2021302121122 黄姗       时间:2022.01.24
//oled接线方式：vcc->3.3v,d0->p3.3,d1->p3.2,res->p3.1,dc->p3.0,cs->p2.2


/*头文件*/
#include <msp430.h>
#include "lib\keyboard.h"
#include "lib\oled.h"
#include "lib\bmp.h"
#include "lib\flash.h"
#include "lib\setclock.h"


/*变量*/
unsigned int count=0;//中断的临时变量
static int y=2022;static int mo=1;static int d=24;static int h=14;static int mi=24;static int s=54; //定义年月日时分秒为静态变量
static int x,a,b,c;//x,a,b,c与星期计算有关
static int flag=24;//时制标志
static int flag2=0;
static int value;
int adc_result=0;//储存adc测量值
volatile int adc_control=0;//adc采集状态标识
volatile int time_deal_flag;//时间处理标识
long temp;
volatile long IntDegC; //温度变量
static unsigned long *Flash_ptr;static unsigned long *addr;//flash存储所用的地址


//函数声明
void deal_time(void); //时间数据处理及打印
void flash_save(void); //将时间等变量存储到flash中
void adc_print(void); //将adc打印出来
double get_adc_value(int times); //获取传感器的数值
void init_adc(void); //初始化adc

//与菜单有关的函数及变量
void menu();  //显示菜单
void SwitchTimeSystem(); //修改时制A
void ChangeTheTime();  //修改时间B
void SetAlarms();  //设置闹钟C
int ah[3]={24};int ami[3]={60};int openornot[3] ={0}; //使用整型数组来存放闹钟，字符来判断闹钟开关
void shining(); //led闪烁
void TimeKeeping();   //计时D

int main(void)//定时器口中断控制函数
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    SetClock_MCLK12MHZ_SMCLK12MHZ_ACLK32_768K();// 下面的时钟频率都是12mhz
    TA0CTL |= TASSEL_2 + TACLR + MC_1 ; // smclk，清除计数器，增计数模式，12mhz
    TA0CCR0 = 21000; // 12mhz
    TA0CCTL1 = OUTMOD_7;

    TA0CCR1 = 10500; // 占空比4500/6000=0.75
    TA0CCTL2 = OUTMOD_3;
    TA0CCR2 = 10500; // 占空比1-4500/6000=0.25
    P1DIR |= BIT1+BIT2 + BIT3; // P1.2 P1.3输出
    P1SEL |= BIT1+BIT2 + BIT3; // P1.2 TA0CCR1 P1.3 TA0CCR2

    OLED_Init();      //初始化屏幕
    P1DIR|=BIT0;


    //判断断电次数，读取flash中的数据
    addr=(unsigned long *)0x1890;
    if(read_flash_int((unsigned long*)0x1890)>0 && read_flash_int((unsigned long*)0x1890)<10000) //初始值为FFFF，故设置一个上限
        value=read_flash_int((unsigned long*)0x1890);
    else value=0;
    if(value>0)
    {
        y=read_flash_int(0x1800);mo=read_flash_int(0x1804);d=read_flash_int(0x1808);
        h=read_flash_int(0x180c);mi=read_flash_int(0x1810);s=read_flash_int(0x1814);
        ah[0]=read_flash_int(0x1818);
        ami[0]=read_flash_int(0x181c);
        openornot[0]=read_flash_int(0x1820);
        ah[1]=read_flash_int(0x1824);
        ami[1]=read_flash_int(0x1828);
        openornot[1]=read_flash_int(0x182c);
        ah[2]=read_flash_int(0x1830);
        ami[2]=read_flash_int(0x1834);
        openornot[2]=read_flash_int(0x1838);
    }
    value++;   write_flash_int(addr,value);

    OLED_AllClear();
    OLED_ShowNum(2, 0, y, 4, 16);
    OLED_ShowChar(36, 0, '-');
    OLED_ShowNum(41, 0, mo, 2,16);
    OLED_ShowChar(56, 0, '-');
    OLED_ShowNum(61, 0, d, 2, 16);

                    a=y/100;//a为世纪
                    b=y%100;//b为后两位
                    if(mo<3)
                    {
                        c=mo+12;b=b-1;
                    }
                    else
                        c=mo;
                    x=(b+(int)(b/4.0)+(int)(a/4.0)-2*a+(int)(26*(c+1)/10.0)+d-1)%7;  //计算星期几
                    if(x==0)
                        OLED_ShowString(100,0,"SUN");
                    else if(x==1)
                        OLED_ShowString(100,0,"MON");
                    else if(x==2)
                        OLED_ShowString(100,0,"TUE");
                    else if(x==3)
                        OLED_ShowString(100,0,"WEN");
                    else if(x==4)
                        OLED_ShowString(100,0,"THU");
                    else if(x==5)
                        OLED_ShowString(100,0,"FRI");
                    else if(x==6)
                        OLED_ShowString(100,0,"SAT");
    if(flag==24) OLED_ShowNum(30, 3, h, 2, 16);
    else {if(h<13) {OLED_ShowNum(30,3,h,2,16);OLED_ShowString(96,3,"a.m.");}
          else {OLED_ShowNum(30,3,h-12,2,16);OLED_ShowString(96,3,"p.m.");}}
    OLED_ShowChar(46, 3, ':');
    OLED_ShowNum(50, 3, mi, 2, 16);
    OLED_ShowChar(66, 3, ':');
    OLED_ShowNum(70, 3, s, 2, 16);
    OLED_ShowCHinese(0,6,0);
    OLED_ShowCHinese(16,6,1);
    OLED_ShowChar(32,6, '*');


    TA0CCTL0 = CCIE;            //CCR0中断使能
    TA0CCR0 = 6000;            //设定计数值，12mhz,1.0
    TA0EX0 |= TAIDEX_4;     //A0计数器分频，5分频
    TA0CTL |=TASSEL_2+MC_1+TACLR+ID_3;//SMCLK，增计数模式，清除TAR
    __enable_interrupt();//允许全局中断，正常不需要低功耗模式

    init_key();//初始化键盘
    init_adc();//初始化adc
    char key1;
    while(1)//主函数循环，用来跑屏幕显示及菜单控制
    {
        if(time_deal_flag)
        {
            deal_time();//处理时间及打印
            time_deal_flag=0;//恢复标识
            char key1;
                if(key1=s_key()=='*')
                {
                    menu();  //显示菜单
                    //重新显示时间
                    OLED_AllClear();
                    OLED_ShowNum(2, 0, y, 4, 16);
                    OLED_ShowChar(36, 0, '-');
                    OLED_ShowNum(41, 0, mo, 2,16);
                    OLED_ShowChar(56, 0, '-');
                    OLED_ShowNum(61, 0, d, 2, 16);
                                    a=y/100;//a为世纪
                                    b=y%100;//b为后两位
                                    if(mo<3)
                                    {
                                        c=mo+12;b=b-1;
                                    }
                                    else
                                        c=mo;
                                    x=(b+(int)(b/4.0)+(int)(a/4.0)-2*a+(int)(26*(c+1)/10.0)+d-1)%7;  //计算星期几
                                    if(x==0)
                                        OLED_ShowString(100,0,"SUN");
                                    else if(x==1)
                                        OLED_ShowString(100,0,"MON");
                                    else if(x==2)
                                        OLED_ShowString(100,0,"TUE");
                                    else if(x==3)
                                        OLED_ShowString(100,0,"WEN");
                                    else if(x==4)
                                        OLED_ShowString(100,0,"THU");
                                    else if(x==5)
                                        OLED_ShowString(100,0,"FRI");
                                    else if(x==6)
                                        OLED_ShowString(100,0,"SAT");
                    if(flag==24) OLED_ShowNum(30, 3, h, 2, 16);
                    else {if(h<13) {OLED_ShowNum(30,3,h,2,16);OLED_ShowString(96,3,"a.m.");}
                          else {OLED_ShowNum(30,3,h-12,2,16);OLED_ShowString(96,3,"p.m.");}}
                    OLED_ShowChar(46, 3, ':');
                    OLED_ShowNum(50, 3, mi, 2, 16);
                    OLED_ShowChar(66, 3, ':');
                    OLED_ShowNum(70, 3, s, 2, 16);
                    OLED_ShowCHinese(0,6,0);
                    OLED_ShowCHinese(16,6,1);
                    OLED_ShowChar(32,6,'*');
                }
            if(key1=s_key()=='#')
            {
                int i;
                for(i=0;i<3;i++)
                  openornot[i]=0;
            }

        }
    }
}

//进行计时的中断
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    count++;
    if(count==52)
    {
        count=0;
        s++;
        time_deal_flag=1;//1s处理一次数据
        P1OUT^=BIT0;
    }
}

//时间数据处理及打印
void deal_time(void)
{
    if(s>=60)
    {
        mi++;s=s-60;
        if(mi>=60)
        {
            h++;
            mi=mi-60;

            if(h>=24)
            {
                h=0;
                d+=1;
                OLED_Clear(61,0,77,1);
                OLED_ShowNum(61, 0, d, 2, 16);
                a=y/100;//a为世纪
                b=y%100;//b为后两位
                if(mo<3)
                {
                    c=mo+12;b=b-1;
                }
                else
                    c=mo;
                x=(b+(int)(b/4.0)+(int)(a/4.0)-2*a+(int)(13*(c+1)/5)+d-1)%7;  //计算星期几
                if(x==0)
                    OLED_ShowString(100,0,"SUN");
                else if(x==1)
                    OLED_ShowString(100,0,"MON");
                else if(x==2)
                    OLED_ShowString(100,0,"TUE");
                else if(x==3)
                    OLED_ShowString(100,0,"WEN");
                else if(x==4)
                    OLED_ShowString(100,0,"THU");
                else if(x==5)
                    OLED_ShowString(100,0,"FRI");
                else if(x==6)
                    OLED_ShowString(100,0,"SAT");
                switch (mo)  //判断是否月末
                {
                    case 1:if(d==32) {mo+=1;d=1;}break;
                    case 2:
                        if(y%100==0)
                        {
                            if(y%400==0)
                                if(d==30) {mo+=1;d=1;}
                            else
                                if(d==29) {mo+=1;d=1;}
                        }
                    else if(y%4==0)  {if(d==30) {mo+=1;d=1;}
                    else if(d==29) {mo+=1;d=1;}}break;//二月的判断尤为复杂
                    case 3:if(d==32) {mo+=1;d=1;}break;
                    case 4:if(d==31) {mo+=1;d=1;}break;
                    case 5:if(d==32) {mo+=1;d=1;}break;
                    case 6:if(d==31) {mo+=1;d=1;}break;
                    case 7:if(d==32) {mo+=1;d=1;}break;
                    case 8:if(d==32) {mo+=1;d=1;}break;
                    case 9:if(d==31) {mo+=1;d=1;}break;
                    case 10:if(d==32) {mo+=1;d=1;}break;
                    case 11:if(d==31) {mo+=1;d=1;}break;
                    case 12:
                        if(d==32)
                        {
                            mo+=1;d=1;y+=1;
                            OLED_Clear(0,0,36,1);
                            OLED_ShowNum(2, 0, y, 4, 16);
                        }
                        break;
                }
                OLED_Clear(41,0,56,1);
                OLED_ShowNum(41, 0, mo, 2,16);
            }
        }
    }
    if(flag==24)
        OLED_ShowNum(30, 3, h, 2, 16);
    else
    {
        if(h<13)
        {
            OLED_ShowNum(30,3,h,2,16);
            OLED_ShowString(96,3,"a.m.");
        }
        else
        {
            OLED_ShowNum(30,3,h-12,2,16);
            OLED_ShowString(96,3,"p.m.");
        }
    }
    OLED_Clear(50,3,66,3);OLED_ShowNum(50,3, mi, 2, 16);
    OLED_Clear(70,3,90,3);
    OLED_ShowNum(70, 3, s, 2, 16);

    //判断闹钟
    int i;
    for(i=0;i<3;i++)
    {
      if((ah[i]==h)&&(ami[i]==mi))
          if(s<=20&&openornot[i]==1)
              {
                      P2DIR |= BIT3+BIT6;
              }

          else
          {
              P2DIR &= ~BIT3+BIT6;
          }

    }


    flash_save();//保存数据到flash里面
    get_adc_value(2);//采集2次adc值，返回平均值
    adc_print();//打印adc的值
}

//保存数据到flash之中
void flash_save(void)
{
    Flash_ptr=(unsigned long*)0x1800;
    FCTL1=FWKEY+ERASE;
    FCTL3=FWKEY;
    _DINT();
    *Flash_ptr=0;
    FCTL1=FWKEY+WRT;
    while(FCTL3 & BUSY);                      // 等待擦除操作完成
    FCTL1 = FWKEY + BLKWRT;                     // 使能长字写入操作
    *Flash_ptr= y;                     // 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr= mo;                     // 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr= d;                     // 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr= h;                     // 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr= mi;                     // 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr= s;                     // 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr= ah[0];
    Flash_ptr++;
    *Flash_ptr=ami[0];// 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr=openornot[0];
    Flash_ptr++;
    *Flash_ptr= ah[1];
    Flash_ptr++;
    *Flash_ptr=ami[1];// 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr=openornot[1];
    Flash_ptr++;
    *Flash_ptr= ah[2];
    Flash_ptr++;
    *Flash_ptr=ami[2];// 将长字写入目的Flash段
    Flash_ptr++;
    *Flash_ptr=openornot[2];
    while(FCTL3 & BUSY);                      // 等待写入操作完成
    _EINT();                                    // 启动全局中断
    FCTL1=FWKEY;
    FCTL3=FWKEY+LOCK;
}

//adc数据打印
void adc_print(void)
{
    OLED_ShowNum(80,6,IntDegC,2,16);
    OLED_ShowCHinese(98,6,17);
    OLED_ShowChar(105,6,'C');
}

//开启指定times次adc测量并返回平均读数，阻塞执行。
double get_adc_value(int times)
{
    double sum=0.0;//累计平均值
    int cycles=times;//循环次数
    ADC12CTL0|=ADC12SC;

    while(cycles>0)
    {
        adc_control=0;
        ADC12CTL0 &=~ADC12SC;
        ADC12CTL0 |= ADC12SC;   //构建上升沿开启转换
        while(adc_control==0);  //等待adc采集完成
        sum+=(double)adc_result;//累加采集量
        cycles--;
    }
    sum=sum/times;//平均
    IntDegC=(sum-746)*0.0355+286;
    return sum;
}
//这个函数用来初始化adc，每次运行只用调用一次
void init_adc(void)
{
    //这里一般还会把需要初始化的IO代码写在这里
    REFCTL0&=~REFMSTR;
    ADC12CTL0=ADC12ON+ADC12SHT0_8+ADC12REFON;
    ADC12CTL1=ADC12SHP;
    ADC12MCTL0=ADC12SREF_1+ADC12INCH_10;
    ADC12IE=0x001;
    __delay_cycles(75);
    ADC12CTL0|=ADC12ENC;
}

//温度中断采样
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch(_even_in_range(ADC12IV,34))
    {
    case 0:break;
    case 2:break;
    case 4:break;
    case 6:
        adc_result=ADC12MEM0;//读取adc采集数据值
        adc_control=1;//更改adc状态
        break;
    case 8:break;
    case 10:break;
    case 12:break;
    case 14:break;
    case 16:break;
    case 18:break;
    case 20:break;
    default:break;

    }
}

void menu()
{
    OLED_Init();      //初始化屏幕
    OLED_AllClear();
  OLED_ShowCHinese(5,0,2);
  OLED_ShowCHinese(21,0,3);
  OLED_ShowCHinese(37,0,4);
  OLED_ShowCHinese(55,0,5);
  OLED_ShowChar(75,0,'A');
  OLED_ShowCHinese(5,2,6);
  OLED_ShowCHinese(21,2,7);
  OLED_ShowCHinese(37,2,8);
  OLED_ShowCHinese(55,2,9);
  OLED_ShowChar(75,2,'B');
  OLED_ShowCHinese(5,4,10);
  OLED_ShowCHinese(21,4,11);
  OLED_ShowCHinese(37,4,12);
  OLED_ShowCHinese(55,4,13);
  OLED_ShowChar(75,4,'C');
  OLED_ShowCHinese(5,6,14);
  OLED_ShowCHinese(21,6,15);
  OLED_ShowCHinese(37,6,16);
  OLED_ShowChar(55,6,'D');
  OLED_ShowCHinese(80,6,18);
  OLED_ShowCHinese(96,6,19);
  OLED_ShowChar(112,6,'#');


char key1;
do{
    key1=key();
}while(key1=='\0');
switch(key1)
{
    case 'A':  SwitchTimeSystem();break;
    case'B':  ChangeTheTime();break;
    case'C':  SetAlarms();break;
    case'D':  TimeKeeping();break;
    default:break;
}

}


void SwitchTimeSystem() //修改时制A
{
 if(flag==24)flag=12;
 else flag=24;
}

void ChangeTheTime()  //修改时间B
{
    OLED_AllClear();
    OLED_ShowCHinese(10,0,20);
    OLED_ShowCHinese(26,0,21);
    OLED_ShowNum(42,0,24,2,16);
    OLED_ShowCHinese(58,0,22);
    OLED_ShowCHinese(74,0,23);
    OLED_ShowCHinese(90,0,24);
    OLED_ShowCHinese(0,2,25);
    OLED_ShowCHinese(16,2,26);
    OLED_ShowCHinese(32,2,27);
    OLED_ShowCHinese(48,2,28);
    OLED_ShowCHinese(64,2,29);
    OLED_ShowChar(80,2,'*');
    OLED_ShowCHinese(96,2,30);
    OLED_ShowCHinese(112,2,31);

rechange:
  y=(int)(getkey()-'0')*1000+(getkey()-'0')*100+(getkey()-'0')*10+(getkey()-'0');
            mo=(int)(getkey()-'0')*10+(getkey()-'0');
            d=(int)(getkey()-'0')*10+(getkey()-'0');
            h=(int)(getkey()-'0')*10+(getkey()-'0');
            mi=(int)(getkey()-'0')*10+(getkey()-'0');
            s=0;
            //判断的语句不够精确
     if(mo>12||d>31)
         {
         OLED_AllClear();
         OLED_ShowString(2,0,"Time is incorrect");
         OLED_ShowString(0,2," please reset");
         goto rechange;
         }
     if(h>23||mi>59)
         {
         OLED_AllClear();
         OLED_ShowString(2,0,"Time is incorrect,please reset");
         goto rechange;
         }
    if(getkey()!='*')
        goto rechange;
}


void SetAlarms()  //设置闹钟C
{
   OLED_AllClear();
   OLED_ShowCHinese(10,0,20);
   OLED_ShowCHinese(26,0,21);//an
   OLED_ShowString(42,0,"ABC");
   OLED_ShowCHinese(66,0,38);//来
   OLED_ShowCHinese(82,0,39);//选
   OLED_ShowCHinese(98,0,40);//ze
   OLED_ShowCHinese(0,2,41);//san
   OLED_ShowCHinese(16,2,42);//ge
   OLED_ShowCHinese(32,2,12);//nao
   OLED_ShowCHinese(48,2,13);//zhong
   OLED_ShowCHinese(84,5,18);
   OLED_ShowCHinese(100,5,19);
   OLED_ShowChar(116,5,'#');


  int i;char key2;
  switch (key2=getkey())//选择A B C 闹钟
  {
    case 'A':i=0;break;
    case 'B':i=1;break;
    case 'C':i=2;break;
    default:break;
  }
  if(i>=0 && i<=2)
  A:{ OLED_AllClear();
  OLED_ShowNum(20,0,ah[i],2,16);
  OLED_ShowChar(36,0,':');
  OLED_ShowNum(40,0,ami[i],2,16);
  if(openornot[i]==1)
        OLED_ShowChar(60,0,'*');
  else
      OLED_ShowChar(60,0,'#');
  OLED_ShowCHinese(0,2,20);//请
  OLED_ShowCHinese(16,2,21);//按
  OLED_ShowChar(42,2,'A');
  OLED_ShowCHinese(50,2,38);//来
  OLED_ShowCHinese(66,2,6);//XIU
  OLED_ShowCHinese(82,2,7);//GAI
  OLED_ShowCHinese(0,4,20);//请
  OLED_ShowCHinese(16,4,21);//AN
  OLED_ShowChar(42,4,'*');
  OLED_ShowCHinese(50,4,43);//da
  OLED_ShowCHinese(66,4,44);//kai
  OLED_ShowCHinese(84,6,18);
  OLED_ShowCHinese(100,6,19);
  OLED_ShowChar(116,6,'#');


   char key3=getkey();
   if(key3=='*'){if(openornot[i]==0)openornot[i]=1;else openornot[i]=0;}
   else if(key3=='A')
  {
       OLED_AllClear();
       OLED_ShowString(0,0,"Follow the 24 hour clock");
       OLED_ShowString(0,2,"Enter a two-digit hour");
       OLED_ShowString(0,4,"a two-digit minutes.");

 ah[i]=(int)((getkey()-'0')*10+(getkey()-'0'));
 ami[i]=(int)((getkey()-'0')*10+(getkey()-'0'));
 goto A; //重新回到闹钟的页面
 }
  }
}

/*void shining(void) //led闪烁，充当闹钟
{
    P2DIR |= BIT3+BIT6;
    P3DIR |= BIT5+BIT6+BIT7;
    P8DIR |= BIT1+BIT2;
    P4DIR |= BIT0;
    int i;
    for(i=1;i<10;i++)
    {
    P2OUT &= ~(BIT3+BIT6);
    P3OUT &= ~(BIT5+BIT6+BIT7);
    P8OUT &= ~(BIT1+BIT2);
    P4OUT &= ~BIT0;
    _delay_cycles(1000000);
    P2OUT |= (BIT3+BIT6);
    P3OUT |= (BIT5+BIT6+BIT7);
    P8OUT |= (BIT1+BIT2);
    P4OUT |= BIT0;
    _delay_cycles(1000000);}
}*/

void TimeKeeping()   //计时D
{
    OLED_AllClear();
    OLED_ShowString(0,0,"To start A");
    OLED_ShowString(0,2,"To pause B");
    OLED_ShowString(0,4,"To reset C");
    OLED_ShowString(0,6,"To close #");

  char k;
  do{
  k=getkey();
  }while(k=='\0');
  if(k=='A')
      retime:{
      int kh=0;int kmi=0;int ks=0;
      count: OLED_AllClear();
  do{
   cont:
   OLED_Clear(0,0,120,2);
   OLED_ShowNum(20,3,kh,2,16);
   OLED_ShowChar(36,3,':');
   OLED_ShowNum(40,3,kmi,2,16);
   OLED_ShowChar(56,3,':');
   OLED_ShowNum(60,3,ks,2,16);
  ks++;
  if(ks>=60)
     {kmi++;ks=ks-60;}
  if(kmi>=60)
     {kh++;kmi=kmi-60;}
     k=key();
  _delay_cycles(12000000);
  }while(k!='B'&&k!='C'&&k!='#');}

  switch (k)
  {
    case 'B':{ OLED_ShowString(0,0,"continue B");char key4; do{key4=key();} while(key4=='\0');if(key4=='B') goto cont;}  //cont指向继续计时
    case 'C':{ OLED_ShowString(0,0,"retime A");  char key5; do{key5=key();} while(key5=='\0');if(key5=='A') goto retime;}  //retime指向重新计时
    case '#':break;
  }
}


