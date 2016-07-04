#include <cy22393-set.h>
#include <i2c-wr.h>
#include <debug.h>


enum cy22393_op {
	refclk_13_5M = 0,
	refclk_27M,
	refclk_27M_1_001M,
	refclk_74_25M,
	refclk_74_25M_1_001M,
	refclk_148_5M
};


// not used
void refclk_13_5MHz(int fd,unsigned int slave_addr)
{
	DEBUG("refclk_13_5MHz");
	set_i2c_register(fd,slave_addr,0x08,0x0e);
	set_i2c_register(fd,slave_addr,0x09,0x0e );
	set_i2c_register(fd,slave_addr,0x0a,0x0e);
	set_i2c_register(fd,slave_addr,0x0b,0x0e);
	set_i2c_register(fd,slave_addr,0x0c,0x00 );
	set_i2c_register(fd,slave_addr,0x0d,0x00);
	set_i2c_register(fd,slave_addr,0x0e,0x05 );
	set_i2c_register(fd,slave_addr,0x0f,0x50 );
	set_i2c_register(fd,slave_addr,0x10,0x55 );
	set_i2c_register(fd,slave_addr,0x11,0x00 );
	set_i2c_register(fd,slave_addr,0x12,0x00 );
	set_i2c_register(fd,slave_addr,0x13,0x00 );
	set_i2c_register(fd,slave_addr,0x14,0x00 );
	set_i2c_register(fd,slave_addr,0x15,0x00 );
	set_i2c_register(fd,slave_addr,0x16,0x00 );
	set_i2c_register(fd,slave_addr,0x17,0x00 );

	set_i2c_register(fd,slave_addr,0x40,0x00 );
	set_i2c_register(fd,slave_addr,0x41,0x19 );
	set_i2c_register(fd,slave_addr,0x42,0x60 );
	set_i2c_register(fd,slave_addr,0x43,0x00);
	set_i2c_register(fd,slave_addr,0x44,0x19 );
	set_i2c_register(fd,slave_addr,0x45,0x60 );
	set_i2c_register(fd,slave_addr,0x46,0x00 );
	set_i2c_register(fd,slave_addr,0x47,0x19 );
	set_i2c_register(fd,slave_addr,0x48,0x60 );
	set_i2c_register(fd,slave_addr,0x49,0x00 );
	set_i2c_register(fd,slave_addr,0x4a,0x19 );
	set_i2c_register(fd,slave_addr,0x4b,0x60 );
	set_i2c_register(fd,slave_addr,0x4c,0x00 );
	set_i2c_register(fd,slave_addr,0x4d,0x19 );
	set_i2c_register(fd,slave_addr,0x4e,0x60 );
	set_i2c_register(fd,slave_addr,0x4f,0x00 );
	set_i2c_register(fd,slave_addr,0x50,0x19 );
	set_i2c_register(fd,slave_addr,0x51,0x60 );
	set_i2c_register(fd,slave_addr,0x52,0x00 );
	set_i2c_register(fd,slave_addr,0x53,0x19);
	set_i2c_register(fd,slave_addr,0x54,0x60 );
	set_i2c_register(fd,slave_addr,0x55,0x00 );
	set_i2c_register(fd,slave_addr,0x56,0x19 );
	set_i2c_register(fd,slave_addr,0x57,0x60 );
}

