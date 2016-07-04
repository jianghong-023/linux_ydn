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
		uint8_t	b_0;
		uint8_t	b_1;
		uint8_t	b_2;
		uint8_t	b_3;
	} l;
} cv;

/* lcdд������ */
void lcd_Write_String( int y, char *c )
{
	int t = 0;
	while ( *c )
	{
		discontrl_t()->tmpbuf[t] = *c;
		c++;
		t++;
	}

	discontrl_t()->tmpbuf[strlen( discontrl_t()->tmpbuf ) + 1] = 0;
	write( discontrl_t()->lcdfd, discontrl_t()->tmpbuf, strlen( discontrl_t()->tmpbuf ) );
	memset( discontrl_t()->tmpbuf, 0, sizeof(char) * 40 );
}


/* ������ҳ���ܿ��� */
void ctl_fun( int lcdfd, uint8_t post )
{
	/*
	 * �����������ƶ����
	 * usleep(1000);
	 */
	ioctl( lcdfd, COURSOR_POST, post );     /* дС��ͷ */
}


/* ������� �����֣��ַ��� */
void ctl_cursor( int lcdfd, unsigned int cursor, uint8_t post )
{
	ioctl( lcdfd, cursor, post );           /* дС��ͷ */
}


/* ��꿪�� */
void cursor_onoff( int lcdfd, unsigned int cursor, unsigned int cursor_offon )
{
	ioctl( lcdfd, cursor, cursor_offon );
}


void cursor_move( int lcdfd, unsigned int cursor, unsigned int move )
{
	ioctl( lcdfd, cursor, move );
}


/* ���� */
void lcd_clear( int lcdfd )
{
	ioctl( lcdfd, CLEAR );
}


/* �����ַ����� */


/***************************************
 * rightlift	����λ�õ�ѡ��
 * updown_date	�ַ�ѡ��
 * lcdfd		ȫ���豸������
 *  �ڶ�����ʾ CHAR_S  ��һ����ʾ  ~CHAR_S
 **************************************/
void key_input_char( char rightlift, char updown_date, uint8_t change_g, int lcdfd )
{
	cv.l.b_0	= updown_date;
	cv.l.b_1	= CHAR_S;
	cv.l.b_2	= change_g;
	cv.l.b_3	= rightlift;

	ioctl( lcdfd, LCD_CHAR_INPUT, cv.l );
}

