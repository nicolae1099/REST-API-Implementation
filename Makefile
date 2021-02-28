CC=g++
CFLAGS = -std=c++11 -g -Werror -Wall 

client: client.cpp requests.cpp helpers.cpp buffer.c
		$(CC) -o client -g client.cpp requests.cpp helpers.cpp buffer.c
		
run: client
		./client

clean:
		rm -f *.o client