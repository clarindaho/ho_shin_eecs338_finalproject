all:
	gcc -o battleship_server battleship_server.c
	gcc -o battleship_client battleship_client.c
	
battleship_server:
	gcc -o battleship_server battleship_server.c

battleship_client:
	gcc -o battleship_client battleship_client.c
	
clean:
	rm battleship_server
	rm battleship_client