include ../config.mk

.PHONY: clean all tools amigasupport

all: tools

tools: amigasupport
	cd tools ; make all

amigasupport:
	cd amigasupport ; make all

clean:
	cd tools ; make clean
	cd amigasupport ; make clean
