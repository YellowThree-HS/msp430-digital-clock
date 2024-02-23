//���빦�ܣ�ʵ�ֻ�����ʱ����ʱ��λ���޸�ʱ�ƣ��޸�ʱ�䣬�������ӣ���ʱ�������жϵ籣�湦��
//���ߣ�2021302121122 ���       ʱ��:2022.01.24
//oled���߷�ʽ��vcc->3.3v,d0->p3.3,d1->p3.2,res->p3.1,dc->p3.0,cs->p2.2


/*ͷ�ļ�*/
#include <msp430.h>
#include "lib\keyboard.h"
#include "lib\oled.h"
#include "lib\bmp.h"
#include "lib\flash.h"
#include "lib\setclock.h"


/*����*/
unsigned int count=0;//�жϵ���ʱ����
static int y=2022;static int mo=1;static int d=24;static int h=14;static int mi=24;static int s=54; //����������ʱ����Ϊ��̬����
static int x,a,b,c;//x,a,b,c�����ڼ����й�
static int flag=24;//ʱ�Ʊ�־
static int flag2=0;
static int value;
int adc_result=0;//����adc����ֵ
volatile int adc_control=0;//adc�ɼ�״̬��ʶ
volatile int time_deal_flag;//ʱ�䴦���ʶ
long temp;
volatile long IntDegC; //�¶ȱ���
static unsigned long *Flash_ptr;static unsigned long *addr;//flash�洢���õĵ�ַ


//��������
void deal_time(void); //ʱ�����ݴ�����ӡ
void flash_save(void); //��ʱ��ȱ����洢��flash��
void adc_print(void); //��adc��ӡ����
double get_adc_value(int times); //��ȡ����������ֵ
void init_adc(void); //��ʼ��adc

//��˵��йصĺ���������
void menu();  //��ʾ�˵�
void SwitchTimeSystem(); //�޸�ʱ��A
void ChangeTheTime();  //�޸�ʱ��B
void SetAlarms();  //��������C
int ah[3]={24};int ami[3]={60};int openornot[3] ={0}; //ʹ������������������ӣ��ַ����ж����ӿ���
void shining(); //led��˸
void TimeKeeping();   //��ʱD

