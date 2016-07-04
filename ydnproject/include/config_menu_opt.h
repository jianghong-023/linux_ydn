#ifndef __CONFIG_MENU_ARR__H
#define __CONFIG_MENU_ARR__H

#include <string.h>
#include "init-configuration.h"


typedef struct {
	char		*first_menu;
	menu_cfg_t	*cfg_t;
	int		lenth;
}menu_opt_t;

menu_opt_t *p_menu_opt_arr( void );


int index_cfg_menu( const char *firstitemmenu, const char *cfgmenu );


#endif