##############################################################################
## Description: src/eMMC Makefile                                        	##
##############################################################################

include ../../Makefile.def

SRC_DIR 	:= $(shell /bin/pwd)
OBJ_DIR		= obj
TOP_DIR		= ../../

MODULE_NAME	= eMMC

SRC = eMMC.c eMMCComm.c emmc04gxx.c

OBJ := $(patsubst %.c,$(OBJ_DIR)/%.o,$(patsubst %.cc,$(OBJ_DIR)/%.o,$(SRC)))

SRC_INC	= -I$(TOP_DIR)/inc \
		  -I$(TOP_DIR)/src/ \
		  -I$(TOP_DIR)/src/queue

CFLAGS	= -Wall -O2 -I.. $(SRC_INC) -D_REENTRANT $(VFP_FLAGS) $(DBG_FLAG) $(GDB_OPT)

all: dir $(OBJ)

$(OBJ_DIR)/%.o: %.c
	$(ECHO) $(MODULE_NAME): Processing $(subst $(SRC_DIR)/,,$<)
	@$(CC) $(CFLAGS) -c -o $@ $<

clean :
	$(ECHO)  $(MODULE_NAME): Cleaning objects and binary files
	rm -f $(OBJ_DIR)/*.o

dir :
	#$(ECHO) Check and Create obj Directory
	@mkdir obj -p
		  