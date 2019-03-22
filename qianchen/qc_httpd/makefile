# Makefile Written by ticktick

# Show how to cross-compile c/c++ code for android platform

.PHONY: clean
#-------------------------------------------------------

_ROOT_DIR_ = $(ROOT_DIR)
_TOOL_CHAIN_ = $(TOOL_CHAIN)
_TOOL_ARCH_ = $(TOOL_ARCH)
_MODEL_TARGET_ = $(MODEL_TARGET)

_TARGET_NAME_ = $(BIN_NAME)
#-------------------------------------------------------
_STAGING_DIR_ = $(_ROOT_DIR_)/staging_dir
_TARGET_DIR_ = $(_STAGING_DIR_)/$(_MODEL_TARGET_)
_TOOL_CHAIN_DIR_ = $(_STAGING_DIR_)/$(_TOOL_CHAIN_)
export STAGING_DIR=$(_TOOL_CHAIN_DIR_):$STAGING_DIR

CC_DIR = $(_TOOL_CHAIN_DIR_)/bin
CROSS_COMPILE = $(CC_DIR)/$(_TOOL_ARCH_)

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
STRIP = $(CROSS_COMPILE)strip
WALL = -Wall

CUR_DIR = $(shell pwd)

INCLUDE = -I$(CUR_DIR)/lib/include\
		-I$(_TOOL_CHAIN_DIR_)/include\
		-I$(_TARGET_DIR_)/usr/include\
		-I$(_TARGET_DIR_)/usr/include/event2\
		-I$(_TARGET_DIR_)/usr/include/json-c

LIB_INC = -L$(_TOOL_CHAIN_DIR_)/lib\
		-L$(_TARGET_DIR_)/usr/lib -lcrypto\
		-L$(_TARGET_DIR_)/usr/lib -levent\
		-L$(_TARGET_DIR_)/usr/lib -ljson-c

CFLAGS = -D_GNU_SOURCE
COMPILE_TARGET_DIR = $(CUR_DIR)/hex/
TARGET_NAME = $(_TARGET_NAME_)
TARGET = $(COMPILE_TARGET_DIR)$(TARGET_NAME)

SRC_DIR = $(CUR_DIR)/src
OBJ_DIR = $(CUR_DIR)/obj

#获取当前目录下所有的.c文件
SOURCE = $(wildcard $(SRC_DIR)/*.c)

#得到相应的.o文件,所有的.c文件替换为.o文件
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SOURCE)))

#-gstabs  $(LIB_INC) -ggdb3 -lssl -lcrypto -ldl 
$(OBJ_DIR)/%.o:$(SRC_DIR)/%.c
	@echo "CC $<"
	@$(CC) -ggdb $(WALL) $(INCLUDE) $(LIB_INC) $< -c -o $@ -lpthread $(CFLAGS)

#生成目标的依赖
$(TARGET): $(OBJ) $(OBJ_CPP)
	@echo "--------------create $(TARGET)---------"
	@$(CC) $(WALL) $(LIB_INC) -o $@ $^ -lpthread $(CFLAGS)
	@$(STRIP) $(TARGET)
	ls -alh $(TARGET)
clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(TARGET)