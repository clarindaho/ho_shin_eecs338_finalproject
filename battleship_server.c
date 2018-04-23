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
} aircraft_carrier;

typedef struct {
	int health;
	int x[4];
	int y[4];
} battleship;

typedef struct {
	int health;
	int x[3];
	int y[3];
} submarine;

typedef struct {
	int health;
	int x[3];
	int y[3];
} cruiser;

typedef struct {
	int health;
	int x[2];
	int y[2];
} destroyer;

//
// global variables and constants
//

int sockfd;			// server socket
int newsockfd;		// new socket connection with client
char buffer[256];	// buffer

int map[20][20];
int mapExtent;

aircraft_carrier listAircraftCarrier[2];
int numAircraftCarrier;

battleship listBattleship[3];
int numBattleship;

submarine listSubmarine[3];
int numSubmarine;

cruiser listCruiser[3];
int numCruiser;

destroyer listDestroyer[4];
int numDestroyer;

//
// method signatures
//

void createServerSocket(char *portNum);
void acceptConnection();
void closeSockets();

void configureGame();
void placeShips();

//
// method definitions
//

int main(int argc, char *argv[]) {
	// Check for proper number of commandline arguments
	// Expect program name in argv[0], port # in argv[1]
	if (argc < 2) { 
		fprintf(stderr,"ERROR: no port provided\n");
	 	return 1;
	}
	
	createServerSocket(argv[1]);
	acceptConnection();
	configureGame();
	
	closeSockets();
	
	return 0;
}

// create and setup server socket
void createServerSocket(char *portNum){
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "ERROR: could not open server socket\n");
		return 2;
	}

	struct sockaddr_in serv_addr; 
	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	int portno = atoi(portNum);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { 
	  	fprintf(stderr, "ERROR: could not bind server socket\n");
		return 2;
	}
}

// listen and accept incoming connection requests
void acceptConnection(){
	listen(sockfd, 5);
	
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr); 
	
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
		error("ERROR: could not accept incoming connection request\n");

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
	char exit[100] = "exit";
	
	// configure board size
	int isBoardSizeSet = 1;
	while (isBoardSizeSet != 0){
		printf("How large do you want the board to be? (Min: 7x7, Max: 20x20) \nFormat: number \n");
		gets(temp);
		
		if (strcmp(temp, exit) == 0)
			return 0;
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
	usleep(200);
	
	// configure aircraft carrier
	int isAircraftCarrierSet = 1;
	while(isAircraftCarrierSet != 0){
		printf("How many aircraft carriers do you want? (Max: 2) \n");
		gets(temp);
		
		if (strcmp(temp, exit) == 0)
			return 0;
		else
			numAircraftCarrier = atoi(temp);
		
		if (numAircraftCarrier > 2 || numAircraftCarrier < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isAircraftCarrierSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d", numAircraftCarrier);
	usleep(200);
	
	// configure battleship
	int isBattleshipSet = 1;
	while(isBattleshipSet != 0){
		printf("How many battleships do you want? (Max: 3) \n");
		gets(temp);
		
		if (strcmp(temp, exit) == 0)
			return 0;
		else
			numBattleship = atoi(temp);
		
		if (numBattleship > 3 || numBattleship < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isBattleshipSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d", numBattleship);
	usleep(200);
	
	// configure submarine
	int isSubmarineSet = 1;
	while(isSubmarineSet != 0){
		printf("How many submarines do you want? (Max: 3) \n");
		gets(temp);
		
		if (strcmp(temp, exit) == 0)
			return 0;
		else
			numSubmarine = atoi(temp);
		
		if (numSubmarine > 3 || numSubmarine < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isSubmarineSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d", numSubmarine);
	usleep(200);
	
	// configure cruiser
	int isCruiserSet = 1;
	while(isCruiserSet != 0){
		printf("How many cruisers do you want? (Max: 3) \n");
		gets(temp);
		
		if (strcmp(temp, exit) == 0)
			return 0;
		else
			numCruiser = atoi(temp);
		
		if (numCruiser > 3 || numCruiser < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isCruiserSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d", numCruiser);
	usleep(200);
	
	// configure destroyer
	int isDestroyerSet = 1;
	while(isDestroyerSet != 0){
		printf("How many destroyers do you want? (Max: 4) \n");
		gets(temp);
		
		if (strcmp(temp, exit) == 0)
			return 0;
		else
			numDestroyer = atoi(temp);
		
		if (numDestroyer > 4 || numDestroyer < 0)
			fprintf(stderr, "ERROR: not a valid option\n");
		else
			isDestroyerSet = 0;
	}
	
	bzero(buffer, sizeof(buffer));
	sprintf(buffer, "%d", numDestroyer);
	usleep(200);
}

void placeShips(){
}

