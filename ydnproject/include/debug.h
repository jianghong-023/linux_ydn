/* 调试打印 */
#ifndef __DEBUG__H
#define __DEBUG__H

#if defined(__STACT_TEXT)
void debug_backtrace( void )
{
#define STACT_SIZE 2000
	void	* arry_size[STACT_SIZE];
	int	size, i;
	char	** strings;
	printf( " ---------^^----------\n" );
	fprintf( stderr, "segmentation fault \n" );

	size = backtrace( arry_size, STACT_SIZE );

	fprintf( stderr, "backtrace (%d deep) : \n", size );

	strings = backtrace_symbols( arry_size, size );

	for ( i = 0; i < size; i++ )
		fprintf( stderr, " %d :  %s \n", i, strings[i] );

	free( strings );

	exit( -1 );
}


#endif


#define DEBUG( args ... ) \
	do { \
		char b__[100]; \
		sprintf( b__, args ); \
		printf( "%s %d %s\n", __FILE__, __LINE__, b__ ); \
	} while ( 0 )

#endif