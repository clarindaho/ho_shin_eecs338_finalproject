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
#include <time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

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

int sockfd;			// server socket
int newsockfd;		// new socket connection with client
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

void createServerSocket(char *portNum);
void acceptConnection();
void closeSockets();

void configureGame();
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
	// Expect program name in argv[0], port # in argv[1]
	if (argc < 2) { 
		fprintf(stderr,"ERROR: no port provided\n");
	 	return 1;
	}
	
	createServerSocket(argv[1]);
	acceptConnection();
	
	clearBoard();
	configureGame();
	
	int i;
	for (i = 0; i < 5; i++)
		chooseShipPositions(i);
	
	turn();
	
	closeSockets();
	
	return 0;
}

// create and setup server socket
void createServerSocket(char *portNum){
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "ERROR: could not open server socket\n");
		exit(2);
	}

	struct sockaddr_in serv_addr; 
	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	int portno = atoi(portNum);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { 
	  	fprintf(stderr, "ERROR: could not bind server socket\n");
		exit(2);
	}
}

// listen and accept incoming connection requests
void acceptConnection(){
	listen(sockfd, 5);
	
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr); 
	
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0){
		fprintf(stderr, "ERROR: could not accept incoming connection request\n");
		exit(2);
	}

	bzero(buffer, sizeof(buffer));
}

// close all sockets
void closeSockets(){
	close(newsockfd);	// close socket connection with client
	close(sockfd);		// close server socket
}

// configure game
void configureGame(){
	char temp[100];
	char exitString[100] = "exit";
	
	int n;
	
	// configure board size
	int isBoardSizeSet = 1;
	while (isBoardSizeSet != 0){
		printf("How large do you want the board to be? (Min: 7x7, Max: 20x20) \nFormat: number \n");
		gets(temp);
		
		if (strcmp(temp, exitString) == 0)
			exit(0);
		else
			mapExtent = atoi(temp);
		
		if (mapExtent < 7)
			fprintf(stderr, "ERROR: board extent is too small\n");
		else if (mapExtent > 20)
			fprintf(stderr, "ERROR: board extent is too large\n");
		else
			isBoardSizeSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d", mapExtent);
	
	n = write(newsockfd, buffer, sizeof(buffer));
	if (n < 0) {
		fprintf(stderr, "ERROR: could not write to socket\n");
		exit(2);
	}
	
	usleep(500);
	
	// configure aircraft carrier
	int isAircraftCarrierSet = 1;
	while(isAircraftCarrierSet != 0){
		printf("How many aircraft carriers do you want? (Max: 2) \n");
		gets(temp);
		
		if (strcmp(temp, exitString) == 0)
			exit(0);
		else
			numAircraftCarrier = atoi(temp);
		
		if (numAircraftCarrier > 2 || numAircraftCarrier < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isAircraftCarrierSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d\n", numAircraftCarrier);
	
	n = write(newsockfd, buffer, sizeof(buffer));
	if (n < 0) {
		fprintf(stderr, "ERROR: could not write to socket\n");
		exit(2);
	}
	
	usleep(500);
	
	// configure battleship
	int isBattleshipSet = 1;
	while(isBattleshipSet != 0){
		printf("How many battleships do you want? (Max: 3) \n");
		gets(temp);
		
		if (strcmp(temp, exitString) == 0)
			exit(0);
		else
			numBattleship = atoi(temp);
		
		if (numBattleship > 3 || numBattleship < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isBattleshipSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d\n", numBattleship);
	
	n = write(newsockfd, buffer, sizeof(buffer));
	if (n < 0) {
		fprintf(stderr, "ERROR: could not write to socket\n");
		exit(2);
	}
	
	usleep(500);
	
	// configure submarine
	int isSubmarineSet = 1;
	while(isSubmarineSet != 0){
		printf("How many submarines do you want? (Max: 3) \n");
		gets(temp);
		
		if (strcmp(temp, exitString) == 0)
			exit(0);
		else
			numSubmarine = atoi(temp);
		
		if (numSubmarine > 3 || numSubmarine < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isSubmarineSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d\n", numSubmarine);
	
	n = write(newsockfd, buffer, sizeof(buffer));
	if (n < 0) {
		fprintf(stderr, "ERROR: could not write to socket\n");
		exit(2);
	}
	
	usleep(500);
	
	// configure cruiser
	int isCruiserSet = 1;
	while(isCruiserSet != 0){
		printf("How many cruisers do you want? (Max: 3) \n");
		gets(temp);
		
		if (strcmp(temp, exitString) == 0)
			exit(0);
		else
			numCruiser = atoi(temp);
		
		if (numCruiser > 3 || numCruiser < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isCruiserSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d\n", numCruiser);
	
	n = write(newsockfd, buffer, sizeof(buffer));
	if (n < 0) {
		fprintf(stderr, "ERROR: could not write to socket\n");
		exit(2);
	}
	
	usleep(500);
	
	// configure destroyer
	int isDestroyerSet = 1;
	while(isDestroyerSet != 0){
		printf("How many destroyers do you want? (Max: 4) \n");
		gets(temp);
		
		if (strcmp(temp, exitString) == 0)
			exit(0);
		else
			numDestroyer = atoi(temp);
		
		if (numDestroyer > 4 || numDestroyer < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isDestroyerSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d\n", numDestroyer);
	
	n = write(newsockfd, buffer, sizeof(buffer));
	if (n < 0) {
		fprintf(stderr, "ERROR: could not write to socket\n");
		exit(2);
	}
	
	usleep(500);
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
		//printf("%d %d\n", currNumPos, currCharPos);
		//fflush(stdout);
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

		//printf("numPos:%d charPos:%d health:%d direction: %s\n", numPos, charPosToNum, currentShip->health, direction);
		fflush(stdout);

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
	printf("Your Board: \n");
	
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
	
	fflush(stdout);
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

// Prints out the current status of the enemy board
void printEnemyBoard() {
	printf("Enemy Board: \n");
	
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
			switch(enemyMap[i][j]) {
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
	
	fflush(stdout);
}

// Sets all spots on the enemy board to 0
void clearEnemyBoard() {
	int i, j;
	for (i = 0; i < mapExtent; i++) {
		for (j = 0; j < mapExtent; j++) {
			enemyMap[i][j] = 0;
		}
	}
}

void turn(){
	while(gameOver == 1){
		attackTurn();
		defendTurn();
	}
}

void attackTurn(){
	if (gameOver == 1){
		printEnemyBoard();
		
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
			
			printEnemyBoard();
		}
	}
}

void defendTurn(){
	if (gameOver == 1){
		printf("Waiting for opponent move...\n");
		fflush(stdout);
		
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
			
			printBoard();
			
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