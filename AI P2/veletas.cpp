// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Draw animated spaceships with collision and shield
// Chapter 6 Veletas.cpp v1.0
// This class is the core of the game

#include "veletas.h"
#include<random>
#include<ctime>

//=============================================================================
// Constructor
//=============================================================================
Veletas::Veletas():targetPos(0,0)
{
	inputFilter=0;
	isPlayer1Turn=true;
	turnSequence = SELECTING;
	isPlayer1AI=isPlayer2AI=false;//TODO:OPTIONAL
	player1Score=player2Score=0;
}

//=============================================================================
// Destructor
//=============================================================================
Veletas::~Veletas()
{
    releaseAll();           // call onLostDevice() for every graphics item
}

//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void Veletas::initialize(HWND hwnd)
{
	//texture initialization
    Game::initialize(hwnd); // throws GameError
	if (!boardTexture.initialize(graphics,BOARD_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing board texture"));
	if (!piecesTexture.initialize(graphics,PIECES_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing pieces texture"));

	if(!background.initialize(graphics,0,0,0,&boardTexture))
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing background"));

	for(int i=0;i<NUM_PIECES;i++){
		if(!pieces[i].initialize(graphics,PIECE_SIZE,PIECE_SIZE,2,&piecesTexture))
			throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing pieces"));
	}

	for(int i = 0 ; i < NUM_HIGHLIGHTS; i++){
		if(!highlights[i].initialize(graphics,PIECE_SIZE,PIECE_SIZE,2,&piecesTexture))
			throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing highlights"));
		highlights[i].setVisible(false);
	}

	if(!text.initialize(graphics,20,true,false,"Verdana")){
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing text"));
	}

	//place shooters
	srand(time(0));
	int shootersPlaced=0;
	while(shootersPlaced<3){
		Pos p(rand()%BOARD_DIMS,rand()%BOARD_DIMS);
		if(board.getType(p)==EMPTY){
			board.setType(p,SHOOTER);
			shootersPlaced++;
		}
	}
	checkEndOfTurn();
	refreshPieces();
    return;
}

//=============================================================================
// Update all game items
//=============================================================================
void Veletas::update()
{

	if(isPlayer1Turn)
		textInfo = "Green Turn: ";
	else
		textInfo = "Blue Turn: ";

	if(turnSequence==SELECTING)
		textInfo += "Select a shooter to move.";
	else if (turnSequence==MOVING)
		textInfo += "Move the selected shooter.";
	else if (turnSequence==SHOOTING)
		textInfo += "Shoot a peice of your color.";
		
	if(isHumanMove()){
		if(input->getMouseLButton()&&inputFilter){
			inputFilter=false;
			int x = (input->getMouseX());
			int y = (input->getMouseY());
			Pos p(0,0);
			if(Pos::fromRaw(x,y,p))
			{
				if(turnSequence==SELECTING){
					if(board.getType(p)==SHOOTER){
						targetPos = p;
						possibleMoves = board.getPossibleMoves(p);
						turnSequence=MOVING;
						clearHighlights(); setHighlights(possibleMoves);//must call after
					}
				}
				else if(turnSequence==MOVING){
					if(contains(possibleMoves,p)){
						board.moveShooter(targetPos,p);
						possibleMoves=board.getPossibleMoves(p,true);
						turnSequence=SHOOTING;
						clearHighlights(); setHighlights(possibleMoves);//must call after
						refreshPieces();
					}
				}
				else if (turnSequence==SHOOTING){
					if(contains(possibleMoves,p)){
						board.setType(p,(isPlayer1Turn?P1:P2));
						clearHighlights();
						turnSequence=SELECTING;
						checkEndOfTurn();//must be called before bool flips
						isPlayer1Turn=!isPlayer1Turn;
						refreshPieces();
					}
				}

			}
		}
	}
	if(isAIMove()){
	//TODO: Make the AI move
	}
	if(!input->getMouseLButton()) inputFilter=true;
}

void Veletas::setHighlights(vector<Pos> & possibleMoves){

	for(int i = 0; i<min(possibleMoves.size(),NUM_HIGHLIGHTS);i++){
		int x, y;
		possibleMoves[i].toRaw(x,y);
		highlights[i].setX(x);
		highlights[i].setY(y);
		highlights[i].setVisible(true);
		if(turnSequence==MOVING)
			highlights[i].setCurrentFrame(2);
		if(turnSequence==SHOOTING)
			if(isPlayer1Turn)
				highlights[i].setCurrentFrame(0);
			else
				highlights[i].setCurrentFrame(1);

	}
	
}
void Veletas::clearHighlights(){
	for(int i = 0 ; i<NUM_HIGHLIGHTS; i++)
		highlights[i].setVisible(false);
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void Veletas::ai()
{}

//=============================================================================
// Handle collisions
//=============================================================================
void Veletas::collisions()
{
    
}

//=============================================================================
// Render game items
//=============================================================================
void Veletas::render()
{
    graphics->spriteBegin();                // begin drawing sprites

	background.draw();

	for(int i=0;i<NUM_PIECES;i++)
		pieces[i].draw();

	for(int i=0; i<NUM_HIGHLIGHTS;i++)
		highlights[i].draw(0x4FFFFFFF);
	
	text.print(textInfo,BOARDER_SIZE,0);
	text.print(textScore,BOARDER_SIZE,GAME_HEIGHT-BOARDER_SIZE);
    graphics->spriteEnd();                  // end drawing sprites
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void Veletas::releaseAll()
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
void Veletas::resetAll()
{
    boardTexture.onResetDevice();
    piecesTexture.onResetDevice();
    Game::resetAll();
    return;
}

bool Veletas::contains(const vector<Pos> & vec, const Pos pos) const{
	for(const Pos & p: vec)
		if(p==pos) return true;
	return false;
}

void Veletas::checkEndOfTurn(){
	vector<Pos> shooters = board.getShooterPos();
	for(Pos p : shooters)
		if(board.getNumEmptyAdjacentSpaces(p)==0){
			int val = board.getControlValue(p);
			if(val==0){
				if(isPlayer1Turn){
					player2Score++;
					board.setType(p,P2);
				}else{
					player1Score++;
					board.setType(p,P1);
				}
			}else{
				if(val>0){
					player1Score++;
					board.setType(p,P1);
				}else{
					player2Score++;
					board.setType(p,P2);
				}
			}
		}
	textScore = "Green: " + std::to_string(player1Score) + "     Blue: " + std::to_string(player2Score);

}

void Veletas::refreshPieces(){
	for(int i=0; i<BOARD_DIMS; i++)
		for(int j=0; j<BOARD_DIMS; j++){
			Pos p(i,j);
			Image * img = &pieces[i*BOARD_DIMS+j];
			int x,y;
			p.toRaw(x,y);
			img->setCurrentFrame((int)board.getType(p));
			img->setX(x); img->setY(y);
		}
}