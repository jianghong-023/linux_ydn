#define _GNU_SOURCE
#include <math_instance.h>
#include <debug.h>

union ch {
	uint8_t c;
	struct {
		uint8_t c_1 : 4;
		uint8_t c_2 : 4;
	} n;
};

#define get_hex_val( x ) ( (x).c)

char * itoa_( int num, char*str, int radix )
{
	/*索引表*/
	char		index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned	unum;           /*中间变量*/
	int		i = 0, j, k;
//	DEBUG( "num= %d ", num );
	/*确定unum的值*/
	if ( radix == 10 && num < 0 )   /*十进制负数*/
	{
		unum		= (unsigned) -num;
		str[i++]	= '-';
	} else unum = (unsigned) num;   /*其他情况*/
	/*转换*/
	do
	{
		str[i++]	= index[unum % (unsigned) radix];
		unum		/= radix;
	}
	while ( unum );
	str[i] = '\0';
	/*逆序*/
	if ( str[0] == '-' )
		k = 1; /*十进制负数*/
	else k = 0;
	char temp;

	for ( j = k; j <= (i - 1) / 2; j++ )
	{
		temp			= str[j];
		str[j]			= str[i - 1 + k - j];
		str[i - 1 + k - j]	= temp;
	}
	return(str);
}


char * safe_strdup( const char *s )
{
	char * retval = NULL;
	if ( !s )
	{
		DEBUG( "safe_strdup called with NULL which would have crashed strdup. Bailing out \r\n" );
		exit( 1 );
	}
	retval = strdup( s );
	if ( !retval )
	{
		DEBUG( "Failed to duplicate a string  Bailing out \r\n" );
		exit( 1 );
	}
	return(retval);
}


int safe_vasprintf( char **strp, const char *fmt, va_list ap )
{
	int retval;

	retval = vasprintf( strp, fmt, ap );

	if ( retval == -1 )
	{
		DEBUG( "Failed to vasprintf \r\n" );
		exit( 1 );
	}
	return(retval);
}


int safe_asprintf( char **strp, const char *fmt, ... )
{
	va_list ap;
	int	retval;

	va_start( ap, fmt );
	retval = safe_vasprintf( strp, fmt, ap );                       /* $ID$ gw_interface */
	va_end( ap );

	return(retval);
}

/*
 * 获取double型字符串长度(除去小数部分长度)
 */
int get_count( char *string )
{
	int i = 0;
	if ( !string )
		return(-1);
	do
	{
		;
	}
	while ( string[i++] != '.' && string[i - 1] != '\0' ); /* \0 */

	return(i - 1);
}

/*
 * 截取小数部分长度
 */
void gcvt_char( double change_num, int precision, char *change_array )  /* 小数部分的长度precision */
{
	char		p[50]	= "", p1[50] = "";
	int		k	= 0;
	int		i	= 0, b = 0;
	double		z	= 0.1;
	unsigned long	y	= 0;


	if ( change_num < 0 )
	{
		p[k]		= '-';
		change_num	= 0 - change_num;
		k++;
	}

	y = (unsigned long) change_num;

	z = change_num - y + 0.0001;

	if ( y )
	{
		while ( y )
		{
			p1[b]	= y % 10 + 0x30;
			y	/= 10;
			b++;
		}
	} else
		p1[0] = 0 + 0x30;


	b = strlen( p1 );

	for ( i = 0; i < strlen( p1 ); i++, k++, b-- )
		p[k] = p1[b - 1];

	for ( i = 0; i < 50; i++ )
		p1[i] = 0;

	k	= 0;
	i	= 0;
	p1[k]	= '.';
	k++;

	while ( i <= precision )
	{
		z	= z * 10;
		y	= (int) (z / 1.0);
		z	= z - y;
		p1[k]	= y + 0x30;
		k++;
		i++;
	}

	p1[strlen( p1 ) - 1] = '\0';
	strcat( p, p1 );

	sprintf( change_array, "%s", p );

	return;
}


char *link_strncpy( const char* srcfisrtstr, const char *srcsecstr, int srcfirslength, int srcseclength )
{
	char * desstr;
	if ( !srcfisrtstr || !srcsecstr )
		return(NULL);
	int sumlength = srcfirslength + srcseclength;
	desstr = calloc( sumlength, sizeof(char) );

	if ( !desstr )
		return(NULL);
	int i, n;

	for ( i = 0; i < srcfirslength && srcfisrtstr[i] != '\0'; i++ )
	{
		desstr[i] = srcfisrtstr[i];
	}

	for ( n = 0; n < srcseclength && srcsecstr[n] != '\0'; n++, i++ )
	{
		desstr[i] = srcsecstr[n];
	}

	for (; i < sumlength; i++ )
	{
		desstr[i] = '\0';
	}

	return(desstr);
}



