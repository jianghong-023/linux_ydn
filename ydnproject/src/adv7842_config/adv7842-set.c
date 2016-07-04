#include <adv7842-set.h>
#include <debug.h>
#include <i2c-wr.h>
#include <init-configuration.h>


/*
 *
 * The CP CSC must also be enabled for the color controls to be effective
 *
 * MAN_CP_CSC_EN, Addr 44 (CP), Address 0x69[4]
 *
 * A control to manually enable the CP CSC. By default the CP CSC will be automatically enabled
 * in the case that either a color-space conversion or video-adjustments (Hue, Saturation, Contrast, Brightness)
 * is determined to be required due to other I2C settings. If MAN_CP_CSC_EN is set to one the CP CSC is forced
 * into the enabled state.
 *
 */
//static void cp_CSC_enable( int fd )
//{
//	set_i2c_register( fd, 0x22, 0x69, 0x01 );
//}


/*
 * sdp亮度设置
 *
 * brightness
 * addr 90(SDP)
 * address 0x14[7:0]
 * address 0x17[3:2]
 * value = 128  => 0x80
 *
 ********/
void sdp_brightness_set( int fd )
{
	s_config	*dconfig = config_get_config(); /*  */
	uint16_t	tmpbright;
	uint16_t	bright;
	tmpbright = dconfig->scfg_Param.encoder_video_brightness;

	if ( (0 <= tmpbright) && (tmpbright <= 128) )
		bright = 511 - 383 - tmpbright;
	else if ( (128 < tmpbright) && (tmpbright <= 255) )
		bright = tmpbright - 512 - 255;

	set_i2c_register( fd, 0x45, 0x14, GET_HEX( bright, 1 ) );
	set_i2c_register( fd, 0x45, 0x17, ( (0xc & GET_HEX( bright, 2 ) ) << 2) );

	DEBUG( "brightness 0x14[7:0] = 0x%02x", GET_HEX( bright, 1 ) );
	DEBUG( "brightness 0x17[3:2] = 0x%02x", ( (GET_HEX( bright, 2 ) ) << 2) );
}


/*
 * sdp色调配置说明
 *
 * SDP_HUE[9:0], Addr 90 (SDP), Address 0x16[7:0]; Address 0x17[7:6].
 * A control to set the Hue (chroma phase rotation). It is a twos compliment
 * number and has a range of 0x200 (-180 degrees) to 0x1FF (+180 degrees).
 *
 * SDP_HUE[9:0]     Description
 * 0x1FF            +180degrees
 * 0x000<<          0
 * 0x200			-180degrees
 *
 */
void sdp_hue_set( int fd )
{
	int16_t		tmphue;
	int16_t		hue;
	s_config	*dconfig = config_get_config(); /*  */
	tmphue = dconfig->scfg_Param.encoder_video_hue;

	if ( (0 <= tmphue) && (tmphue <= 127) )
		hue = 511 - 384 - tmphue;
	else if ( (tmphue < 0) && (tmphue >= -127) )
		hue = tmphue - 384;

	set_i2c_register( fd, 0x45, 0x16, GET_HEX( hue, 1 ) );
	set_i2c_register( fd, 0x45, 0x17, ( (0xc & GET_HEX( hue, 2 ) ) << 6) );

	DEBUG( "n_hue 0x16[7:0] = 0x%02x", GET_HEX( hue, 1 ) );
	DEBUG( "n_hue 0x17[7:6] = 0x%02x", ( (GET_HEX( hue, 2 ) ) << 6) );
}


/*
 * sdp对比度配置说明
 *
 * SDP_SATURATION[9:0], Addr 90 (SDP), Address 0x15[7:0]; Address 0x17[5:4]
 * This is a control to set the saturation level (chroma gain).
 * It has a valid range 0 -> 1.75. It is an unsigned number and has a range of
 * 0x000 (lowest saturation, no color) to 0x3FF (highest saturation).
 *
 * SDP_SATURATION[9:0]  Description
 * 0x000				Lowest saturation (No Color)
 * 0x200<<                              Default value
 * 0x3FF                                Highest saturation
 *
 */
void sdp_saturation_set( int fd )
{
	uint16_t	tmpsaturation;
	uint16_t	saturation;
	s_config	*dconfig = config_get_config(); /*  */
	tmpsaturation = dconfig->scfg_Param.encoder_video_saturation;

	if ( (0 <= tmpsaturation) && (tmpsaturation <= 128) )
		saturation = 512 - 384 - tmpsaturation;
	else if ( (128 < tmpsaturation) && (tmpsaturation <= 255) )
		saturation = tmpsaturation - 1023 - 255;


	set_i2c_register( fd, 0x45, 0x15, GET_HEX( saturation, 1 ) );
	set_i2c_register( fd, 0x45, 0x17, ( (0x3 & GET_HEX( saturation, 2 ) ) << 4) );

	DEBUG( "saturation 0x15[7:0] = 0x%02x", GET_HEX( saturation, 1 ) );
	DEBUG( "saturation 0x17[5:4] = 0x%02x", ( (0x3 & GET_HEX( saturation, 2 ) ) << 4) );
}


/*
 * sdp饱和度配置说明
 *
 * SDP_CONTRAST[9:0], Addr 90 (SDP), Address 0x13[7:0]; Address 0x17[1:0]
 * A control to set the Contrast level (luma gain).
 * This control has a range of gain from 0 to 2. It is an unsigned number and has
 * a range from 0x000 (lowest contrast, all black) to 0x3FF (highest contrast).
 *
 * SDP_CONTRAST[9:0]	Description
 * 0x000				Lowest contrast
 * 0x200<<              Default contrast
 * 0x3FF                Highest contrast
 *
 */
void sdp_contrast_set( int fd )
{
	uint16_t	tmpcontr;
	uint16_t	contr;
	s_config	*dconfig = config_get_config(); /*  */
	tmpcontr = dconfig->scfg_Param.encoder_video_contrast;
	if ( (0 <= tmpcontr) && (tmpcontr <= 128) )
		contr = 512 - 384 - tmpcontr;
	else if ( (128 < tmpcontr) && (tmpcontr <= 255) )
		contr = tmpcontr - 1023 - 255;

	set_i2c_register( fd, 0x45, 0x13, GET_HEX( contr, 1 ) );
	set_i2c_register( fd, 0x45, 0x17, ( (0x3 & GET_HEX( contr, 2 ) ) << 0) );

	DEBUG( "contrast 0x13[7:0] = 0x%02x", GET_HEX( contr, 1 ) );
	DEBUG( "contrast 0x17[1:0] = 0x%02x", ( (GET_HEX( contr, 2 ) ) << 0) );
}


/*
 * 视频调节器使能说明
 *
 * VID_ADJ_EN, Addr 44 (CP), Address 0x3E[7]
 * Video Adjustment Enable. This control selects whether or not the color controls
 * feature is enabled. The color controls feature is configured via the parameters
 * CP_CONTRAST[7:0], CP_SATURATION[7:0], CP_BRIGHTNESS[7:0] and CP_HUE[7:0].
 * The CP CSC must also be enabled for the color controls to be effective.
 *
 * VID_ADJ_EN		Description
 * 0<<				Disable color controls.
 * 1				Enable color controls.
 *
 */
void cp_adjustment_enable( int fd )
{
	set_i2c_register( fd, 0x22, 0x3E, 0x80 );
}


/*
 * CP_BRIGHTNESS[7:0], Addr 44 (CP), Address 0x3C[7:0]
 *
 * A control to set the brightness. This field is a signed value.
 * The effective brightness value applied to the Luma is obtained by
 * multiplying the programmed value CP_BRIGHTNESS with a gain of 4.
 * The brightness applied to the Luma has a range of [-512 to 508].
 * This control is functional if VID_ADJ_EN is set to 1.
 *
 */
void cp_brightness_cfg( int fd )
{
	s_config	*dconfig = config_get_config(); /*  */
	uint8_t		tmpbight, bright = dconfig->scfg_Param.encoder_video_cp_brightness;
	if ( (0 <= bright) && (bright <= 127) )
		tmpbight = 255 - bright;
	else if ( (128 <= bright) && (bright <= 255) )
		tmpbight = bright - 128;
	DEBUG( "bright :%d ", tmpbight );
	set_i2c_register( fd, 0x22, 0x3C, tmpbight );
}


/*
 * CP_CONTRAST[7:0], Addr 44 (CP), Address 0x3A[7:0]
 * A control to set the contrast. This field is an unsigned value represented in a 1.7
 * binary format. The MSB represents the integer part of the contrast value which is either 0 or 1.
 * The seven LSBs represents the fractional part of the contrast value. The fractional part has the
 * range [0 to 0.99]. This control is functional if VID_ADJ_EN is set to 1.
 *
 */

void cp_contast_cfg( int fd )
{
	s_config	*dconfig	= config_get_config(); /*  */
	short		contrast	= dconfig->scfg_Param.encoder_video_cp_contrast;
	DEBUG( "contrast :%d ", contrast );
	set_i2c_register( fd, 0x22, 0x3A, contrast );
}


/*
 * CP_SATURATION[7:0], Addr 44 (CP), Address 0x3B[7:0]
 *
 * A control to set the saturation. This field is an unsigned value represented in a 1.7
 * binary format. The MSB represents the integer part of the contrast value which is either 0 or 1.
 * The seven LSBs represent the fractional part of the saturation value. The fractional part has a
 * [0 to 0.99] range. This control is functional if VID_ADJ_EN is set to 1.
 */

void cp_saturation_cfg( int fd )
{
	s_config	*dconfig	= config_get_config(); /*  */
	short		saturation	= dconfig->scfg_Param.encoder_video_cp_saturation;
	DEBUG( "saturation :%d ", saturation );
	set_i2c_register( fd, 0x22, 0x3B, saturation );
}


/*
 * CP_HUE[7:0], Addr 44 (CP), Address 0x3D[7:0]
 *
 * A control to set the hue. This register a represent an unsigned value which provides hue adjustment.
 * The effective hue applied to the Chroma is [(CP_HUE[7:0] * 180)/256 - 90]. The range of the effective hue
 * applied to the Chroma is [-90° to 90°]. This control is functional if VID_ADJ_EN is set to 1.
 *
 */
void cp_hue_cfg( int fd )
{
	s_config	*dconfig	= config_get_config(); /*  */
	short		hue		= dconfig->scfg_Param.encoder_video_cp_hue + 128;
	DEBUG( "hue :%d ", hue );
	set_i2c_register( fd, 0x22, 0x3D, hue );
}


