#/**----------------------------------------------------------------------------
# Tecnequip Tecnologia em Equipamentos Ltda                                 
#-------------------------------------------------------------------------------
# @author Alessandro Holanda
# @version 1.0
# @begin 06.02.2009
# @brief Script de linkagem dos modulos MiniLPC
#-----------------------------------------------------------------------------*/

#------------------------------------------------------------------------------
# Definições
#------------------------------------------------------------------------------

GNU_CC=arm-elf
GCC_PREFIX=$(GNU_CC)-
GCC_VERSION = 4.3.3

#------------------------------------------------------------------------------
# Diretorios de trabalho
#------------------------------------------------------------------------------

GCC_DIR=/usr/local/$(GNU_CC)
#GCC_DIR=/usr/local
GCC_INC = $(GCC_DIR)/include

HOME_DIR=.

SRC_DIR=$(HOME_DIR)
ifndef LIB_DIR
LIB_DIR=$(HOME_DIR)/../../
endif
APP_DIR=$(HOME_DIR)

LIB_BIN=$(LIB_DIR)/bin
LIB_INC=$(LIB_DIR)/inc
LIB_DOC=$(LIB_DIR)/doc

#------------------------------------------------------------------------------
# Ferramentas ARM ELF
#------------------------------------------------------------------------------

CC=$(GCC_DIR)/bin/$(GCC_PREFIX)gcc
AS=$(GCC_DIR)/bin/$(GCC_PREFIX)as
LD=$(GCC_DIR)/bin/$(GCC_PREFIX)ld
AR=$(GCC_DIR)/bin/$(GCC_PREFIX)ar
OBJSIZE=$(GCC_DIR)/bin/$(GCC_PREFIX)size
OBJCOPY=$(GCC_DIR)/bin/$(GCC_PREFIX)objcopy

#CC=/usr/local/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-gcc
#AS=/usr/local/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-as
#LD=/usr/local/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-ld
#AR=/usr/local/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-ar
#OBJSIZE=/usr/local/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-size
#OBJCOPY=/usr/local/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-objcopy

#---------------------------------------------------------------------------
# Bibliotecas e includes
#------------------------------------------------------------------------------

LIB_GCC = $(GCC_DIR)/lib/gcc/$(GNU_CC)/$(GCC_VERSION)/libgcc.a
LIB_C   = $(GCC_DIR)/$(GNU_CC)/lib/libc.a
LIB_M   = $(GCC_DIR)/$(GNU_CC)/lib/libm.a
LIB_MINILPC = $(LIB_BIN)/libARM7.a

#LIB_GCC = /usr/local/arm-none-linux-gnueabi/lib/gcc/arm-none-linux-gnueabi/4.1.2/libgcc.a
#LIB_C   = /usr/local/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/usr/lib/libc.a
#LIB_M   = /usr/local/arm-none-linux-gnueabi/arm-none-linux-gnueabi/sysroot/usr/lib/libm.a
#LIB_MINILPC	= $(LIB_BIN)/MiniLPC.a

#------------------------------------------------------------------------------
# Flags de compilação
#------------------------------------------------------------------------------
# -mcpu=arm7tdmi-s
# -msoft-float
#  -Wall
#  -fpack-struct=1  Alinha as estruturas em 1 byte. 
#  -nodefaultlibs -nostdlib -nostartfiles 
#  -fomit-frame-pointer 
#  -Wstrict-prototypes 
# -Wno-trigraphs 
# -fno-strict-aliasing 
# -fno-common 
# -fstack-protector "no memory region specified for loadable section `__libc_freeres_fn'"
# -fomit-frame-pointer -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Wdeclaration-after-statement -Wno-pointer-sign 
CFLAGS0=-mcpu=arm7tdmi-s -static -fpack-struct=1 -msoft-float -Wall $(GCC_MODE_FLAG) -I $(GCC_INC) -I $(LIB_INC) -I . -DCONFIG_CPU_lpc21xx
CFLAGS1=$(CFLAGS0)

# -D
# --gstabs
# -mfpu=softfpa
AAFLAGS=-D -alh --gstabs -mfpu=softfpa -I $(CORE_INC) -I . 

# -N
# -v
# -T
LDFLAGS=-N -v

#------------------------------------------------------------------------------
# Diretorio de destino
#------------------------------------------------------------------------------