/*
 * 获取文件名中最后的数字字符串
 * dest 目的
 * src  源
 * cpy_size 而要的长度
 *
 * 例如: default-023.ts  取得的是 "023"
 */
char *get_string_last_digit_str( char *dest, const char *src, size_t cpy_size )
{
	size_t	i;
	char	dest_str[10] = "";
	strncpy( dest_str, src + (strlen( src ) - 6), cpy_size );

	for ( i = 0; dest_str[i] != 0; i++ )
	{
		if ( !isdigit( dest_str[i] ) )
			return(NULL);
		else{
			strncpy( dest, dest_str, cpy_size + 1 );
		}
	}
	return(dest);
}


/*
 * 追加字符串
 * dest 目的
 * src  源
 * cpy_size 而要追加的长度
 *
 * 例如: default- 追加后 "default-023.ts"
 */
char *append_string_last_digit_str( char *dest, const char *src,const char *append,size_t cpy_size )
{
	if(!append)
		return NULL;
	
	strncpy( dest, src,(strlen( src ) - 6) );
	strncat(dest,append,cpy_size);
	return(dest);
}


/*
 * 将字符数组中的字符转换成16进制
 * 成功返回0，否则-1
 */
int arr_to_hex( const uint8_t *src, uint8_t *value )
{
	int	i, n;
	uint8_t tmp[8];

	if ( !src )
		return(-1);
	union ch ch_c;

	for ( i = 7; i >= 0; i-- )
	{
		if ( src[i] >= 48 && src[i] <= 57 )
			tmp[i] = (src[i] - 48);
		else if ( src[i] >= 65 || src[i] <= 70 )
			tmp[i] = (src[i] - 55);
	}

	for ( i = 0, n = 0; i < 4; i++, n += 2 )
	{
		ch_c.n.c_1	= tmp[n + 1];
		ch_c.n.c_2	= tmp[n];
		value[i]	= get_hex_val( ch_c ) & 0xFF;
	}

	return(0);
}


/*
 * 将字符串中的16进制数转化为10进制数
 *
 */

int hextodec( int inputhexnum, char*str )
{
	int		value;
	int		radix	= 10;
	char		index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned	unum;                   /*中间变量*/
	int		i = 0, j, k;

/*确定unum的值*/
	if ( radix == 10 && inputhexnum < 0 )   /*十进制负数*/
	{
		unum		= (unsigned) -inputhexnum;
		str[i++]	= '-';
	}else unum = (unsigned) inputhexnum;    /*其他情况*/
/*转换*/
	do
	{
		str[i++]	= index[unum % (unsigned) radix];
		unum		/= radix;
	}
	while ( unum );
	str[i] = '\0';

	if ( str[0] == '-' )
		k = 1; /*十进制负数*/
	else k = 0;
	char temp;
	for ( j = k; j <= (i - 1) / 2; j++ )
	{
		temp			= str[j];
		str[j]			= str[i - 1 + k - j];
		str[i - 1 + k - j]	= temp;
	}

	value = 0;
	while ( 1 )
	{
		if ( (*str >= '0') && (*str <= '9') )
		{
			value = value * 16 + (*str - '0');
		}else if ( (*str >= 'A') && (*str <= 'F') )
		{
			value = value * 16 + (*str - 'A') + 10;
		}else if ( (*str >= 'a') && (*str <= 'f') )
		{
			value = value * 16 + (*str - 'a') + 10;
		}else {
			break;
		}
		str++;
	}
	return(value);
}


/*
 * 字符串中添加字符':'
 *
 *
 */
void str_after_pending_colon( char *src, char *pendingstr )
{
	pendingstr[0]	= src[0];
	pendingstr[1]	= src[1];
	pendingstr[2]	= ':';
	pendingstr[3]	= src[2];
	pendingstr[4]	= src[3];
	pendingstr[5]	= ':';
	pendingstr[6]	= src[4];
	pendingstr[7]	= src[5];
	pendingstr[8]	= '\0';
}


/*
 * 过滤掉字符串中字符':'
 *
 *
 */
void str_prase_pending_colon( char *src, char *dec )
{
	int i = 0, n = 0;
	if ( !src )
		return;

	while ( src[i] != '\0' && n < 6 )
	{
		if ( src[i] != ':' )
		{
			dec[n] = src[i];
			++n;
		}
		++i;
	}

	dec[n] = '\0';
}


