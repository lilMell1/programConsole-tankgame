// FileName.cpp

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 13

// Enum declaration
enum Bool {
    FALSE,
    TRUE
};

enum Type {
    LAND,
    WALL,
    OUTSIDE
};

typedef struct {
    Type landType;
    Bool hasTank;
    int nTankID;
} Land;

enum Directions {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
};

// Typedef declaration
typedef struct {
    int x;  // x-coordinate
    int y;  // y-coordinate
} Coordinates;

typedef struct {
    int dx; // Change in x-coordinate
    int dy; // Change in y-coordinate
} Direction;

// Global variable definition
Direction g_dir[8];

// Function prototypes
void initializeMap(Land mat[MATRIX_SIZE][MATRIX_SIZE]);
void printMatrix(Land mat[MATRIX_SIZE][MATRIX_SIZE]);
void moveTank(int nDirection, int nSteps, int nTankID, Land mat[MATRIX_SIZE][MATRIX_SIZE]);
void initializeDirections(Direction dir[]);
void normalShot(int nShotDirection, int nTankID, Land mat[MATRIX_SIZE][MATRIX_SIZE]);
void ricochetShot(int nShotDirection, int nTankID, Land mat[MATRIX_SIZE][MATRIX_SIZE]);
void airBomb(int nBombPointX, int nBombPointY, int nTankID, Land mat[MATRIX_SIZE][MATRIX_SIZE]);

//-----------------------------------------------------------------------------
//                                 Main
//                                ------
//
// General : This program creates a tank game for 2 players, each move the player 
//           can choose an option to play, the first to destroy the other player
//           wins.
//
// Input : 1 - to move
//         2 - to shoot normal shot
//         3 - to shoot ricochet shot
//         4 - to air bomb an area
//
// Process : Each option has it own unique way to win a game.
//
// Output : Win / Lose Or current round map if no one won.
//
//-----------------------------------------------------------------------------
// Programmer : Or mellibovsky
// Date : 19.01.24
//-----------------------------------------------------------------------------
void main() 
{
    Land matrix[MATRIX_SIZE][MATRIX_SIZE];
    initializeMap(matrix);  
    initializeDirections(g_dir);
    int nDirection;
    int nTankID = 1;
    int nInput=0;
    while (nInput != 999) {
        printMatrix(matrix);

        printf("Enter a command (999 to quit): \n\n");
        printf("!!tank %d move now!!\n\n",nTankID );
        printf("1 to move\n\n");
        printf("2 to shoot normal shot\n\n");
        printf("3 to shoot ricochet shot\n\n");
        printf("4 to air bomb an area\n\n");
        printf("Choice: ");
        scanf("%d", &nInput);  // Read user input
        
        switch (nInput) {
        case 1: {  // Move
            int nSteps;
            printf("Enter movement direction (0 for UP, 1 for DOWN, 2 for LEFT, 3 for RIGHT");
            printf(", 4 for UP-LEFT, 5 for UP-RIGHT, 6 for DOWN-LEFT, 7 for DOWN-RIGHT): ");
            scanf("%d", &nDirection);
            printf("\nEnter amount of steps: ");
            scanf("%d", &nSteps);
            if (nDirection >= 0 && nDirection <= 7) {
                moveTank(nDirection,nSteps, nTankID, matrix);
            }
            else {
                printf("Invalid direction.\n");
            }

            break;
        }
        case 2:
            printf("Enter shot direction (0 for UP, 1 for DOWN, 2 for LEFT, 3 for RIGHT");
            printf(", 4 for UP-LEFT, 5 for UP-RIGHT, 6 for DOWN-LEFT, 7 for DOWN-RIGHT): ");
            scanf("%d", &nDirection);
            normalShot(nDirection, nTankID, matrix);

            break;
        case 3:
            printf("Enter shot direction (0 for UP, 1 for DOWN, 2 for LEFT, 3 for RIGHT: ");
            scanf("%d", &nDirection);
            ricochetShot(nDirection, nTankID, matrix);

            break;
        case 4:
            int pointX, pointY;
            printf("chose X and Y to shoot bomb: ");
            scanf("%d %d", &pointY, &pointX);
            airBomb(pointX, pointY, nTankID, matrix);
            break;
        case 999:
            printf("Exiting...\n");
            exit(0);
        default:
            printf("Invalid command.\n");
            break;
        }

        // Toggle tankID between 0 and 1 for each iteration
        nTankID = (nTankID == 1) ? 2 : 1;
    }


}

