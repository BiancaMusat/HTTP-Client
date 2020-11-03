CC=gcc
CFLAGS=-I -l json

client: client.c requests.c helpers.c
		$(CC) -o client client.c requests.c helpers.c -Wall -l json
		
run: client
		./client

clean:
		rm -f *.o client
