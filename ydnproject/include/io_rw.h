#ifndef __IO_RW__H
#define __IO_RW__H

#include <stdint.h>


static inline void writeb( uint8_t b, volatile void  *addr )
{
	*(volatile uint8_t *) addr = b;
}


static inline void writew( uint16_t b, volatile void  *addr )
{
	*(volatile uint16_t *) addr = b;
}


static inline void writel( uint32_t b, volatile void  *addr )
{
	*(volatile uint32_t *) addr = b;
}


static inline uint8_t readb( const volatile void  *addr )
{
	return(*(const volatile uint8_t *) addr);
}


static inline uint16_t readw( const volatile void  *addr )
{
	return(*(const volatile uint16_t *) addr);
}


static inline uint32_t readl( const volatile void  *addr )
{
	return(*(const volatile uint32_t *) addr);
}


#endif