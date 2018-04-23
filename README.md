## EECS 338 Final Project
Clarinda Ho (cqh), Jason Shin (jjs270)

### Concept
Create a 2-player battleship that uses sockets (server and client).

### Map
	- Legend
		- B = Ship
		- X = Hit
		- _ = Empty
		-   = Miss
		
	- Map View: left is what player sees, right is what opponent sees
	  _ _ _ _ _   _ _ _ _ _
	  _ _ _ _ _   _ _ _ _ _
	  _ B X B _   _ _ X _ _
	  _ _ _ _ _   _ _ _ _ _
	  _ _ _ _ _   _ _   _ _ 

### Initialization
	- Printing out instructions at the beginning
	
    - Configuration method
		- How large the board will be (max 20x20), how many ships to generate, etc. 
 
    - Creating the data structure to store the map (2D Array)
		- Each player will have two -- one for their map and one for their opponent

    - Populate the board
		- Each player will choose where they will place the ship and the direction to orient the ship
		- Format: (Coordinate, Direction) EX) 4A E
  
### Running
    - Reading user input for two players
		- Format: Coordinates (Number, Letter) 
		
    - On Miss:
		- Show the miss on the map for the person shooting

    - On Hit:
		- Check to see if ship is sunk
        - Keep track of ships in a struct, decrement a value representing how much health is left
    
    - Check to see if total health is 0, if it is 0, end the game
    - If not, clear the terminal, switch turns    