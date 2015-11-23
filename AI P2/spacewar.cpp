// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Draw animated spaceships with collision and shield
// Chapter 6 spacewar.cpp v1.0
// This class is the core of the game

#include "spaceWar.h"
#include<random>
#include<ctime>

//=============================================================================
// Constructor
//=============================================================================
Spacewar::Spacewar()
{
	board=Board(this);
	inputFilter=0;
	targetPiece=nullptr;
}

//=============================================================================
// Destructor
//=============================================================================
Spacewar::~Spacewar()
{
    releaseAll();           // call onLostDevice() for every graphics item
}

//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void Spacewar::initialize(HWND hwnd)
{
	//texture initialization
    Game::initialize(hwnd); // throws GameError
	if (!boardTexture.initialize(graphics,BOARD_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing board texture"));
	if (!piecesTexture.initialize(graphics,PIECES_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing pieces texture"));
	if (!board.initialize(this, boardTexture.getWidth(), boardTexture.getHeight(), 1, &boardTexture))
		 throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing board"));
	for(int i=0;i<boardDims;i++){
		for(int j=0;j<boardDims;j++){
			board.board[i][j].initialize(this,80,80,2,&piecesTexture);
		}
	}

	//place shooters
	srand(time(0));
	int shootersPlaced=0;
	while(shootersPlaced<3){
		int r = rand()%NUM_PIECES+1;
		if(board.board[r/boardDims][r%boardDims].getType()==EMPTY){
			board.board[r/boardDims][r%boardDims].setType(SHOOTER);
			shootersPlaced++;
		}
	}
    return;
}

//=============================================================================
// Update all game items
//=============================================================================
void Spacewar::update()
{
	if(input->getMouseLButton()&&inputFilter==0){
		inputFilter=1;
		int x = (input->getMouseX());
		int y = (input->getMouseY());
		if(x>550|y>550)return;
		if(targetPiece==nullptr)targetPiece=&board.board[x/80][y/80];
		else{
			board.moveShooter(targetPiece->x,targetPiece->y,x/80,y/80);
			targetPiece=nullptr;
		}
	}
	if(!input->getMouseLButton()) inputFilter=0;
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void Spacewar::ai()
{}

//=============================================================================
// Handle collisions
//=============================================================================
void Spacewar::collisions()
{
    
}

//=============================================================================
// Render game items
//=============================================================================
void Spacewar::render()
{
    graphics->spriteBegin();                // begin drawing sprites

	board.draw();

    graphics->spriteEnd();                  // end drawing sprites
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void Spacewar::releaseAll()
{
	boardTexture.onLostDevice();
	piecesTexture.onLostDevice();
    Game::releaseAll();
    return;
}

//=============================================================================
// The grahics device has been reset.
// Recreate all surfaces.
//=============================================================================
void Spacewar::resetAll()
{
    boardTexture.onResetDevice();
    piecesTexture.onResetDevice();
    Game::resetAll();
    return;
}
