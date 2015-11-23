// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 6 ship.h v1.0

#ifndef _PIECE_H                 // Prevent multiple definitions if this 
#define _PIECE_H               // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "constants.h"

// inherits from Entity class
class Piece : public Entity
{
	pieceType type;
public:
    // constructors
	Piece(pieceType pt,int nx, int ny){setType(pt);x=nx;y=ny;};
	Piece(){type=EMPTY;x=0;y=0;};
    // inherited member functions
    virtual void draw(){
		Image::draw();
	}

    virtual bool initialize(Game *gamePtr, int width, int height, int ncols,TextureManager *textureM){
		return Entity::initialize(gamePtr,width,height,ncols,textureM);
	}

	bool inUse;
	int x,y;
	void setType(pieceType t){
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

	pieceType getType(){
		return type;
	}

};
#endif

