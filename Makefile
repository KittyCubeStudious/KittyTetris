.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "PSL1GHT not set")
endif

include $(PSL1GHT)/ppu_rules

TARGET		:=	EBOOT
TITLE		:=	KITTY TETRIS
APPID		:=	KITTY0001
BUILD		:=	build

SOURCE		:=	.
INCLUDE		:=	.

CFLAGS		:=	-O2 -Wall
CXXFLAGS	:=	-O2 -Wall
LDFLAGS		:=	

# PSL1GHT açık kaynak kodlu olduğu için, grafik ve sistem kütüphanelerini 
# Sony gibi gizli değil, açıkça yazmamız gerekiyor:
LIBS		:=	-ltiny3d -lfont -lpngdec -lz -lm -lrsx -lgcm_cmd -lgcm_sys -lio -lsysmodule -lsysutil -lrt -llv2

include $(PSL1GHT)/Makefile.base
