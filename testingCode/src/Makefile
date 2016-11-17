##############################################################################
## Description: src/ Makefile                                           	##
##############################################################################

include ../Makefile.def

TOP_DIR	= ..

BUILD_DIR := $(TOP_DIR)/build

INC_DIR	= -I$(TOP_DIR)/inc         \
          -I$(TOP_DIR)/inc/mtd     \
          -I$(TOP_DIR)/inc/linux   \
          -I$(TOP_DIR)/src/        \
          -I$(TOP_DIR)/src/queue   \
          -I$(TOP_DIR)/src/rtc     \
          -I$(TOP_DIR)/src/battery \
          -I$(TOP_DIR)/src/audio   \
          -I$(TOP_DIR)/src/gsensor \
          -I$(TOP_DIR)/src/sFlash  \
          -I$(TOP_DIR)/src/sdram   \
          -I$(TOP_DIR)/src/eMMC    \
          -I$(TOP_DIR)/src/ccm     \
          -I$(TOP_DIR)/src/usb     \
          -I$(TOP_DIR)/src/tpanel  \
          -I$(TOP_DIR)/src/pmic    \
          -I$(TOP_DIR)/src/can



LIB_DIR = $(TOP_DIR)/lib

MAIN_SRC = main.c

SYS_SRC = system.c

TARGET = testingCode

OBJ = obj

########## Library ##########
SHARE_LIB = $(LIB_DIR)/*.a

########## Module ##########

QUEUE = queue

RTC = rtc

BATTERY = battery

AUDIO = audio

GSENSOR = gsensor

SFLASH = sFlash

SDRAM = sdram

EMMC = eMMC

CCM = ccm

USB = usb

TPANEL = tpanel

PMIC = pmic

CAN = can

########## Module OBJ ##########

MAIN_OBJ = main.o

SYS_OBJ = system.o

QUEUE_OBJ = $(QUEUE)/$(OBJ)/*.o

RTC_OBJ = $(RTC)/$(OBJ)/*.o

BATTERY_OBJ = $(BATTERY)/$(OBJ)/*.o

AUDIO_OBJ = $(AUDIO)/$(OBJ)/*.o

GSENSOR_OBJ = $(GSENSOR)/$(OBJ)/*.o

SFLASH_OBJ = $(SFLASH)/$(OBJ)/*.o

SDRAM_OBJ = $(SDRAM)/$(OBJ)/*.o

EMMC_OBJ = $(EMMC)/$(OBJ)/*.o

CCM_OBJ = $(CCM)/$(OBJ)/*.o

USB_OBJ = $(USB)/$(OBJ)/*.o

TPANEL_OBJ = $(TPANEL)/$(OBJ)/*.o

PMIC_OBJ = $(PMIC)/$(OBJ)/*.o

CAN_OBJ = $(CAN)/$(OBJ)/*.o

########## Module Collection ##########

SUB_DIRS = $(QUEUE) $(RTC) $(BATTERY) $(AUDIO) $(GSENSOR) $(SFLASH) $(SDRAM) $(EMMC) $(USB) $(TPANEL) $(PMIC) $(CAN)

SUB_OBJ_DIRS = $(MAIN_OBJ) $(SYS_OBJ) $(QUEUE_OBJ) $(RTC_OBJ) $(BATTERY_OBJ) $(AUDIO_OBJ) $(GSENSOR_OBJ) $(SFLASH_OBJ) $(SDRAM_OBJ) $(EMMC_OBJ) $(USB_OBJ) $(TPANEL_OBJ) $(PMIC_OBJ) $(CAN_OBJ)

CFLAGS	= -Wall -O2 -g -I.. $(INC_DIR) -D_REENTRANT -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed

LIBS = -lpthread  -lrt -lm -lc

.PHONY: clean all

all : $(MAIN_OBJ) $(SYS_OBJ)
	# Comipile
	@for i in $(SUB_DIRS);                                      \
	do                                                          \
	  (cd $$i; $(MAKE) $(MFLAGS) all);                          \
	done
	$(ECHO) "========= Compile Complete!! ========="

	# Link
	#$(CC) -o $(TARGET) $(SUB_OBJ_DIRS) $(LIBS) $(SHARE_LIB)
	$(CC) -o $(TARGET) $(SUB_OBJ_DIRS) $(LIBS)
	$(ECHO) "========= Link Complete!! =========";

	# Generate Target
	#cp -f $(TARGET) ../
	mv $(TARGET) $(BUILD_DIR)
	$(ECHO) "========= Generate $(TARGET) Complete!! =========";


$(MAIN_OBJ): $(MAIN_SRC)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(SYS_OBJ): $(SYS_SRC)
	@$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -f $(BUILD_DIR)/*

	rm -f *.o

	@for i in $(SUB_DIRS);                                      \
	do                                                          \
	  (cd $$i; $(MAKE) $(MFLAGS) clean);                        \
	done
	$(ECHO) "========= Clear Complete!! ========="

hk:
	echo $(MTD_LIB)
