TOPDIR:=../../

#CROSS:=arm-linux-gnueabi-
CC:=$(CROSS)gcc

export TOPDIR CC

TARGET:=alsa_record.elf

CPPFLAGS:=-I $(TOPDIR)inc 
LDFLAGS:=-L $(TOPDIR)lib
#LDFLAGS+=-lasound

exclude_dirs:=image inc lib bin share
include $(TOPDIR)Makefile.env