//-----------------------------------------------------------------------------
//                           Initialize Directions
//                          ------------------------
//
// General : Initializing the directions in the "Direction" array that the tanks move by.
//           Main creates it once.
//
// Parameters : 
//       dir - A global array for the movment
//
// Return Value : The array with the directions.
//
//-----------------------------------------------------------------------------
void initializeDirections(Direction dir[]) {
    dir[UP].dx = -1;   dir[UP].dy = 0;   // Left
    dir[DOWN].dx = 1;  dir[DOWN].dy = 0;  // Right
    dir[LEFT].dx = 0;  dir[LEFT].dy = -1;  // Up
    dir[RIGHT].dx = 0; dir[RIGHT].dy = 1;  // Down
    dir[UP_LEFT].dx = -1; dir[UP_LEFT].dy = -1;  // Up-Left (remains the same)
    dir[UP_RIGHT].dx = -1; dir[UP_RIGHT].dy = 1;  // Down-Left
    dir[DOWN_LEFT].dx = 1; dir[DOWN_LEFT].dy = -1;  // Up-Right
    dir[DOWN_RIGHT].dx = 1; dir[DOWN_RIGHT].dy = 1;  // Down-Right (remains the same)
}

//-----------------------------------------------------------------------------
//                         Get Tank Coordinates
//                       ------------------------
//
// General : Gets the tank coordinates from the matrix in current round
//
// Parameters : 
//      mat - The matrix the game works on
//      tanks - An empty Array that will contain coordinates of the tanks
//
// Return Value : Tanks array will contain the coordiantes of each tank
//                after calling the function
//
//-----------------------------------------------------------------------------
void getTankCoordinates(const Land mat[MATRIX_SIZE][MATRIX_SIZE], Coordinates tanks[2]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (mat[i][j].hasTank) {
                if (mat[i][j].nTankID == 1) {
                    tanks[1].x = i;
                    tanks[1].y = j;
                }
                else if (mat[i][j].nTankID == 2) {
                    tanks[2].x = i;
                    tanks[2].y = j;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
//                               Move Tank
//                              -----------
//
// General : Moves each tank according to current round
//
// Parameters : 
//       direction - The direction that the tank will move 
//       steps - The amount of steps the tank will move
//       tankID - The current round tank ID
//       mat - A two-dimensional array representing the game map.
//
// Return Value : The tank will move in the matrix according to the 
//                input
//
//-----------------------------------------------------------------------------
void moveTank(int nDirection,int nSteps, int nTankID, Land mat[MATRIX_SIZE][MATRIX_SIZE]) {   

    for (int i = 0; i < nSteps; i++)
    {
        // Find current position of the tank
        Coordinates tanksPlace[3];
        getTankCoordinates(mat, tanksPlace);

        // Calculate new position based on direction
        int newX = tanksPlace[nTankID].x + g_dir[nDirection].dx;
        int newY = tanksPlace[nTankID].y + g_dir[nDirection].dy;

        // Check if new position is valid and move tank
        if (mat[newX][newY].landType == LAND) {
            // Move tank: Update matrix
            mat[tanksPlace[nTankID].x][tanksPlace[nTankID].y].hasTank = FALSE; // Remove tank from old position
            mat[newX][newY].hasTank = TRUE;  // Place tank at new position
            mat[newX][newY].nTankID = nTankID;
            mat[tanksPlace[nTankID].x][tanksPlace[nTankID].y].nTankID = 0;
        }
        else if (mat[newX][newY].landType == WALL) {
            mat[newX][newY].landType = LAND;
            break;
        }
        else {
            printf("Move out of bounds.\n");
            break;
        }
    }
    
    
}

//-----------------------------------------------------------------------------
//                              Normal Shot
//                             -------------
//
// General : Shoots a shot the can destroy a wall / miss / win a game
//
// Parameters : 
//       shotDirection - The direction that the shot will move 
//       tankID - The current round tank ID
//       mat - The matrix of the game
//
// Return Value : an output of what happend (miss / hit wall/ win)
//
//-----------------------------------------------------------------------------
void normalShot(int nShotDirection, int nTankID, Land mat[MATRIX_SIZE][MATRIX_SIZE])
{
    // Find current position of the tank
    Coordinates tanksPlace[3];
    getTankCoordinates(mat, tanksPlace);

    // Calculate initial shot position
    int newX = tanksPlace[nTankID].x + g_dir[nShotDirection].dx;
    int newY = tanksPlace[nTankID].y + g_dir[nShotDirection].dy;

    // Loop until shot goes out of bounds or hits something
    while (mat[newX][newY].landType != OUTSIDE) {
        if (mat[newX][newY].hasTank) {
            printf("Game ended, tank %d won.\n", nTankID );  
            exit(1);
        }
        else if (mat[newX][newY].landType == WALL) {
            printf("Hit wall\n");
            mat[newX][newY].landType = LAND;
            return;
        }

        // Update coordinates for next iteration
        newX += g_dir[nShotDirection].dx;
        newY += g_dir[nShotDirection].dy;
    }

    // If loop exits, it means shot went out of bounds
    printf("Shot went out of bounds.\n");   
}

//-----------------------------------------------------------------------------
//                              turn Right
//                             ------------
//
// General: Returns a new direction that is 90 degrees clockwise from the given
//          direction and updates the global array index for direction.
//
// Parameters:
// direction - The direction of the shot.
//
// Return Value: A new direction that is 90 degrees clockwise from
//               the given direction, and the global array
//               index is updated accordingly.
//
//-----------------------------------------------------------------------------
int turnRight(int nDirection) {
    switch (nDirection) {
    case UP:
        return RIGHT;
    case RIGHT:
        return DOWN;
    case DOWN:
        return LEFT;
    case LEFT:
        return UP;
    default:
        // If it's a diagonal direction or invalid, we just return the same direction
        return nDirection;
    }
}

//-----------------------------------------------------------------------------
//                             Ricochet Shot
//                            ---------------
//
// General : A shot that goes around the map
//
// Parameters : 
//       shotDirection - The direction that the shot will accordingly 
//       tankID - The current round tank ID
//       mat - A two-dimensional array representing the game map.
//
// Return Value : Miss / Win / Loss / Self hit
//
//-----------------------------------------------------------------------------
void ricochetShot(int nShotDirection, int tankID, Land mat[MATRIX_SIZE][MATRIX_SIZE]) {
    // Find current position of the tank
    Coordinates tanksPlace[3];
    getTankCoordinates(mat, tanksPlace);

    // Find current position of the tank
    int flag = 5;
    int newX = tanksPlace[tankID].x;
    int newY = tanksPlace[tankID].y;

    // Check initial direction, avoid shooting oneself
    if (mat[newX + g_dir[nShotDirection].dx][newY + g_dir[nShotDirection].dy].landType == OUTSIDE) {
        printf("shot yourself! tank %d won\n", tankID = (tankID == 1) ? 2 : 1);
        exit(1);
    }
    newX += g_dir[nShotDirection].dx;
    newY += g_dir[nShotDirection].dy;

    // Loop to move the shot until it hits something or goes out of bounds
    while (TRUE) {     
        // Check if bomb coordinates are within the map
        if (mat[newX + g_dir[nShotDirection].dx][newY + g_dir[nShotDirection].dy].landType == OUTSIDE && flag > 0) { 
            // Change direction when hitting the border
            nShotDirection = turnRight(nShotDirection);
            flag--;
            continue;
        }
        if (mat[newX][newY].hasTank) {
            if (mat[newX][newY].nTankID != tankID)
                printf("HIT! Game ended, tank %d won.\n", tankID);
            else
                printf("HIT! Game ended, tank %d won.\n", tankID = (tankID == 1) ? 2 : 1);
            exit(1);
        }
        else if (mat[newX][newY].landType == LAND)
        {
            newX += g_dir[nShotDirection].dx;
            newY += g_dir[nShotDirection].dy;
        }
        else if (mat[newX][newY].landType == WALL) {
            printf("Shot hit a wall.\n");
            mat[newX][newY].landType = LAND;
            return;
        }
       
        
    }

    printf("Shot went out of bounds.\n");
}

//-----------------------------------------------------------------------------
//                               Air Bomb
//                              ----------
//
// General : Shots an air bomb the covers all the tile around (including given point)
//           a given X and Y.
//
// Parameters : 
//       bombPointX - X coordinate
//       bombPointY - Y coordinate
//       tankID - The current round tank ID
//       mat - A two-dimensional array representing the game map.
//
// Return Value : Win / Loss / Self hit
//
//-----------------------------------------------------------------------------
void airBomb(int nBombPointX, int nBombPointY, int nTankID, Land mat[MATRIX_SIZE][MATRIX_SIZE]) {
    int isHit, hitMade = 0;

    // Check if bomb coordinates are within the map
    if (nBombPointX >= 0 && nBombPointX < MATRIX_SIZE && nBombPointY >= 0 && nBombPointY < MATRIX_SIZE) {
        // Iterate over adjacent squares
        for (int i = nBombPointX - 1; i <= nBombPointX + 1; i++) {
            for (int j = nBombPointY - 1; j <= nBombPointY + 1; j++) {
                // Check if square is within map bounds
                if (i >= 0 && i < MATRIX_SIZE && j >= 0 && j < MATRIX_SIZE) {
                    // Random chance to hit each square
                    isHit = rand() % 2;
                    if (isHit == 1) {
                        hitMade = 1;
                        if (mat[i][j].landType == WALL) {
                            mat[i][j].landType = LAND;
                        }
                        else if (mat[i][j].hasTank) {
                            printf("Bomb at (%d,%d) - HIT! ", j, i);
                            if (mat[i][j].nTankID != nTankID) {
                                printf("Tank %d is destroyed. Tank %d wins!\n", mat[i][j].nTankID, nTankID);
                            }
                            else {
                                printf("Tank %d hit itself and loses!\n", nTankID);
                            }
                            exit(1);
                        }
                    }
                }
            }
        }      
    }
    else {
        // Target coordinates are outside of the map
        printf("Target coordinates outside of map, cannot apply air bomb.\n");
    }
}

//-----------------------------------------------------------------------------
//                             Initialize Map
//                            ----------------
//
// General : Initializes the game map. Sets the outside border, places walls 
//           at regular intervals, and initializes the starting positions of 
//           the tanks. This function is called at the start of the game.
//
// Parameters : 
//       mat - A two-dimensional array representing the game map.
//
// Return Value : None. The function modifies the passed matrix to represent
//                the initial game state with walls, open land, and tank 
//                positions.
//
//-----------------------------------------------------------------------------
void initializeMap(Land mat[MATRIX_SIZE][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (i == 0 || i == MATRIX_SIZE - 1 || j == 0 || j == MATRIX_SIZE - 1) {
                mat[i][j].landType = OUTSIDE;
                mat[i][j].hasTank = FALSE;
            }
            else {
                mat[i][j].landType = LAND;
                mat[i][j].hasTank = FALSE;
            }
        }
    }

    // X is wall
    for (int i = 2; i < MATRIX_SIZE-1; i += 2) {
        for (int j = 2; j < MATRIX_SIZE-1; j += 2) {
            mat[i][j].landType = WALL;
        }
    }

    mat[6][1].landType = LAND;
    mat[6][1].hasTank = TRUE;
    mat[6][1].nTankID = 1;
    mat[6][11].landType = LAND;
    mat[6][11].hasTank = TRUE;
    mat[6][11].nTankID = 2;
}

//-----------------------------------------------------------------------------
//                             Print Matrix
//                            --------------
//
// General : Prints the current state of the game map to the console. This 
//           function visually represents different elements of the game such as 
//           walls, tanks, and open land, making it easier to understand the 
//           current game situation.
//
// Parameters : 
//       mat - A two-dimensional array representing the game map.
//
// Return Value : None. The function outputs the current state of the game map 
//                to the console. Walls are represented by 'X', tanks by 'O', 
//                the outside border by '.', and open land by blank spaces.
//
//-----------------------------------------------------------------------------
void printMatrix(Land mat[MATRIX_SIZE][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; i++) {  // Corrected bounds to N
        for (int j = 0; j < MATRIX_SIZE; j++) {  // Corrected bounds to N
            if (mat[i][j].landType == WALL) {
                printf("X ");
            }
            else if (mat[i][j].hasTank) {
                printf("O ");
            }
            else if (mat[i][j].landType == OUTSIDE) {
                printf(". ");
            }
            else {
                printf("  ");  // Two spaces for LAND
            }
        }
        printf("\n");
    }
}