/*
 * 过滤浮点型数据是否满足所需长度，不满足时前面n位添0
 *
 * filterdata 过滤后的字符串
 * orgdata  需要过滤的值
 * orgsize  需要过滤的值的长度
 * missvalid 不满足长度时的填充位数
 *
 */
void char_filter_to_flaot( char *filterdata, char *orgdata, int orgsize, int missvalid )
{
	int n = 0, i = 0;

	if ( !filterdata && !orgdata )
		return;
	memset( filterdata, ' ', 16 );

	for (; n < missvalid; n++ )
		filterdata[n] = '0';

	for (; i < orgsize; i++ )
	{
		filterdata[n] = orgdata[i];
		n++;
	}

	filterdata[n]		= orgdata[orgsize]; /* '.' 从"."开始填充 */
	filterdata[n + 1]	= orgdata[orgsize + 1];
	filterdata[n + 2]	= orgdata[orgsize + 2];
	filterdata[n + 3]	= orgdata[orgsize + 3];
	filterdata[n + 4]	= '\0';
}


/*
 * 过滤整型数据是否满足所需长度，不满足时前面n位添0
 *
 * filterdata 过滤后的字符串
 * orgdata  需要过滤的值
 * orgsize  需要过滤的值的长度
 * missvalid 不满足长度时的填充位数
 *
 */
void char_filter_to_digit( char *filterdata, char *orgdata, int orgsize, int missvalid )
{
	int n = 0, i = 0;

	if ( !filterdata && !orgdata )
		return;

	memset( filterdata, ' ', 10 );

	if ( orgdata[0] == '-' )
	{
		filterdata[n]	= orgdata[0];
		missvalid	+= 1;
		orgsize		+= 1;
		++n;
		++i;
	}

	for (; n < missvalid; n++ )
		filterdata[n] = '0';

	for (; i < orgsize; i++ )
	{
		filterdata[n] = orgdata[i];
		n++;
	}

	filterdata[n] = '\0';
}



void get_char_filter_to_digit( int org_length, int in_length, char * filterdata, char * orgdata )
{
	int i = 0;

	if ( !orgdata )
		return;

	if ( orgdata[0] == '-' )
	{
		while ( orgdata[++i] != '.' && orgdata[i - 1] != '\0' )
			;

		org_length = i - 1;
	}

	int misslenght;
	if ( org_length < in_length )
	{
		misslenght = in_length - org_length;
	}else{
		misslenght = 0;
	}

	char_filter_to_digit( filterdata, orgdata, org_length, misslenght );
}



void get_char_filter_to_flaot( int org_length, int in_length, char * filterdata, char * orgdata )
{
	if ( !orgdata )
		return;

	int misslenght;
	if ( org_length < in_length - 4 )
	{
		misslenght = in_length - org_length - 4;
	}else{
		misslenght = 0;
	}

	char_filter_to_flaot( filterdata, orgdata, org_length, misslenght );
}


/*
 * 10 ^n幂
 *
 */
double power( double _number, int _power )
{
	if ( _power < 0 )
		return( (1 / _number) * power( _number, _power + 1 ) );         /* 整形 */
	else if ( _power > 0 )
		return(_number * power( _number, _power - 1 ) );                /*  浮点 */
	else return(1);                                                         /* 返回时需要的是一个整数，所以 1 */
}


/*
 * key加减计算
 * number 输入数值
 * pos  保留
 * flag 加或者减标志 true[+],false[-]
 * _power 幂
 *
 * 返回运算后的值
 */
double add_sub( double * number, int pos, int flag, int _power )
{
	double n = power( 10, _power );
/*	DEBUG("power_n =%f ",n); */
	double revale = *number;


	if ( flag )
	{
		revale += n;
	}else{
		revale -= n;
	}

	return(revale);
}


unsigned int divide( unsigned int a, unsigned int b )
{
	unsigned int c = 1, d = 0, _b = b;

	if ( 0 == b )
		return(0);

	while ( a >= b )
	{
		c	<<= 1;
		b	<<= 1;
	}
	b	>>= 1;
	c	>>= 1;
	while ( b >= _b )
	{
		if ( a >= b )
		{
			a	-= b;
			d	+= c;
		}

		b	>>= 1;
		c	>>= 1;
	}

	return(d);
}


#if 0
/* 瀛楃涓叉煡鎵� */
int strn_casecmp( const char *s1, const char *s2, int size )
{
	int c1, c2, count;
	count = 0;
	do
	{
		c1	= tolower( *s1++ );
		c2	= tolower( *s2++ );
		if ( c1 == c2 )
		{
		} else {
			return(c1 - c2);
			break;
		}
		count++;
	}
	while ( count <= size );
	return(c1 - c2);
}


#endif