DIR_OBJ		= $(APP_DIR)
DIR_LST		= $(APP_DIR)
PROG_NAME	= IHM_MX27_GUI

#------------------------------------------------------------------------------
# Linker script files
#------------------------------------------------------------------------------

#LDSCRIPT_RAM   = $(LIB_DIR)/ram.ld
#LDSCRIPT_FLASH = $(LIB_DIR)/flash.ld
#LDSCRIPT_RAM   = $(SRC_DIR)/lpc2109.ld
#LDSCRIPT_FLASH = $(SRC_DIR)/lpc2109.ld

#------------------------------------------------------------------------------
# Objects to link
#------------------------------------------------------------------------------

#OBJ_START	= $(LIB_BIN)/dev/lpc21xx.o
OBJ_FILES   = $(patsubst %.c,%.o,$(wildcard *.c))
OBJ_APP     = $(addprefix $(DIR_OBJ)/, $(OBJ_FILES))

#------------------------------------------------------------------------------
# Libraries to link to. LIB_GCC must be the last
#------------------------------------------------------------------------------

#LIB_LIST		= $(LIB_MINILPC) $(LIB_C) $(LIB_M) $(LIB_GCC)

#------------------------------------------------------------------------------
# Implicit rule to compile all C files in the directory
#------------------------------------------------------------------------------

$(DIR_OBJ)/%.o : %.c 
	$(CC) $(CFLAGS1) -c $< -o $@

#------------------------------------------------------------------------------
# Implicit rule to assemble all ASM files in the directory
#------------------------------------------------------------------------------

$(DIR_OBJ)/%.o : %.s
	$(AS) $(AAFLAGS) $< -o $@

#------------------------------------------------------------------------------
# default target: Generates application WITHOUT optimization
#------------------------------------------------------------------------------
#$(CC) $(CFLAGS1) -Wa,-a,-ad -g -c main.c > main.lst
$(DIR_OBJ)/$(PROG_NAME).elf:  makefile $(OBJ_APP)
	$(LD) -Map $(DIR_LST)/$(PROG_NAME).map $(LDFLAGS)\
                                         $(OBJ_APP) $(OBJ_START) $(LIB_LIST)\
                                         -o $(DIR_OBJ)/$(PROG_NAME).elf 
	$(OBJSIZE) $(DIR_LST)/$(PROG_NAME).elf
	$(OBJCOPY) -O binary $(DIR_LST)/$(PROG_NAME).elf $(DIR_LST)/$(PROG_NAME).bin
	$(OBJCOPY) -O ihex $(DIR_LST)/$(PROG_NAME).elf $(DIR_LST)/$(PROG_NAME).hex

#------------------------------------------------------------------------------
# release target: Generates application with optimization
#------------------------------------------------------------------------------

release: CFLAGS1 = -O2 $(CFLAGS0)
release: $(DIR_OBJ)/$(PROG_NAME).elf

#------------------------------------------------------------------------------
# debug target: Generates application WITHOUT optimization
#------------------------------------------------------------------------------

debug: CFLAGS1 = -O0 -save-temps -ggdb $(CFLAGS0)
debug: $(DIR_OBJ)/$(PROG_NAME).elf

#------------------------------------------------------------------------------
# clean target: Removes all generated files
#------------------------------------------------------------------------------

clean:
	rm -rf $(DIR_OBJ)/*.o $(DIR_OBJ)/*.elf \
         $(DIR_OBJ)/*.bin $(DIR_OBJ)/*.map $(DIR_OBJ)/*.hex $(DIR_OBJ)/*.s $(DIR_OBJ)/*.i

install:
	#cp -v $(DIR_OBJ)/*.bin /var/lib/tftpboot
	#cp -v $(DIR_OBJ)/*.elf /var/lib/tftpboot
	sudo cp -v $(DIR_OBJ)/*.bin $(DIR_OBJ)/*.hex $(DIR_OBJ)/*.elf /mnt/eletronica/windows/Fontes/ARM/NXP/MiniLPC_gcc/release
	
pendrive:
	cp -v $(DIR_OBJ)/*.bin /media/PENDRIVE8G

backup:
	sudo cp -av ../../MiniLPC/ /mnt/eletronica/windows/Fontes/ARM/NXP/MiniLPC_gcc/
