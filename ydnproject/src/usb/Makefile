CFLAGS =  -Wall -g -c -O2 -D_FILE_OFFSET_BITS=64 -I../../include/
#-DMEMWATCH -DMW_STDIO
PTHREAD =  -lpthread  
SUBDIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

all:$(SUBDIRS) $(CUR_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	$(CC) $(CFLAGS) $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ $(PTHREAD)
ECHO:
	@echo $(SUBDIRS)