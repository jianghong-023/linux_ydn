CFLAGS = -Wall -g -c 

SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(SUBDIRS) $(CUR_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	$(CC) $(CFLAGS) $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ 
ECHO:
	@echo $(SUBDIRS)