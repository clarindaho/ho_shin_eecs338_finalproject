//
// EECS 338 - Final Project
// Clarinda Ho (cqh), Jason Shin (jjs270)
//

//
// library imports
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

//
// structs
//

typedef struct {
	int health;
	int x[5];
	int y[5];
} ship;

void INIT_AIRCRAFT_CARRIER(ship *aircraftCarrier){
	aircraftCarrier->health = 5;

	int i;
	for (i = 0; i < 5; i++) {
		aircraftCarrier->x[i] = -1;
		aircraftCarrier->y[i] = -1;
	}
}

void INIT_BATTLESHIP(ship *battleship){
	battleship->health = 4;
	
	int i;
	for (i = 0; i < 5; i++) {
		battleship->x[i] = -1;
		battleship->y[i] = -1;
	}
}

void INIT_SUBMARINE(ship *submarine){
	submarine->health = 3;
	
	int i;
	for (i = 0; i < 5; i++) {
		submarine->x[i] = -1;
		submarine->y[i] = -1;
	}
}

void INIT_CRUISER(ship *cruiser){
	cruiser->health = 3;

	int i;
	for (i = 0; i < 5; i++) {
		cruiser->x[i] = -1;
		cruiser->y[i] = -1;
	}
}

void INIT_DESTROYER(ship *destroyer){
	destroyer->health = 2;
	
	int i;
	for (i = 0; i < 5; i++) {
		destroyer->x[i] = -1;
		destroyer->y[i] = -1;
	}
}

//
// global variables and constants
//

int sockfd;			// client socket
char buffer[256];	// buffer

int map[20][20];
int mapExtent;

ship *listAircraftCarrier[2];
int numAircraftCarrier;

ship *listBattleship[3];
int numBattleship;

ship *listSubmarine[3];
int numSubmarine;

ship *listCruiser[3];
int numCruiser;

ship *listDestroyer[4];
int numDestroyer;

//
// method signatures
//

void connectToServer(char *IPAddress, char* portNum);
void closeSockets();

void setupFromServer();
int checkValidPos(int startNumPos, int startCharPos, int length, char *direction);
void setShipPos(ship *currentShip, int startNumPos, int startCharPos, char *direction);
void chooseShipPositions(int type);

//
// method definitions
//

// main method
int main(int argc, char *argv[]) {
	// Check for proper number of commandline arguments
	// Expect program name in argv[0], IP address in argv[1], and port # in argv[2]
	if (argc < 3) {
		fprintf(stderr, "Program Usage: %s hostname port\n", argv[0]);
		return 1;
	}
	
	connectToServer(argv[1], argv[2]);
	setupFromServer();
	
	int i;
	for (i = 0; i < 5; i++)
		chooseShipPositions(i);
	
	closeSockets();

	return 0;
}

// create and set up client socket
void connectToServer(char *IPAddress, char* portNum) {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "ERROR: could not open client socket to the server\n");
		exit(2);
	}

	struct hostent *server;
	server = gethostbyname(IPAddress);
	struct sockaddr_in serv_addr;
  	bzero((char *) &serv_addr, sizeof(serv_addr));

	int portno = atoi(portNum);
	serv_addr.sin_family = AF_INET;
  	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  	serv_addr.sin_port = htons(portno);

  	if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    		fprintf(stderr, "ERROR: could not bind client socket\n");
		exit(2);
	}
}

// close all sockets
void closeSockets() {
	close(sockfd);
}

// set up board based on information from server
void setupFromServer() {
	int setupState = 0;
	while (setupState < 6){
		bzero(buffer, sizeof(buffer));
		int n = read(sockfd, buffer, sizeof(buffer));
		if (n < 0) {
			fprintf(stderr, "ERROR: could not read from socket\n");
			exit(2);
		}
		else {
			switch(setupState){
				case 0:
					mapExtent = atoi(buffer);
					break;
				case 1:
					numAircraftCarrier = atoi(buffer);
					break;
				case 2:
					numBattleship = atoi(buffer);
					break;
				case 3:
					numSubmarine = atoi(buffer);
					break;
				case 4:
					numCruiser = atoi(buffer);
					break;
				case 5:
					numDestroyer = atoi(buffer);
					break;
			}
			
			setupState++;
		}
	}
}

