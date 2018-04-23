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

//
// global variables and constants
//

int sockfd;			// client socket
char buffer[256];	// buffer

int map[20][20];
int mapExtent = 0;

int numAircraftCarrier = 0;
ship listAircraftCarrier[2];
#define INIT_AIRCRAFT_CARRIER(X) ship X = {.health = 5, .x = {-1,-1,-1,-1,-1} .y = {-1,-1,-1,-1,-1}}

int numBattleship = 0;
ship listBattleship[3];
#define INIT_BATTLESHIP(X) ship X = {.health = 4, .x = {-1,-1,-1,-1,-1} .y = {-1,-1,-1,-1,-1}}

int numSubmarine = 0;
ship listSubmarine[3];
#define INIT_SUBMARINE(X) ship X = {.health = 3, .x = {-1,-1,-1,-1,-1} .y = {-1,-1,-1,-1,-1}}

int numCruiser = 0;
ship listCruiser[3];
#define INIT_CRUISER(X) ship X = {.health = 3, .x = {-1,-1,-1,-1,-1} .y = {-1,-1,-1,-1,-1}}

int numDestroyer = 0;
ship listDestroyer[4];
#define INIT_DESTROYER(X) ship X = {.health = 2, .x = {-1,-1,-1,-1,-1} .y = {-1,-1,-1,-1,-1}}

//
// method signatures
//

//
// method definitions
//

// Helper function to conveniently print to stderr AND exit (terminate)
void error(const char *msg) {
  perror(msg);
  exit(0);
}

//create and set up client socket
void connectToServer(char *IPAddress, char* portNum) {
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "ERROR: could not open client socket to the server\n");
		exit(0);
	}

	struct hostent *server;
	server = gethostbyname(IPAddress);
	struct sockaddr_in serv_addr;
  	bzero((char *) &serv_addr, sizeof(serv_addr));

	int portno = atoi(portNum);
	serv_addr.sin_family = AF_INET;
  	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  	serv_addr.sin_port = htons(portno);

  	if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    	error("ERROR could not bind client socket");
}

// Close all sockets
void closeSocket() {
	close(sockfd);
}

// Setting up board from information from server
void setupFromServer() {
	int setupState;
	for (setupState = 0; setupState < 6; setupState++) {
		bzero(buffer, sizeof(buffer));
		int n = read(sockfd, buffer, sizeof(buffer));
		if (n < 0)
			error("ERROR reading from socket");
		else if (setupState == 0)
			mapExtent = atoi(buffer);
		else if (setupState == 1)
			numAircraftCarrier = atoi(buffer);
		else if (setupState == 2)
			numBattleship = atoi(buffer);
		else if (setupState == 3)
			numSubmarine = atoi(buffer);
		else if (setupState == 4)
			numCruiser = atoi(buffer);
		else if (setupState == 5)
			numDestroyer = atoi(buffer);
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

void setShipPos(ship *ship, int startNumPos, int startCharPos, char *direction) {
	int length = (&ship).health;

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
		(&ship).x[i] = currNumPos;
		(&ship).y[i] = currCharPos;

		currNumPos += numPosOffset;
		currCharPos += charPosOffset;
	}
}

void chooseShipPositions(int type) {
	int i = 0;
	int numShips;
	char *shipType;
	switch (type) {
		case(0) {
			shipType = "Aircraft Carrier";
			numShips = numAircraftCarrier;
			break;
		}
		case(1) {
			shipType = "Battleship";
			numShips = numBattleship;
			break;
		}
		case(2) {
			shipType = "Submarine";
			numShips = numSubmarine;
			break;
		}
		case(3) {
			shipType = "Cruiser";
			numShips = numCruiser;
			break;
		}
		case(4) {
			shipType = "Destroyer";
			numDestroyer = numDestroyer;
			break;
		}
	}
	while (i < numShips) {
		ship *ship;
		switch (type) {
			case(0) {
				INIT_AIRCRAFT_CARRIER(ship);
				break;
			}
			case(1) {
				INIT_BATTLESHIP(ship);
				break;
			}
			case(2) {
				INIT_SUBMARINE(ship);
				break;
			}
			case(3) {
				INIT_CRUISER(ship);
				break;
			}
			case(4) {
				INIT_DESTROYER(ship);
				break;
			}
		}
		int *numPos;
		char *charPos;
		char *direction[5];
		int charPosToNum;
		printf("Where do you want to put %s %d?\nFormat: Num Char Direction (North, South, East, West)\n
			Example: 4 A South", shipType, i);
		scanf("%d %c %s", numPos, charPos, direction);
		charPosToNum = (int)(&charPos) - 64;
		if (checkValidPos(&numPos, charPosToNum, direction)) {
			setAircraftPos(tmp, numPos, charPosToNum, direction);
			i++;
		} else {
			printf("ERROR: Position is not valid\n");
		}
	}
}


// Main method
int main(int argc, char *argv[]) {
	// Check for proper number of commandline arguments
	// Expect program name in argv[0], IP address in argv[1], and port # in argv[2]
	if (argc < 3) {
		fprintf(stderr,"Program Usage: %s hostname port\n", argv[0]);
		return 1;
	}
	connectToServer(argv[1], argv[2]);
	chooseShipPositions(0);
	chooseShipPositions(1);
	chooseShipPositions(2);
	chooseShipPositions(3);
	chooseShipPositions(4);
	closeSocket();

	return 0;
}