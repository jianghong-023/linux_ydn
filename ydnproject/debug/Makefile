OBJS=*.o
ODIR=obj

PTHREAD =  -lrt -lpthread  -lm

$(ROOT_DIR)/$(BIN_DIR)/$(BIN):$(ODIR)/$(OBJS)
	$(CC) -o $@ $^ $(PTHREAD) 
	
	@cp $(ROOT_DIR)/$(BIN_DIR)/$(BIN) ../../../