void common_set( int fd /* ,int ypbpr */ )
{
/*
 * 40 FF 80 ; I2C reset
 * 40 F1 90 ; SDP map
 * 40 F2 94 ; SDPIO map
 * 40 F3 84 ; AVLINK
 * 40 F4 80 ; CEC
 * 40 F5 7C ; INFOFRAME
 * 40 F8 4C ; AFE
 * 40 F9 64 ; KSV
 * 40 FA 6C ; EDID
 * 40 FB 68 ; HDMI
 * 40 FD 44 ; CP
 * 40 FE 48 ; VDP
 * ** slave address=0x40 *********************** //
 * DEBUG("----00--\n");
 */

	set_i2c_register( fd, 0x20, 0xFF, 0x80 );/* 软件复位 */

/* DEBUG("----00--\n"); */

	set_i2c_register( fd, 0x20, 0xF1, 0x90 );
	set_i2c_register( fd, 0x20, 0xF2, 0x94 );
	set_i2c_register( fd, 0x20, 0xF3, 0x84 );
	set_i2c_register( fd, 0x20, 0xF4, 0x80 );
	set_i2c_register( fd, 0x20, 0xF5, 0x7C );
	set_i2c_register( fd, 0x20, 0xF8, 0x4C );
	set_i2c_register( fd, 0x20, 0xF9, 0x64 );
	set_i2c_register( fd, 0x20, 0xFA, 0x6C );
	set_i2c_register( fd, 0x20, 0xFB, 0x68 );
	set_i2c_register( fd, 0x20, 0xFD, 0x44 );
	set_i2c_register( fd, 0x20, 0xFE, 0x48 );


/*
 * 亮度对应的核
 * cp核		【HDMI,YPbPr】
 * sdp		【CVBS】
 *
 */
	s_config *dconfig = config_get_config(); /*  */
	if ( (strncmp( HDMI_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) || \
	     (strncmp( YPbPr_M, dconfig->scfg_Param.encoder_video_interface_name, \
		       strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0) )
	{
		cp_contast_cfg( fd );
		cp_saturation_cfg( fd );
		cp_brightness_cfg( fd );
		cp_hue_cfg( fd );
		cp_adjustment_enable( fd );
		DEBUG( "hdmi/ypbpr" );
	} else if ( strncmp( CVBS_M, dconfig->scfg_Param.encoder_video_interface_name, \
			     strlen( dconfig->scfg_Param.encoder_video_interface_name ) - 1 ) == 0 )
	{
		sdp_contrast_set( fd );
		sdp_brightness_set( fd );
		sdp_saturation_set( fd );
		sdp_hue_set( fd );
		cp_adjustment_enable( fd );
		DEBUG( "cvbs" );
	}


	/* DEBUG("adv7842 init ..."); */
}


void internel_EDID_mod( int fd )
{
	DEBUG( "internel_EDID_mod ..." );
	/* :8-2 EDID Download HDMI : */
	common_set( fd );


/*
 * 64 77 50 ; Enable VGA Segment
 * 64 77 00 ; Disable the Internal EDID for all 4 Ports - Programming HDMI EDID
 * ** slave address=0x64 *********************** //
 */
	set_i2c_register( fd, 0x34, 0x5A, 0x08 );

	set_i2c_register( fd, 0x32, 0x77, 0x50 );
	set_i2c_register( fd, 0x32, 0x77, 0x00 );


/*
 * 6C 00 00 ;
 * 6C 01 FF ;
 * 6C 02 FF ;
 * 6C 03 FF ;
 * 6C 04 FF ;
 * 6C 05 FF ;
 * 6C 06 FF ;
 * 6C 07 00 ;
 * 6C 08 06 ;
 * 6C 09 96 ;
 * 6C 0A 48 ;
 * 6C 0B 44 ;
 * 6C 0C 01 ;
 * 6C 0D 00 ;
 * 6C 0E 00 ;
 * 6C 0F 00 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x00, 0x00 );
	set_i2c_register( fd, 0x36, 0x01, 0xFF );
	set_i2c_register( fd, 0x36, 0x02, 0xFF );
	set_i2c_register( fd, 0x36, 0x03, 0xFF );
	set_i2c_register( fd, 0x36, 0x04, 0xFF );
	set_i2c_register( fd, 0x36, 0x05, 0xFF );
	set_i2c_register( fd, 0x36, 0x06, 0xFF );
	set_i2c_register( fd, 0x36, 0x07, 0x00 );
	set_i2c_register( fd, 0x36, 0x08, 0x06 );
	set_i2c_register( fd, 0x36, 0x09, 0x96 );
	set_i2c_register( fd, 0x36, 0x0A, 0x48 );
	set_i2c_register( fd, 0x36, 0x0B, 0x44 );
	set_i2c_register( fd, 0x36, 0x0C, 0x01 );
	set_i2c_register( fd, 0x36, 0x0D, 0x00 );
	set_i2c_register( fd, 0x36, 0x0E, 0x00 );
	set_i2c_register( fd, 0x36, 0x0F, 0x00 );


/*
 * 6C 10 25 ;
 * 6C 11 10 ;
 * 6C 12 01 ;
 * 6C 13 03 ;
 * 6C 14 80 ;
 * 6C 15 31 ;
 * 6C 16 1C ;
 * 6C 17 A0 ;
 * 6C 18 2A ;
 * 6C 19 AA ;
 * 6C 1A 33 ;
 * 6C 1B A4 ;
 * 6C 1C 55 ;
 * 6C 1D 48 ;
 * 6C 1E 93 ;
 * 6C 1F 25 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x10, 0x25 );
	set_i2c_register( fd, 0x36, 0x11, 0x10 );
	set_i2c_register( fd, 0x36, 0x12, 0x01 );
	set_i2c_register( fd, 0x36, 0x13, 0x03 );
	set_i2c_register( fd, 0x36, 0x14, 0x80 );
	set_i2c_register( fd, 0x36, 0x15, 0x31 );
	set_i2c_register( fd, 0x36, 0x16, 0x1C );
	set_i2c_register( fd, 0x36, 0x17, 0xA0 );
	set_i2c_register( fd, 0x36, 0x18, 0x2A );
	set_i2c_register( fd, 0x36, 0x19, 0xAA );
	set_i2c_register( fd, 0x36, 0x1A, 0x33 );
	set_i2c_register( fd, 0x36, 0x1B, 0xA4 );
	set_i2c_register( fd, 0x36, 0x1C, 0x55 );
	set_i2c_register( fd, 0x36, 0x1D, 0x48 );
	set_i2c_register( fd, 0x36, 0x1E, 0x93 );
	set_i2c_register( fd, 0x36, 0x1F, 0x25 );


/*
 * 6C 20 10 ;
 * 6C 21 45 ;
 * 6C 22 47 ;
 * 6C 23 FF ;
 * 6C 24 FF ;
 * 6C 25 80 ;
 * 6C 26 81 ;
 * 6C 27 8F ;
 * 6C 28 81 ;
 * 6C 29 99 ;
 * 6C 2A A9 ;
 * 6C 2B 40 ;
 * 6C 2C 61 ;
 * 6C 2D 59 ;
 * 6C 2E 45 ;
 * 6C 2F 59 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x20, 0x10 );
	set_i2c_register( fd, 0x36, 0x21, 0x45 );
	set_i2c_register( fd, 0x36, 0x22, 0x47 );
	set_i2c_register( fd, 0x36, 0x23, 0xFF );
	set_i2c_register( fd, 0x36, 0x24, 0xFF );
	set_i2c_register( fd, 0x36, 0x25, 0x80 );
	set_i2c_register( fd, 0x36, 0x26, 0x81 );
	set_i2c_register( fd, 0x36, 0x27, 0x8F );
	set_i2c_register( fd, 0x36, 0x28, 0x81 );
	set_i2c_register( fd, 0x36, 0x29, 0x99 );
	set_i2c_register( fd, 0x36, 0x2A, 0xA9 );
	set_i2c_register( fd, 0x36, 0x2B, 0x40 );
	set_i2c_register( fd, 0x36, 0x2C, 0x61 );
	set_i2c_register( fd, 0x36, 0x2D, 0x59 );
	set_i2c_register( fd, 0x36, 0x2E, 0x45 );
	set_i2c_register( fd, 0x36, 0x2F, 0x59 );


/*
 * 6C 30 31 ;
 * 6C 31 59 ;
 * 6C 32 71 ;
 * 6C 33 4A ;
 * 6C 34 81 ;
 * 6C 35 40 ;
 * 6C 36 02 ;
 * 6C 37 3A ;
 * 6C 38 80 ;
 * 6C 39 18 ;
 * 6C 3A 71 ;
 * 6C 3B 38 ;
 * 6C 3C 2D ;
 * 6C 3D 40 ;
 * 6C 3E 46 ;
 * 6C 3F 28 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x30, 0x31 );
	set_i2c_register( fd, 0x36, 0x31, 0x59 );
	set_i2c_register( fd, 0x36, 0x32, 0x71 );
	set_i2c_register( fd, 0x36, 0x33, 0x4A );
	set_i2c_register( fd, 0x36, 0x34, 0x81 );
	set_i2c_register( fd, 0x36, 0x35, 0x40 );
	set_i2c_register( fd, 0x36, 0x36, 0x02 );
	set_i2c_register( fd, 0x36, 0x37, 0x3A );
	set_i2c_register( fd, 0x36, 0x38, 0x80 );
	set_i2c_register( fd, 0x36, 0x39, 0x18 );
	set_i2c_register( fd, 0x36, 0x3A, 0x71 );
	set_i2c_register( fd, 0x36, 0x3B, 0x38 );
	set_i2c_register( fd, 0x36, 0x3C, 0x2D );
	set_i2c_register( fd, 0x36, 0x3D, 0x40 );
	set_i2c_register( fd, 0x36, 0x3E, 0x46 );
	set_i2c_register( fd, 0x36, 0x3F, 0x28 );


/*
 * 6C 80 02 ;
 * 6C 81 03 ;
 * 6C 82 21 ;
 * 6C 83 71 ;
 * 6C 84 4D ;
 * 6C 85 90 ;
 * 6C 86 04 ;
 * 6C 87 03 ;
 * 6C 88 12 ;
 * 6C 89 01 ;
 * 6C 8A 13 ;
 * 6C 8B 11 ;
 * 6C 8C 07 ;
 * 6C 8D 05 ;
 * 6C 8E 06 ;
 * 6C 8F 22 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x80, 0x02 );
	set_i2c_register( fd, 0x36, 0x81, 0x03 );
	set_i2c_register( fd, 0x36, 0x82, 0x21 );
	set_i2c_register( fd, 0x36, 0x83, 0x71 );
	set_i2c_register( fd, 0x36, 0x84, 0x4D );
	set_i2c_register( fd, 0x36, 0x85, 0x90 );
	set_i2c_register( fd, 0x36, 0x86, 0x04 );
	set_i2c_register( fd, 0x36, 0x87, 0x03 );
	set_i2c_register( fd, 0x36, 0x88, 0x12 );
	set_i2c_register( fd, 0x36, 0x89, 0x01 );
	set_i2c_register( fd, 0x36, 0x8A, 0x13 );
	set_i2c_register( fd, 0x36, 0x8B, 0x11 );
	set_i2c_register( fd, 0x36, 0x8C, 0x07 );
	set_i2c_register( fd, 0x36, 0x8D, 0x05 );
	set_i2c_register( fd, 0x36, 0x8E, 0x06 );
	set_i2c_register( fd, 0x36, 0x8F, 0x22 );


/*
 * 6C 90 14 ;
 * 6C 91 15 ;
 * 6C 92 23 ;
 * 6C 93 09 ;
 * 6C 94 07 ;
 * 6C 95 07 ;
 * 6C 96 83 ;
 * 6C 97 4F ;
 * 6C 98 00 ;
 * 6C 99 00 ;
 * 6C 9A 66 ;
 * 6C 9B 03 ;
 * 6C 9C 0C ;
 * 6C 9D 00 ;
 * 6C 9E 10 ;
 * 6C 9F 00 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x90, 0x14 );
	set_i2c_register( fd, 0x36, 0x91, 0x15 );
	set_i2c_register( fd, 0x36, 0x92, 0x23 );
	set_i2c_register( fd, 0x36, 0x93, 0x09 );
	set_i2c_register( fd, 0x36, 0x94, 0x07 );
	set_i2c_register( fd, 0x36, 0x95, 0x07 );
	set_i2c_register( fd, 0x36, 0x96, 0x83 );
	set_i2c_register( fd, 0x36, 0x97, 0x4F );
	set_i2c_register( fd, 0x36, 0x98, 0x00 );
	set_i2c_register( fd, 0x36, 0x99, 0x00 );
	set_i2c_register( fd, 0x36, 0x9A, 0x66 );
	set_i2c_register( fd, 0x36, 0x9B, 0x03 );
	set_i2c_register( fd, 0x36, 0x9C, 0x0C );
	set_i2c_register( fd, 0x36, 0x9D, 0x00 );
	set_i2c_register( fd, 0x36, 0x9E, 0x10 );
	set_i2c_register( fd, 0x36, 0x9F, 0x00 );


/*
 * 6C A0 80 ;
 * 6C A1 01 ;
 * 6C A2 1D ;
 * 6C A3 00 ;
 * 6C A4 72 ;
 * 6C A5 51 ;
 * 6C A6 D0 ;
 * 6C A7 1E ;
 * 6C A8 20 ;
 * 6C A9 6E ;
 * 6C AA 28 ;
 * 6C AB 55 ;
 * 6C AC 00 ;
 * 6C AD E8 ;
 * 6C AE 12 ;
 * 6C AF 11 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0xA0, 0x80 );
	set_i2c_register( fd, 0x36, 0xA1, 0x01 );
	set_i2c_register( fd, 0x36, 0xA2, 0x1D );
	set_i2c_register( fd, 0x36, 0xA3, 0x00 );
	set_i2c_register( fd, 0x36, 0xA4, 0x72 );
	set_i2c_register( fd, 0x36, 0xA5, 0x51 );
	set_i2c_register( fd, 0x36, 0xA6, 0xD0 );
	set_i2c_register( fd, 0x36, 0xA7, 0x1E );
	set_i2c_register( fd, 0x36, 0xA8, 0x20 );
	set_i2c_register( fd, 0x36, 0xA9, 0x6E );
	set_i2c_register( fd, 0x36, 0xAA, 0x28 );
	set_i2c_register( fd, 0x36, 0xAB, 0x55 );
	set_i2c_register( fd, 0x36, 0xAC, 0x00 );
	set_i2c_register( fd, 0x36, 0xAD, 0xE8 );
	set_i2c_register( fd, 0x36, 0xAE, 0x12 );
	set_i2c_register( fd, 0x36, 0xAF, 0x11 );


/*
 * 6C B0 00 ;
 * 6C B1 00 ;
 * 6C B2 1E ;
 * 6C B3 8C ;
 * 6C B4 0A ;
 * 6C B5 D0 ;
 * 6C B6 90 ;
 * 6C B7 20 ;
 * 6C B8 40 ;
 * 6C B9 31 ;
 * 6C BA 20 ;
 * 6C BB 0C ;
 * 6C BC 40 ;
 * 6C BD 55 ;
 * 6C BE 00 ;
 * 6C BF E8 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0xB0, 0x00 );
	set_i2c_register( fd, 0x36, 0xB1, 0x00 );
	set_i2c_register( fd, 0x36, 0xB2, 0x1E );
	set_i2c_register( fd, 0x36, 0xB3, 0x8C );
	set_i2c_register( fd, 0x36, 0xB4, 0x0A );
	set_i2c_register( fd, 0x36, 0xB5, 0xD0 );
	set_i2c_register( fd, 0x36, 0xB6, 0x90 );
	set_i2c_register( fd, 0x36, 0xB7, 0x20 );
	set_i2c_register( fd, 0x36, 0xB8, 0x40 );
	set_i2c_register( fd, 0x36, 0xB9, 0x31 );
	set_i2c_register( fd, 0x36, 0xBA, 0x20 );
	set_i2c_register( fd, 0x36, 0xBB, 0x0C );
	set_i2c_register( fd, 0x36, 0xBC, 0x40 );
	set_i2c_register( fd, 0x36, 0xBD, 0x55 );
	set_i2c_register( fd, 0x36, 0xBE, 0x00 );
	set_i2c_register( fd, 0x36, 0xBF, 0xE8 );


/*
 * 6C 40 55 ;
 * 6C 41 00 ;
 * 6C 42 E8 ;
 * 6C 43 12 ;
 * 6C 44 11 ;
 * 6C 45 00 ;
 * 6C 46 00 ;
 * 6C 47 18 ;
 * 6C 48 01 ;
 * 6C 49 1D ;
 * 6C 4A 00 ;
 * 6C 4B 72 ;
 * 6C 4C 51 ;
 * 6C 4D D0 ;
 * 6C 4E 1E ;
 * 6C 4F 20 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x40, 0x55 );
	set_i2c_register( fd, 0x36, 0x41, 0x00 );
	set_i2c_register( fd, 0x36, 0x42, 0xE8 );
	set_i2c_register( fd, 0x36, 0x43, 0x12 );
	set_i2c_register( fd, 0x36, 0x44, 0x11 );
	set_i2c_register( fd, 0x36, 0x45, 0x00 );
	set_i2c_register( fd, 0x36, 0x46, 0x00 );
	set_i2c_register( fd, 0x36, 0x47, 0x18 );
	set_i2c_register( fd, 0x36, 0x48, 0x01 );
	set_i2c_register( fd, 0x36, 0x49, 0x1D );
	set_i2c_register( fd, 0x36, 0x4A, 0x00 );
	set_i2c_register( fd, 0x36, 0x4B, 0x72 );
	set_i2c_register( fd, 0x36, 0x4C, 0x51 );
	set_i2c_register( fd, 0x36, 0x4D, 0xD0 );
	set_i2c_register( fd, 0x36, 0x4E, 0x1E );
	set_i2c_register( fd, 0x36, 0x4F, 0x20 );


/*
 * 6C 50 46 ;
 * 6C 51 28 ;
 * 6C 52 55 ;
 * 6C 53 00 ;
 * 6C 54 E8 ;
 * 6C 55 12 ;
 * 6C 56 11 ;
 * 6C 57 00 ;
 * 6C 58 00 ;
 * 6C 59 18 ;
 * 6C 5A 00 ;
 * 6C 5B 00 ;
 * 6C 5C 00 ;
 * 6C 5D FC ;
 * 6C 5E 00 ;
 * 6C 5F 48 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x50, 0x46 );
	set_i2c_register( fd, 0x36, 0x51, 0x28 );
	set_i2c_register( fd, 0x36, 0x52, 0x55 );
	set_i2c_register( fd, 0x36, 0x53, 0x00 );
	set_i2c_register( fd, 0x36, 0x54, 0xE8 );
	set_i2c_register( fd, 0x36, 0x55, 0x12 );
	set_i2c_register( fd, 0x36, 0x56, 0x11 );
	set_i2c_register( fd, 0x36, 0x57, 0x00 );
	set_i2c_register( fd, 0x36, 0x58, 0x00 );
	set_i2c_register( fd, 0x36, 0x59, 0x18 );
	set_i2c_register( fd, 0x36, 0x5A, 0x00 );
	set_i2c_register( fd, 0x36, 0x5B, 0x00 );
	set_i2c_register( fd, 0x36, 0x5C, 0x00 );
	set_i2c_register( fd, 0x36, 0x5D, 0xFC );
	set_i2c_register( fd, 0x36, 0x5E, 0x00 );
	set_i2c_register( fd, 0x36, 0x5F, 0x48 );


/*
 * 6C 60 44 ;
 * 6C 61 4D ;
 * 6C 62 49 ;
 * 6C 63 20 ;
 * 6C 64 54 ;
 * 6C 65 56 ;
 * 6C 66 0A ;
 * 6C 67 20 ;
 * 6C 68 20 ;
 * 6C 69 20 ;
 * 6C 6A 20 ;
 * 6C 6B 20 ;
 * 6C 6C 00 ;
 * 6C 6D 00 ;
 * 6C 6E 00 ;
 * 6C 6F FD ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x60, 0x44 );
	set_i2c_register( fd, 0x36, 0x61, 0x4D );
	set_i2c_register( fd, 0x36, 0x62, 0x49 );
	set_i2c_register( fd, 0x36, 0x63, 0x20 );
	set_i2c_register( fd, 0x36, 0x64, 0x54 );
	set_i2c_register( fd, 0x36, 0x65, 0x56 );
	set_i2c_register( fd, 0x36, 0x66, 0x0A );
	set_i2c_register( fd, 0x36, 0x67, 0x20 );
	set_i2c_register( fd, 0x36, 0x68, 0x20 );
	set_i2c_register( fd, 0x36, 0x69, 0x20 );
	set_i2c_register( fd, 0x36, 0x6A, 0x20 );
	set_i2c_register( fd, 0x36, 0x6B, 0x20 );
	set_i2c_register( fd, 0x36, 0x6C, 0x00 );
	set_i2c_register( fd, 0x36, 0x6D, 0x00 );
	set_i2c_register( fd, 0x36, 0x6E, 0x00 );
	set_i2c_register( fd, 0x36, 0x6F, 0xFD );


/*
 * 6C 70 00 ;
 * 6C 71 1D ;
 * 6C 72 56 ;
 * 6C 73 0F ;
 * 6C 74 6F ;
 * 6C 75 11 ;
 * 6C 76 00 ;
 * 6C 77 0A ;
 * 6C 78 20 ;
 * 6C 79 20 ;
 * 6C 7A 20 ;
 * 6C 7B 20 ;
 * 6C 7C 20 ;
 * 6C 7D 20 ;
 * 6C 7E 01 ;
 * 6C 7F AB ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0x70, 0x00 );
	set_i2c_register( fd, 0x36, 0x71, 0x1D );
	set_i2c_register( fd, 0x36, 0x72, 0x56 );
	set_i2c_register( fd, 0x36, 0x73, 0x0F );
	set_i2c_register( fd, 0x36, 0x74, 0x6F );
	set_i2c_register( fd, 0x36, 0x75, 0x11 );
	set_i2c_register( fd, 0x36, 0x76, 0x00 );
	set_i2c_register( fd, 0x36, 0x77, 0x0A );
	set_i2c_register( fd, 0x36, 0x78, 0x20 );
	set_i2c_register( fd, 0x36, 0x79, 0x20 );
	set_i2c_register( fd, 0x36, 0x7A, 0x20 );
	set_i2c_register( fd, 0x36, 0x7B, 0x20 );
	set_i2c_register( fd, 0x36, 0x7C, 0x20 );
	set_i2c_register( fd, 0x36, 0x7D, 0x20 );
	set_i2c_register( fd, 0x36, 0x7E, 0x01 );
	set_i2c_register( fd, 0x36, 0x7F, 0xAB );


/*
 * 6C C0 12 ;
 * 6C C1 11 ;
 * 6C C2 00 ;
 * 6C C3 00 ;
 * 6C C4 18 ;
 * 6C C5 8C ;
 * 6C C6 0A ;
 * 6C C7 D0 ;
 * 6C C8 8A ;
 * 6C C9 20 ;
 * 6C CA E0 ;
 * 6C CB 2D ;
 * 6C CC 10 ;
 * 6C CD 10 ;
 * 6C CE 3E ;
 * 6C CF 96 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0xC0, 0x12 );
	set_i2c_register( fd, 0x36, 0xC1, 0x11 );
	set_i2c_register( fd, 0x36, 0xC2, 0x00 );
	set_i2c_register( fd, 0x36, 0xC3, 0x00 );
	set_i2c_register( fd, 0x36, 0xC4, 0x18 );
	set_i2c_register( fd, 0x36, 0xC5, 0x8C );
	set_i2c_register( fd, 0x36, 0xC6, 0x0A );
	set_i2c_register( fd, 0x36, 0xC7, 0xD0 );
	set_i2c_register( fd, 0x36, 0xC8, 0x8A );
	set_i2c_register( fd, 0x36, 0xC9, 0x20 );
	set_i2c_register( fd, 0x36, 0xCA, 0xE0 );
	set_i2c_register( fd, 0x36, 0xCB, 0x2D );
	set_i2c_register( fd, 0x36, 0xCC, 0x10 );
	set_i2c_register( fd, 0x36, 0xCD, 0x10 );
	set_i2c_register( fd, 0x36, 0xCE, 0x3E );
	set_i2c_register( fd, 0x36, 0xCF, 0x96 );


/*
 * 6C D0 00 ;
 * 6C D1 E8 ;
 * 6C D2 12 ;
 * 6C D3 11 ;
 * 6C D4 00 ;
 * 6C D5 00 ;
 * 6C D6 18 ;
 * 6C D7 01 ;
 * 6C D8 1D ;
 * 6C D9 80 ;
 * 6C DA 18 ;
 * 6C DB 71 ;
 * 6C DC 1C ;
 * 6C DD 16 ;
 * 6C DE 20 ;
 * 6C DF 58 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0xD0, 0x00 );
	set_i2c_register( fd, 0x36, 0xD1, 0xE8 );
	set_i2c_register( fd, 0x36, 0xD2, 0x12 );
	set_i2c_register( fd, 0x36, 0xD3, 0x11 );
	set_i2c_register( fd, 0x36, 0xD4, 0x00 );
	set_i2c_register( fd, 0x36, 0xD5, 0x00 );
	set_i2c_register( fd, 0x36, 0xD6, 0x18 );
	set_i2c_register( fd, 0x36, 0xD7, 0x01 );
	set_i2c_register( fd, 0x36, 0xD8, 0x1D );
	set_i2c_register( fd, 0x36, 0xD9, 0x80 );
	set_i2c_register( fd, 0x36, 0xDA, 0x18 );
	set_i2c_register( fd, 0x36, 0xDB, 0x71 );
	set_i2c_register( fd, 0x36, 0xDC, 0x1C );
	set_i2c_register( fd, 0x36, 0xDD, 0x16 );
	set_i2c_register( fd, 0x36, 0xDE, 0x20 );
	set_i2c_register( fd, 0x36, 0xDF, 0x58 );


/*
 * 6C E0 28 ;
 * 6C E1 25 ;
 * 6C E2 00 ;
 * 6C E3 E8 ;
 * 6C E4 12 ;
 * 6C E5 11 ;
 * 6C E6 00 ;
 * 6C E7 00 ;
 * 6C E8 98 ;
 * 6C E9 01 ;
 * 6C EA 1D ;
 * 6C EB 00 ;
 * 6C EC BC ;
 * 6C ED 52 ;
 * 6C EE D0 ;
 * 6C EF 1E ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0xE0, 0x28 );
	set_i2c_register( fd, 0x36, 0xE1, 0x25 );
	set_i2c_register( fd, 0x36, 0xE2, 0x00 );
	set_i2c_register( fd, 0x36, 0xE3, 0xE8 );
	set_i2c_register( fd, 0x36, 0xE4, 0x12 );
	set_i2c_register( fd, 0x36, 0xE5, 0x11 );
	set_i2c_register( fd, 0x36, 0xE6, 0x00 );
	set_i2c_register( fd, 0x36, 0xE7, 0x00 );
	set_i2c_register( fd, 0x36, 0xE8, 0x98 );
	set_i2c_register( fd, 0x36, 0xE9, 0x01 );
	set_i2c_register( fd, 0x36, 0xEA, 0x1D );
	set_i2c_register( fd, 0x36, 0xEB, 0x00 );
	set_i2c_register( fd, 0x36, 0xEC, 0xBC );
	set_i2c_register( fd, 0x36, 0xED, 0x52 );
	set_i2c_register( fd, 0x36, 0xEE, 0xD0 );
	set_i2c_register( fd, 0x36, 0xEF, 0x1E );


/*
 * 6C F0 20 ;
 * 6C F1 B8 ;
 * 6C F2 28 ;
 * 6C F3 55 ;
 * 6C F4 40 ;
 * 6C F5 E8 ;
 * 6C F6 12 ;
 * 6C F7 11 ;
 * 6C F8 00 ;
 * 6C F9 00 ;
 * 6C FA 1E ;
 * 6C FB 00 ;
 * 6C FC 00 ;
 * 6C FD 00 ;
 * 6C FE 00 ;
 * 6C FF A5 ;
 * ** slave address=0x6C *********************** //
 */
	set_i2c_register( fd, 0x36, 0xF0, 0x20 );
	set_i2c_register( fd, 0x36, 0xF1, 0xB8 );
	set_i2c_register( fd, 0x36, 0xF2, 0x28 );
	set_i2c_register( fd, 0x36, 0xF3, 0x55 );
	set_i2c_register( fd, 0x36, 0xF4, 0x40 );
	set_i2c_register( fd, 0x36, 0xF5, 0xE8 );
	set_i2c_register( fd, 0x36, 0xF6, 0x12 );
	set_i2c_register( fd, 0x36, 0xF7, 0x11 );
	set_i2c_register( fd, 0x36, 0xF8, 0x00 );
	set_i2c_register( fd, 0x36, 0xF9, 0x00 );
	set_i2c_register( fd, 0x36, 0xFA, 0x1E );
	set_i2c_register( fd, 0x36, 0xFB, 0x00 );
	set_i2c_register( fd, 0x36, 0xFC, 0x00 );
	set_i2c_register( fd, 0x36, 0xFD, 0x00 );
	set_i2c_register( fd, 0x36, 0xFE, 0x00 );
	set_i2c_register( fd, 0x36, 0xFF, 0xA5 );


/*
 * 64 77 00 ; Set the Most Significant Bit of the SPA location to 0
 * 64 70 20 ; Set the SPA for port B
 * 64 71 00 ; Set the SPA for port B. The checksum should be set to 0x95
 * 64 72 30 ; Set the SPA for port C
 * 64 73 00 ; Set the SPA for port C. The checksum should be set to 0x85
 * 64 74 40 ; Set the SPA for port D
 * 64 75 00 ; Set the SPA for port D. The checksum should be set to 0x75
 * 64 76 9E ; Set the Least Significant Byte of the SPA location
 * 64 77 0F ; Enable the Internal EDID for all 4 Ports
 * ** slave address=0x64 *********************** //
 */
	set_i2c_register( fd, 0x32, 0x77, 0x00 );
	set_i2c_register( fd, 0x32, 0x70, 0x20 );
	set_i2c_register( fd, 0x32, 0x71, 0x00 );
	set_i2c_register( fd, 0x32, 0x72, 0x30 );
	set_i2c_register( fd, 0x32, 0x73, 0x00 );
	set_i2c_register( fd, 0x32, 0x74, 0x40 );
	set_i2c_register( fd, 0x32, 0x75, 0x00 );
	set_i2c_register( fd, 0x32, 0x76, 0x9E );
	set_i2c_register( fd, 0x32, 0x77, 0x0F );

	/* sdp_brightness_set( fd ); */
}


void CVBS_576ix50_mod( int fd )
{
	common_set( fd );


/*
 * :1-1b CVBS NTSC_PAL-M 480i 10bit H_V Encoder:
 * 40 00 01 ; CVBS 4x1 mode
 * 40 01 00 ; SD core
 * 40 03 01 ; 10 bit Mode
 * 40 04 22 ; Output bus rotation
 * 40 0C 40 ; Power up Core
 * 40 15 80 ; Power up pads
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x00 );       /* Set VID_STD(IO Map 0x00[5:0])：	   011110b=0x1E -> HD 1x1 1125p 1920 x 1080p */
	set_i2c_register( fd, 0x20, 0x01, 0x10 );       /* Set VFREQ(IO Map 0x01[6:4])：		   000b -> 60 Hz */


	/*
	 *                                                                                                                              001b -> 50 Hz
	 *                                                                                                                              010b -> 30 Hz
	 *                                                                                                                              011b -> 25 Hz
	 *                                                                                                                              100b -> 24 Hz
	 * Set PRIM_MODE(IO Map 0x01[3:0]):  0000b -> SDP mode
	 *                                                                                                                              0001b -> Component Mode
	 *                                                                                                                              0010b -> Graphics mode
	 *                                                                                                                              0100b -> CVBS & HDMI AUDIO Mode
	 *                                                                                                                              0101b -> HDMI-Comp
	 *                                                                                                                              0110b -> HDMI-GR
	 */
	set_i2c_register( fd, 0x20, 0x03, 0x00 );       /* 8-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );       /* AV Codes On */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );       /*  */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 0E ; ADC0 power Up
 * 4C 02 80 ; Manual Mux
 * 4C 03 B0 ; Ain11
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x0E );
	set_i2c_register( fd, 0x26, 0x02, 0x80 );
	set_i2c_register( fd, 0x26, 0x03, 0xB0 );


/*
 * 94 7A A5 ; Timing Adjustment
 * 94 7B 8F ; Timing Adjustment
 * 94 60 01 ; SDRAM reset
 * 94 97 00 ; Hsync width Adjustment
 * 94 B2 60 ; Disable AV codes
 */
	set_i2c_register( fd, 0x4A, 0x7A, 0xA5 );
	set_i2c_register( fd, 0x4A, 0x7B, 0x8F );
	set_i2c_register( fd, 0x4A, 0x60, 0x01 );
	set_i2c_register( fd, 0x4A, 0x97, 0x00 );
	set_i2c_register( fd, 0x4A, 0xB2, 0x6c );


/*
 * 90 00 7F ; Autodetect PAL NTSC SECAM
 * 90 01 00 ; Pedestal Off
 * 90 03 E4 ; Manual VCR Gain Luma 0x40B
 * 90 04 0B ; Manual Luma setting
 * 90 05 C3 ; Manual Chroma setting 0x3FE
 * 90 06 FE ; Manual Chroma setting
 * 90 12 05 ; Frame TBC,3D comb enabled
 * 90 A7 00 ; ADI Recommended Write
 */
	set_i2c_register( fd, 0x48, 0x00, 0x7F );
	set_i2c_register( fd, 0x48, 0x01, 0x00 );
	set_i2c_register( fd, 0x48, 0x03, 0xE4 );
	set_i2c_register( fd, 0x48, 0x04, 0x0B );
	set_i2c_register( fd, 0x48, 0x05, 0xC3 );
	set_i2c_register( fd, 0x48, 0x06, 0xFE );
	set_i2c_register( fd, 0x48, 0x12, 0x00 );
	set_i2c_register( fd, 0x48, 0xA7, 0x00 );
/*	set_i2c_register(fd, 0x48, 0xDD, 0x0F); */
	sdp_brightness_set( fd );
	DEBUG( "CVBS_576ix50_mod ..." );
}


