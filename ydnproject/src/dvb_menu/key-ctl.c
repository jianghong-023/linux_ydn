#include <key-ctl.h>
#include <lcd1602.h>
#include <common_conf.h>
#include <defineident.h>

#define PUT_CHAR	0x0
#define CLEAR		0x1
#define COURSOR_POST	0x3

/* extern dis_contrl discontrl; */

union {
	unsigned long n;
	struct {
		uint8_t b_0;
		uint8_t b_1;
		uint8_t b_2;
		uint8_t b_3;
	} l;
} cv;

/* lcd写入数据 */
void lcd_Write_String( int y, char *c )
{
	int lent = 0;

	lent = strlen( c );

	if ( y == 0 )
	{
		int i;
		for ( i = 0; i < lent; i++ )
			key_input_char( i, c[i], ~CHANGE_G, discontrl_t()->lcdfd );
	}else if ( y == 1 )
	{
		int i;
		for ( i = 0; i < lent; i++ )
			key_input_char( i, c[i], CHANGE_G, discontrl_t()->lcdfd );
	}
}


/* 按键翻页功能控制 */
void ctl_fun( int lcdfd, uint8_t post )
{
	/*
	 * 在行中上下移动光标
	 * usleep(1000);
	 */
	ioctl( lcdfd, COURSOR_POST, post );     /* 写小箭头 */
}


/* 按键光标 【数字，字符】 */
void ctl_cursor( int lcdfd, unsigned int cursor, uint8_t post )
{
	ioctl( lcdfd, cursor, post );           /* 写小箭头 */
}


/* 光标开关 */
void cursor_onoff( int lcdfd, unsigned int cursor, unsigned int cursor_offon )
{
	ioctl( lcdfd, cursor, cursor_offon );
}


void cursor_move( int lcdfd, unsigned int cursor, unsigned int move )
{
	ioctl( lcdfd, cursor, move );
}


/* 清屏 */
void lcd_clear( int lcdfd )
{
	ioctl( lcdfd, CLEAR );
}


/* 按键字符输入 */


/***************************************
 * rightlift	用于位置的选择
 * updown_date	字符选择
 * lcdfd		全局设备描述符
 *  第二行显示 CHAR_S  第一行显示  ~CHAR_S
 **************************************/
void key_input_char( char rightlift, char updown_date, uint8_t change_g, int lcdfd )
{
	cv.l.b_0	= updown_date;
	cv.l.b_1	= CHAR_S;
	cv.l.b_2	= change_g;
	cv.l.b_3	= rightlift;

	ioctl( lcdfd, LCD_CHAR_INPUT, cv.l );
}


