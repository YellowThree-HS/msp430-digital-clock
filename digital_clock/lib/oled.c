//////////////////////////////////////////////////////////////////////////////////
//閺堫剛鈻兼惔蹇撳涧娓氭稑顒熸稊鐘卞▏閻㈩煉绱濋張顏嗙病娴ｆ粏锟藉懓顔忛崣顖ょ礉娑撳秴绶遍悽銊ょ艾閸忚泛鐣犳禒璁崇秿閻€劑锟斤拷
//娑擃厽娅欓崶顓犳暩鐎涳拷
//鎼存鎽甸崷鏉挎絻閿涙ttp://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  閺傦拷 娴狅拷 閸氾拷   : oled.c
//  閻楋拷 閺堬拷 閸欙拷   : v2.0
//  娴ｏ拷    閼帮拷   : HuangKai
//  閻㈢喐鍨氶弮銉︽埂   : 2014-0101
//  閺堬拷鏉╂垳鎱ㄩ弨锟�   :
//  閸旂喕鍏橀幓蹇氬牚   : 0.96鐎电LED 閹恒儱褰涘鏃傘仛娓氬鈻�(MSP430F149缁鍨�)
//  妞瑰崬濮㊣C     :SSD1309
//              鐠囧瓨妲�:
//              ----------------------------------------------------------------
//              GND    閻㈠灚绨崷锟�
//              VCC  閹猴拷5V閹达拷3.3v閻㈠灚绨�
//              D0   P13閿涘牊妞傞柦鐕傜礆
//              D1   P14閿涘牊鏆熼幑顕嗙礆
//              RES  閹侯櫀11
//              DC   閹侯櫀12
//              CS   閹侯櫀10
//              ----------------------------------------------------------------
// 娣囶喗鏁奸崢鍡楀蕉   :
// 閺冿拷    閺堬拷   :
// 娴ｏ拷    閼帮拷   : HuangKai
// 娣囶喗鏁奸崘鍛啇   : 閸掓稑缂撻弬鍥︽
//閻楀牊娼堥幍锟介張澶涚礉閻╂澧楄箛鍛敀閵嗭拷
//Copyright(C) 娑擃厽娅欓崶顓犳暩鐎涳拷2014/3/16
//All rights reserved
//******************************************************************************/
#include "oled.h"
//#include "stdlib.h"
#include "oledfont.h"
//#include "delay.h"
//OLED閻ㄥ嫭妯夌�涳拷
//鐎涙ɑ鏂侀弽鐓庣础婵″倷绗�.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127
void delay_ms(unsigned int ms) {
	unsigned int a;
	while (ms) {
		a = 1800;
		while (a--)
			;
		ms--;
	}
	return;
}

//閸氭叀SD1306閸愭瑥鍙嗘稉锟芥稉顏勭摟閼哄倶锟斤拷
//dat:鐟曚礁鍟撻崗銉ф畱閺佺増宓�/閸涙垝鎶�
//cmd:閺佺増宓�/閸涙垝鎶ら弽鍥х箶 0,鐞涖劎銇氶崨鎴掓姢;1,鐞涖劎銇氶弫鐗堝祦;
void OLED_WR_Byte(u8 dat, u8 cmd) {
	u8 i;
	if (cmd)
		OLED_DC_Set();
	else
		OLED_DC_Clr();
	OLED_CS_Clr();
	for (i = 0; i < 8; i++) {
		OLED_SCLK_Clr();
		if (dat & 0x80) {
			OLED_SDIN_Set();
		} else
			OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat <<= 1;
	}
	OLED_CS_Set();
	OLED_DC_Set();
}

void OLED_Set_Pos(unsigned char x, unsigned char y) {
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f) | 0x01, OLED_CMD);
}
//瀵拷閸氱枾LED閺勫墽銇�
void OLED_Display_On(void) {
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC閸涙垝鎶�
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON
}
//閸忔娊妫碠LED閺勫墽銇�
void OLED_Display_Off(void) {
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC閸涙垝鎶�
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF
}
//濞撳懎鐫嗛崙鑺ユ殶,濞撳懎鐣仦锟�,閺佺繝閲滅仦蹇撶閺勵垶绮﹂懝鑼畱!閸滃本鐥呴悙閫涘瘨娑擄拷閺嶏拷!!!
void OLED_AllClear(void) {
	u8 i, n;
	for (i = 0; i < 8; i++) {
		OLED_WR_Byte(0xb0 + i, OLED_CMD);    //鐠佸墽鐤嗘い闈涙勾閸э拷閿涳拷0~7閿涳拷
		OLED_WR_Byte(0x00, OLED_CMD);      //鐠佸墽鐤嗛弰鍓с仛娴ｅ秶鐤嗛垾鏂垮灙娴ｅ骸婀撮崸锟�
		OLED_WR_Byte(0x10, OLED_CMD);      //鐠佸墽鐤嗛弰鍓с仛娴ｅ秶鐤嗛垾鏂垮灙妤傛ê婀撮崸锟�
		for (n = 0; n < 130; n++)
			OLED_WR_Byte(0, OLED_DATA);
	} //閺囧瓨鏌婇弰鍓с仛
}

void OLED_Clear(u8 x0,u8 y0,u8 x1,u8 y1){
    u8 i,n;
    for(i=y0;i<y1;i++)
        {
            OLED_WR_Byte (0xb0+i,OLED_CMD);
            OLED_WR_Byte (0x00,OLED_CMD);
            OLED_WR_Byte (0x10,OLED_CMD);
            for(n=x0;n<x1;n++) OLED_WR_Byte(0,OLED_DATA);
        }
}