/* 14402 480 [20160128 jh] */
void CVBS_480ix60_mod( int fd )
{
	common_set( fd );


/*
 * :1-1b CVBS NTSC_PAL-M 480i 10bit H_V Encoder:
 * 40 00 01 ; CVBS 4x1 mode
 * 40 01 00 ; SD core
 * 40 03 01 ; 10 bit Mode
 * 40 04 22 ; Output bus rotation
 * 40 0C 40 ; Power up Core
 * 40 15 80 ; Power up pads
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x00 );       /* Set VID_STD(IO Map 0x00[5:0])：	   011110b=0x1E -> HD 1x1 1125p 1920 x 1080p */
	set_i2c_register( fd, 0x20, 0x01, 0x00 );       /* Set VFREQ(IO Map 0x01[6:4])：		   000b -> 60 Hz */


	/*
	 *                                                                                                                              001b -> 50 Hz
	 *                                                                                                                              010b -> 30 Hz
	 *                                                                                                                              011b -> 25 Hz
	 *                                                                                                                              100b -> 24 Hz
	 * Set PRIM_MODE(IO Map 0x01[3:0]):  0000b -> SDP mode
	 *                                                                                                                              0001b -> Component Mode
	 *                                                                                                                              0010b -> Graphics mode
	 *                                                                                                                              0100b -> CVBS & HDMI AUDIO Mode
	 *                                                                                                                              0101b -> HDMI-Comp
	 *                                                                                                                              0110b -> HDMI-GR
	 */
	set_i2c_register( fd, 0x20, 0x03, 0x00 );       /* 8-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );       /* AV Codes On */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );       /*  */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 0E ; ADC0 power Up
 * 4C 02 80 ; Manual Mux
 * 4C 03 B0 ; Ain11
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x0E );
	set_i2c_register( fd, 0x26, 0x02, 0x80 );
	set_i2c_register( fd, 0x26, 0x03, 0xB0 );


/*
 * 94 7A A5 ; Timing Adjustment
 * 94 7B 8F ; Timing Adjustment
 * 94 60 01 ; SDRAM reset
 * 94 97 00 ; Hsync width Adjustment
 * 94 B2 60 ; Disable AV codes
 */
	set_i2c_register( fd, 0x4A, 0x7A, 0xA5 );
	set_i2c_register( fd, 0x4A, 0x7B, 0x8F );
	set_i2c_register( fd, 0x4A, 0x60, 0x01 );
	set_i2c_register( fd, 0x4A, 0x97, 0x00 );
	set_i2c_register( fd, 0x4A, 0xB2, 0x6c );


/*
 * 90 00 7F ; Autodetect PAL NTSC SECAM
 * 90 01 00 ; Pedestal Off
 * 90 03 E4 ; Manual VCR Gain Luma 0x40B
 * 90 04 0B ; Manual Luma setting
 * 90 05 C3 ; Manual Chroma setting 0x3FE
 * 90 06 FE ; Manual Chroma setting
 * 90 12 05 ; Frame TBC,3D comb enabled
 * 90 A7 00 ; ADI Recommended Write
 */
	set_i2c_register( fd, 0x48, 0x00, 0x7F );
	set_i2c_register( fd, 0x48, 0x01, 0x00 );
	set_i2c_register( fd, 0x48, 0x03, 0xE4 );
	set_i2c_register( fd, 0x48, 0x04, 0x0B );
	set_i2c_register( fd, 0x48, 0x05, 0xC3 );
	set_i2c_register( fd, 0x48, 0x06, 0xFE );
	set_i2c_register( fd, 0x48, 0x12, 0x00 );
	set_i2c_register( fd, 0x48, 0xA7, 0x00 );
/*	set_i2c_register(fd, 0x48, 0xDD, 0x0F); */
	sdp_brightness_set( fd );
	DEBUG( "CVBS_480ix60_mod..." );
}


