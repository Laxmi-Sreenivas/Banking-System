system : server client
	
server :
	gcc server.c database.c -o server

client :
	gcc client.c -o client

clean :
	rm server client