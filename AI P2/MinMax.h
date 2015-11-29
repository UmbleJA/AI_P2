#pragma once
#include "board.h"
#include "constants.h"

static int garbage[BOARD_DIMS][BOARD_DIMS];

class MinMax{
	int score, depth, heuristicVal;
	Board* board;
	Pos bestChildMoveFrom, bestChildMoveTo,bestChildShoot;//best move from this board
	bool isMax;
	bool validChildVals;
	bool foundNextPoint;

	MinMax(MinMax * parent, Board* b){
		board = b;
		depth = parent->depth+1;
		isMax = !parent->isMax;
		score=0;
		validChildVals=false;
		foundNextPoint=false;
		heuristicVal=parent->heuristicVal;
	}

public:
	MinMax(Board* b, bool isMax){
		validChildVals=false;
		foundNextPoint=false;
		board = b;
		depth = 0;
		this->isMax=isMax;
		score=board->getPlayer1Score()-board->getPlayer2Score();
		heuristicVal=0;
	}

	int findBestMoves(Pos & moveFrom, Pos & moveTo, Pos & shoot){
		foundNextPoint=false;
		traverseTree();
		moveFrom = bestChildMoveFrom;
		moveTo = bestChildMoveTo;
		shoot = bestChildShoot;
		return score;
	}

private:

	void traverseTree(){
		if(depth==1)
			heuristicVal = board->getGoodness(garbage);
		if(depth==MOVES_LOOKAHEAD || board->isGameOver()){
			score = (board->getPlayer1Score()-board->getPlayer2Score());
		}else{
			for(Pos moveFrom : board->getShooterPos())
				for(Pos moveTo : board->getPossibleMoves(moveFrom)){
					Board cpy(*board);
					cpy.moveShooter(moveFrom,moveTo);
					for(Pos shoot : cpy.getPossibleMoves(moveTo,true)){
						cpy.setType(shoot, (isMax?P1:P2) );
						cpy.processBoardEndOfTurn(isMax);
						if(cpy.getPlayer1Score()!=board->getPlayer1Score() ||
							cpy.getPlayer2Score()!=board->getPlayer2Score())
								foundNextPoint=true;
						//min has now generated a board representing one of his moves
						MinMax child(this,&cpy);
						child.traverseTree();
						//reset cpy
						cpy = *board;cpy.moveShooter(moveFrom,moveTo);
						//if the child is best	
						if(!validChildVals || (isMax && child.score > score) || (!isMax&& child.score < score)
							|| (score==child.score && ((isMax && child.heuristicVal>heuristicVal) || (!isMax && child.heuristicVal<heuristicVal) ))
							){
							bestChildMoveFrom=moveFrom;
							bestChildMoveTo=moveTo;
							bestChildShoot=shoot;
							score=child.score;
							validChildVals=true;
							foundNextPoint=child.foundNextPoint;
							heuristicVal = child.heuristicVal;
						}
						
					}
				}
		}
	}
};