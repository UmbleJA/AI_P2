#include "piece.h"
#include "entity.h"
#include "pos.h"
#include <vector>
#include <set>
#include <stack>
using std::set;
using std::vector;
using std::stack;

class Board : public Entity{
public:

	virtual bool initializePieces(Game *gamePtr, int width, int height, int ncols,TextureManager *textureM){
		for(int i=0;i<BOARD_DIMS;i++){
			for(int j=0;j<BOARD_DIMS;j++){
				if(!data[i][j].initialize(gamePtr,width,height,ncols,textureM))return false;
				data[i][j].pos = Pos(i,j);
				data[i][j].setX((i*PIECE_SIZE)+BOARDER_SIZE);
				data[i][j].setY((j*PIECE_SIZE)+BOARDER_SIZE);
			}
		}
		return true;
	}

	void draw(){
		Image::draw();  
		for(int i=0;i<BOARD_DIMS;i++){
			for(int j=0;j<BOARD_DIMS;j++){
				data[i][j].draw();
			}
		}
	}

	Piece* get(Pos pos){
		return &data[pos.getX()][pos.getY()];
	}

	PieceType getType(Pos pos){
		return data[pos.getX()][pos.getY()].getType();
	}

	void setType(Pos pos, PieceType type){
		data[pos.getX()][pos.getY()].setType(type);
	}

	//moves a piece to a location
	bool moveShooter(Piece* piece, Pos pos){
		if(piece->getType()==SHOOTER){
			if(pos==piece->pos)
				return true;
			if(getType(pos)==EMPTY){
				setType(pos,SHOOTER);
				setType(piece->pos,EMPTY);
				return true;
			}
		}
		return false;
	}

	vector<Pos> getPossibleMoves(Pos pos, bool isShooting = false){
		vector<Pos> ret;
		if(getType(pos)==SHOOTER){
			if(!isShooting) ret.push_back(pos);
			//N
			for(int x = pos.getX(),y = pos.getY()-1; isLegal(x,y) && isEmpty(x,y); y--){
				ret.push_back(Pos(x,y));
			}
			//NE
			for(int x = pos.getX()+1,y = pos.getY()-1; isLegal(x,y) && isEmpty(x,y); y--,x++){
				ret.push_back(Pos(x,y));
			}
			//E
			for(int x = pos.getX()+1,y = pos.getY(); isLegal(x,y) && isEmpty(x,y); x++){
				ret.push_back(Pos(x,y));
			}
			//SE
			for(int x = pos.getX()+1,y = pos.getY()+1; isLegal(x,y) && isEmpty(x,y); y++,x++){
				ret.push_back(Pos(x,y));
			}
			//S
			for(int x = pos.getX(),y = pos.getY()+1; isLegal(x,y) && isEmpty(x,y); y++){
				ret.push_back(Pos(x,y));
			}
			//SW
			for(int x = pos.getX()-1,y = pos.getY()+1; isLegal(x,y) && isEmpty(x,y); y++,x--){
				ret.push_back(Pos(x,y));
			}
			//W
			for(int x = pos.getX()-1,y = pos.getY(); isLegal(x,y) && isEmpty(x,y); x--){
				ret.push_back(Pos(x,y));
			}
			//NW
			for(int x = pos.getX()-1,y = pos.getY()-1; isLegal(x,y) && isEmpty(x,y); y--,x--){
				ret.push_back(Pos(x,y));
			}
		}
		return ret;
	}

	vector<Pos> getShooterPos(){
		vector<Pos> ret;
		for(int x=0;x<BOARD_DIMS;x++)
			for(int y=0;y<BOARD_DIMS;y++)
				if(data[x][y].getType()==SHOOTER)
					ret.push_back(Pos(x,y));
		return ret;
	}

	//returns positive for p1, negative for p2
	int getGroupSize(Pos p){
		PieceType target = getType(p);
		if(target==P1||target==P2){
			set<Pos> visited;
			stack<Pos> stk;
			stk.push(p);
			while(!stk.empty()){
				Pos c = stk.top();
				stk.pop();
				visited.insert(c);
				//N
				if(isLegal(c.getX(),c.getY()-1)){
					Pos n(c.getX(),c.getY()-1);
					if(visited.count(n)==0 && getType(n)==target)
						stk.push(n);
				}
				//S
				if(isLegal(c.getX(),c.getY()+1)){
					Pos n(c.getX(),c.getY()+1);
					if(visited.count(n)==0 && getType(n)==target)
						stk.push(n);
				}
				//E
				if(isLegal(c.getX()+1,c.getY())){
					Pos n(c.getX()+1,c.getY());
					if(visited.count(n)==0 && getType(n)==target)
						stk.push(n);
				}
				//W
				if(isLegal(c.getX()-1,c.getY())){
					Pos n(c.getX()-1,c.getY());
					if(visited.count(n)==0 && getType(n)==target)
						stk.push(n);
				}
			}
			//returns positive if player 1, negative if p2
			return visited.size() * (target==P1?1:-1);
		}else return 0;
		
	}

	int getControlValue(Pos p){
		int largest, smallest;
		largest=smallest=0;
		for(int i = -1; i<=1; i++)
			for(int j = -1; j<=1;j++)
				if((i!=0||j!=0) && (i==0||j==0) && isLegal(p.getX()+i,p.getY()+j)){
					Pos c(p.getX()+i,p.getY()+j);
					int d = getGroupSize(c);
					largest = max(largest,d);
					smallest = min(smallest,d);
				}
		return abs(largest)-abs(smallest);		
	}
	
	int getNumEmptyAdjacentSpaces(Pos p){
		int count=0;
		for(int i = -1; i<=1; i++)
			for(int j = -1; j<=1;j++)
				if((i!=0||j!=0) && isLegal(p.getX()+i,p.getY()+j)){
					Pos c(p.getX()+i,p.getY()+j);
					if(getType(c)==EMPTY)count++;
				}
		return count;
	}

private:
	Piece data[BOARD_DIMS][BOARD_DIMS];
	Game* game;

	bool isEmpty(int x, int y){
		return data[x][y].getType()==EMPTY;
	}
	bool isLegal(int x, int y){return x>=0 && x<BOARD_DIMS && y>=0 && y<BOARD_DIMS;}
};