#ifndef MATH_INSTANCE_H

#define MATH_INSTANCE_H

#include <stdio.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>


int safe_vasprintf( char **strp, const char *fmt, va_list ap );


int safe_asprintf( char **strp, const char *fmt, ... );


char * safe_strdup( const char *s );


char * itoa_( int num, char*str, int radix );


void gcvt_char( double change_num, int precision, char *change_array );


int strn_casecmp( const char *s1, const char *s2, int size );


char *link_strncpy( const char* srcfisrtstr, const char *srcsecstr, int srcfirslength, int srcseclength );


int arr_to_hex( const uint8_t *src, uint8_t *value );


int hextodec(int inputhexnum,char*str);


void str_after_pending_colon(char *src,char *pendingstr);

void str_prase_pending_colon(char *src,char *dec);


void char_filter_to_flaot( char *filterdata, char *orgdata, int orgsize, int missvalid );


void char_filter_to_digit( char *filterdata, char *orgdata, int orgsize, int missvalid );

double power(double _number,int _power);

double add_sub(double * number, int pos, int flag,int _power);

unsigned int divide(unsigned int a, unsigned int b);


void get_char_filter_to_digit( int , int , char * , char *  );


void get_char_filter_to_flaot( int , int , char * , char *  );

int get_count( char * );

char *get_string_last_digit_str( char *dest, const char *src, size_t cpy_size );


char *append_string_last_digit_str( char *dest, const char *src,const char *append,size_t cpy_size );


#endif