void HDMI_GR_480px60_mod( int fd )
{
	common_set( fd );
/* :6-1a Port A, 480p 576p YPrPb 444 in, RGB 444 out, Through DAC (Fast Switching)VIC[1-3,14,17,18,29,30]: */


/*
 * 40 01 06 ; Prim_Mode =110b HDMI-GR
 * 40 02 F6 ; Auto input color space, Limited Range RGB Output
 * 40 03 42 ; 36 bit SDR 444 Mode 0
 * 40 05 28 ; AV Codes Off
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x01, 0x06 );
	set_i2c_register( fd, 0x20, 0x02, 0xF6 );
	set_i2c_register( fd, 0x20, 0x03, 0x42 );
	set_i2c_register( fd, 0x20, 0x05, 0x28 );


/*
 * 68 C1 FF ; HDMI power control (power saving)
 * 68 C2 FF ; HDMI power control (power saving)
 * 68 C3 FF ; HDMI power control (power saving)
 * 68 C4 FF ; HDMI power control (power saving)
 * 68 C5 00 ; HDMI power control (power saving)
 * 68 C6 00 ; HDMI power control (power saving)
 * 68 C0 FF ; HDMI power control (power saving)
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0xC1, 0xFF );
	set_i2c_register( fd, 0x34, 0xC2, 0xFF );
	set_i2c_register( fd, 0x34, 0xC3, 0xFF );
	set_i2c_register( fd, 0x34, 0xC4, 0xFF );
	set_i2c_register( fd, 0x34, 0xC5, 0x00 );
	set_i2c_register( fd, 0x34, 0xC6, 0x00 );
	set_i2c_register( fd, 0x34, 0xC0, 0xFF );


/*
 * 40 0C 40 ; Power up part and power down VDP
 * 40 15 80 ; Disable Tristate of Pins
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 44 BA 01 ; Set HDMI FreeRun
 * 44 3E 00 ; Disable CP Pregain Block
 * 44 6C 00 ; Use fixed clamp values
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0xBA, 0x01 );
/*	set_i2c_register(fd, 0x22, 0x3E, 0x80); */
	set_i2c_register( fd, 0x22, 0x6C, 0x00 );


/*
 * 4C 00 FF ; Power Down ADC's and there associated clocks
 * 4C 01 FE ; Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
 * 4C B5 01 ; Setting MCLK to 256Fs
 */
	set_i2c_register( fd, 0x26, 0x00, 0xFF );
	set_i2c_register( fd, 0x26, 0x01, 0xFE );
	set_i2c_register( fd, 0x26, 0xB5, 0x01 );


/*
 * 68 00 32 ; Set HDMI Input Port A (Enable BG monitoring)
 * 68 01 18 ; Enable clock terminators
 * 68 0D 34 ; ADI recommended write
 * 68 1A 8A ; Unmute audio
 * 68 3D 10 ; HDMI ADI recommended write
 * 68 44 85 ; TMDS PLL Optimization
 * 68 46 1F ; ADI Recommended Write ES3/Final silicon
 * 68 60 88 ; TMDS PLL Optimization
 * 68 61 88 ; TMDS PLL Optimization
 * 68 6C 18 ; Disable ISRC clearing bit, Improve robustness
 * 68 57 B6 ; TMDS PLL Optimization
 * 68 58 03 ; TMDS PLL Setting
 * 68 75 10 ; DDC drive strength
 * 68 85 1F ; ADI Equaliser Setting
 * 68 87 70 ; HDMI Recommended write
 * 68 89 04 ; ADI Equaliser Setting
 * 68 8A 1E ; ADI Equaliser Setting
 * 68 93 04 ; ADI Equaliser Setting
 * 68 94 1E ; ADI Equaliser Setting
 * 68 9D 02 ; ADI Equaliser Setting
 * 68 99 A1 ; HDMI ADI recommended write
 * 68 9B 09 ; HDMI ADI recommended write
 * 68 C9 01 ; HDMI free Run based on PRIM_MODE, VID _STD
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0x00, 0x32 );
	set_i2c_register( fd, 0x34, 0x01, 0x18 );
	set_i2c_register( fd, 0x34, 0x0D, 0x34 );
	set_i2c_register( fd, 0x34, 0x1A, 0x8A );
	set_i2c_register( fd, 0x34, 0x3D, 0x10 );
	set_i2c_register( fd, 0x34, 0x44, 0x85 );
	set_i2c_register( fd, 0x34, 0x46, 0x1F );
	set_i2c_register( fd, 0x34, 0x60, 0x88 );
	set_i2c_register( fd, 0x34, 0x61, 0x88 );
	set_i2c_register( fd, 0x34, 0x6C, 0x18 );
	set_i2c_register( fd, 0x34, 0x57, 0xB6 );
	set_i2c_register( fd, 0x34, 0x58, 0x03 );
	set_i2c_register( fd, 0x34, 0x67, 0x20 );
	set_i2c_register( fd, 0x34, 0x75, 0x10 );
	set_i2c_register( fd, 0x34, 0x85, 0x1F );
	set_i2c_register( fd, 0x34, 0x87, 0x70 );
	set_i2c_register( fd, 0x34, 0x89, 0x04 );
	set_i2c_register( fd, 0x34, 0x8A, 0x1E );
	set_i2c_register( fd, 0x34, 0x93, 0x04 );
	set_i2c_register( fd, 0x34, 0x94, 0x1E );
	set_i2c_register( fd, 0x34, 0x9D, 0x02 );
	set_i2c_register( fd, 0x34, 0x99, 0xA1 );
	set_i2c_register( fd, 0x34, 0x9B, 0x09 );
	set_i2c_register( fd, 0x34, 0xC9, 0x01 );

	sdp_brightness_set( fd );
	DEBUG( "HDMI_GR_480px60_mod ..." );
}


void YPbPr_720px60_mod( int fd )
{
	common_set( fd );


/*
 * 40 00 1E ; VID_STD= 11110b for 1080p
 * 40 01 01 ; Prim Mode to Component
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 82 ; 24 bit SDR 422
 * 40 05 28 ; Disable AV Codes
 * 40 06 A6 ; Invert HS, VS for EIA-861 Compliance
 * 40 0C 40 ; Power up Part
 * 40 14 3F ; Max Drive Strength
 * 40 15 B0 ; Disable Tristate of Pins except for Audio pins
 * 40 19 80 ; LLC DLL setting
 * 40 33 43 ; LLC Phase Shift
 */
	set_i2c_register( fd, 0x20, 0x00, 0x13 );       /*  */
	set_i2c_register( fd, 0x20, 0x01, 0x11 );       /*  */

	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 8-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
	set_i2c_register( fd, 0x20, 0x06, 0xA6 );
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0xB0 );
	set_i2c_register( fd, 0x20, 0x19, 0x80 );
	set_i2c_register( fd, 0x20, 0x33, 0x43 );


/*
 * 44 40 5C ; LCVS threshold adjustment
 * 44 73 EA ; Set manual gain of 0x2A8
 * 44 74 8A ; Set manual gain of 0x2A8
 * 44 75 A2 ; Set manual gain of 0x2A8
 * 44 76 A8 ; Set manual gain of 0x2A8
 * 44 85 1B ; Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use Embedded Sync.
 * 44 C3 33 ; ADI recommended write
 * ** slave address=0x44(CP) *********************** //
 */
	set_i2c_register( fd, 0x22, 0x40, 0x5C );
	set_i2c_register( fd, 0x22, 0x73, 0xEA );
	set_i2c_register( fd, 0x22, 0x74, 0x8A );
	set_i2c_register( fd, 0x22, 0x75, 0xA2 );
	set_i2c_register( fd, 0x22, 0x76, 0xA8 );
	set_i2c_register( fd, 0x22, 0x85, 0x1B );
	set_i2c_register( fd, 0x22, 0xC3, 0x33 );

	set_i2c_register( fd, 0x22, 0xBF, 0x10 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 80 ; ADC power Up
 * 4C 02 01 ; Ain_Sel to 001. (Ain 4,5,6) sync3 to SOG1
 * 4C 05 07 ; Enable AA filters
 * 4C 06 20 ; set AA filter bandwidth to 75MHz
 * 4C 07 20 ; set AA filter bandwidth to 75MHz
 * ** slave address=0x4C(AFE) *********************** //
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x80 );
	set_i2c_register( fd, 0x26, 0x02, 0x01 );
	set_i2c_register( fd, 0x26, 0x05, 0x07 );
	set_i2c_register( fd, 0x26, 0x06, 0x20 );
	set_i2c_register( fd, 0x26, 0x07, 0x80 );

	sdp_brightness_set( fd );
	DEBUG( "YPbPr_720px60_mod ..." );
}


void YPbPr_720px50_mod( int fd )
{
	common_set( fd );
	/* 40 00 1E ; VID_STD= 11110b for 1080p */


/*
 * 40 01 01 ; Prim Mode to Component
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 82 ; 24 bit SDR 422
 * 40 05 28 ; Disable AV Codes
 * 40 06 A6 ; Invert HS, VS for EIA-861 Compliance
 * 40 0C 40 ; Power up Part
 * 40 14 3F ; Max Drive Strength
 * 40 15 B0 ; Disable Tristate of Pins except for Audio pins
 * 40 19 80 ; LLC DLL setting
 * 40 33 43 ; LLC Phase Shift
 * ** slave address=0x40 *********************** //
 *  set_i2c_register(fd, 0x20, 0x00, 0x07);//Set VID_STD(IO Map 0x00[5:0])：	   000111b=0x07 -> auto graphics
 * set_i2c_register(fd, 0x20, 0x01, 0x12);//																		50Hz,GR mode
 * ** slave address=0x40(IO) *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x13 );       /*  */
	set_i2c_register( fd, 0x20, 0x01, 0x11 );       /*  */

	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 8-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
	set_i2c_register( fd, 0x20, 0x06, 0xA6 );
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0xB0 );
	set_i2c_register( fd, 0x20, 0x19, 0x80 );
	set_i2c_register( fd, 0x20, 0x33, 0x43 );


/*
 * 44 40 5C ; LCVS threshold adjustment
 * 44 73 EA ; Set manual gain of 0x2A8
 * 44 74 8A ; Set manual gain of 0x2A8
 * 44 75 A2 ; Set manual gain of 0x2A8
 * 44 76 A8 ; Set manual gain of 0x2A8
 * 44 85 1B ; Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use Embedded Sync.
 * 44 C3 33 ; ADI recommended write
 * ** slave address=0x44(CP) *********************** //
 */
	set_i2c_register( fd, 0x22, 0x40, 0x5C );
	set_i2c_register( fd, 0x22, 0x73, 0xEA );
	set_i2c_register( fd, 0x22, 0x74, 0x8A );
	set_i2c_register( fd, 0x22, 0x75, 0xA2 );
	set_i2c_register( fd, 0x22, 0x76, 0xA8 );
	set_i2c_register( fd, 0x22, 0x85, 0x1B );
	set_i2c_register( fd, 0x22, 0xC3, 0x33 );

	set_i2c_register( fd, 0x22, 0xBF, 0x10 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 80 ; ADC power Up
 * 4C 02 01 ; Ain_Sel to 001. (Ain 4,5,6) sync3 to SOG1
 * 4C 05 07 ; Enable AA filters
 * 4C 06 20 ; set AA filter bandwidth to 75MHz
 * 4C 07 20 ; set AA filter bandwidth to 75MHz
 * ** slave address=0x4C(AFE) *********************** //
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x80 );
	set_i2c_register( fd, 0x26, 0x02, 0x01 );
	set_i2c_register( fd, 0x26, 0x05, 0x07 );
	set_i2c_register( fd, 0x26, 0x06, 0x20 );
	set_i2c_register( fd, 0x26, 0x07, 0x80 );


/*
 * set_i2c_register(fd, 0x26, 0x15, 0x0a);
 * set_i2c_register(fd, 0x26, 0x16, 0x1f);
 */

	sdp_brightness_set( fd );
	DEBUG( "YPbPr_720px50_mod ..." );
}


void YPbPr_1080px50_mod( int fd )
{
	DEBUG( "YPbPr_1080px50_mod ..." );
	common_set( fd );


/*
 * :4-3g 1080p YPbPr in_ 422 24bit H_V_DE HDMI:
 * 40 00 1E ; VID_STD= 11110b for 1080p
 * 40 01 01 ; Prim Mode to Component
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 82 ; 24 bit SDR 422
 * 40 05 28 ; Disable AV Codes
 * 40 06 A6 ; Invert HS, VS for EIA-861 Compliance
 * 40 0C 40 ; Power up Part
 * 40 14 3F ; Max Drive Strength
 * 40 15 B0 ; Disable Tristate of Pins except for Audio pins
 * 40 19 80 ; LLC DLL setting
 * 40 33 43 ; LLC Phase Shift
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x1E );       /* Set VID_STD(IO Map 0x00[5:0])：	   011110b=0x1E -> HD 1x1 1125p 1920 x 1080p */
	set_i2c_register( fd, 0x20, 0x01, 0x11 );       /* Set VFREQ(IO Map 0x01[6:4])：		   000b -> 60 Hz */


	/*
	 *                                                                                                                              001b -> 50 Hz
	 *                                                                                                                              010b -> 30 Hz
	 *                                                                                                                              011b -> 25 Hz
	 *                                                                                                                              100b -> 24 Hz
	 * Set PRIM_MODE(IO Map 0x01[3:0]):  0000b -> SDP mode
	 *                                                                                                                              0001b -> Component Mode
	 *                                                                                                                              0010b -> Graphics mode
	 *                                                                                                                              0100b -> CVBS & HDMI AUDIO Mode
	 *                                                                                                                              0101b -> HDMI-Comp
	 *                                                                                                                              0110b -> HDMI-GR
	 */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
	set_i2c_register( fd, 0x20, 0x06, 0xA6 );
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0xB0 );
	set_i2c_register( fd, 0x20, 0x19, 0x80 );
	set_i2c_register( fd, 0x20, 0x33, 0x43 );


/*
 * 44 40 5C ; LCVS threshold adjustment
 * 44 73 EA ; Set manual gain of 0x2A8
 * 44 74 8A ; Set manual gain of 0x2A8
 * 44 75 A2 ; Set manual gain of 0x2A8
 * 44 76 A8 ; Set manual gain of 0x2A8
 * 44 85 1B ; Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use Embedded Sync.
 * 44 C3 33 ; ADI recommended write
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0x40, 0x5C );
	set_i2c_register( fd, 0x22, 0x73, 0xEA );
	set_i2c_register( fd, 0x22, 0x74, 0x8A );
	set_i2c_register( fd, 0x22, 0x75, 0xA2 );
	set_i2c_register( fd, 0x22, 0x76, 0xA8 );
	set_i2c_register( fd, 0x22, 0x85, 0x1B );
	set_i2c_register( fd, 0x22, 0xC3, 0x33 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 80 ; ADC power Up
 * 4C 02 01 ; Ain_Sel to 001. (Ain 4,5,6) sync3 to SOG1
 * 4C 05 07 ; Enable AA filters
 * 4C 06 20 ; set AA filter bandwidth to 75MHz
 * 4C 07 20 ; set AA filter bandwidth to 75MHz
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x80 );
	set_i2c_register( fd, 0x26, 0x02, 0x01 );
	set_i2c_register( fd, 0x26, 0x05, 0x07 );
	set_i2c_register( fd, 0x26, 0x06, 0x20 );
	set_i2c_register( fd, 0x26, 0x07, 0x20 );

	sdp_brightness_set( fd );
}


void YPbPr_1080ix50_mod( int fd )
{
	DEBUG( "YPbPr_1080ix50_mod ..." );
	common_set( fd );


/*
 * 40 00 1E ; VID_STD= 11110b for 1080p
 * 40 01 01 ; Prim Mode to Component
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 82 ; 24 bit SDR 422
 * 40 05 28 ; Disable AV Codes
 * 40 06 A6 ; Invert HS, VS for EIA-861 Compliance
 * 40 0C 40 ; Power up Part
 * 40 14 3F ; Max Drive Strength
 * 40 15 B0 ; Disable Tristate of Pins except for Audio pins
 * 40 19 80 ; LLC DLL setting
 * 40 33 43 ; LLC Phase Shift
 */

	set_i2c_register( fd, 0x20, 0x00, 0x14 );       /*  */
	set_i2c_register( fd, 0x20, 0x01, 0x11 );       /*  */

	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
	set_i2c_register( fd, 0x20, 0x06, 0xA6 );
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0xB0 );
	set_i2c_register( fd, 0x20, 0x19, 0x80 );
	set_i2c_register( fd, 0x20, 0x33, 0x43 );


/*
 * 44 40 5C ; LCVS threshold adjustment
 * 44 73 EA ; Set manual gain of 0x2A8
 * 44 74 8A ; Set manual gain of 0x2A8
 * 44 75 A2 ; Set manual gain of 0x2A8
 * 44 76 A8 ; Set manual gain of 0x2A8
 * 44 85 1B ; Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use Embedded Sync.
 * 44 C3 33 ; ADI recommended write
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0x40, 0x5C );
	set_i2c_register( fd, 0x22, 0x73, 0xEA );
	set_i2c_register( fd, 0x22, 0x74, 0x8A );
	set_i2c_register( fd, 0x22, 0x75, 0xA2 );
	set_i2c_register( fd, 0x22, 0x76, 0xA8 );
	set_i2c_register( fd, 0x22, 0x85, 0x1B );
	set_i2c_register( fd, 0x22, 0xC3, 0x33 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 80 ; ADC power Up
 * 4C 02 01 ; Ain_Sel to 001. (Ain 4,5,6) sync3 to SOG1
 * 4C 05 07 ; Enable AA filters
 * 4C 06 20 ; set AA filter bandwidth to 75MHz
 * 4C 07 20 ; set AA filter bandwidth to 75MHz
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x80 );
	set_i2c_register( fd, 0x26, 0x02, 0x01 );
	set_i2c_register( fd, 0x26, 0x05, 0x07 );
	set_i2c_register( fd, 0x26, 0x06, 0x20 );
	set_i2c_register( fd, 0x26, 0x07, 0x1f );

	sdp_brightness_set( fd );
}


void YPbPr_1080ix60_mod( int fd )
{
	common_set( fd );
	/* 40 00 1E ; VID_STD= 11110b for 1080p */


/*
 * 40 01 01 ; Prim Mode to Component
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 82 ; 24 bit SDR 422
 * 40 05 28 ; Disable AV Codes
 * 40 06 A6 ; Invert HS, VS for EIA-861 Compliance
 * 40 0C 40 ; Power up Part
 * 40 14 3F ; Max Drive Strength
 * 40 15 B0 ; Disable Tristate of Pins except for Audio pins
 * 40 19 80 ; LLC DLL setting
 * 40 33 43 ; LLC Phase Shift
 * ** slave address=0x40 *********************** //
 *  set_i2c_register(fd, 0x20, 0x00, 0x07);//Set VID_STD(IO Map 0x00[5:0])：	   000111b=0x07 -> auto graphics
 * set_i2c_register(fd, 0x20, 0x01, 0x12);//																		50Hz,GR mode
 */

	set_i2c_register( fd, 0x20, 0x00, 0x14 );       /*  */
	set_i2c_register( fd, 0x20, 0x01, 0x01 );       /*  */


/*
 * set_i2c_register(fd, 0x20, 0x16, 0xCA);//
 * set_i2c_register(fd, 0x20, 0x17, 0x50);//
 */

	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
	set_i2c_register( fd, 0x20, 0x06, 0xA6 );
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0xB0 );
	set_i2c_register( fd, 0x20, 0x19, 0x80 );
	set_i2c_register( fd, 0x20, 0x33, 0x43 );


