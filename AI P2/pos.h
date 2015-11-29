#pragma once

class ILLIGAL_POS{};

//pos is a safe board position.
class Pos{
public:

	static bool fromRaw(int x, int y, Pos & ret){
		x -= BOARDER_SIZE;
		y -= BOARDER_SIZE;
		x/=PIECE_SIZE;
		y/=PIECE_SIZE;
		if(x < BOARD_DIMS && y < BOARD_DIMS && x>=0 && y>=0){
			ret = Pos(x,y);
			return true;
		}
		return false;
	}

	Pos(unsigned int x, unsigned int y){set(x,y);}
	Pos(){set(0,0);}
	
	void toRaw(int & retX, int & retY){
		retX = BOARDER_SIZE+x*PIECE_SIZE;
		retY = BOARDER_SIZE+y*PIECE_SIZE;
	}

	void set(int x, int y){
		if(x<BOARD_DIMS && y<BOARD_DIMS){
			this->x=x; this->y=y;
		}else throw ILLIGAL_POS();
	}

	unsigned int getX(){return x;}
	unsigned int getY(){return y;}

	//the following let us keep sets of positions
	bool operator< (const Pos & other) const{
		if(x==other.x)
			return y<other.y;
		return x<other.x;
	}
	bool operator==(const Pos& other)const{
		return x==other.x && y==other.y;
	}
	bool operator!=(const Pos& other)const{
		return !(*this==other);
	}

private:
	unsigned int x;
	unsigned int y;
};