int main(void)//��ʱ�����жϿ��ƺ���
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    SetClock_MCLK12MHZ_SMCLK12MHZ_ACLK32_768K();// �����ʱ��Ƶ�ʶ���12mhz
    TA0CTL |= TASSEL_2 + TACLR + MC_1 ; // smclk�������������������ģʽ��12mhz
    TA0CCR0 = 21000; // 12mhz
    TA0CCTL1 = OUTMOD_7;

    TA0CCR1 = 10500; // ռ�ձ�4500/6000=0.75
    TA0CCTL2 = OUTMOD_3;
    TA0CCR2 = 10500; // ռ�ձ�1-4500/6000=0.25
    P1DIR |= BIT1+BIT2 + BIT3; // P1.2 P1.3���
    P1SEL |= BIT1+BIT2 + BIT3; // P1.2 TA0CCR1 P1.3 TA0CCR2

    OLED_Init();      //��ʼ����Ļ
    P1DIR|=BIT0;


    //�ж϶ϵ��������ȡflash�е�����
    addr=(unsigned long *)0x1890;
    if(read_flash_int((unsigned long*)0x1890)>0 && read_flash_int((unsigned long*)0x1890)<10000) //��ʼֵΪFFFF��������һ������
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

                    a=y/100;//aΪ����
                    b=y%100;//bΪ����λ
                    if(mo<3)
                    {
                        c=mo+12;b=b-1;
                    }
                    else
                        c=mo;
                    x=(b+(int)(b/4.0)+(int)(a/4.0)-2*a+(int)(26*(c+1)/10.0)+d-1)%7;  //�������ڼ�
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


    TA0CCTL0 = CCIE;            //CCR0�ж�ʹ��
    TA0CCR0 = 6000;            //�趨����ֵ��12mhz,1.0
    TA0EX0 |= TAIDEX_4;     //A0��������Ƶ��5��Ƶ
    TA0CTL |=TASSEL_2+MC_1+TACLR+ID_3;//SMCLK��������ģʽ�����TAR
    __enable_interrupt();//����ȫ���жϣ���������Ҫ�͹���ģʽ

    init_key();//��ʼ������
    init_adc();//��ʼ��adc
    char key1;
    while(1)//������ѭ������������Ļ��ʾ���˵�����
    {
        if(time_deal_flag)
        {
            deal_time();//����ʱ�估��ӡ
            time_deal_flag=0;//�ָ���ʶ
            char key1;
                if(key1=s_key()=='*')
                {
                    menu();  //��ʾ�˵�
                    //������ʾʱ��
                    OLED_AllClear();
                    OLED_ShowNum(2, 0, y, 4, 16);
                    OLED_ShowChar(36, 0, '-');
                    OLED_ShowNum(41, 0, mo, 2,16);
                    OLED_ShowChar(56, 0, '-');
                    OLED_ShowNum(61, 0, d, 2, 16);
                                    a=y/100;//aΪ����
                                    b=y%100;//bΪ����λ
                                    if(mo<3)
                                    {
                                        c=mo+12;b=b-1;
                                    }
                                    else
                                        c=mo;
                                    x=(b+(int)(b/4.0)+(int)(a/4.0)-2*a+(int)(26*(c+1)/10.0)+d-1)%7;  //�������ڼ�
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

//���м�ʱ���ж�
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    count++;
    if(count==52)
    {
        count=0;
        s++;
        time_deal_flag=1;//1s����һ������
        P1OUT^=BIT0;
    }
}

//ʱ�����ݴ�����ӡ
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
                a=y/100;//aΪ����
                b=y%100;//bΪ����λ
                if(mo<3)
                {
                    c=mo+12;b=b-1;
                }
                else
                    c=mo;
                x=(b+(int)(b/4.0)+(int)(a/4.0)-2*a+(int)(13*(c+1)/5)+d-1)%7;  //�������ڼ�
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
                switch (mo)  //�ж��Ƿ���ĩ
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
                    else if(d==29) {mo+=1;d=1;}}break;//���µ��ж���Ϊ����
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

    //�ж�����
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


    flash_save();//�������ݵ�flash����
    get_adc_value(2);//�ɼ�2��adcֵ������ƽ��ֵ
    adc_print();//��ӡadc��ֵ
}

//�������ݵ�flash֮��
void flash_save(void)
{
    Flash_ptr=(unsigned long*)0x1800;
    FCTL1=FWKEY+ERASE;
    FCTL3=FWKEY;
    _DINT();
    *Flash_ptr=0;
    FCTL1=FWKEY+WRT;
    while(FCTL3 & BUSY);                      // �ȴ������������
    FCTL1 = FWKEY + BLKWRT;                     // ʹ�ܳ���д�����
    *Flash_ptr= y;                     // ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr= mo;                     // ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr= d;                     // ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr= h;                     // ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr= mi;                     // ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr= s;                     // ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr= ah[0];
    Flash_ptr++;
    *Flash_ptr=ami[0];// ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr=openornot[0];
    Flash_ptr++;
    *Flash_ptr= ah[1];
    Flash_ptr++;
    *Flash_ptr=ami[1];// ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr=openornot[1];
    Flash_ptr++;
    *Flash_ptr= ah[2];
    Flash_ptr++;
    *Flash_ptr=ami[2];// ������д��Ŀ��Flash��
    Flash_ptr++;
    *Flash_ptr=openornot[2];
    while(FCTL3 & BUSY);                      // �ȴ�д��������
    _EINT();                                    // ����ȫ���ж�
    FCTL1=FWKEY;
    FCTL3=FWKEY+LOCK;
}

