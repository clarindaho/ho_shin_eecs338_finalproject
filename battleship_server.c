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

//
// structs
//

struct aircraft_carrier {
	int health = 5;
	int x[5];
	int y[5];
}

struct battleship {
	int health = 4;
	int x[4];
	int y[4];
}

struct submarine {
	int health = 3;
	int x[3];
	int y[3];
}

struct cruiser {
	int health = 3;
	int x[3];
	int y[3];
}

struct destroyer {
	int health = 2;
	int x[2];
	int y[2];
}

//
// global variables and constants
//

int sockfd;			// server socket
int newsockfd;		// new socket connection with client
char buffer[256];	// buffer

int map[20][20];
int *xExtent;
int *yExtent;

aircraft_carrier listAircraftCarrier[3];
int *numAircraftCarrier;

battleship listBattleship[4];
int *numBattleship;
int *numSubmarine;
int *numCruiser;
int *numDestroyer;

//
// method signatures
//

void createServerSocket(char *portNum);
void connect();
void closeSockets();

void configureGame();

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
	connect();
	
	
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
void connect(){
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
	int isBoardSizeSet = 1;
	
	while (isBoardSizeSet != 0){
		printf("How large do you want the board to be? (Min: 10x5, Max: 20x20) \nFormat: xExtent x yExtent \n");
		scanf("%d %d", &xExtent, &yExtent);
		
		if (&xExtent < 10)
			fprintf(stederr, "ERROR: board xExtent is too small\n");
		else if (&xExtent > 20)
			fprintf(stderr, "ERROR: board xExtent is too large\n");
		else if (&yExtent < 5)
			fprintf(stderr, "ERROR: board yExtent is too small\n");
		else if (&yExtent > 20)
			fprintf(stderr, "ERROR: board yExtent is too large\n");
		else
			isBoardSizeSet = 0;
	}
	
	int isAircraftCarrierSet = 1;
	while(isAircraftCarrierSet != 0){
		printf("How many aircraft carriers do you want? (Max: 3)");
	}
}