void refclk_27MHz(int fd,unsigned int slave_addr)
{
	//DEBUG("refclk_27MHz...");
	//refclk=27MHz //
	set_i2c_register(fd,slave_addr,0x08,0x01);
	set_i2c_register(fd,slave_addr,0x09,0x01 );
	set_i2c_register(fd,slave_addr,0x0a,0x01);
	set_i2c_register(fd,slave_addr,0x0b,0x01);
	set_i2c_register(fd,slave_addr,0x0c,0x00 );
	set_i2c_register(fd,slave_addr,0x0d,0x00);
	set_i2c_register(fd,slave_addr,0x0e,0x00 );
	set_i2c_register(fd,slave_addr,0x0f,0x50 );
	set_i2c_register(fd,slave_addr,0x10,0x55 );
	set_i2c_register(fd,slave_addr,0x11,0x00 );
	set_i2c_register(fd,slave_addr,0x12,0x00 );
	set_i2c_register(fd,slave_addr,0x13,0x00 );
	set_i2c_register(fd,slave_addr,0x14,0x00 );
	set_i2c_register(fd,slave_addr,0x15,0x00 );
	set_i2c_register(fd,slave_addr,0x16,0x00 );
	set_i2c_register(fd,slave_addr,0x17,0x01 );

	set_i2c_register(fd,slave_addr,0x40,0x00 );
	set_i2c_register(fd,slave_addr,0x41,0x00 );
	set_i2c_register(fd,slave_addr,0x42,0x00 );
	set_i2c_register(fd,slave_addr,0x43,0x00);
	set_i2c_register(fd,slave_addr,0x44,0x00 );
	set_i2c_register(fd,slave_addr,0x45,0x00 );
	set_i2c_register(fd,slave_addr,0x46,0x00 );
	set_i2c_register(fd,slave_addr,0x47,0x00 );
	set_i2c_register(fd,slave_addr,0x48,0x00 );
	set_i2c_register(fd,slave_addr,0x49,0x00 );
	set_i2c_register(fd,slave_addr,0x4a,0x00 );
	set_i2c_register(fd,slave_addr,0x4b,0x00 );
	set_i2c_register(fd,slave_addr,0x4c,0x00 );
	set_i2c_register(fd,slave_addr,0x4d,0x00 );
	set_i2c_register(fd,slave_addr,0x4e,0x00 );
	set_i2c_register(fd,slave_addr,0x4f,0x00 );
	set_i2c_register(fd,slave_addr,0x50,0x00 );
	set_i2c_register(fd,slave_addr,0x51,0x00 );
	set_i2c_register(fd,slave_addr,0x52,0x00 );
	set_i2c_register(fd,slave_addr,0x53,0x00);
	set_i2c_register(fd,slave_addr,0x54,0x00 );
	set_i2c_register(fd,slave_addr,0x55,0x00 );
	set_i2c_register(fd,slave_addr,0x56,0x00 );
	set_i2c_register(fd,slave_addr,0x57,0x00 );
}

void refclk_27MHz_1_001MHz(int fd,unsigned int slave_addr)
{
	DEBUG("refclk_27MHz_1_001MHz...");
	//refclk=27/1.001MHz
	set_i2c_register(fd,slave_addr,0x08,0x0a);
	set_i2c_register(fd,slave_addr,0x09,0x0a );
	set_i2c_register(fd,slave_addr,0x0a,0x0a);
	set_i2c_register(fd,slave_addr,0x0b,0x0a);
	set_i2c_register(fd,slave_addr,0x0c,0x00 );
	set_i2c_register(fd,slave_addr,0x0d,0x00);
	set_i2c_register(fd,slave_addr,0x0e,0x05 );
	set_i2c_register(fd,slave_addr,0x0f,0x50 );
	set_i2c_register(fd,slave_addr,0x10,0x55 );
	set_i2c_register(fd,slave_addr,0x11,0x00 );
	set_i2c_register(fd,slave_addr,0x12,0x00 );
	set_i2c_register(fd,slave_addr,0x13,0x00 );
	set_i2c_register(fd,slave_addr,0x14,0x00 );
	set_i2c_register(fd,slave_addr,0x15,0x00 );
	set_i2c_register(fd,slave_addr,0x16,0x00 );
	set_i2c_register(fd,slave_addr,0x17,0x01 );

	set_i2c_register(fd,slave_addr,0x40,0x62 );
	set_i2c_register(fd,slave_addr,0x41,0xf1 );
	set_i2c_register(fd,slave_addr,0x42,0x65 );
	set_i2c_register(fd,slave_addr,0x43,0x62);
	set_i2c_register(fd,slave_addr,0x44,0xf1 );
	set_i2c_register(fd,slave_addr,0x45,0x65 );
	set_i2c_register(fd,slave_addr,0x46,0x62 );
	set_i2c_register(fd,slave_addr,0x47,0xf1 );
	set_i2c_register(fd,slave_addr,0x48,0x65 );
	set_i2c_register(fd,slave_addr,0x49,0x62 );
	set_i2c_register(fd,slave_addr,0x4a,0xf1 );
	set_i2c_register(fd,slave_addr,0x4b,0x65 );
	set_i2c_register(fd,slave_addr,0x4c,0x62 );
	set_i2c_register(fd,slave_addr,0x4d,0xf1 );
	set_i2c_register(fd,slave_addr,0x4e,0x65 );
	set_i2c_register(fd,slave_addr,0x4f,0x62 );
	set_i2c_register(fd,slave_addr,0x50,0xf1 );
	set_i2c_register(fd,slave_addr,0x51,0x65 );
	set_i2c_register(fd,slave_addr,0x52,0x62 );
	set_i2c_register(fd,slave_addr,0x53,0xf1);
	set_i2c_register(fd,slave_addr,0x54,0x65 );
	set_i2c_register(fd,slave_addr,0x55,0x62 );
	set_i2c_register(fd,slave_addr,0x56,0xf1 );
	set_i2c_register(fd,slave_addr,0x57,0x65 );
}

