all: server client

server: HTTP_Server.c
	g++ -o server HTTP_Server.c

client: HTTP_Client.c
	g++ -o client HTTP_Client.c

clean:
	rm -f server client
