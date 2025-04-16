CC=gcc
CFLAGS=-Wall -Wextra
INSTALL_DIR=/usr/local/bin

all: mysys

mysys: src/SystemInfo.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$@ $<

install: all
	sudo cp bin/mysys $(INSTALL_DIR)
	sudo chmod +x $(INSTALL_DIR)/mysys

clean:
	rm -f bin/mysys
	rm -f /usr/local/bin/mysys

.PHONY: all install clean