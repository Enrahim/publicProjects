#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

struct MoveData {
	vector<vector<vector<int>>> entering;
	vector<vector<vector<int>>> leaving;
	vector<vector<vector<int>>> maxEnter;
	MoveData(int p, int x, int y);
};
	
MoveData::MoveData(int p, int x, int y) : 
	entering(p,vector<vector<int>>(x,vector<int>(y, 0))),
	leaving(p,vector<vector<int>>(x,vector<int>(y, 0))),
	maxEnter(p,vector<vector<int>>(x,vector<int>(y, 0)))
{
	//entering.resize(p);
	//leaving.resize(p);
	//maxEnter.resize(p);
	//for(int i=0; i<p; i++) {
	//	entering[i].resize(x);
	//	leaving[i].resize(x);
	//	maxEnter[i].resize(x);
	//	for(int j=0; j<x; j++) {
	//		entering[i][j].resize(y,0);
	//		leaving[i][j].resize(y,0);
	//		maxEnter[i][j].resize(y,0);
	//	}
	//}
}



class GameBoard {
	vector<vector<vector<int>>> pieces; 
	vector<vector<int>> fortification;
	vector<vector<int>> defendability;
	vector<vector<int>> hospitality;
	int players, width, height;

	void difuse(int p, int x, int y, MoveData& data);
	void difuse(int p, int x1, int y1, int x2, int y2, MoveData& data);
	MoveData difuse();
	void defend(MoveData& data);
	void move(MoveData& data);
	void fight();
	void growth();
	void print(vector<vector<int>>& output);

public:

	GameBoard(int p, int w, int h, int seed);
	void tick();
	void add(player, x, y, number) { pieces[player][x][y]+=number };	
	void printPlayer(int p);

	enum PrintType {
		DEFENDABILITY, HOSPITALITY
	}

	void print(PrintType type) 
				

};


GameBoard::GameBoard(int p, int w, int h, int seed) :
			players(p), width(w), height(h),
			pieces(p,vector<vector<int>>(w,vector<int>(h, 0))),
			defendability(w, vector<int>(h, 2)),
			hospitality(w, vector<int>(h, 3)),
			fortification(w, vector<int>(h, 0))
{
}


void GameBoard::tick(){
	MoveData data(difuse());
	defend(data);
	move(data);
	fight();
	growth();
}

void GameBoard::print(PrintType type){
	switch(type) {
		case DEFENDABILITY:
			print(defendability);
			break;
		case HOSPITALITY:
			print(hospitality);
			break;
	}
}


void GameBoard::printPlayer(int p) {
	if(p>=players||p<0) return;
	print(pieces[p]);
}

void GameBoard::print(vector<vector<int>>& output) {
	for(int x=0; x<width; x++) {
		for(int y=0; y<height; y++) {
			cout << setw(5) << output[x][y];
		}
		cout<<endl;
	}
}

void GameBoard::difuse(int p, int x1, int y1, int x2, int y2, MoveData& data) {
	int delta=hospitality[x2][y2]*pieces[p][x1][y1]-hospitality[x1][y1]*pieces[p][x2][y2];
	delta/=(hospitality[x1][y1]+hospitality[x2][y2])*9;
	if(delta==0) return;
	if(delta>0) {
		data.entering[p][x2][y2]+=delta;
		if(delta>data.maxEnter[p][x2][y2]) data.maxEnter[p][x2][y2]=delta;
		data.leaving[p][x1][y1]+=delta;
	} else {
		data.entering[p][x1][y1]+=delta;
		if(delta>data.maxEnter[p][x1][y1]) data.maxEnter[p][x1][y1]=delta;
		data.leaving[p][x2][y2]+=delta;
	}
}


void GameBoard::difuse(int p, int x, int y, MoveData& data) {
	if(x+1<width) {
		difuse(p, x, y, x+1, y, data);
		if(y+1<height) {
			difuse(p, x, y, x+1, y+1, data);
		}
	} 
	if(y+1<height) {
		difuse(p, x, y, x, y+1, data);
		if(x>0) {
			difuse(p, x, y, x-1, y+1, data);
		}
	}
}

MoveData GameBoard::difuse() {
	MoveData data(players, width, height);
	for(int p=0; p<players; p++) {
		for(int x=0; x<width; x++) {
			for(int y=0; y<height; y++) {
				if(pieces[p][x][y]>0) {
					difuse(p, x, y, data);
				}
			}
		}
	}
	return data;
}