/*
 * 44 40 5C ; LCVS threshold adjustment
 * 44 73 EA ; Set manual gain of 0x2A8
 * 44 74 8A ; Set manual gain of 0x2A8
 * 44 75 A2 ; Set manual gain of 0x2A8
 * 44 76 A8 ; Set manual gain of 0x2A8
 * 44 85 1B ; Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use Embedded Sync.
 * 44 C3 33 ; ADI recommended write
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0x40, 0x5C );
	set_i2c_register( fd, 0x22, 0x73, 0xEA );
	set_i2c_register( fd, 0x22, 0x74, 0x8A );
	set_i2c_register( fd, 0x22, 0x75, 0xA2 );
	set_i2c_register( fd, 0x22, 0x76, 0xA8 );
	set_i2c_register( fd, 0x22, 0x85, 0x1B );
	set_i2c_register( fd, 0x22, 0xC3, 0x33 );


/*
 * set_i2c_register(fd, 0x22, 0x8F, 0x43);
 * set_i2c_register(fd, 0x22, 0x90, 0xFA);
 * set_i2c_register(fd, 0x22, 0xAB, 0x46);
 * set_i2c_register(fd, 0x22, 0xAC, 0x50);
 * set_i2c_register(fd, 0x22, 0x91, 0x40);
 * set_i2c_register(fd, 0x22, 0xC8, 0x03);
 * set_i2c_register(fd, 0x22, 0xC9, 0x20);
 * set_i2c_register(fd, 0x22, 0x26, 0x00);
 * set_i2c_register(fd, 0x22, 0x27, 0xBC);
 * set_i2c_register(fd, 0x22, 0x28, 0x08);
 * set_i2c_register(fd, 0x22, 0x29, 0x40);
 * set_i2c_register(fd, 0x22, 0xA5, 0x46);
 * set_i2c_register(fd, 0x22, 0xA6, 0x40);
 * set_i2c_register(fd, 0x22, 0xA7, 0x16);
 * set_i2c_register(fd, 0x22, 0xA8, 0x23);
 * set_i2c_register(fd, 0x22, 0xA9, 0x12);
 * set_i2c_register(fd, 0x22, 0xAA, 0x48);
 */


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 80 ; ADC power Up
 * 4C 02 01 ; Ain_Sel to 001. (Ain 4,5,6) sync3 to SOG1
 * 4C 05 07 ; Enable AA filters
 * 4C 06 20 ; set AA filter bandwidth to 75MHz
 * 4C 07 20 ; set AA filter bandwidth to 75MHz
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x80 );
	set_i2c_register( fd, 0x26, 0x02, 0x01 );
	set_i2c_register( fd, 0x26, 0x05, 0x07 );
	set_i2c_register( fd, 0x26, 0x06, 0x20 );
	set_i2c_register( fd, 0x26, 0x07, 0x1f );

	sdp_brightness_set( fd );
}


void SD_YPbPr_576i_480ix50_mod( int fd )
{
	DEBUG( "SD_YPbPr_576i_480ix50_mod ..." );
	common_set( fd );


/*
 * 40 00 1E ; VID_STD= 11110b for 1080p
 * 40 01 01 ; Prim Mode to Component
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 82 ; 24 bit SDR 422
 * 40 05 28 ; Disable AV Codes
 * 40 06 A6 ; Invert HS, VS for EIA-861 Compliance
 * 40 0C 40 ; Power up Part
 * 40 14 3F ; Max Drive Strength
 * 40 15 B0 ; Disable Tristate of Pins except for Audio pins
 * 40 19 80 ; LLC DLL setting
 * 40 33 43 ; LLC Phase Shift
 */
	set_i2c_register( fd, 0x20, 0x00, 0x12 );       /*  */
	set_i2c_register( fd, 0x20, 0x01, 0x00 );       /*  */

	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x00 );       /* 8-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
	set_i2c_register( fd, 0x20, 0x06, 0xA6 );
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0xB0 );
	set_i2c_register( fd, 0x20, 0x19, 0x80 );
	set_i2c_register( fd, 0x20, 0x33, 0x43 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 80 ; ADC power Up
 * 4C 02 01 ; Ain_Sel to 001. (Ain 4,5,6) sync3 to SOG1
 * 4C 05 07 ; Enable AA filters
 * 4C 06 20 ; set AA filter bandwidth to 75MHz
 * 4C 07 20 ; set AA filter bandwidth to 75MHz
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x80 );
	set_i2c_register( fd, 0x26, 0x02, 0x01 );
	set_i2c_register( fd, 0x26, 0x05, 0x07 );
	set_i2c_register( fd, 0x26, 0x06, 0x20 );
	set_i2c_register( fd, 0x26, 0x07, 0x20 );


/*
 * 94 7A A5 ; Timing Adjustment
 * 94 7B 8F ; Timing Adjustment
 * 94 60 01 ; SDRAM reset
 * 94 97 00 ; Hsync width Adjustment
 * 94 B2 60 ; Disable AV codes
 */
	set_i2c_register( fd, 0x4A, 0x7A, 0xA5 );
	set_i2c_register( fd, 0x4A, 0x7B, 0x8F );
	set_i2c_register( fd, 0x4A, 0x60, 0x01 );
	set_i2c_register( fd, 0x4A, 0x97, 0x00 );
	set_i2c_register( fd, 0x4A, 0xB2, 0x6c );


/*
 * 90 00 7F ; Autodetect PAL NTSC SECAM
 * 90 01 00 ; Pedestal Off
 * 90 03 E4 ; Manual VCR Gain Luma 0x40B
 * 90 04 0B ; Manual Luma setting
 * 90 05 C3 ; Manual Chroma setting 0x3FE
 * 90 06 FE ; Manual Chroma setting
 * 90 12 05 ; Frame TBC,3D comb enabled
 * 90 A7 00 ; ADI Recommended Write
 */
	set_i2c_register( fd, 0x48, 0x00, 0x7F );
	set_i2c_register( fd, 0x48, 0x01, 0x00 );
	set_i2c_register( fd, 0x48, 0x03, 0xE4 );
	set_i2c_register( fd, 0x48, 0x04, 0x0B );
	set_i2c_register( fd, 0x48, 0x05, 0xC3 );
	set_i2c_register( fd, 0x48, 0x06, 0xFE );
	set_i2c_register( fd, 0x48, 0x12, 0x00 );
	set_i2c_register( fd, 0x48, 0xA7, 0x00 );

	sdp_brightness_set( fd );
}


void CP_YPbPr_576ix50_mod( int fd )
{
	DEBUG( "CP_YPbPr_576ix50_mod ..." );
	common_set( fd );


/*
 * 40 00 1E ; VID_STD= 11110b for 1080p
 * 40 01 01 ; Prim Mode to Component
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 82 ; 24 bit SDR 422
 * 40 05 28 ; Disable AV Codes
 * 40 06 A6 ; Invert HS, VS for EIA-861 Compliance
 * 40 0C 40 ; Power up Part
 * 40 14 3F ; Max Drive Strength
 * 40 15 B0 ; Disable Tristate of Pins except for Audio pins
 * 40 19 80 ; LLC DLL setting
 * 40 33 43 ; LLC Phase Shift
 */
	set_i2c_register( fd, 0x20, 0x00, 0x03 );       /*  */
	set_i2c_register( fd, 0x20, 0x01, 0x11 );       /*  */

	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x00 );       /* 8-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
	set_i2c_register( fd, 0x20, 0x06, 0xA6 );
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0xB0 );
	set_i2c_register( fd, 0x20, 0x19, 0xC0 );
	set_i2c_register( fd, 0x20, 0x33, 0x43 );


/*
 * 44 40 5C ; LCVS threshold adjustment
 * 44 73 EA ; Set manual gain of 0x2A8
 * 44 74 8A ; Set manual gain of 0x2A8
 * 44 75 A2 ; Set manual gain of 0x2A8
 * 44 76 A8 ; Set manual gain of 0x2A8
 * 44 85 1B ; Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use Embedded Sync.
 * 44 C3 33 ; ADI recommended write
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0x40, 0x5C );
	set_i2c_register( fd, 0x22, 0x73, 0xEA );
	set_i2c_register( fd, 0x22, 0x74, 0x8A );
	set_i2c_register( fd, 0x22, 0x75, 0xA2 );
	set_i2c_register( fd, 0x22, 0x76, 0xA8 );
	set_i2c_register( fd, 0x22, 0x85, 0x1B );
	set_i2c_register( fd, 0x22, 0xC3, 0x33 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 80 ; ADC power Up
 * 4C 02 01 ; Ain_Sel to 001. (Ain 4,5,6) sync3 to SOG1
 * 4C 05 07 ; Enable AA filters
 * 4C 06 20 ; set AA filter bandwidth to 75MHz
 * 4C 07 20 ; set AA filter bandwidth to 75MHz
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x80 );
	set_i2c_register( fd, 0x26, 0x02, 0x01 );
	set_i2c_register( fd, 0x26, 0x05, 0x07 );
	set_i2c_register( fd, 0x26, 0x06, 0x20 );
	set_i2c_register( fd, 0x26, 0x07, 0x20 );

	sdp_brightness_set( fd );
}


void CP_YPbPr_576px50_mod( int fd )
{
	DEBUG( "CP_YPbPr_576px50_mod ..." );
	common_set( fd );


/*
 * 40 00 1E ; VID_STD= 11110b for 1080p
 * 40 01 01 ; Prim Mode to Component
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 82 ; 24 bit SDR 422
 * 40 05 28 ; Disable AV Codes
 * 40 06 A6 ; Invert HS, VS for EIA-861 Compliance
 * 40 0C 40 ; Power up Part
 * 40 14 3F ; Max Drive Strength
 * 40 15 B0 ; Disable Tristate of Pins except for Audio pins
 * 40 19 80 ; LLC DLL setting
 * 40 33 43 ; LLC Phase Shift
 */
	set_i2c_register( fd, 0x20, 0x00, 0x0D );       /*  */
	set_i2c_register( fd, 0x20, 0x01, 0x11 );       /*  */

	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x00 );       /* 8-bit ITU-656 SDR mode */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
	set_i2c_register( fd, 0x20, 0x06, 0xA6 );
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0xB0 );
	set_i2c_register( fd, 0x20, 0x19, 0x80 );
	set_i2c_register( fd, 0x20, 0x33, 0x43 );


/*
 * 44 40 5C ; LCVS threshold adjustment
 * 44 73 EA ; Set manual gain of 0x2A8
 * 44 74 8A ; Set manual gain of 0x2A8
 * 44 75 A2 ; Set manual gain of 0x2A8
 * 44 76 A8 ; Set manual gain of 0x2A8
 * 44 85 1B ; Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use Embedded Sync.
 * 44 C3 33 ; ADI recommended write
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0x40, 0x5C );
	set_i2c_register( fd, 0x22, 0x73, 0xEA );
	set_i2c_register( fd, 0x22, 0x74, 0x8A );
	set_i2c_register( fd, 0x22, 0x75, 0xA2 );
	set_i2c_register( fd, 0x22, 0x76, 0xA8 );
	set_i2c_register( fd, 0x22, 0x85, 0x1B );
	set_i2c_register( fd, 0x22, 0xC3, 0x33 );


/*
 * 4C 0C 1F ; ADI recommended write
 * 4C 12 63 ; ADI recommended write
 * 4C 00 80 ; ADC power Up
 * 4C 02 01 ; Ain_Sel to 001. (Ain 4,5,6) sync3 to SOG1
 * 4C 05 07 ; Enable AA filters
 * 4C 06 20 ; set AA filter bandwidth to 75MHz
 * 4C 07 20 ; set AA filter bandwidth to 75MHz
 */
	set_i2c_register( fd, 0x26, 0x0C, 0x1F );
	set_i2c_register( fd, 0x26, 0x12, 0x63 );
	set_i2c_register( fd, 0x26, 0x00, 0x80 );
	set_i2c_register( fd, 0x26, 0x02, 0x01 );
	set_i2c_register( fd, 0x26, 0x05, 0x07 );
	set_i2c_register( fd, 0x26, 0x06, 0x20 );
	set_i2c_register( fd, 0x26, 0x07, 0x20 );

	sdp_brightness_set( fd );
}


void HDMI_COMP_1080ix50_mod( int fd )
{
	common_set( fd );


/*
 * 40 01 06 ; Prim_Mode =110b HDMI-GR
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 42 ; 36 bit SDR 444 Mode 0
 * 40 05 28 ; AV Codes Off
 * 40 06 A6 ; Invert HS,VS for compliance
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x14 );       /* Set VID_STD(IO Map 0x00[5:0])：	   010110b=0x16 -> HD 1x1 1920 x 1080p */
/*                                                                                                                                                       //																  010100b=0x14 -> HD 1x1 1920 x 1080i */
	set_i2c_register( fd, 0x20, 0x01, 0x15 );       /* Set VFREQ(IO Map 0x01[6:4])：		   000b -> 60 Hz */


/*
 *                                                                                                                                                       //																	001b -> 50 Hz
 *                                                                                                                                                       //																	010b -> 30 Hz
 *                                                                                                                                                       //																	011b -> 25 Hz
 *                                                                                                                                                       //																	100b -> 24 Hz
 *                                                                                                                                                       //Set PRIM_MODE(IO Map 0x01[3:0]): 0000b -> SDP mode
 *                                                                                                                                                       //																	0001b -> Component Mode
 *                                                                                                                                                       //																	0010b -> Graphics mode
 *                                                                                                                                                       //																	0100b -> CVBS & HDMI AUDIO Mode
 *                                                                                                                                                       //																	0101b -> HDMI-Comp
 *                                                                                                                                                       //																	0110b -> HDMI-GR
 */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */


/*
 * set_i2c_register(fd, 0x20, 0x03, 0x82);//24-bit ITU-656 SDR mode 0
 * ****modified 20150820************************* //
 */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
/* ********************************************** // */
	set_i2c_register( fd, 0x20, 0x06, 0xa6 );


/*
 * 68 C1 FF ; HDMI power control (power saving)
 * 68 C2 FF ; HDMI power control (power saving)
 * 68 C3 FF ; HDMI power control (power saving)
 * 68 C4 FF ; HDMI power control (power saving)
 * 68 C5 00 ; HDMI power control (power saving)
 * 68 C6 00 ; HDMI power control (power saving)
 * 68 C0 FF ; HDMI power control (power saving)
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0xC1, 0xFF );
	set_i2c_register( fd, 0x34, 0xC2, 0xFF );
	set_i2c_register( fd, 0x34, 0xC3, 0xFF );
	set_i2c_register( fd, 0x34, 0xC4, 0xFF );
	set_i2c_register( fd, 0x34, 0xC5, 0x00 );
	set_i2c_register( fd, 0x34, 0xC6, 0x00 );
	set_i2c_register( fd, 0x34, 0xC0, 0xFF );


/*
 * 40 0C 40 ; Power up part and power down VDP
 * 40 14 3F ; Max Drive Strength
 * 40 15 80 ; Disable Tristate of Pins
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 44 BA 01 ; Set HDMI FreeRun
 * 44 3E 00 ; Disable CP Pregain Block
 * 44 6C 00 ; Use fixed clamp values
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0xBA, 0x01 );
/*	set_i2c_register(fd, 0x22, 0x3E, 0x80); */
	set_i2c_register( fd, 0x22, 0x6C, 0x00 );


/*
 * 4C 00 FF ; Power Down ADC's and there associated clocks
 * 4C 01 FE ; Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
 * 4C B5 01 ; Setting MCLK to 256Fs
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x00, 0xFF );
	set_i2c_register( fd, 0x26, 0x01, 0xFE );
	set_i2c_register( fd, 0x26, 0xB5, 0x01 );


/*
 * 68 00 32 ; Set HDMI Input Port A (Enable BG monitoring)
 * 68 01 18 ; Enable clock terminators
 * 68 0D 34 ; ADI recommended write
 * 68 1A 8A ; Unmute audio
 * 68 3D 10 ; HDMI ADI recommended write
 * 68 44 85 ; TMDS PLL Optimization
 * 68 46 1F ; ADI Recommended Write ES3/Final silicon
 * 68 60 88 ; TMDS PLL Optimization
 * 68 61 88 ; TMDS PLL Optimization
 * 68 6C 18 ; Disable ISRC clearing bit, Improve robustness
 * 68 57 B6 ; TMDS PLL Optimization
 * 68 58 03 ; TMDS PLL Setting
 * 68 67 20 ; TMDS PLL Setting
 * 68 75 10 ; DDC drive strength
 * 68 85 1F ; ADI Equaliser Setting
 * 68 87 70 ; HDMI Recommended write
 * 68 89 04 ; ADI Equaliser Setting
 * 68 8A 1E ; ADI Equaliser Setting
 * 68 93 04 ; ADI Equaliser Setting
 * 68 94 1E ; ADI Equaliser Setting
 * 68 9D 02 ; ADI Equaliser Setting
 * 68 99 A1 ; HDMI ADI recommended write
 * 68 9B 09 ; HDMI ADI recommended write
 * 68 C9 01 ; HDMI free Run based on PRIM_MODE, VID _STD
 * ** slave address=0x68 *********************** //
 */

	set_i2c_register( fd, 0x34, 0x5A, 0x04 );

	set_i2c_register( fd, 0x34, 0x00, 0x32 );
/*	set_i2c_register(fd, 0x34, 0x00, 0x12);//Register 0x00[7:5] Set to 0x00 for non-fast switching applications */
	set_i2c_register( fd, 0x34, 0x01, 0x18 );

	set_i2c_register( fd, 0x34, 0x03, 0x10 );

	set_i2c_register( fd, 0x34, 0x0D, 0x34 );
	set_i2c_register( fd, 0x34, 0x1A, 0x8A );
	set_i2c_register( fd, 0x34, 0x3D, 0x10 );
	set_i2c_register( fd, 0x34, 0x44, 0x85 );
/*	set_i2c_register(fd, 0x34, 0x44, 0x05);//Register 0x44[7] Set to 0x0 for non-fast switching applications */
	set_i2c_register( fd, 0x34, 0x46, 0x1F );
/*	set_i2c_register(fd, 0x34, 0x4C, 0x04);//Register 0x4C[2] Set = 1 for low frame rate video formats */
	set_i2c_register( fd, 0x34, 0x60, 0x88 );
	set_i2c_register( fd, 0x34, 0x61, 0x88 );

/*	set_i2c_register(fd, 0x34, 0x69, 0x01); */

	set_i2c_register( fd, 0x34, 0x6C, 0x18 );
	set_i2c_register( fd, 0x34, 0x57, 0xB6 );
	set_i2c_register( fd, 0x34, 0x58, 0x03 );
	set_i2c_register( fd, 0x34, 0x67, 0x20 );
	set_i2c_register( fd, 0x34, 0x75, 0x10 );
	set_i2c_register( fd, 0x34, 0x85, 0x1F );
	set_i2c_register( fd, 0x34, 0x87, 0x70 );
	set_i2c_register( fd, 0x34, 0x89, 0x04 );
	set_i2c_register( fd, 0x34, 0x8A, 0x1E );
	set_i2c_register( fd, 0x34, 0x93, 0x04 );
	set_i2c_register( fd, 0x34, 0x94, 0x1E );
	set_i2c_register( fd, 0x34, 0x9D, 0x02 );
	set_i2c_register( fd, 0x34, 0x99, 0xA1 );
	set_i2c_register( fd, 0x34, 0x9B, 0x09 );
	set_i2c_register( fd, 0x34, 0xC9, 0x01 );

	set_i2c_register( fd, 0x34, 0x48, 0x80 );/* PWRDN1 has no effect */

	sdp_brightness_set( fd );
	DEBUG( "HDMI_COMP_1080ix50_mod..." );
}


