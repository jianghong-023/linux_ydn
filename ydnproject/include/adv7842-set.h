#ifndef __ADV7842_SET__H
#define __ADV7842_SET__H



void HDMI_COMP_576ix50_mod( int fd );


void HDMI_COMP_720px60_mod( int fd );


void HDMI_COMP_720px50_mod( int fd );


void HDMI_COMP_1080px50_mod( int fd );


void HDMI_COMP_1080px60_mod( int fd );


void HDMI_COMP_1080ix50_mod( int fd );


void HDMI_COMP_1080ix60_mod( int fd );


void CP_YPbPr_576px50_mod( int fd );


void CP_YPbPr_576ix50_mod( int fd );


void SD_YPbPr_576i_480ix50_mod( int fd );


void YPbPr_1080ix50_mod( int fd );


void YPbPr_1080px50_mod( int fd );


void YPbPr_1080ix60_mod( int fd );


void YPbPr_720px60_mod( int fd );


void YPbPr_720px50_mod( int fd );


void HDMI_GR_480px60_mod( int fd );


void CVBS_576ix50_mod( int fd );


void internel_EDID_mod( int fd );


void common_set( int fd );


void CVBS_480ix60_mod( int fd );


void sdp_hue_set( int fd );

void sdp_brightness_set( int fd );



/*  */
extern struct adv_782_rg *adv7842_read_rg( int fd );


#endif
