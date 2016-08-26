#ifndef __PARSE_PSI__H_

#define __PARSE_PSI__H_
#if 0
typedef struct pmt_stream{
	uint8_t stream_type;
	uint16_t elementary_pid;
	uint16_t es_info_length;
	unsigned descriptor;
}pmt_stream;

typedef struct ts_pmt_t{
	uint8_t table_id;
	uint16_t section_syntax_indicator:1;
	uint16_t zero	:1;
	uint16_t reserved_1 :2;
	uint16_t section_length:12;
	uint16_t program_number;
	uint8_t reserved_2 :2;
	uint8_t version_number:5;
	uint8_t current_next_indicator:1;
	uint8_t section_number;
	uint8_t last_section_number;
	uint16_t reserved_3 :3;
	uint16_t PCR_PID :13;
	uint16_t reserved_4 :4;
	uint16_t program_info_length:12;
	pmt_stream pmt_stream_arr;
	uint8_t reserved_5 :3;
	uint8_t reserved_6:4;
	uint32_t crc_32;

}ts_pmt;
#endif
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