void HDMI_COMP_1080ix60_mod( int fd )
{
	common_set( fd );


/*
 * 40 01 06 ; Prim_Mode =110b HDMI-GR
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 42 ; 36 bit SDR 444 Mode 0
 * 40 05 28 ; AV Codes Off
 * 40 06 A6 ; Invert HS,VS for compliance
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x14 );       /* Set VID_STD(IO Map 0x00[5:0])：	   010110b=0x16 -> HD 1x1 1920 x 1080p */
/*                                                                                                                                                       //																  010100b=0x14 -> HD 1x1 1920 x 1080i */
	set_i2c_register( fd, 0x20, 0x01, 0x05 );       /* Set VFREQ(IO Map 0x01[6:4])：		   000b -> 60 Hz */


/*
 *                                                                                                                                                       //																	001b -> 50 Hz
 *                                                                                                                                                       //																	010b -> 30 Hz
 *                                                                                                                                                       //																	011b -> 25 Hz
 *                                                                                                                                                       //																	100b -> 24 Hz
 *                                                                                                                                                       //Set PRIM_MODE(IO Map 0x01[3:0]): 0000b -> SDP mode
 *                                                                                                                                                       //																	0001b -> Component Mode
 *                                                                                                                                                       //																	0010b -> Graphics mode
 *                                                                                                                                                       //																	0100b -> CVBS & HDMI AUDIO Mode
 *                                                                                                                                                       //																	0101b -> HDMI-Comp
 *                                                                                                                                                       //																	0110b -> HDMI-GR
 */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */


/*
 * set_i2c_register(fd, 0x20, 0x03, 0x82);//24-bit ITU-656 SDR mode 0
 * ****modified 20150820************************* //
 */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
/* ********************************************** // */
	set_i2c_register( fd, 0x20, 0x06, 0xa6 );


/*
 * *****added 20150820********************* //
 * set_i2c_register(fd, 0x20, 0x07, 0x3C);
 * set_i2c_register(fd, 0x20, 0x08, 0x3C);
 * **************************************** //
 */


/*	set_i2c_register(fd, 0x20, 0x20, 0x40); */


/*
 * 68 C1 FF ; HDMI power control (power saving)
 * 68 C2 FF ; HDMI power control (power saving)
 * 68 C3 FF ; HDMI power control (power saving)
 * 68 C4 FF ; HDMI power control (power saving)
 * 68 C5 00 ; HDMI power control (power saving)
 * 68 C6 00 ; HDMI power control (power saving)
 * 68 C0 FF ; HDMI power control (power saving)
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0xC1, 0xFF );
	set_i2c_register( fd, 0x34, 0xC2, 0xFF );
	set_i2c_register( fd, 0x34, 0xC3, 0xFF );
	set_i2c_register( fd, 0x34, 0xC4, 0xFF );
	set_i2c_register( fd, 0x34, 0xC5, 0x00 );
	set_i2c_register( fd, 0x34, 0xC6, 0x00 );
	set_i2c_register( fd, 0x34, 0xC0, 0xFF );


/*
 * 40 0C 40 ; Power up part and power down VDP
 * 40 14 3F ; Max Drive Strength
 * 40 15 80 ; Disable Tristate of Pins
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 44 BA 01 ; Set HDMI FreeRun
 * 44 3E 00 ; Disable CP Pregain Block
 * 44 6C 00 ; Use fixed clamp values
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0xBA, 0x01 );
/*	set_i2c_register(fd, 0x22, 0x3E, 0x80); */
	set_i2c_register( fd, 0x22, 0x6C, 0x00 );


/*
 * 4C 00 FF ; Power Down ADC's and there associated clocks
 * 4C 01 FE ; Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
 * 4C B5 01 ; Setting MCLK to 256Fs
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x00, 0xFF );
	set_i2c_register( fd, 0x26, 0x01, 0xFE );
	set_i2c_register( fd, 0x26, 0xB5, 0x01 );


/*
 * 68 00 32 ; Set HDMI Input Port A (Enable BG monitoring)
 * 68 01 18 ; Enable clock terminators
 * 68 0D 34 ; ADI recommended write
 * 68 1A 8A ; Unmute audio
 * 68 3D 10 ; HDMI ADI recommended write
 * 68 44 85 ; TMDS PLL Optimization
 * 68 46 1F ; ADI Recommended Write ES3/Final silicon
 * 68 60 88 ; TMDS PLL Optimization
 * 68 61 88 ; TMDS PLL Optimization
 * 68 6C 18 ; Disable ISRC clearing bit, Improve robustness
 * 68 57 B6 ; TMDS PLL Optimization
 * 68 58 03 ; TMDS PLL Setting
 * 68 67 20 ; TMDS PLL Setting
 * 68 75 10 ; DDC drive strength
 * 68 85 1F ; ADI Equaliser Setting
 * 68 87 70 ; HDMI Recommended write
 * 68 89 04 ; ADI Equaliser Setting
 * 68 8A 1E ; ADI Equaliser Setting
 * 68 93 04 ; ADI Equaliser Setting
 * 68 94 1E ; ADI Equaliser Setting
 * 68 9D 02 ; ADI Equaliser Setting
 * 68 99 A1 ; HDMI ADI recommended write
 * 68 9B 09 ; HDMI ADI recommended write
 * 68 C9 01 ; HDMI free Run based on PRIM_MODE, VID _STD
 * ** slave address=0x68 *********************** //
 */

	set_i2c_register( fd, 0x34, 0x5A, 0x04 );

	set_i2c_register( fd, 0x34, 0x00, 0x32 );
/*	set_i2c_register(fd, 0x34, 0x00, 0x12);//Register 0x00[7:5] Set to 0x00 for non-fast switching applications */
	set_i2c_register( fd, 0x34, 0x01, 0x18 );

	set_i2c_register( fd, 0x34, 0x03, 0x10 );

	set_i2c_register( fd, 0x34, 0x0D, 0x34 );
	set_i2c_register( fd, 0x34, 0x1A, 0x8A );
	set_i2c_register( fd, 0x34, 0x3D, 0x10 );
	set_i2c_register( fd, 0x34, 0x44, 0x85 );
/*	set_i2c_register(fd, 0x34, 0x44, 0x05);//Register 0x44[7] Set to 0x0 for non-fast switching applications */
	set_i2c_register( fd, 0x34, 0x46, 0x1F );
/*	set_i2c_register(fd, 0x34, 0x4C, 0x04);//Register 0x4C[2] Set = 1 for low frame rate video formats */
	set_i2c_register( fd, 0x34, 0x60, 0x88 );
	set_i2c_register( fd, 0x34, 0x61, 0x88 );

/*	set_i2c_register(fd, 0x34, 0x69, 0x01); */

	set_i2c_register( fd, 0x34, 0x6C, 0x18 );
	set_i2c_register( fd, 0x34, 0x57, 0xB6 );
	set_i2c_register( fd, 0x34, 0x58, 0x03 );
	set_i2c_register( fd, 0x34, 0x67, 0x20 );
	set_i2c_register( fd, 0x34, 0x75, 0x10 );
	set_i2c_register( fd, 0x34, 0x85, 0x1F );
	set_i2c_register( fd, 0x34, 0x87, 0x70 );
	set_i2c_register( fd, 0x34, 0x89, 0x04 );
	set_i2c_register( fd, 0x34, 0x8A, 0x1E );
	set_i2c_register( fd, 0x34, 0x93, 0x04 );
	set_i2c_register( fd, 0x34, 0x94, 0x1E );
	set_i2c_register( fd, 0x34, 0x9D, 0x02 );
	set_i2c_register( fd, 0x34, 0x99, 0xA1 );
	set_i2c_register( fd, 0x34, 0x9B, 0x09 );
	set_i2c_register( fd, 0x34, 0xC9, 0x01 );

	set_i2c_register( fd, 0x34, 0x48, 0x80 );/* PWRDN1 has no effect */
	sdp_brightness_set( fd );
	DEBUG( "HDMI_COMP_1080ix60_mod" );
}


void HDMI_COMP_1080px50_mod( int fd )
{
	common_set( fd );
	DEBUG( "HDMI_COMP_1080px50_mod..." );


/*
 * :6-4b - 1920 x 1080p60, YPrPb 444 in, YPrPb 444 out, 48kHz 8 Ch VIC 16:
 * 40 01 06 ; Prim_Mode =110b HDMI-GR
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 42 ; 36 bit SDR 444 Mode 0
 * 40 05 28 ; AV Codes Off
 * 40 06 A6 ; Invert HS,VS for compliance
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x16 );       /* Set VID_STD(IO Map 0x00[5:0])：	   010110b=0x16 -> HD 1x1 1920 x 1080p */
/*                                                                                                                                                       //																  010100b=0x14 -> HD 1x1 1920 x 1080i */
	set_i2c_register( fd, 0x20, 0x01, 0x15 );       /* Set VFREQ(IO Map 0x01[6:4])：		   000b -> 60 Hz */


/*
 *                                                                                                                                                       //																	001b -> 50 Hz
 *                                                                                                                                                       //																	010b -> 30 Hz
 *                                                                                                                                                       //																	011b -> 25 Hz
 *                                                                                                                                                       //																	100b -> 24 Hz
 *                                                                                                                                                       //Set PRIM_MODE(IO Map 0x01[3:0]): 0000b -> SDP mode
 *                                                                                                                                                       //																	0001b -> Component Mode
 *                                                                                                                                                       //																	0010b -> Graphics mode
 *                                                                                                                                                       //																	0100b -> CVBS & HDMI AUDIO Mode
 *                                                                                                                                                       //																	0101b -> HDMI-Comp
 *                                                                                                                                                       //																	0110b -> HDMI-GR
 * set_i2c_register(fd, 0x20, 0x02, 0xF4);
 */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
/*	set_i2c_register(fd, 0x20, 0x03, 0x42);//36-bit 4:4:4 SDR mode 0 */
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */


/* ****modified 20150820************************* // */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
/* ********************************************** // */


/*
 * set_i2c_register(fd, 0x20, 0x05, 0x38);
 * set_i2c_register(fd, 0x20, 0x06, 0x20);
 */
	set_i2c_register( fd, 0x20, 0x06, 0xa6 );


/*
 * *****added 20150820********************* //
 * set_i2c_register(fd, 0x20, 0x07, 0x3C);
 * set_i2c_register(fd, 0x20, 0x08, 0x3C);
 * **************************************** //
 */


/*	set_i2c_register(fd, 0x20, 0x20, 0x40); */


/*
 * 68 C1 FF ; HDMI power control (power saving)
 * 68 C2 FF ; HDMI power control (power saving)
 * 68 C3 FF ; HDMI power control (power saving)
 * 68 C4 FF ; HDMI power control (power saving)
 * 68 C5 00 ; HDMI power control (power saving)
 * 68 C6 00 ; HDMI power control (power saving)
 * 68 C0 FF ; HDMI power control (power saving)
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0xC1, 0xFF );
	set_i2c_register( fd, 0x34, 0xC2, 0xFF );
	set_i2c_register( fd, 0x34, 0xC3, 0xFF );
	set_i2c_register( fd, 0x34, 0xC4, 0xFF );
	set_i2c_register( fd, 0x34, 0xC5, 0x00 );
	set_i2c_register( fd, 0x34, 0xC6, 0x00 );
	set_i2c_register( fd, 0x34, 0xC0, 0xFF );


/*
 * 40 0C 40 ; Power up part and power down VDP
 * 40 14 3F ; Max Drive Strength
 * 40 15 80 ; Disable Tristate of Pins
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 44 BA 01 ; Set HDMI FreeRun
 * 44 3E 00 ; Disable CP Pregain Block
 * 44 6C 00 ; Use fixed clamp values
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0xBA, 0x01 );
/*	set_i2c_register(fd, 0x22, 0x3E, 0x80); */
	set_i2c_register( fd, 0x22, 0x6C, 0x00 );


/*
 * 4C 00 FF ; Power Down ADC's and there associated clocks
 * 4C 01 FE ; Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
 * 4C B5 01 ; Setting MCLK to 256Fs
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x00, 0xFF );
	set_i2c_register( fd, 0x26, 0x01, 0xFE );
	set_i2c_register( fd, 0x26, 0xB5, 0x01 );


/*
 * 68 00 32 ; Set HDMI Input Port A (Enable BG monitoring)
 * 68 01 18 ; Enable clock terminators
 * 68 0D 34 ; ADI recommended write
 * 68 1A 8A ; Unmute audio
 * 68 3D 10 ; HDMI ADI recommended write
 * 68 44 85 ; TMDS PLL Optimization
 * 68 46 1F ; ADI Recommended Write ES3/Final silicon
 * 68 60 88 ; TMDS PLL Optimization
 * 68 61 88 ; TMDS PLL Optimization
 * 68 6C 18 ; Disable ISRC clearing bit, Improve robustness
 * 68 57 B6 ; TMDS PLL Optimization
 * 68 58 03 ; TMDS PLL Setting
 * 68 67 20 ; TMDS PLL Setting
 * 68 75 10 ; DDC drive strength
 * 68 85 1F ; ADI Equaliser Setting
 * 68 87 70 ; HDMI Recommended write
 * 68 89 04 ; ADI Equaliser Setting
 * 68 8A 1E ; ADI Equaliser Setting
 * 68 93 04 ; ADI Equaliser Setting
 * 68 94 1E ; ADI Equaliser Setting
 * 68 9D 02 ; ADI Equaliser Setting
 * 68 99 A1 ; HDMI ADI recommended write
 * 68 9B 09 ; HDMI ADI recommended write
 * 68 C9 01 ; HDMI free Run based on PRIM_MODE, VID _STD
 * ** slave address=0x68 *********************** //
 */

	set_i2c_register( fd, 0x34, 0x5A, 0x04 );

	set_i2c_register( fd, 0x34, 0x00, 0x32 );
/*	set_i2c_register(fd, 0x34, 0x00, 0x12);//Register 0x00[7:5] Set to 0x00 for non-fast switching applications */
	set_i2c_register( fd, 0x34, 0x01, 0x18 );

	set_i2c_register( fd, 0x34, 0x03, 0x10 );

	set_i2c_register( fd, 0x34, 0x0D, 0x34 );
	set_i2c_register( fd, 0x34, 0x1A, 0x8A );
	set_i2c_register( fd, 0x34, 0x3D, 0x10 );
	set_i2c_register( fd, 0x34, 0x44, 0x85 );
/*	set_i2c_register(fd, 0x34, 0x44, 0x05);//Register 0x44[7] Set to 0x0 for non-fast switching applications */
	set_i2c_register( fd, 0x34, 0x46, 0x1F );
	set_i2c_register( fd, 0x34, 0x4C, 0x04 );/* Register 0x4C[2] Set = 1 for low frame rate video formats */
	set_i2c_register( fd, 0x34, 0x60, 0x88 );
	set_i2c_register( fd, 0x34, 0x61, 0x88 );

/*	set_i2c_register(fd, 0x34, 0x69, 0x01); */

	set_i2c_register( fd, 0x34, 0x6C, 0x18 );
	set_i2c_register( fd, 0x34, 0x57, 0xB6 );
	set_i2c_register( fd, 0x34, 0x58, 0x03 );
	set_i2c_register( fd, 0x34, 0x67, 0x20 );
	set_i2c_register( fd, 0x34, 0x75, 0x10 );
	set_i2c_register( fd, 0x34, 0x85, 0x1F );
	set_i2c_register( fd, 0x34, 0x87, 0x70 );
	set_i2c_register( fd, 0x34, 0x89, 0x04 );
	set_i2c_register( fd, 0x34, 0x8A, 0x1E );
	set_i2c_register( fd, 0x34, 0x93, 0x04 );
	set_i2c_register( fd, 0x34, 0x94, 0x1E );
	set_i2c_register( fd, 0x34, 0x9D, 0x02 );
	set_i2c_register( fd, 0x34, 0x99, 0xA1 );
	set_i2c_register( fd, 0x34, 0x9B, 0x09 );
	set_i2c_register( fd, 0x34, 0xC9, 0x01 );

	set_i2c_register( fd, 0x34, 0x48, 0x80 );/* PWRDN1 has no effect */

	sdp_brightness_set( fd );
}


