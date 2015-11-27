// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 6 ship.h v1.0

#ifndef _PIECE_H                 // Prevent multiple definitions if this 
#define _PIECE_H               // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "constants.h"
#include "pos.h"

// inherits from Entity class
class Piece : public Entity
{
	PieceType type;
public:
    // constructors
	Piece(PieceType pt, Pos p):pos(p){setType(pt);}
	Piece():pos(0,0){setType(EMPTY);}

	Pos pos;

	void setType(PieceType t){
		type=t;
		switch(type){
		case EMPTY:
			setCurrentFrame(3);
			break;
		case P1:
			setCurrentFrame(0);
			break;
		case P2:
			setCurrentFrame(1);
			break;
		case SHOOTER:
			setCurrentFrame(2);
			break;
		}
	}

	PieceType getType(){
		return type;
	}

};
#endif

