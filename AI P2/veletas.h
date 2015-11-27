// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 6 Veletas.h v1.0

#ifndef _Veletas_H             // Prevent multiple definitions if this 
#define _Veletas_H             // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "game.h"
#include "textureManager.h"
#include "image.h"
#include "board.h"
#include "piece.h"
#include <vector>
#include <string>
#include "TextDX.h"
using std::vector;
using std::string;

//=============================================================================
// This class is the core of the game
//=============================================================================
enum TurnSequence{SELECTING,MOVING,SHOOTING};

class Veletas : public Game
{
private:
    //// game items
    //TextureManager nebulaTexture;   // nebula texture
    //TextureManager gameTextures;    // game texture
    //Ship    ship1, ship2;           // spaceships
    //Planet  planet;         // the planet
    //Image   nebula;         // backdrop image
	TextureManager boardTexture;
	TextureManager piecesTexture;
	Board board;
	Piece* targetPiece;
	Image highlights[NUM_HIGHLIGHTS];
	vector<Pos> possibleMoves;
	TextDX text;
	std::string textInfo;
	std::string textScore;

	bool isPlayer1Turn;
	
	TurnSequence turnSequence;

	bool isPlayer1AI,isPlayer2AI;

	int player1Score, player2Score;

	bool isHumanMove(){return isPlayer1Turn&&!isPlayer1AI || !isPlayer1Turn&&!isPlayer2AI;}
	bool isAIMove(){return isPlayer1Turn&&isPlayer1AI || !isPlayer1Turn&&isPlayer2AI;}

	void checkEndOfTurn();

public:
    // Constructor
    Veletas();
	bool inputFilter;


    // Destructor
    virtual ~Veletas();

    // Initialize the game
    void initialize(HWND hwnd);
    void update();      // must override pure virtual from Game
    void ai();          // "
    void collisions();  // "
    void render();      // "
    void releaseAll();
    void resetAll();

	void setHighlights(vector<Pos> & possibleMoves);
	void clearHighlights();

	bool contains(const vector<Pos> & vec, const Pos pos) const;
};

#endif