void refclk_74_25MHz(int fd,unsigned int slave_addr)
{

	//refclk=74.25MHz
	set_i2c_register(fd,slave_addr,0x08,0x0e);
	set_i2c_register(fd,slave_addr,0x09,0x0e );
	set_i2c_register(fd,slave_addr,0x0a,0x0e);
	set_i2c_register(fd,slave_addr,0x0b,0x0e);
	set_i2c_register(fd,slave_addr,0x0c,0x00 );
	set_i2c_register(fd,slave_addr,0x0d,0x00);
	set_i2c_register(fd,slave_addr,0x0e,0x05 );
	set_i2c_register(fd,slave_addr,0x0f,0x50 );
	set_i2c_register(fd,slave_addr,0x10,0x55 );
	set_i2c_register(fd,slave_addr,0x11,0x00 );
	set_i2c_register(fd,slave_addr,0x12,0x00 );
	set_i2c_register(fd,slave_addr,0x13,0x00 );
	set_i2c_register(fd,slave_addr,0x14,0x00 );
	set_i2c_register(fd,slave_addr,0x15,0x00 );
	set_i2c_register(fd,slave_addr,0x16,0x00 );
	set_i2c_register(fd,slave_addr,0x17,0x02 );

	set_i2c_register(fd,slave_addr,0x40,0x09 );
	set_i2c_register(fd,slave_addr,0x41,0x19 );
	set_i2c_register(fd,slave_addr,0x42,0x60 );
	set_i2c_register(fd,slave_addr,0x43,0x09);
	set_i2c_register(fd,slave_addr,0x44,0x19 );
	set_i2c_register(fd,slave_addr,0x45,0x60 );
	set_i2c_register(fd,slave_addr,0x46,0x09 );
	set_i2c_register(fd,slave_addr,0x47,0x19 );
	set_i2c_register(fd,slave_addr,0x48,0x60 );
	set_i2c_register(fd,slave_addr,0x49,0x09 );
	set_i2c_register(fd,slave_addr,0x4a,0x19 );
	set_i2c_register(fd,slave_addr,0x4b,0x60 );
	set_i2c_register(fd,slave_addr,0x4c,0x09 );
	set_i2c_register(fd,slave_addr,0x4d,0x19 );
	set_i2c_register(fd,slave_addr,0x4e,0x60 );
	set_i2c_register(fd,slave_addr,0x4f,0x09 );
	set_i2c_register(fd,slave_addr,0x50,0x19 );
	set_i2c_register(fd,slave_addr,0x51,0x60 );
	set_i2c_register(fd,slave_addr,0x52,0x09 );
	set_i2c_register(fd,slave_addr,0x53,0x19);
	set_i2c_register(fd,slave_addr,0x54,0x60 );
	set_i2c_register(fd,slave_addr,0x55,0x09 );
	set_i2c_register(fd,slave_addr,0x56,0x19 );
	set_i2c_register(fd,slave_addr,0x57,0x60 );
	DEBUG("refclk_74_25MHz...");
}