//adc���ݴ�ӡ
void adc_print(void)
{
    OLED_ShowNum(80,6,IntDegC,2,16);
    OLED_ShowCHinese(98,6,17);
    OLED_ShowChar(105,6,'C');
}

//����ָ��times��adc����������ƽ������������ִ�С�
double get_adc_value(int times)
{
    double sum=0.0;//�ۼ�ƽ��ֵ
    int cycles=times;//ѭ������
    ADC12CTL0|=ADC12SC;

    while(cycles>0)
    {
        adc_control=0;
        ADC12CTL0 &=~ADC12SC;
        ADC12CTL0 |= ADC12SC;   //���������ؿ���ת��
        while(adc_control==0);  //�ȴ�adc�ɼ����
        sum+=(double)adc_result;//�ۼӲɼ���
        cycles--;
    }
    sum=sum/times;//ƽ��
    IntDegC=(sum-746)*0.0355+286;
    return sum;
}
//�������������ʼ��adc��ÿ������ֻ�õ���һ��
void init_adc(void)
{
    //����һ�㻹�����Ҫ��ʼ����IO����д������
    REFCTL0&=~REFMSTR;
    ADC12CTL0=ADC12ON+ADC12SHT0_8+ADC12REFON;
    ADC12CTL1=ADC12SHP;
    ADC12MCTL0=ADC12SREF_1+ADC12INCH_10;
    ADC12IE=0x001;
    __delay_cycles(75);
    ADC12CTL0|=ADC12ENC;
}

//�¶��жϲ���
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch(_even_in_range(ADC12IV,34))
    {
    case 0:break;
    case 2:break;
    case 4:break;
    case 6:
        adc_result=ADC12MEM0;//��ȡadc�ɼ�����ֵ
        adc_control=1;//����adc״̬
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
    OLED_Init();      //��ʼ����Ļ
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


void SwitchTimeSystem() //�޸�ʱ��A
{
 if(flag==24)flag=12;
 else flag=24;
}

void ChangeTheTime()  //�޸�ʱ��B
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
            //�жϵ���䲻����ȷ
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


void SetAlarms()  //��������C
{
   OLED_AllClear();
   OLED_ShowCHinese(10,0,20);
   OLED_ShowCHinese(26,0,21);//an
   OLED_ShowString(42,0,"ABC");
   OLED_ShowCHinese(66,0,38);//��
   OLED_ShowCHinese(82,0,39);//ѡ
   OLED_ShowCHinese(98,0,40);//ze
   OLED_ShowCHinese(0,2,41);//san
   OLED_ShowCHinese(16,2,42);//ge
   OLED_ShowCHinese(32,2,12);//nao
   OLED_ShowCHinese(48,2,13);//zhong
   OLED_ShowCHinese(84,5,18);
   OLED_ShowCHinese(100,5,19);
   OLED_ShowChar(116,5,'#');


  int i;char key2;
  switch (key2=getkey())//ѡ��A B C ����
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
  OLED_ShowCHinese(0,2,20);//��
  OLED_ShowCHinese(16,2,21);//��
  OLED_ShowChar(42,2,'A');
  OLED_ShowCHinese(50,2,38);//��
  OLED_ShowCHinese(66,2,6);//XIU
  OLED_ShowCHinese(82,2,7);//GAI
  OLED_ShowCHinese(0,4,20);//��
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
 goto A; //���»ص����ӵ�ҳ��
 }
  }
}

/*void shining(void) //led��˸���䵱����
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

void TimeKeeping()   //��ʱD
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
    case 'B':{ OLED_ShowString(0,0,"continue B");char key4; do{key4=key();} while(key4=='\0');if(key4=='B') goto cont;}  //contָ�������ʱ
    case 'C':{ OLED_ShowString(0,0,"retime A");  char key5; do{key5=key();} while(key5=='\0');if(key5=='A') goto retime;}  //retimeָ�����¼�ʱ
    case '#':break;
  }
}