//閸︺劍瀵氱�规矮缍呯純顔芥▔缁�杞扮娑擃亜鐡х粭锟�,閸栧懏瀚柈銊ュ瀻鐎涙顑�
//x:0~127
//y:0~63
//mode:0,閸欏秶娅ч弰鍓с仛;1,濮濓絽鐖堕弰鍓с仛
//size:闁瀚ㄧ�涙ぞ缍� 16/12
void OLED_ShowChar(u8 x, u8 y, u8 chr) {
	unsigned char c = 0, i = 0;
	c = chr - ' '; //瀵版鍩岄崑蹇曅╅崥搴ｆ畱閸婏拷
	if (x > Max_Column - 1) {
		x = 0;
		y = y + 2;
	}
	if (SIZE == 16) {
		OLED_Set_Pos(x, y);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
	} else {
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 6; i++)
			OLED_WR_Byte(F6x8[c][i], OLED_DATA);

	}
}
//m^n閸戣姤鏆�
u32 oled_pow(u8 m, u8 n) {
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}
//閺勫墽銇�2娑擃亝鏆熺�涳拷
//x,y :鐠ч鍋ｉ崸鎰垼
//len :閺佹澘鐡ч惃鍕秴閺侊拷
//size:鐎涙ぞ缍嬫径褍鐨�
//mode:濡�崇础	0,婵夘偄鍘栧Ο鈥崇础;1,閸欑姴濮炲Ο鈥崇础
//num:閺佹澘锟斤拷(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2) {
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++) {
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1)) {
			if (temp == 0) {
				OLED_ShowChar(x + (size2 / 2) * t, y, ' ');
				continue;
			} else
				enshow = 1;

		}
		OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0');
	}
}
//閺勫墽銇氭稉锟芥稉顏勭摟缁楋箑褰挎稉锟�
void OLED_ShowString(u8 x, u8 y, u8 *chr) {
	unsigned char j = 0;
	while (chr[j] != '\0') {
		OLED_ShowChar(x, y, chr[j]);
		x += 8;
		if (x > 120) {
			x = 0;
			y += 2;
		}
		j++;
	}
}
//閺勫墽銇氬Ч澶婄摟
void OLED_ShowCHinese(u8 x, u8 y, u8 no) {
	u8 t, adder = 0;
	OLED_Set_Pos(x, y);
	for (t = 0; t < 16; t++) {
		OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
		adder += 1;
	}
	OLED_Set_Pos(x, y + 1);
	for (t = 0; t < 16; t++) {
		OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
		adder += 1;
	}
}
/***********閸旂喕鍏橀幓蹇氬牚閿涙碍妯夌粈鐑樻▔缁�绡塎P閸ュ墽澧�130鑴�64鐠у嘲顫愰悙鐟版綏閺嶏拷(x,y),x閻ㄥ嫯瀵栭崶锟�0閿濓拷127閿涘瘔娑撴椽銆夐惃鍕瘱閸ワ拷0閿濓拷7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1,unsigned char y1, unsigned char BMP[]) {
    unsigned int j = 0;
    unsigned char x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++) {
        OLED_Set_Pos(x0, y);
        for (x = x0; x < x1; x++) {
            OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}

//閸掓繂顫愰崠鏈D1306
void OLED_Init(void) {
	OLED_SSD1306_SCLK_IO_INIT;
	OLED_SSD1306_SDIN_IO_INIT;
	OLED_SSD1306_DC_IO_INIT;
	OLED_SSD1306_CE_IO_INIT;
	OLED_SSD1306_RST_IO_INIT;

	OLED_RST_Set();
	delay_ms(100);
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set();
	/*
	 OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	 OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	 OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	 OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	 OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	 OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	 OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0瀹革箑褰搁崣宥囩枂 0xa1濮濓絽鐖�
	 OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0娑撳﹣绗呴崣宥囩枂 0xc8濮濓絽鐖�
	 OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	 OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	 OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	 OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	 OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	 OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	 OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	 OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	 OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	 OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	 OLED_WR_Byte(0x12,OLED_CMD);
	 OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	 OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	 OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	 OLED_WR_Byte(0x02,OLED_CMD);//
	 OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	 OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	 OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	 OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7)
	 OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	 */

	OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel
	OLED_WR_Byte(0x00, OLED_CMD); //---set low column address
	OLED_WR_Byte(0x10, OLED_CMD); //---set high column address
	OLED_WR_Byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81, OLED_CMD); //--set contrast control register
	OLED_WR_Byte(0xCF, OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0瀹革箑褰搁崣宥囩枂 0xa1濮濓絽鐖�
	OLED_WR_Byte(0xC8, OLED_CMD); //Set COM/Row Scan Direction   0xc0娑撳﹣绗呴崣宥囩枂 0xc8濮濓絽鐖�
	OLED_WR_Byte(0xA6, OLED_CMD); //--set normal display
	OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f, OLED_CMD); //--1/64 duty
	OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_CMD); //-not offset
	OLED_WR_Byte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9, OLED_CMD); //--set pre-charge period
	OLED_WR_Byte(0xF1, OLED_CMD); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA, OLED_CMD); //--set com pins hardware configuration
	OLED_WR_Byte(0x12, OLED_CMD);
	OLED_WR_Byte(0xDB, OLED_CMD); //--set vcomh
	OLED_WR_Byte(0x40, OLED_CMD); //Set VCOM Deselect Level
	OLED_WR_Byte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02, OLED_CMD); //
	OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
	OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable
	OLED_WR_Byte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)
	OLED_WR_Byte(0xAF, OLED_CMD); //--turn on oled panel

	OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
	OLED_AllClear();
	OLED_Set_Pos(0, 0);
}
