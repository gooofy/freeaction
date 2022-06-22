include config.mk

.PHONY: clean all src directories

all: directories src

src:
	cd src ; make all

directories:
	mkdir -p target/m68k-amigaos/bin
	mkdir -p target/m68k-amigaos/obj

clean:
	cd src ; make clean
