CC = gcc
CFLAGS = -Wall 

all: server client

server: server.c
	@echo "compiler serveur"
	$(CC) server.c -lpthread -o server $(CFLAGS) 

client: client.c
	@echo "compilet client"
	$(CC) client.c -lpthread -o client $(CFLAGS) 

run: server client
	@echo "lancer tout"
	./server &
	sleep 2 # Laisser le temps au serveur de démarrer
	./client &
	./client &
	./client

clean:
	@echo "nettoyage"
	rm -f server client
