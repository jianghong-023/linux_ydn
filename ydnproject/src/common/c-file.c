#include <string.h>

#include <c-file.h>
#include <defineident.h>

#define   FILELEN 10

void creat_t_f( int n, char reval[], char usr_d_name[] )
{
	int	year, month, day, hours, min /*,sec*/;
	char	m_name[16];

	/* 获取当前时间的函数 */
	time_t		nowtime;
	struct tm	*timeinfo;

	time( &nowtime );
	timeinfo = localtime( &nowtime );

	year	= timeinfo->tm_year + 1900;
	month	= timeinfo->tm_mon + 1;
	day	= timeinfo->tm_mday;
	hours	= timeinfo->tm_hour;
	min	= timeinfo->tm_min;
	/* sec   = timeinfo->tm_sec; */
	year = year % 100;

	/*
	 * printf("%02d%02d%02d%02d%02d-%c.ts \n" ,year,month,day,hours,min,IDENT_CH+n);
	 * memset(t_buf,0,23);
	 */

	if ( (usr_d_name == NULL) || (0 == strncmp( "default", usr_d_name, strlen( usr_d_name ) ) ) )
		sprintf( reval, "%02d%02d%02d%02d%02d-%c.ts", year, month, day, hours, min, IDENT_CH + n );
	else {
		memset( m_name, 0, sizeof(m_name) );
		if ( FILELEN <= strlen( usr_d_name ) )
		{
			snprintf( m_name, FILELEN, "%s", usr_d_name );
		} else {
			sprintf( m_name, "%s", usr_d_name );
			sprintf( reval, "%s-%c.ts", m_name, IDENT_CH + n );
		}
	}


	/* memcpy(reval,t_buf,15); */
#ifdef __test__creatfile
	char t_buf[23] = { 0 };
	sprintf( t_buf, "%02d%02d%02d%02d%02d-%c.ts", year, month, day, hours, min, IDENT_CH + n );
	if ( -1 == access( t_buf, 0 ) )
	{
		int fd;
		if ( (fd = open( t_buf, O_WRONLY | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR ) ) < 0 )
			printf( "File create faile ...\n" );
		close( fd );
	} else {
		printf( "File already exists \n" );
	}
#endif
}


void is_current_size( int size_count, int flag, int* n )
{
	int tmp;
	tmp = size_count / (MAX_FILE_LIMIT);
	/* 启动时第一次 */
	if ( (size_count >= 1) && (size_count <= MAX_FILE_LIMIT) )
	{
		*n = tmp;
		/* printf("n=%d\r\n",*n); */
	} else if ( size_count >= MAX_FILE_LIMIT )
	{
		/* 增加新的文件 */
		*n = tmp;
		/* printf("n=%d\r\n",*n); */
	}
}


/*
 * 若是指定了文件大小，返回 DIG_SZIE
 * 若是用信号方式，返回 SIGL_C
 */

int is_size_sign( int w_size, char *top_sign, int falg )
{
	int retval;
	/* printf("w_size =%d   top_sign=%s \n",w_size,top_sign); */
	if ( (w_size == 0) && (falg == 0) )
		return(retval = -1);

	if ( (w_size != 0) && (falg != 0) )
	{
		/* do thing ... */
		retval = DIG_SZIE;
	} else if ( (w_size == 0) && (falg != 0) )
	{
		/* do thing ... */
		retval = SIGL_C;
	}

	return(retval);
}