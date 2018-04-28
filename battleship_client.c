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

ship* INIT_SHIP(int healthPts){
	ship *newShip = malloc(sizeof(ship));
	newShip->health = healthPts;

	int i;
	for (i = 0; i < 5; i++) {
		newShip->x[i] = -1;
		newShip->y[i] = -1;
	}

	return newShip;
}

//
// global variables and constants
//

int sockfd;			// client socket
char buffer[256];	// buffer

int map[20][20];
int enemyMap[20][20];
int mapExtent;

int gameOver = 1;

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

void printBoard();
void clearBoard();
void printEnemyBoard();
void clearEnemyBoard();

void turn();
void attackTurn();
void defendTurn();

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
	
	turn();
	
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

// Checks to see whether or not ship can be placed at this position
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

// Configures the map using the placement of the ship
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

// Takes in user input to determine where to put the ships on the board.
void chooseShipPositions(int type) {
	int numShips;
	char *shipType;
	int healthPts;
	switch (type) {
		case 0:
			shipType = "Aircraft Carrier";
			numShips = numAircraftCarrier;
			healthPts = 5;
			break;
		case 1:
			shipType = "Battleship";
			numShips = numBattleship;
			healthPts = 4;
			break;
		case 2:
			shipType = "Submarine";
			numShips = numSubmarine;
			healthPts = 3;
			break;
		case 3:
			shipType = "Cruiser";
			numShips = numCruiser;
			healthPts = 3;
			break;
		case 4:
			shipType = "Destroyer";
			numDestroyer = numDestroyer;
			healthPts = 2;
			break;
	}
	
	int i = 0;
	while (i < numShips) {
		ship *currentShip = INIT_SHIP(healthPts);
		
		int numPos = 0;
		char charPos = 'A';
		int charPosToNum = 0;
		char direction[5] = "";

		printBoard();
		printf("Where do you want to put %s %d? \nFormat: Num Char Direction (North, East, South, West) \nExample: 4 A South \n", shipType, i);
		scanf("%d %c %s", &numPos, &charPos, &direction);

		charPosToNum = charPos - 'A' + 1;

		if (checkValidPos(numPos, charPosToNum, currentShip->health, direction) == 1) {
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

// Prints out the current status of the board
void printBoard() {
	int i, j, c, n;
	printf("   ");
	for (c = 65; c < 65 + mapExtent; c++) {
		printf("%c ", c);
	}
	printf("\n");
	for (i = 0; i < mapExtent; i++) {
		if (i < 9)
			printf("%d  ", (i + 1));
		else
			printf("%d ", (i + 1));
		for (j = 0; j < mapExtent; j++) {
			switch(map[i][j]) {
				case 0:
					printf("_ ");
					break;
				case 1:
					printf("B ");
					break;
				case 2:
					printf("X ");
				case 3:
					printf("  ");
			}
		}
		printf("\n");
	}
}

// Sets all spots on the board to 0
void clearBoard() {
	int i, j;
	for (i = 0; i < mapExtent; i++) {
		for (j = 0; j < mapExtent; j++) {
			map[i][j] = 0;
		}
	}
}

void turn(){
	while(gameOver == 1){
		defendTurn();
		attackTurn();
	}
}

void attackTurn(){
	if (gameOver == 1){
		int numPos;
		char charPos;
		int charPosToNum;
		
		// ask user for input
		printf("What position do you want to hit? \nFormat: Num Char \nExample: 4 A \n");
		scanf("%d %c", &numPos, &charPos);

		charPosToNum = charPos - 'A' + 1;
		
		// send user input through socket to opponent to check
		bzero(buffer, sizeof(buffer));
		sprintf(buffer, "%d %d\n", numPos, charPosToNum);
		
		n = write(newsockfd, buffer, sizeof(buffer));
		if (n < 0) {
			fprintf(stderr, "ERROR: could not write to socket\n");
			exit(2);
		}
		
		usleep(500);
		
		// read response from opponent
		bzero(buffer, sizeof(buffer));
		int n = read(sockfd, buffer, sizeof(buffer));
		if (n < 0) {
			fprintf(stderr, "ERROR: could not read from socket\n");
			exit(2);
		}
		else {
			int code;
			char* token;
			
			token = strtok(buffer, ' ');
			code = atoi(token);
			while (token != NULL){
				printf("%s ", token);
				token = strtok(NULL, '.');
			}
			printf("\n");
			
			if (code == 0) {			// miss
				enemyMap[numPos - 1][charPosToNum - 1] = 3;
			} else if (code == 1) {		// hit
				enemyMap[numPos - 1][charPosToNum - 1] = 2;
			} else if (code == 2) {		// game over
				enemyMap[numPos - 1][charPosToNum - 1] = 2;
				gameOver = 0;
			}
		}
	}
}

void defendTurn(){
	if (gameOver == 1){
		// read input from opponent
		bzero(buffer, sizeof(buffer));
		int n = read(sockfd, buffer, sizeof(buffer));
		if (n < 0) {
			fprintf(stderr, "ERROR: could not read from socket\n");
			exit(2);
		}
		else {
			int numPos = atoi(strtok(buffer, ' '));
			int charPosToNum = atoi(stroken(NULL, ' '));
			
			// determine if the opponent hit your ship
			char* message = hitShip(numPos, charPosToNum);
			
			// send response to opponent
			bzero(buffer, sizeof(buffer));
			sprintf(buffer, "%s\n", message);
		
			n = write(newsockfd, buffer, sizeof(buffer));
			if (n < 0) {
				fprintf(stderr, "ERROR: could not write to socket\n");
				exit(2);
			}
		}
	}
}