void refclk_74_25MHz_1_001MHz(int fd,unsigned int slave_addr)
{
	DEBUG("refclk_74_25MHz_1_001MHz...");
	//refclk=74.25MHz/1.001
	set_i2c_register(fd,slave_addr,0x08,0x0e);
	set_i2c_register(fd,slave_addr,0x09,0x0e );
	set_i2c_register(fd,slave_addr,0x0a,0x0e);
	set_i2c_register(fd,slave_addr,0x0b,0x0e);
	set_i2c_register(fd,slave_addr,0x0c,0x00 );
	set_i2c_register(fd,slave_addr,0x0d,0x00);
	set_i2c_register(fd,slave_addr,0x0e,0x05 );
	set_i2c_register(fd,slave_addr,0x0f,0x50 );
	set_i2c_register(fd,slave_addr,0x10,0x55 );
	set_i2c_register(fd,slave_addr,0x11,0x00 );
	set_i2c_register(fd,slave_addr,0x12,0x00 );
	set_i2c_register(fd,slave_addr,0x13,0x00 );
	set_i2c_register(fd,slave_addr,0x14,0x00 );
	set_i2c_register(fd,slave_addr,0x15,0x00 );
	set_i2c_register(fd,slave_addr,0x16,0x00 );
	set_i2c_register(fd,slave_addr,0x17,0x02 );

	set_i2c_register(fd,slave_addr,0x40,0x7b );
	set_i2c_register(fd,slave_addr,0x41,0x3b );
	set_i2c_register(fd,slave_addr,0x42,0x65 );
	set_i2c_register(fd,slave_addr,0x43,0x7b);
	set_i2c_register(fd,slave_addr,0x44,0x3b );
	set_i2c_register(fd,slave_addr,0x45,0x65 );
	set_i2c_register(fd,slave_addr,0x46,0x7b );
	set_i2c_register(fd,slave_addr,0x47,0x3b );
	set_i2c_register(fd,slave_addr,0x48,0x65 );
	set_i2c_register(fd,slave_addr,0x49,0x7b );
	set_i2c_register(fd,slave_addr,0x4a,0x3b );
	set_i2c_register(fd,slave_addr,0x4b,0x65 );
	set_i2c_register(fd,slave_addr,0x4c,0x7b );
	set_i2c_register(fd,slave_addr,0x4d,0x3b );
	set_i2c_register(fd,slave_addr,0x4e,0x65 );
	set_i2c_register(fd,slave_addr,0x4f,0x7b );
	set_i2c_register(fd,slave_addr,0x50,0x3b );
	set_i2c_register(fd,slave_addr,0x51,0x65 );
	set_i2c_register(fd,slave_addr,0x52,0x7b );
	set_i2c_register(fd,slave_addr,0x53,0x3b);
	set_i2c_register(fd,slave_addr,0x54,0x65 );
	set_i2c_register(fd,slave_addr,0x55,0x7b );
	set_i2c_register(fd,slave_addr,0x56,0x3b );
	set_i2c_register(fd,slave_addr,0x57,0x65 );
}

void refclk_148_5MHz(int fd,unsigned int slave_addr)
{

	//refclk=148.5MHz
	set_i2c_register(fd,slave_addr,0x08,0x0e);
	set_i2c_register(fd,slave_addr,0x09,0x0e );
	set_i2c_register(fd,slave_addr,0x0a,0x0e);
	set_i2c_register(fd,slave_addr,0x0b,0x0e);
	set_i2c_register(fd,slave_addr,0x0c,0x00 );
	set_i2c_register(fd,slave_addr,0x0d,0x00);
	set_i2c_register(fd,slave_addr,0x0e,0x05 );
	set_i2c_register(fd,slave_addr,0x0f,0x50 );
	set_i2c_register(fd,slave_addr,0x10,0x55 );
	set_i2c_register(fd,slave_addr,0x11,0x00 );
	set_i2c_register(fd,slave_addr,0x12,0x00 );
	set_i2c_register(fd,slave_addr,0x13,0x00 );
	set_i2c_register(fd,slave_addr,0x14,0x00 );
	set_i2c_register(fd,slave_addr,0x15,0x00 );
	set_i2c_register(fd,slave_addr,0x16,0x00 );
	set_i2c_register(fd,slave_addr,0x17,0x03 );

	set_i2c_register(fd,slave_addr,0x40,0x09 );
	set_i2c_register(fd,slave_addr,0x41,0x0b );
	set_i2c_register(fd,slave_addr,0x42,0x50 );
	set_i2c_register(fd,slave_addr,0x43,0x09);
	set_i2c_register(fd,slave_addr,0x44,0x0b );
	set_i2c_register(fd,slave_addr,0x45,0x50 );
	set_i2c_register(fd,slave_addr,0x46,0x09 );
	set_i2c_register(fd,slave_addr,0x47,0x0b );
	set_i2c_register(fd,slave_addr,0x48,0x50 );
	set_i2c_register(fd,slave_addr,0x49,0x09 );
	set_i2c_register(fd,slave_addr,0x4a,0x0b );
	set_i2c_register(fd,slave_addr,0x4b,0x50 );
	set_i2c_register(fd,slave_addr,0x4c,0x09 );
	set_i2c_register(fd,slave_addr,0x4d,0x0b );
	set_i2c_register(fd,slave_addr,0x4e,0x50 );
	set_i2c_register(fd,slave_addr,0x4f,0x09 );
	set_i2c_register(fd,slave_addr,0x50,0x0b );
	set_i2c_register(fd,slave_addr,0x51,0x50 );
	set_i2c_register(fd,slave_addr,0x52,0x09 );
	set_i2c_register(fd,slave_addr,0x53,0x0b);
	set_i2c_register(fd,slave_addr,0x54,0x50 );
	set_i2c_register(fd,slave_addr,0x55,0x09 );
	set_i2c_register(fd,slave_addr,0x56,0x0b );
	set_i2c_register(fd,slave_addr,0x57,0x50 );
	DEBUG("refclk_148_5MHz...");
}
