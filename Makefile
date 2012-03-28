CC = gcc

P1_SOURCE_FILE = practica1/practica1.c
P1_OBJ_DIR = practica1/bin
P1_EXECUTABLE = mask_sim
 
P2_SOURCE_FILE = practica2/practica2.c
P2_OBJ_DIR = practica2/bin
P2_EXECUTABLE = creasin

P3_OBJ_DIR = practica3/bin
P3_CLIENT_EXECUTABLE = cliente
P3_CLIENT_SOURCE_FILE = practica3/cliente.c
P3_SERVER_EXECUTABLE = servidor
P3_SERVER_SOURCE_FILE = practica3/servidor.c

all: alumno p1 p2 p3

alumno:
	cat alumno.txt
p1: practica1/practica1.c
	mkdir -p $(P1_OBJ_DIR)
	$(CC) -o $(P1_OBJ_DIR)/$(P1_EXECUTABLE) $(P1_SOURCE_FILE)
p2: practica2/practica2.c
	mkdir -p $(P2_OBJ_DIR)
	$(CC) -o $(P2_OBJ_DIR)/$(P2_EXECUTABLE) $(P2_SOURCE_FILE)
p3: practica3/comun.h practica3/servidor.c practica3/cliente.c
	mkdir -p $(P3_OBJ_DIR)
	$(CC) -o $(P3_OBJ_DIR)/$(P3_CLIENT_EXECUTABLE) $(P3_CLIENT_SOURCE_FILE)
	$(CC) -o $(P3_OBJ_DIR)/$(P3_SERVER_EXECUTABLE) $(P3_SERVER_SOURCE_FILE)
