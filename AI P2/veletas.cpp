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
	isGameActive=true;
	inputFilter=0;
	isPlayer1Turn=true;
	turnSequence = SELECTING;
	isPlayer1AI= false;
	isPlayer2AI=true;
	for(int i=0;i<BOARD_DIMS;i++)
		for(int j=0;j<BOARD_DIMS;j++)
			debugGoodness[i][j]=0;
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

	if(!text.initialize(graphics,BOARDER_SIZE,true,false,"Verdana")){
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing text"));
	}

	//place shooters
	srand(time(0));
	int shootersPlaced=0;
	while(shootersPlaced<NUM_SHOOTERS){
		Pos p(rand()%BOARD_DIMS,rand()%BOARD_DIMS);
		if(board.getType(p)==EMPTY && board.getNumAdjacentSpaces(p)==board.getNumEmptyAdjacentSpaces(p)){
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
	static bool skipFirstFrame = true;
	if(skipFirstFrame){
		skipFirstFrame=false;
		return;
	}

	if(!isGameActive)return;

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
	} else if(isAIMove()){
	//TODO: Make the AI move
		static Pos moveFrom, moveTo, shoot;
		static float aiMovementLag = 0;
		aiMovementLag+=frameTime;
		if(turnSequence==SELECTING || aiMovementLag >= AI_MOVEMENT_TIME){
			aiMovementLag=0;
			if(turnSequence==SELECTING){
				MinMax minMax(&board,isPlayer1Turn);
				minMax.findBestMoves(moveFrom,moveTo,shoot);
				targetPos = moveFrom;
				possibleMoves = board.getPossibleMoves(moveFrom);
				turnSequence=MOVING;
				clearHighlights(); setHighlights(possibleMoves);//must call after
			
			} else if(turnSequence==MOVING){
				board.moveShooter(targetPos,moveTo);
				possibleMoves=board.getPossibleMoves(moveTo,true);
				turnSequence=SHOOTING;
				clearHighlights(); setHighlights(possibleMoves);//must call after
				refreshPieces();
			
			} else if(turnSequence==SHOOTING){
				board.setType(shoot,(isPlayer1Turn?P1:P2));
				clearHighlights();
				turnSequence=SELECTING;
				checkEndOfTurn();//must be called before bool flips
				isPlayer1Turn=!isPlayer1Turn;
				refreshPieces();
			}
		}

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
	
	text.setFontColor(0xFFFFFFFF);
	text.print(textInfo,BOARDER_SIZE,0);
	//text.print(debugText,BOARDER_SIZE,0);
	text.print(textScore,BOARDER_SIZE,GAME_HEIGHT-BOARDER_SIZE);

	text.setFontColor(0xFF000000);
	for(int i=0;i<BOARD_DIMS;i++)
		for(int j=0;j<BOARD_DIMS;j++)
			text.print(std::to_string(debugGoodness[i][j]),BOARDER_SIZE+PIECE_SIZE*i,BOARDER_SIZE+PIECE_SIZE*j);

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
	board.processBoardEndOfTurn(isPlayer1Turn);
	if(board.getPlayer1Score()==POINTS_TO_WIN){
		textScore = "GREEN WINS!";
		isGameActive=false;
	}
	else if (board.getPlayer2Score()==POINTS_TO_WIN){
		textScore = "BLUE WINS!";
		isGameActive=false;
	}
	else{
		int goodness = board.getGoodness(debugGoodness);
		textScore = "Green: " + std::to_string(board.getPlayer1Score()) + "     Blue: " + std::to_string(board.getPlayer2Score()) + "       Goodness: "+std::to_string(goodness);
	}
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