void GameBoard::growth() {
	for(int p=0; p<players; p++) {
		for(int x=0; x<width; x++) {
			for(int y=0; y<height; y++) {
				if(pieces[p][x][y]<=0) continue;
				int add;
				_BitScanForward(&add, pieces[p][x][y]);
				pieces[p][x][y]+=add;
			}
		}	
	}
}



void GameBoard::fight() {
	for(int x=0; x<width; x++) {
		for(int y=0; y<height; y++) {
			vector<int> dying.resize(players);
			for(int p1=0; p1<players-1; p1++) {
				if(pieces[p1][x][y]==0) continue;
				for(int p2=p1+1; p2<players; p1++) {
					if(pieces[p2][x][y]==0) continue;
					//TODO! Problematic with more than 2 (now it is standoff if equal forces)
					int f1=pieces[p1][x][y];
					int f2=pieces[p2][x][y];
					if(f1>f2) {
						int dif=f1-f2;
						if(dif>f2){
							dying[p2]=f2;
							dying[p1]+=(f2*f2)/dif;
						} else {
							dying[p1]+=dif;
							dying[p2]+=dif;
						}
					} else {
						int dif=f2-f1;
						if(dif>f1){
							dying[p1]=f2;
							dying[p2]+=(f1*f1)/dif;
						} else {
							dying[p1]+=dif;
							dying[p2]+=dif;
						}
					}
				}
			}
			for(int p=0; p<players; p++) {
				if(pieces[p][x][y]<dying[p]) pieces[p][x][y]=0;
				else pieces[p][x][y]-=dying[p];
			}
		}
	}
}


void GameBoard::defend(MoveData& data)
{
	for(int x=0; x<width; x++) {
		for(int y=0; y<height; y++) {
			int maxplayer=-1;
			int maxown=0;
			for(int p=0; p<players; p++) {
				if(pieces[p][x][y]>maxown) {
					maxplayer=p;
					maxown=pieces[p][x][y];
				}
			}
			for(int p=0; p<players; p++) {
				if(p==maxplayer) continue;
				if(data.entering[p][x][y]>0) {
					int dying=(pieces[maxplayer][x][y]-pieces[p][x][y])*
								(fortification[x][y]+defendability[x][y])
								*data.maxEnter[p][x][y]/data.entering[p][x][y];
					if(dying>=data.entering[p][x][y]) data.entering[p][x][y]=0;
					else data.entering[p][x][y]-=dying;
				}
			}
		}
	}
}

void GameBoard::move(MoveData& data)
{
	for(int player=0; player<players; player++) {
		for(int x=0; x<width; x++) {
			for(int y=0; y<height; y++) {
				pieces[player][x][y]+=data.entering[player][x][y]-data.leaving[player][x][y];
			}
		}
	}
}

void initiate(GameBoard& board) {
	cout << "Chose number of players ";
	int players;
	cin >> players;
	cout << "Choose starting number ";
	int starter;
	cin >> starter;

	board.print(GameBoard::HOSPITALIY);
	board.print(GameBoard::DEFENDABILITY);

	for(int i=0; i<players; i++) {
		cout<<"Select starting position for player " << i << "(x y) ";
		int x, y;
		cin >> x >> y;
		board.add(i, x, y, starter)
	}
}

void playerMapMenu(GameBoard board) {
	cout << endl << "player number: ";
	int player;
	cin >> player;
	board.printPlayer(player);
}

void mapMenu(GameBoard& board) {
	cout << endl<<"Do you want to see:"<<endl;
	cout << "1: Hospitality map" << endl;
	cout << "2: Defendability map" << endl;
	cout << "3: Player map" <<endl;
	int choice;
	cin >> choice;
	switch(choice) {
		case 1:
			board.print(GameBoard::HOSPITALITY);
			break;
		case 2:
			board.print(GameBoard::DEFENDABILITY);
			break;
		case 3:
			playerMapMenu(board);
			break;
	}
}


bool mainMenu(GameBoard& board) {
	while(true) {
		cout << "Do you want to:" <<endl;
		cout << "0: Quit" <<endl;
		cout << "1: Look at map" << endl;
		cout << "2: End turn" << endl;
		cout << "Selection ";
		int choice;
		cin >> choice;
		switch(choice) {
			case 0:
				return false;
			case 1:
				MapMenu(board);
				break;
			case 2:
				return true;
		}
	}
}

void gameLoop(GameBoard& board) {
	bool playing=true;
	while(playing) {
		playing=mainMenu(board);
	}
}

void main(int argc, char* argv) {
	GameBoard board(2,5,5);
	initiate(board);
	gameLoop(board);
}
