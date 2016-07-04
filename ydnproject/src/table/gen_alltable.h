#ifndef  __GEN_TABLE__H

#define __GEN_TABLE__H
#include <stdbool.h>
#include <reset-oparetion.h>


/* pat */
typedef struct {
	uint32_t max_num_program;

	uint16_t	ts_id;
	uint16_t	program_number;
	uint16_t	pat_pid;

	uint8_t pat_version;

	bool pat_current_next;
}usr_cfg_pat_t;

/* pmt */
typedef struct {
	uint16_t	program_number;
	uint16_t	video_pid;
	uint16_t	audio_pid[2];
	uint16_t	pcr_pid;

	uint8_t pmt_version;
	uint8_t video_type;
	uint8_t audio_type;

	bool pmt_current_next;
}usr_cfg_pmt_t;
/* sdt */
typedef struct {
	uint8_t sdt_table_id;
	uint8_t sdt_running_status;
	uint8_t sdt_descriptor_tag;
	uint8_t sdt_descriptor_length;
	uint8_t *service_data;
	uint8_t sdt_version;

	uint16_t	sdt_extension;
	uint16_t	network_id;
	uint16_t	program_number;

	bool	sdt_current_next;
	bool	sdt_eit_schedule;
	bool	sdt_eit_present;
	bool	sdt_free_ca;
}usr_cfg_sdt_t;
/* nit */
typedef struct {
	uint16_t	nit_extension;
	uint16_t	network_id;
	uint16_t	ts_id;

	uint8_t		nit_table_id;
	uint8_t		network_name_tag;
	uint8_t		network_name_length;
	uint8_t		*network_name;
	uint8_t		nit_version;
	uint16_t	program_ifo_lcn;
	bool		nit_current_next;
}usr_cfg_nit_t;

/* eit */
typedef struct {
	uint8_t		eit_now_startyear[9];
	uint8_t		eit_now_starttime[7];
	uint8_t		eit_now_duration[7];
	uint8_t		eit_now_eventname[17];
	uint8_t		eit_next_startyear[9];
	uint8_t		eit_next_starttime[7];
	uint8_t		eit_next_duration[7];
	uint8_t		eit_next_eventname[17];
	uint8_t		nit_private_data[9];
	uint16_t	program_number;
	uint16_t	network_id;
	uint16_t	program_ifo_lcn;
}usr_cfg_eit_t;


typedef struct {
	uint8_t		dvb_t_descriptor_tag;
	uint8_t		dvb_t_descriptor_length;
	uint32_t	centre_frequency;       /* 500MHz */
	uint8_t		bandwidth;              /* 8MHz 3bit */
	uint8_t		priority;               /* 1bit */
	uint8_t		Time_Slicing_indicator; /* 1bit */
	uint8_t		MPE_FEC_indicator;      /* 1bit */
	uint8_t		reserved_future_use1;   /* 2bit */
	uint8_t		constellation;          /* 64QAM 2bit */
	uint8_t		hierarchy_information;  /* NO 3bit */
	uint8_t		code_rat_HP_stream;     /* 7/8 3bit */
	uint8_t		code_rat_LP_stream;
	uint8_t		guard_interval;
	uint8_t		transmission_mode;
	uint8_t		other_frequency_flag;
	uint32_t	reserved_future_use2;
	uint8_t		dvb_t_descriptor[11];
}usr_cfg_dvb_t;

typedef struct {
	uint8_t user_defined_descriptor_tag;
	uint8_t user_defined_descriptor_length;
	uint8_t user_defined_descriptor[10];/* 4 */
}usr_def_des_t;

void set_pat_table( uint16_t program_number, uint16_t pat_pid, uint8_t pat_version, uint16_t ts_id );


void set_usr_des( uint8_t * usedescriptoe, uint8_t deslength,uint8_t program_number,uint16_t lcn_mode );


void set_nit_table( uint16_t network_id, uint8_t *network_name,
		    uint8_t network_name_length, uint8_t nit_table_id,
		    uint8_t nit_version, uint16_t ts_id );


void set_sdt_table( uint16_t network_id, uint16_t program_number,
		    uint8_t sdt_descriptor_length, uint8_t sdt_table_id,
		    uint8_t sdt_version, uint8_t   *service_data );


void set_pmt_table( uint16_t audio_pid, uint16_t video_pid,
		    uint16_t pcr_pid, uint16_t program_number, uint8_t pmt_version, uint16_t pmt_pid );


void set_dvb_des( uint32_t centre_frequency, uint8_t bandwidth,
		  uint8_t constellation, uint8_t code_rat_HP_stream,
		  uint8_t code_rat_LP_stream, uint8_t guard_interval,
		  uint8_t transmission_mode );


void set_eit_table( usr_cfg_eit_t *cfg_eit_t );


#endif


