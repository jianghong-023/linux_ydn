
#include <errno.h>
#include <common.h>
#include <i2c-wr.h>

int set_i2c_register(int file,uint8_t addr,uint8_t reg,uint8_t value)
{

	uint8_t outbuf[2];
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[1];

	messages[0].addr  = addr;
	messages[0].flags = 0;
	messages[0].len   = sizeof(outbuf);
	messages[0].buf   = outbuf;

	outbuf[0] = reg;
	outbuf[1] = value;

	packets.msgs  = messages;
	packets.nmsgs = 1;
	if(ioctl(file, I2C_RDWR, &packets) < 0) {
		printf("Unable to send data\n");
		return 1;
	}

	return 0;
}


uint8_t get_i2c_register_value(int file,uint8_t addr,uint8_t reg)
{
	uint8_t inbuf, outbuf;
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg messages[2];

	outbuf = reg;
	messages[0].addr  = addr;
	messages[0].flags = 0;
	messages[0].len   = sizeof(outbuf);
	messages[0].buf   = &outbuf;

	/* The data will get returned in this structure */
	messages[1].addr  = addr;
	messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
	messages[1].len   = sizeof(inbuf);
	messages[1].buf   = &inbuf;

	/* Send the request to the kernel and get the result back */
	packets.msgs      = messages;
	packets.nmsgs     = 2;
	if(ioctl(file, I2C_RDWR, &packets) < 0) {
		printf("Unable to get data\n");
		return 1;
	}

	//printf(" messages[1].buf=%x\n", *messages[1].buf);
	return inbuf;
}
