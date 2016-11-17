
include Makefile.def

SRC_DIR = src
INC_DIR = inc

.PHONY: clean all

all : clean
	$(ECHO) "========= Starting to make modules in $(SRC_DIR) ========="
	cd $(SRC_DIR); $(MAKE) $(MFLAGS) all

clean :
	$(ECHO) "========= Starting to clean modules in $(SRC_DIR) ========="
	cd $(SRC_DIR); $(MAKE) $(MFLAGS) clean
