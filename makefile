#SO - Projeto1 - Grupo SO-052
#Trabalho realizado pelos seguintes alunos:
#fc51051 - Ivo Estrela
#fc55180 - Ricardo Rocha
#fc57528 - Tomás Salgueiro

#DIRECTORIES
OBJ_DIR = obj
BIN_DIR = bin

#OBJECTS variable save all objects to compile
OBJECTS = main.o client.o driver.o memory.o process.o restaurant.o utils.o memory-private.o synchronization.o configuration.o log.o metime.o mesignal.o stats.o
CC = gcc

main.o =
client.o =
driver.o =
memory.o =
process.o =
restaurant.o =
utils.o =
memory-private.o =
synchronization.o =
configuration.o =
log.o =
metime.o =
mesignal.o =
stats.o =

magnaeats: $(OBJECTS)
	$(CC) $(addprefix $(OBJ_DIR)/,$(OBJECTS)) -lm -lrt -lpthread -o $(BIN_DIR)/$@

%.o: src/%.c $($@)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
	$(CC) -I include -o $(OBJ_DIR)/$@ -c $<

clean:
	rm -f *.o out
	rm -f out