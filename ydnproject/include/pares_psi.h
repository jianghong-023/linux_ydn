#ifndef __PARSE_PSI__H_

#define __PARSE_PSI__H_

struct parse_ts_id{
	uint16_t	i_number;                       /*!< program_number */
	uint16_t	i_pmt_pid;  
	uint16_t	i_video_pid;  
	uint16_t	i_audieo_pid; 
	uint16_t 	i_pcr_pid;
	uint16_t	i_parse_status;
};

int pare( void );
int pat_parse_enter(char *file);
int pmt_parse_enter(char *file,uint16_t i_program_number, uint16_t i_pmt_pid);

#endif
