
CSTD = -std=gnu11
SERVER_FILENAME = motorcontrol
CLIENT_FILENAME = client_mc
SRC_DIR = src
BIN_DIR = bin

CMD_MKDIR = mkdir -p

default: debug

server_debug: mk_bin $(SRC_DIR)/motorcontrol.c $(SRC_DIR)/mcc.h $(SRC_DIR)/defaults.h
	gcc $(CSTD) -lwiringPi -o $(BIN_DIR)/$(SERVER_FILENAME) $(SRC_DIR)/$(SERVER_FILENAME).c

server_release: mk_bin $(SRC_DIR)/motorcontrol.c $(SRC_DIR)/mcc.h $(SRC_DIR)/defaults.h
	gcc $(CSTD) -O3 -lwiringPi -o $(BIN_DIR)/$(SERVER_FILENAME) $(SRC_DIR)/$(SERVER_FILENAME).c

server: server_debug

client_debug: mk_bin $(SRC_DIR)/client_mc.c $(SRC_DIR)/defaults.h
	gcc $(CSTD) -lSDL2 -lSDL2_ttf -o $(BIN_DIR)/$(CLIENT_FILENAME) $(SRC_DIR)/$(CLIENT_FILENAME).c

client_release: mk_bin $(SRC_DIR)/client_mc.c $(SRC_DIR)/defaults.h
	gcc $(CSTD) -O3 -lSDL2 -lSDL2_ttf -o $(BIN_DIR)/$(CLIENT_FILENAME) $(SRC_DIR)/$(CLIENT_FILENAME).c

client: client_debug

debug: server_debug client_debug
release: server_release client_release

mk_bin:
	$(CMD_MKDIR) $(BIN_DIR)