void HDMI_COMP_1080px60_mod( int fd )
{
	common_set( fd );
	/* :6-4b - 1920 x 1080p60, YPrPb 444 in, YPrPb 444 out, 48kHz 8 Ch VIC 16: */


/*
 * 40 01 06 ; Prim_Mode =110b HDMI-GR
 * 40 02 F4 ; Auto input color space, Limited Range YPbPr Output
 * 40 03 42 ; 36 bit SDR 444 Mode 0
 * 40 05 28 ; AV Codes Off
 * 40 06 A6 ; Invert HS,VS for compliance
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x16 );       /* Set VID_STD(IO Map 0x00[5:0])：	   010110b=0x16 -> HD 1x1 1920 x 1080p */
/*                                                                                                                                                       //																  010100b=0x14 -> HD 1x1 1920 x 1080i */
	set_i2c_register( fd, 0x20, 0x01, 0x05 );       /* Set VFREQ(IO Map 0x01[6:4])：		   000b -> 60 Hz */


/*
 *                                                                                                                                                       //																	001b -> 50 Hz
 *                                                                                                                                                       //																	010b -> 30 Hz
 *                                                                                                                                                       //																	011b -> 25 Hz
 *                                                                                                                                                       //																	100b -> 24 Hz
 *                                                                                                                                                       //Set PRIM_MODE(IO Map 0x01[3:0]): 0000b -> SDP mode
 *                                                                                                                                                       //																	0001b -> Component Mode
 *                                                                                                                                                       //																	0010b -> Graphics mode
 *                                                                                                                                                       //																	0100b -> CVBS & HDMI AUDIO Mode
 *                                                                                                                                                       //																	0101b -> HDMI-Comp
 *                                                                                                                                                       //																	0110b -> HDMI-GR
 * set_i2c_register(fd, 0x20, 0x02, 0xF4);
 */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
/*	set_i2c_register(fd, 0x20, 0x03, 0x42);//36-bit 4:4:4 SDR mode 0 */
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */


/*
 * set_i2c_register(fd, 0x20, 0x03, 0x82);//24-bit ITU-656 SDR mode 0
 *  set_i2c_register(fd, 0x20, 0x04, 0x62);//P[35:24] V/R, P[23:12] Y/G, P[11:0] U/CrCb/B
 * set_i2c_register(fd, 0x20, 0x05, 0x28);
 */

/* ****modified 20150820************************* // */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );
/* ********************************************** // */


/*
 * set_i2c_register(fd, 0x20, 0x05, 0x38);
 * set_i2c_register(fd, 0x20, 0x06, 0x20);
 */
	set_i2c_register( fd, 0x20, 0x06, 0xa6 );


/*
 * *****added 20150820********************* //
 * set_i2c_register(fd, 0x20, 0x07, 0x3C);
 * set_i2c_register(fd, 0x20, 0x08, 0x3C);
 * **************************************** //
 */


/*	set_i2c_register(fd, 0x20, 0x20, 0x40); */


/*
 * 68 C1 FF ; HDMI power control (power saving)
 * 68 C2 FF ; HDMI power control (power saving)
 * 68 C3 FF ; HDMI power control (power saving)
 * 68 C4 FF ; HDMI power control (power saving)
 * 68 C5 00 ; HDMI power control (power saving)
 * 68 C6 00 ; HDMI power control (power saving)
 * 68 C0 FF ; HDMI power control (power saving)
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0xC1, 0xFF );
	set_i2c_register( fd, 0x34, 0xC2, 0xFF );
	set_i2c_register( fd, 0x34, 0xC3, 0xFF );
	set_i2c_register( fd, 0x34, 0xC4, 0xFF );
	set_i2c_register( fd, 0x34, 0xC5, 0x00 );
	set_i2c_register( fd, 0x34, 0xC6, 0x00 );
	set_i2c_register( fd, 0x34, 0xC0, 0xFF );


/*
 * 40 0C 40 ; Power up part and power down VDP
 * 40 14 3F ; Max Drive Strength
 * 40 15 80 ; Disable Tristate of Pins
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x14, 0x3F );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 44 BA 01 ; Set HDMI FreeRun
 * 44 3E 00 ; Disable CP Pregain Block
 * 44 6C 00 ; Use fixed clamp values
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0xBA, 0x01 );
/*	set_i2c_register(fd, 0x22, 0x3E, 0x80); */
	set_i2c_register( fd, 0x22, 0x6C, 0x00 );


/*
 * set_i2c_register(fd, 0x22, 0xC0, 0x23);
 * set_i2c_register(fd, 0x22, 0xC1, 0x72);
 * set_i2c_register(fd, 0x22, 0xC2, 0xD4);
 * set_i2c_register(fd, 0x22, 0xBF, 0x03);
 */


/*
 * set_i2c_register(fd, 0x22, 0xC9, 0x01);
 * set_i2c_register(fd, 0x22, 0xBF, 0x00);
 */


/*
 * set_i2c_register(fd, 0x22, 0xBD, 0x00);
 * set_i2c_register(fd, 0x22, 0x68, 0x08);
 * set_i2c_register(fd, 0x22, 0x69, 0x10);
 */


/*
 * set_i2c_register(fd, 0x22, 0x3e, 0x80);
 * set_i2c_register(fd, 0x22, 0x3D, 0x0F);
 * set_i2c_register(fd, 0x22, 0x3B, 0xFF);
 * set_i2c_register(fd, 0x22, 0x3A, 0xFF);
 */


/*
 * *****added 20150820********************* //
 * set_i2c_register(fd, 0x22, 0x81, 0x10);
 * **************************************** //
 */


/*
 * set_i2c_register(fd, 0x22, 0x7C, 0x0C);	//START_HS[9:0], Addr 44 (CP), Address 0x7C[3:2]; Address 0x7E[7:0]
 * set_i2c_register(fd, 0x22, 0x7E, 0xA4);
 *
 * set_i2c_register(fd, 0x22, 0x7C, 0x0C);	//END_HS[9:0], Addr 44 (CP), Address 0x7C[1:0]; Address 0x7D[7:0]
 * set_i2c_register(fd, 0x22, 0x7D, 0x90);
 */


/*
 * set_i2c_register(fd, 0x22, 0xBA, 0x03);//HDMI_FRUN_EN, Addr 44 (CP), Address 0xBA[0]
 *                                                                                                                                                       //HDMI_FRUN_MODE, Addr 44 (CP), Address 0xBA[1]
 * set_i2c_register(fd, 0x22, 0xC9, 0x2D);//Enable free-run based on primary mode and video standard (CP Map 0xC9[0])
 */


/*
 * ***added 20150820************** //
 * set_i2c_register(fd, 0x22, 0xC9, 0x2D);//Enable free-run based on primary mode and video standard (CP Map 0xC9[0])
 * set_i2c_register(fd, 0x22, 0xBF, 0x13);//Enable HDMI Free-Run (CP Map 0xBF[0])
 * ******************************* //
 */


/*
 * 4C 00 FF ; Power Down ADC's and there associated clocks
 * 4C 01 FE ; Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
 * 4C B5 01 ; Setting MCLK to 256Fs
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x00, 0xFF );
	set_i2c_register( fd, 0x26, 0x01, 0xFE );
	set_i2c_register( fd, 0x26, 0xB5, 0x01 );


/*
 * 68 00 32 ; Set HDMI Input Port A (Enable BG monitoring)
 * 68 01 18 ; Enable clock terminators
 * 68 0D 34 ; ADI recommended write
 * 68 1A 8A ; Unmute audio
 * 68 3D 10 ; HDMI ADI recommended write
 * 68 44 85 ; TMDS PLL Optimization
 * 68 46 1F ; ADI Recommended Write ES3/Final silicon
 * 68 60 88 ; TMDS PLL Optimization
 * 68 61 88 ; TMDS PLL Optimization
 * 68 6C 18 ; Disable ISRC clearing bit, Improve robustness
 * 68 57 B6 ; TMDS PLL Optimization
 * 68 58 03 ; TMDS PLL Setting
 * 68 67 20 ; TMDS PLL Setting
 * 68 75 10 ; DDC drive strength
 * 68 85 1F ; ADI Equaliser Setting
 * 68 87 70 ; HDMI Recommended write
 * 68 89 04 ; ADI Equaliser Setting
 * 68 8A 1E ; ADI Equaliser Setting
 * 68 93 04 ; ADI Equaliser Setting
 * 68 94 1E ; ADI Equaliser Setting
 * 68 9D 02 ; ADI Equaliser Setting
 * 68 99 A1 ; HDMI ADI recommended write
 * 68 9B 09 ; HDMI ADI recommended write
 * 68 C9 01 ; HDMI free Run based on PRIM_MODE, VID _STD
 * ** slave address=0x68 *********************** //
 */

	set_i2c_register( fd, 0x34, 0x5A, 0x04 );

	set_i2c_register( fd, 0x34, 0x00, 0x32 );
/*	set_i2c_register(fd, 0x34, 0x00, 0x12);//Register 0x00[7:5] Set to 0x00 for non-fast switching applications */
	set_i2c_register( fd, 0x34, 0x01, 0x18 );

	set_i2c_register( fd, 0x34, 0x03, 0x10 );

	set_i2c_register( fd, 0x34, 0x0D, 0x34 );
	set_i2c_register( fd, 0x34, 0x1A, 0x8A );
	set_i2c_register( fd, 0x34, 0x3D, 0x10 );
	set_i2c_register( fd, 0x34, 0x44, 0x85 );
/*	set_i2c_register(fd, 0x34, 0x44, 0x05);//Register 0x44[7] Set to 0x0 for non-fast switching applications */
	set_i2c_register( fd, 0x34, 0x46, 0x1F );
	set_i2c_register( fd, 0x34, 0x4C, 0x04 );/* Register 0x4C[2] Set = 1 for low frame rate video formats */
	set_i2c_register( fd, 0x34, 0x60, 0x88 );
	set_i2c_register( fd, 0x34, 0x61, 0x88 );

/*	set_i2c_register(fd, 0x34, 0x69, 0x01); */

	set_i2c_register( fd, 0x34, 0x6C, 0x18 );
	set_i2c_register( fd, 0x34, 0x57, 0xB6 );
	set_i2c_register( fd, 0x34, 0x58, 0x03 );
	set_i2c_register( fd, 0x34, 0x67, 0x20 );
	set_i2c_register( fd, 0x34, 0x75, 0x10 );
	set_i2c_register( fd, 0x34, 0x85, 0x1F );
	set_i2c_register( fd, 0x34, 0x87, 0x70 );
	set_i2c_register( fd, 0x34, 0x89, 0x04 );
	set_i2c_register( fd, 0x34, 0x8A, 0x1E );
	set_i2c_register( fd, 0x34, 0x93, 0x04 );
	set_i2c_register( fd, 0x34, 0x94, 0x1E );
	set_i2c_register( fd, 0x34, 0x9D, 0x02 );
	set_i2c_register( fd, 0x34, 0x99, 0xA1 );
	set_i2c_register( fd, 0x34, 0x9B, 0x09 );
	set_i2c_register( fd, 0x34, 0xC9, 0x01 );

	set_i2c_register( fd, 0x34, 0x48, 0x80 );/* PWRDN1 has no effect */


/*
 * ********************************************************************* //
 * set_i2c_register(fd, 0x22, 0xBD, 0x10);	//DPP_BYPASS_EN, Addr 44 (CP), Address 0xBD[4]
 * set_i2c_register(fd, 0x34, 0x1D, 0x00);	//UP_CONVERSION_MODE, Addr 68 (HDMI), Address 0x1D[5]
 * set_i2c_register(fd, 0x20, 0xE0, 0x80);	//DS_WITHOUT_FILTER, Addr 40 (IO), Address 0xE0[7]
 */

	sdp_brightness_set( fd );
}


void HDMI_COMP_720px50_mod( int fd )
{
	common_set( fd );

/* :6-1b Port A, 720p,1080i YPrPb 444 in, RGB 444 out, Through DAC (Fast Switching)VIC[4,5,19,20]: */


/*
 * 40 01 06 ; Prim_Mode =110b HDMI-GR
 * 40 02 F6 ; Auto input color space, Limited Range RGB Output
 * 40 03 42 ; 36 bit SDR 444 Mode 0
 * 40 05 28 ; AV Codes Off
 * 40 06 A6 ; Invert VS,HS pins
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x13 );       /* Set VID_STD(IO Map 0x00[5:0])：	 010110=0x16 -> HD 1x1 1920 x 1080 */


	/*
	 *                                                                                                                        010100=0x14 -> HD 1x1 1920 x 1080i
	 *                                                                                                                        010011=0x13 -> HD 1x1 1280 x 720p
	 */
	set_i2c_register( fd, 0x20, 0x01, 0x15 );       /* Set VFREQ(IO Map 0x01[6:4])：		 000 -> 60 Hz */


	/*
	 *                                                                                                                              001 -> 50 Hz
	 *                                                                                                                              010 -> 30 Hz
	 *                                                                                                                              011 -> 25 Hz
	 *                                                                                                                              100 -> 24 Hz
	 * Set PRIM_MODE(IO Map 0x01[3:0]): 0000 -> SDP mode
	 *                                                                                                                              0001 -> Component Mode
	 *                                                                                                                              0010 -> Graphics mode
	 *                                                                                                                              0100 -> CVBS & HDMI AUDIO Mode
	 *                                                                                                                              0101 -> HDMI-Comp
	 *                                                                                                                              0110 -> HDMI-GR
	 */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */
/* ****modified 20150820************************* // */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );       /* AV Codes On */
/* ********************************************** // */
	set_i2c_register( fd, 0x20, 0x06, 0xa6 );


/*
 * 68 C1 FF ; HDMI power control (power saving)
 * 68 C2 FF ; HDMI power control (power saving)
 * 68 C3 FF ; HDMI power control (power saving)
 * 68 C4 FF ; HDMI power control (power saving)
 * 68 C5 00 ; HDMI power control (power saving)
 * 68 C6 00 ; HDMI power control (power saving)
 * 68 C0 FF ; HDMI power control (power saving)
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0xC1, 0xFF );
	set_i2c_register( fd, 0x34, 0xC2, 0xFF );
	set_i2c_register( fd, 0x34, 0xC3, 0xFF );
	set_i2c_register( fd, 0x34, 0xC4, 0xFF );
	set_i2c_register( fd, 0x34, 0xC5, 0x00 );
	set_i2c_register( fd, 0x34, 0xC6, 0x00 );
	set_i2c_register( fd, 0x34, 0xC0, 0xFF );


/*
 * 40 0C 40 ; Power up part and power down VDP
 * 40 15 80 ; Disable Tristate of Pins
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 44 BA 01 ; Set HDMI FreeRun
 * 44 3E 00 ; Disable CP Pregain Block
 * 44 6C 00 ; Use fixed clamp values
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0xBA, 0x01 );
/*	set_i2c_register(fd, 0x22, 0x3E, 0x80); */
	set_i2c_register( fd, 0x22, 0x6C, 0x00 );

/*	set_i2c_register(fd, 0x22, 0xbf, 0x03); */


/*
 * 4C 00 FF ; Power Down ADC's and there associated clocks
 * 4C 01 FE ; Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
 * 4C B5 01 ; Setting MCLK to 256Fs
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x00, 0xFF );
	set_i2c_register( fd, 0x26, 0x01, 0xFE );
	set_i2c_register( fd, 0x26, 0xB5, 0x01 );


/*
 * 68 00 32 ; Set HDMI Input Port A (Enable BG monitoring)
 * 68 01 18 ; Enable clock terminators
 * 68 0D 34 ; ADI recommended write
 * 68 1A 8A ; Unmute audio
 * 68 3D 10 ; HDMI ADI recommended write
 * 68 44 85 ; TMDS PLL Optimization
 * 68 46 1F ; ADI Recommended Write ES3/Final silicon
 * 68 60 88 ; TMDS PLL Optimization
 * 68 61 88 ; TMDS PLL Optimization
 * 68 6C 18 ; Disable ISRC clearing bit, Improve robustness
 * 68 57 B6 ; TMDS PLL Optimization
 * 68 58 03 ; TMDS PLL Setting
 * 68 67 20 ; TMDS PLL Setting
 * 68 75 10 ; DDC drive strength
 * 68 85 1F ; ADI Equaliser Setting
 * 68 87 70 ; HDMI Recommended write
 * 68 89 04 ; ADI Equaliser Setting
 * 68 8A 1E ; ADI Equaliser Setting
 * 68 93 04 ; ADI Equaliser Setting
 * 68 94 1E ; ADI Equaliser Setting
 * 68 9D 02 ; ADI Equaliser Setting
 * 68 99 A1 ; HDMI ADI recommended write
 * 68 9B 09 ; HDMI ADI recommended write
 * 68 C9 01 ; HDMI free Run based on PRIM_MODE, VID _STD
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0x00, 0x32 );
	set_i2c_register( fd, 0x34, 0x01, 0x18 );
	set_i2c_register( fd, 0x34, 0x0D, 0x34 );
	set_i2c_register( fd, 0x34, 0x1A, 0x8A );
	set_i2c_register( fd, 0x34, 0x3D, 0x10 );
	set_i2c_register( fd, 0x34, 0x44, 0x85 );
	set_i2c_register( fd, 0x34, 0x46, 0x1F );
	set_i2c_register( fd, 0x34, 0x60, 0x88 );
	set_i2c_register( fd, 0x34, 0x61, 0x88 );
	set_i2c_register( fd, 0x34, 0x6C, 0x18 );
	set_i2c_register( fd, 0x34, 0x57, 0xB6 );
	set_i2c_register( fd, 0x34, 0x58, 0x03 );
	set_i2c_register( fd, 0x34, 0x67, 0x20 );
	set_i2c_register( fd, 0x34, 0x75, 0x10 );
	set_i2c_register( fd, 0x34, 0x85, 0x1F );
	set_i2c_register( fd, 0x34, 0x87, 0x70 );
	set_i2c_register( fd, 0x34, 0x89, 0x04 );
	set_i2c_register( fd, 0x34, 0x8A, 0x1E );
	set_i2c_register( fd, 0x34, 0x93, 0x04 );
	set_i2c_register( fd, 0x34, 0x94, 0x1E );
	set_i2c_register( fd, 0x34, 0x9D, 0x02 );
	set_i2c_register( fd, 0x34, 0x99, 0xA1 );
	set_i2c_register( fd, 0x34, 0x9B, 0x09 );
	set_i2c_register( fd, 0x34, 0xC9, 0x01 );

	set_i2c_register( fd, 0x34, 0x48, 0x80 );/* PWRDN1 has no effect */