int checkValidPos(int startNumPos, int startCharPos, int length, char *direction) {
	int numPosOffset = 0;
	int charPosOffset = 0;
	int currNumPos = startNumPos;
	int currCharPos = startCharPos;

	if (strcmp(direction, "North") == 0 || strcmp(direction, "NORTH") == 0 || strcmp(direction, "north") == 0)
		numPosOffset = -1;
	else if (strcmp(direction, "East") == 0 || strcmp(direction, "EAST") == 0 || strcmp(direction, "east") == 0)
		charPosOffset = 1;
	else if (strcmp(direction, "South") == 0 || strcmp(direction, "SOUTH") == 0 || strcmp(direction, "south") == 0)
		numPosOffset = 1;
	else if (strcmp(direction, "West") == 0 || strcmp(direction, "WEST") == 0 || strcmp(direction, "west") == 0)
		charPosOffset = -1;

	int i;
	for (i = 0; i < length; i++){
		if (currNumPos < 1 || currNumPos > mapExtent || currCharPos < 1 || currCharPos > mapExtent)
			return 0;
		else if (map[currNumPos - 1][currCharPos - 1] != 0)
			return 0;

		currNumPos += numPosOffset;
		currCharPos += charPosOffset;
	}
	return 1;
}

void setShipPos(ship *currentShip, int startNumPos, int startCharPos, char *direction) {
	int numPosOffset = 0;
	int charPosOffset = 0;
	int currNumPos = startNumPos;
	int currCharPos = startCharPos;
	int length = currentShip->health;

	if (strcmp(direction, "North") == 0 || strcmp(direction, "NORTH") == 0 || strcmp(direction, "north") == 0)
		numPosOffset = -1;
	else if (strcmp(direction, "East") == 0 || strcmp(direction, "EAST") == 0 || strcmp(direction, "east") == 0)
		charPosOffset = 1;
	else if (strcmp(direction, "South") == 0 || strcmp(direction, "SOUTH") == 0 || strcmp(direction, "south") == 0)
		numPosOffset = 1;
	else if (strcmp(direction, "West") == 0 || strcmp(direction, "WEST") == 0 || strcmp(direction, "west") == 0)
		charPosOffset = -1;

	int i;
	for (i = 0; i < length; i++){
		currentShip->x[i] = currNumPos;
		currentShip->y[i] = currCharPos;
		map[currNumPos - 1][currCharPos - 1] = 1;

		currNumPos += numPosOffset;
		currCharPos += charPosOffset;
	}
}

void chooseShipPositions(int type) {
	int numShips;
	char *shipType;
	switch (type) {
		case 0:
			shipType = "Aircraft Carrier";
			numShips = numAircraftCarrier;
			break;
		case 1:
			shipType = "Battleship";
			numShips = numBattleship;
			break;
		case 2:
			shipType = "Submarine";
			numShips = numSubmarine;
			break;
		case 3:
			shipType = "Cruiser";
			numShips = numCruiser;
			break;
		case 4:
			shipType = "Destroyer";
			numDestroyer = numDestroyer;
			break;
	}
	
	int i = 0;
	while (i < numShips) {
		ship *currentShip;
		switch (type) {
			case 0:
				INIT_AIRCRAFT_CARRIER(currentShip);
				break;
			case 1:
				INIT_BATTLESHIP(currentShip);
				break;
			case 2:
				INIT_SUBMARINE(currentShip);
				break;
			case 3:
				INIT_CRUISER(currentShip);
				break;
			case 4:
				INIT_DESTROYER(currentShip);
				break;
		}
		
		int *numPos;
		char *charPos;
		int charPosToNum;
		char *direction[5];
		printf("Where do you want to put %s %d? \nFormat: Num Char Direction (North, East, South, West) \nExample: 4 A South \n", shipType, i);
		scanf("%d %c %s", numPos, charPos, direction);
		charPosToNum = (int)(&charPos) - 64;
		
		if (checkValidPos(&numPos, charPosToNum, currentShip->health, direction)) {
			setShipPos(currentShip, numPos, charPosToNum, direction);
			
			switch (type) {
				case 0:
					listAircraftCarrier[i] = currentShip;
					break;
				case 1:
					listBattleship[i] = currentShip;
					break;
				case 2:
					listSubmarine[i] = currentShip;
					break;
				case 3:
					listCruiser[i] = currentShip;
					break;
				case 4:
					listDestroyer[i] = currentShip;
					break;
			}
			
			i++;
		} else
			fprintf(stderr, "ERROR: position is not valid\n");
	}
}
