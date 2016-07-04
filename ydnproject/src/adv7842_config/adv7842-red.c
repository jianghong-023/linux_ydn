#include <i2c-wr.h>
#include <adv7842_config.h>

// struct adv_7842_rg *adv7842_read_rg(int fd) {
	// static struct adv_7842_rg read_reg;
	// u8 addr;


	// addr = 0x7C/2;

	// read_reg.hdmi = get_i2c_register_value(fd,addr,0x04);

	// addr = 0x44/2;
	// read_reg.ch1_bl_b1 = get_i2c_register_value(fd,addr,0xB1);
	// read_reg.ch1_bl_b2 = get_i2c_register_value(fd,addr,0xB2);
	// read_reg.ch1_lcvs_b3 = get_i2c_register_value(fd,addr,0xB3);
	// read_reg.ch1_lcf_b3 = get_i2c_register_value(fd,addr,0xB3);
	// read_reg.ch1_lcf_b4 = get_i2c_register_value(fd,addr,0xB4);
	// read_reg.ch1_fcl_b8 = get_i2c_register_value(fd,addr,0xB8);
	// read_reg.ch1_fcl_b9 = get_i2c_register_value(fd,addr,0xB9);

	// read_reg.rd.hdmi = read_reg.hdmi;
	// read_reg.rd.ch1_bl  = ((0x1F&read_reg.ch1_bl_b1)<<8)|read_reg.ch1_bl_b2;
	// read_reg.rd.ch1_lcvs = (read_reg.ch1_lcvs_b3 & 0xF8) >> 3;
	// read_reg.rd.ch1_lcf  = ((read_reg.ch1_lcf_b3&0x3) << 8) |read_reg.ch1_lcf_b4;
	// read_reg.rd.ch1_fcl  = (( read_reg.ch1_fcl_b8 & 0x1F) << 8) | read_reg.ch1_fcl_b9;
	
	// return &read_reg;
// }
