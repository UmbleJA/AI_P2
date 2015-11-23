#include "piece.h"
#include "entity.h"

class Board : public Entity{
public:
	Piece board[boardDims][boardDims];
	Game* game;

	Board(){};

	Board(Game* g){
		spriteData.x=0;
		spriteData.y=0;
		for(int i=0;i<boardDims;i++){
			for(int j=0;j<boardDims;j++){
				board[i][j]=Piece(EMPTY,i,j);
				board[i][j].setX((i*80)+20);
				board[i][j].setY((j*80)+20);
			}
		}
	};

	void draw(){
		Image::draw();  
		for(int i=0;i<boardDims;i++){
			for(int j=0;j<boardDims;j++){
				board[i][j].draw();
			}
		}
	}

	bool openMove(int x1, int y1, int x2, int y2){
	}

	//moves a piece FROM location 1 TO location 2
	bool moveShooter(int x1, int y1, int x2, int y2){
		if(x1<0||x1>=boardDims||y1<0||y1>=boardDims||
			x2<0||x2>=boardDims||y2<0||y2>=boardDims)
			return 0;
		if(board[x1][y1].getType()==EMPTY) return 0;
		if(board[x2][y2].getType()!=EMPTY) return 0;
		board[x2][y2].setType(board[x1][y1].getType());
		if(&board[x1][y1]!=&board[x2][y2])board[x1][y1].setType(EMPTY);
		return 1;
	}

	bool addPiece(int x1, int y1, pieceType t){
		if(board[x1][y1].getType()!=EMPTY) return 0;
		board[x1][y1].setType(t);
	}

	bool removePiece(int x1, int y1){
		if(board[x1][y1].getType()==EMPTY) return 0;
		board[x1][y1].setType(EMPTY);
	}

	bool replacePiece(int x1, int y1, pieceType t){
		if(board[x1][y1].getType()==EMPTY) return 0;
		board[x1][y1].setType(t);
	}

	virtual bool initialize(Game *gamePtr, int width, int height, int ncols,TextureManager *textureM){
		return Entity::initialize(gamePtr,width,height,ncols,textureM);
	}
};