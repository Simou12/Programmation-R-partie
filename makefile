all: server client  client.o server.o

client: client.o	
	gcc -o client client.o  -I ./include
	
client.o: client.c
	gcc -c client.c -o client.o -I ./include

server: server.o  
	gcc -o server server.o  -I ./include

server.o: server.c
	gcc -c server.c -o server.o -I ./include
rm:
	rm *.o
	rm server
	rm client 
