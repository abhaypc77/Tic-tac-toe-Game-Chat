all: build

clean:
	rm xo_server
	rm xo_client
	rm xo_client.o
	rm xo_server.o	
	rm game_xo_3.o
	rm game_xo_4.o
	rm xo_debug.o

build: xo_server xo_client

xo_server: xo_server.o xo_debug.o xo_server_ssl.o
	gcc -o xo_server xo_server.o xo_debug.o xo_server_ssl.o -lssl -lcrypto

xo_server.o: xo_server.c xo_server.h
	gcc -c xo_server.c -o xo_server.o

xo_client: xo_client.o game_xo_3.o game_xo_4.o xo_debug.o xo_client_ssl.o 
	gcc -o xo_client xo_client.o game_xo_3.o game_xo_4.o xo_debug.o xo_client_ssl.o -lssl -lcrypto

xo_client.o: xo_client.c xo_client.h
	gcc -c xo_client.c -o xo_client.o

game_xo_3.o: game_xo_3.c game_xo_3.h
	gcc -c game_xo_3.c -o game_xo_3.o

game_xo_4.o: game_xo_4.c game_xo_4.h
	gcc -c game_xo_4.c -o game_xo_4.o

xo_debug.o: xo_debug.c xo_debug.h
	gcc -c xo_debug.c -o xo_debug.o

xo_server_ssl.o: xo_server_ssl.c xo_server_ssl.h
	gcc -c xo_server_ssl.c -o xo_server_ssl.o

xo_client_ssl.o: xo_client_ssl.c xo_client_ssl.h
	gcc -c xo_client_ssl.c -o xo_client_ssl.o