/*
 * set_i2c_register(fd, 0x34, 0x40, 0x40);//OVERRIDE_DEEP_COLOR_MODE
 * set_i2c_register(fd, 0x34, 0x5A, 0x04);//DCFIFO_RECENTER
 */
	sdp_brightness_set( fd );
	DEBUG( "HDMI_COMP_720px50_mod" );
}


void HDMI_COMP_720px60_mod( int fd )
{
	common_set( fd );
	/* :6-1b Port A, 720p,1080i YPrPb 444 in, RGB 444 out, Through DAC (Fast Switching)VIC[4,5,19,20]: */


/*
 * 40 01 06 ; Prim_Mode =110b HDMI-GR
 * 40 02 F6 ; Auto input color space, Limited Range RGB Output
 * 40 03 42 ; 36 bit SDR 444 Mode 0
 * 40 05 28 ; AV Codes Off
 * 40 06 A6 ; Invert VS,HS pins
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x13 );       /* Set VID_STD(IO Map 0x00[5:0])：	 010110=0x16 -> HD 1x1 1920 x 1080 */


	/*
	 *                                                                                                                        010100=0x14 -> HD 1x1 1920 x 1080i
	 *                                                                                                                        010011=0x13 -> HD 1x1 1280 x 720p
	 */
	set_i2c_register( fd, 0x20, 0x01, 0x05 );       /* Set VFREQ(IO Map 0x01[6:4])：		 000 -> 60 Hz */


	/*
	 *                                                                                                                              001 -> 50 Hz
	 *                                                                                                                              010 -> 30 Hz
	 *                                                                                                                              011 -> 25 Hz
	 *                                                                                                                              100 -> 24 Hz
	 * Set PRIM_MODE(IO Map 0x01[3:0]): 0000 -> SDP mode
	 *                                                                                                                              0001 -> Component Mode
	 *                                                                                                                              0010 -> Graphics mode
	 *                                                                                                                              0100 -> CVBS & HDMI AUDIO Mode
	 *                                                                                                                              0101 -> HDMI-Comp
	 *                                                                                                                              0110 -> HDMI-GR
	 */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x80 );       /* 16-bit ITU-656 SDR mode */
/* ****modified 20150820************************* // */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );       /* AV Codes On */
/* ********************************************** // */
	set_i2c_register( fd, 0x20, 0x06, 0xa6 );


/*
 * 68 C1 FF ; HDMI power control (power saving)
 * 68 C2 FF ; HDMI power control (power saving)
 * 68 C3 FF ; HDMI power control (power saving)
 * 68 C4 FF ; HDMI power control (power saving)
 * 68 C5 00 ; HDMI power control (power saving)
 * 68 C6 00 ; HDMI power control (power saving)
 * 68 C0 FF ; HDMI power control (power saving)
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0xC1, 0xFF );
	set_i2c_register( fd, 0x34, 0xC2, 0xFF );
	set_i2c_register( fd, 0x34, 0xC3, 0xFF );
	set_i2c_register( fd, 0x34, 0xC4, 0xFF );
	set_i2c_register( fd, 0x34, 0xC5, 0x00 );
	set_i2c_register( fd, 0x34, 0xC6, 0x00 );
	set_i2c_register( fd, 0x34, 0xC0, 0xFF );


/*
 * 40 0C 40 ; Power up part and power down VDP
 * 40 15 80 ; Disable Tristate of Pins
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 44 BA 01 ; Set HDMI FreeRun
 * 44 3E 00 ; Disable CP Pregain Block
 * 44 6C 00 ; Use fixed clamp values
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0xBA, 0x01 );
/*	set_i2c_register(fd, 0x22, 0x3E, 0x80); */
	set_i2c_register( fd, 0x22, 0x6C, 0x00 );

/*	set_i2c_register(fd, 0x22, 0xbf, 0x03); */


/*
 * 4C 00 FF ; Power Down ADC's and there associated clocks
 * 4C 01 FE ; Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
 * 4C B5 01 ; Setting MCLK to 256Fs
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x00, 0xFF );
	set_i2c_register( fd, 0x26, 0x01, 0xFE );
	set_i2c_register( fd, 0x26, 0xB5, 0x01 );


/*
 * 68 00 32 ; Set HDMI Input Port A (Enable BG monitoring)
 * 68 01 18 ; Enable clock terminators
 * 68 0D 34 ; ADI recommended write
 * 68 1A 8A ; Unmute audio
 * 68 3D 10 ; HDMI ADI recommended write
 * 68 44 85 ; TMDS PLL Optimization
 * 68 46 1F ; ADI Recommended Write ES3/Final silicon
 * 68 60 88 ; TMDS PLL Optimization
 * 68 61 88 ; TMDS PLL Optimization
 * 68 6C 18 ; Disable ISRC clearing bit, Improve robustness
 * 68 57 B6 ; TMDS PLL Optimization
 * 68 58 03 ; TMDS PLL Setting
 * 68 67 20 ; TMDS PLL Setting
 * 68 75 10 ; DDC drive strength
 * 68 85 1F ; ADI Equaliser Setting
 * 68 87 70 ; HDMI Recommended write
 * 68 89 04 ; ADI Equaliser Setting
 * 68 8A 1E ; ADI Equaliser Setting
 * 68 93 04 ; ADI Equaliser Setting
 * 68 94 1E ; ADI Equaliser Setting
 * 68 9D 02 ; ADI Equaliser Setting
 * 68 99 A1 ; HDMI ADI recommended write
 * 68 9B 09 ; HDMI ADI recommended write
 * 68 C9 01 ; HDMI free Run based on PRIM_MODE, VID _STD
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0x00, 0x32 );
	set_i2c_register( fd, 0x34, 0x01, 0x18 );
	set_i2c_register( fd, 0x34, 0x0D, 0x34 );
	set_i2c_register( fd, 0x34, 0x1A, 0x8A );
	set_i2c_register( fd, 0x34, 0x3D, 0x10 );
	set_i2c_register( fd, 0x34, 0x44, 0x85 );
	set_i2c_register( fd, 0x34, 0x46, 0x1F );
	set_i2c_register( fd, 0x34, 0x60, 0x88 );
	set_i2c_register( fd, 0x34, 0x61, 0x88 );
	set_i2c_register( fd, 0x34, 0x6C, 0x18 );
	set_i2c_register( fd, 0x34, 0x57, 0xB6 );
	set_i2c_register( fd, 0x34, 0x58, 0x03 );
	set_i2c_register( fd, 0x34, 0x67, 0x20 );
	set_i2c_register( fd, 0x34, 0x75, 0x10 );
	set_i2c_register( fd, 0x34, 0x85, 0x1F );
	set_i2c_register( fd, 0x34, 0x87, 0x70 );
	set_i2c_register( fd, 0x34, 0x89, 0x04 );
	set_i2c_register( fd, 0x34, 0x8A, 0x1E );
	set_i2c_register( fd, 0x34, 0x93, 0x04 );
	set_i2c_register( fd, 0x34, 0x94, 0x1E );
	set_i2c_register( fd, 0x34, 0x9D, 0x02 );
	set_i2c_register( fd, 0x34, 0x99, 0xA1 );
	set_i2c_register( fd, 0x34, 0x9B, 0x09 );
	set_i2c_register( fd, 0x34, 0xC9, 0x01 );

	set_i2c_register( fd, 0x34, 0x48, 0x80 );/* PWRDN1 has no effect */


/*
 * set_i2c_register(fd, 0x34, 0x40, 0x40);//OVERRIDE_DEEP_COLOR_MODE
 * set_i2c_register(fd, 0x34, 0x5A, 0x04);//DCFIFO_RECENTER
 */
	sdp_brightness_set( fd );
	DEBUG( "HDMI_COMP_720px60_mod..." );
}


void HDMI_COMP_576ix50_mod( int fd )
{
	common_set( fd );
/* :6-1b Port A, 720p,1080i YPrPb 444 in, RGB 444 out, Through DAC (Fast Switching)VIC[4,5,19,20]: */


/*
 * 40 01 06 ; Prim_Mode =110b HDMI-GR
 * 40 02 F6 ; Auto input color space, Limited Range RGB Output
 * 40 03 42 ; 36 bit SDR 444 Mode 0
 * 40 05 28 ; AV Codes Off
 * 40 06 A6 ; Invert VS,HS pins
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x00, 0x01 );       /* Set VID_STD(IO Map 0x00[5:0])：	 000011=0x03 -> SD 2x1 625i--720 x 576i */
	set_i2c_register( fd, 0x20, 0x01, 0x15 );       /* Set VFREQ(IO Map 0x01[6:4])：		 000 -> 60 Hz */


	/*
	 *                                                                                                                              001 -> 50 Hz
	 *                                                                                                                              010 -> 30 Hz
	 *                                                                                                                              011 -> 25 Hz
	 *                                                                                                                              100 -> 24 Hz
	 * Set PRIM_MODE(IO Map 0x01[3:0]): 0000 -> SDP mode
	 *                                                                                                                              0001 -> Component Mode
	 *                                                                                                                              0010 -> Graphics mode
	 *                                                                                                                              0100 -> CVBS & HDMI AUDIO Mode
	 *                                                                                                                              0101 -> HDMI-Comp
	 *                                                                                                                              0110 -> HDMI-GR
	 */
	set_i2c_register( fd, 0x20, 0x02, 0xF4 );
	set_i2c_register( fd, 0x20, 0x03, 0x00 );       /* 8-bit ITU-656 SDR mode */
/* ****modified 20150820************************* // */
	set_i2c_register( fd, 0x20, 0x05, 0x2C );       /* AV Codes On */
/* ********************************************** // */
	set_i2c_register( fd, 0x20, 0x06, 0xa6 );


/*
 * 68 C1 FF ; HDMI power control (power saving)
 * 68 C2 FF ; HDMI power control (power saving)
 * 68 C3 FF ; HDMI power control (power saving)
 * 68 C4 FF ; HDMI power control (power saving)
 * 68 C5 00 ; HDMI power control (power saving)
 * 68 C6 00 ; HDMI power control (power saving)
 * 68 C0 FF ; HDMI power control (power saving)
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0xC1, 0xFF );
	set_i2c_register( fd, 0x34, 0xC2, 0xFF );
	set_i2c_register( fd, 0x34, 0xC3, 0xFF );
	set_i2c_register( fd, 0x34, 0xC4, 0xFF );
	set_i2c_register( fd, 0x34, 0xC5, 0x00 );
	set_i2c_register( fd, 0x34, 0xC6, 0x00 );
	set_i2c_register( fd, 0x34, 0xC0, 0xFF );


/*
 * 40 0C 40 ; Power up part and power down VDP
 * 40 15 80 ; Disable Tristate of Pins
 * 40 19 83 ; LLC DLL phase
 * 40 33 40 ; LLC DLL enable
 * ** slave address=0x40 *********************** //
 */
	set_i2c_register( fd, 0x20, 0x0C, 0x40 );
	set_i2c_register( fd, 0x20, 0x15, 0x80 );
	set_i2c_register( fd, 0x20, 0x19, 0x83 );
	set_i2c_register( fd, 0x20, 0x33, 0x40 );


/*
 * 44 BA 01 ; Set HDMI FreeRun
 * 44 3E 00 ; Disable CP Pregain Block
 * 44 6C 00 ; Use fixed clamp values
 * ** slave address=0x44 *********************** //
 */
	set_i2c_register( fd, 0x22, 0xBA, 0x01 );
/*	set_i2c_register(fd, 0x22, 0x3E, 0x80); */
	set_i2c_register( fd, 0x22, 0x6C, 0x00 );

/*	set_i2c_register(fd, 0x22, 0xbf, 0x03); */


/*
 * 4C 00 FF ; Power Down ADC's and there associated clocks
 * 4C 01 FE ; Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
 * 4C B5 01 ; Setting MCLK to 256Fs
 * ** slave address=0x4C *********************** //
 */
	set_i2c_register( fd, 0x26, 0x00, 0xFF );
	set_i2c_register( fd, 0x26, 0x01, 0xFE );
	set_i2c_register( fd, 0x26, 0xB5, 0x01 );


/*
 * 68 00 32 ; Set HDMI Input Port A (Enable BG monitoring)
 * 68 01 18 ; Enable clock terminators
 * 68 0D 34 ; ADI recommended write
 * 68 1A 8A ; Unmute audio
 * 68 3D 10 ; HDMI ADI recommended write
 * 68 44 85 ; TMDS PLL Optimization
 * 68 46 1F ; ADI Recommended Write ES3/Final silicon
 * 68 60 88 ; TMDS PLL Optimization
 * 68 61 88 ; TMDS PLL Optimization
 * 68 6C 18 ; Disable ISRC clearing bit, Improve robustness
 * 68 57 B6 ; TMDS PLL Optimization
 * 68 58 03 ; TMDS PLL Setting
 * 68 67 20 ; TMDS PLL Setting
 * 68 75 10 ; DDC drive strength
 * 68 85 1F ; ADI Equaliser Setting
 * 68 87 70 ; HDMI Recommended write
 * 68 89 04 ; ADI Equaliser Setting
 * 68 8A 1E ; ADI Equaliser Setting
 * 68 93 04 ; ADI Equaliser Setting
 * 68 94 1E ; ADI Equaliser Setting
 * 68 9D 02 ; ADI Equaliser Setting
 * 68 99 A1 ; HDMI ADI recommended write
 * 68 9B 09 ; HDMI ADI recommended write
 * 68 C9 01 ; HDMI free Run based on PRIM_MODE, VID _STD
 * ** slave address=0x68 *********************** //
 */
	set_i2c_register( fd, 0x34, 0x00, 0x32 );
	set_i2c_register( fd, 0x34, 0x01, 0x18 );
	set_i2c_register( fd, 0x34, 0x0D, 0x34 );
	set_i2c_register( fd, 0x34, 0x1A, 0x8A );
	set_i2c_register( fd, 0x34, 0x3D, 0x10 );
	set_i2c_register( fd, 0x34, 0x44, 0x85 );
	set_i2c_register( fd, 0x34, 0x46, 0x1F );
	set_i2c_register( fd, 0x34, 0x60, 0x88 );
	set_i2c_register( fd, 0x34, 0x61, 0x88 );
	set_i2c_register( fd, 0x34, 0x6C, 0x18 );
	set_i2c_register( fd, 0x34, 0x57, 0xB6 );
	set_i2c_register( fd, 0x34, 0x58, 0x03 );
	set_i2c_register( fd, 0x34, 0x67, 0x20 );
	set_i2c_register( fd, 0x34, 0x75, 0x10 );
	set_i2c_register( fd, 0x34, 0x85, 0x1F );
	set_i2c_register( fd, 0x34, 0x87, 0x70 );
	set_i2c_register( fd, 0x34, 0x89, 0x04 );
	set_i2c_register( fd, 0x34, 0x8A, 0x1E );
	set_i2c_register( fd, 0x34, 0x93, 0x04 );
	set_i2c_register( fd, 0x34, 0x94, 0x1E );
	set_i2c_register( fd, 0x34, 0x9D, 0x02 );
	set_i2c_register( fd, 0x34, 0x99, 0xA1 );
	set_i2c_register( fd, 0x34, 0x9B, 0x09 );
	set_i2c_register( fd, 0x34, 0xC9, 0x01 );

	set_i2c_register( fd, 0x34, 0x41, 0x10 );

	set_i2c_register( fd, 0x34, 0x48, 0x80 );/* PWRDN1 has no effect */


/*
 * set_i2c_register(fd, 0x34, 0x40, 0x40);//OVERRIDE_DEEP_COLOR_MODE
 * set_i2c_register(fd, 0x34, 0x5A, 0x04);//DCFIFO_RECENTER
 */
	sdp_brightness_set( fd );
	DEBUG( "HDMI_COMP_576ix50_mod..." );
}


