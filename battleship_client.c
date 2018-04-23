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


//
// method signatures
//

//
// method definitions
//

int main(int argc, char *argv[]) {
	// Check for proper number of commandline arguments
	// Expect program name in argv[0], IP address in argv[1], and port # in argv[2]
	if (argc < 3) {
		fprintf(stderr,"Program Usage: %s hostname port\n", argv[0]);
		return 1;
	}

	return 0;
}