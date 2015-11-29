#pragma once
#include "board.h"
#include "constants.h"

static int garbage[BOARD_DIMS][BOARD_DIMS];

class MinMax{
	int goodness, depth;
	Board* board;
	Pos bestChildMoveFrom, bestChildMoveTo,bestChildShoot;//best move from this board
	bool isMax;
	bool validChildVals;
	bool traversedToEndOfGame;

	MinMax(MinMax * parent, Board* b){
		board = b;
		depth = parent->depth+1;
		isMax = !parent->isMax;
		goodness=0;
		validChildVals=false;
		traversedToEndOfGame=false;
	}

public:
	MinMax(Board* b, bool isMax){
		validChildVals=false;
		traversedToEndOfGame=false;
		board = b;
		depth = 0;
		this->isMax=isMax;
		goodness=0;
	}

	int findBestMoves(Pos & moveFrom, Pos & moveTo, Pos & shoot){
		traversedToEndOfGame=false;
		traverseTree();
		if(!traversedToEndOfGame)heuristicNextMove();
		moveFrom = bestChildMoveFrom;
		moveTo = bestChildMoveTo;
		shoot = bestChildShoot;
		return goodness;
	}

	int getGoodness(){return goodness;}

private:
	
	void heuristicNextMove(){
		for(Pos moveFrom : board->getShooterPos())
				for(Pos moveTo : board->getPossibleMoves(moveFrom)){
					Board cpy(*board);
					cpy.moveShooter(moveFrom,moveTo);
					for(Pos shoot : cpy.getPossibleMoves(moveTo,true)){
						cpy.setType(shoot, (isMax?P1:P2) );
						cpy.processBoardEndOfTurn(isMax);
						int val = cpy.getGoodness(garbage);
						if((isMax && val>goodness) || (!isMax && val<goodness)){
							goodness=val;
							bestChildMoveFrom=moveFrom;
							bestChildMoveTo=moveTo;
							bestChildShoot=shoot;
						}
						cpy=*board;cpy.moveShooter(moveFrom,moveTo);
					}
				}
	}

	void traverseTree(){
		if(board->isGameOver())
			traversedToEndOfGame=true;
		if(depth==MOVES_LOOKAHEAD || board->isGameOver()){
			goodness = (board->getPlayer1Score()-board->getPlayer2Score())*1000;
		}else{
			for(Pos moveFrom : board->getShooterPos())
				for(Pos moveTo : board->getPossibleMoves(moveFrom)){
					Board cpy(*board);
					cpy.moveShooter(moveFrom,moveTo);
					for(Pos shoot : cpy.getPossibleMoves(moveTo,true)){
						cpy.setType(shoot, (isMax?P1:P2) );
						cpy.processBoardEndOfTurn(isMax);
						//min has now generated a board representing one of his moves
						MinMax child(this,&cpy);
						child.traverseTree();
						//reset cpy
						cpy = *board;cpy.moveShooter(moveFrom,moveTo);
						//if the child is best	
						if(!validChildVals || (isMax && child.goodness > goodness) || (!isMax&& child.goodness < goodness)){
							bestChildMoveFrom=moveFrom;
							bestChildMoveTo=moveTo;
							bestChildShoot=shoot;
							goodness=child.goodness;
							validChildVals=true;
							traversedToEndOfGame=child.traversedToEndOfGame;
						}
					}
				}
		}
	}
};