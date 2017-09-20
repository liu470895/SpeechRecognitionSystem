TOPDIR:=$(shell pwd)/
CC:=gcc
export TOPDIR CC

TARGET:=sperec.elf

CPPFLAGS:=-I ../../inc/
CFLAGS:=-Wl,-rpath=../../lib/
LDFLAGS:=-L ../../lib/
LDFLAGS+=-lmsc

exclude_dirs:=
include $(TOPDIR)Makefile.env

