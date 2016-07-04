CC=arm-linux-gnueabihf-gcc
CFLAGS = -Wall -c -O2 -D_FILE_OFFSET_BITS=64 
#PTHREAD = -lpthread
# 过滤debug目录
SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug" && $$9 != "doc") print $$9}')
ROOT_DIR=$(shell pwd)
BIN=yadproject
OBJS_DIR=debug/obj
BIN_DIR=debug/bin

CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}

export CC BIN OBJS_DIR BIN_DIR ROOT_DIR
#注意这里的顺序，需要先执行SUBDIRS最后才能是DEBUG
all:$(SUBDIRS) $(CUR_OBJS) DEBUG
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
	make -C debug
ECHO:
	@echo $(SUBDIRS)
#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR
$(CUR_OBJS):%.o:%.c
	$(CC) $(CFLAGS) $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ 
clean:
	@rm $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)/*