#include <iostream>
#include <bits/stdc++.h>

using namespace std;

// board[x][y] means here xth column, yth row 

// 0 means empty 
// 1 means white 
// 2 means black
// -1 means white_townhall
// -2 means black_townhall

// direction = 1 for white
// direction = -1 for black
// self = (direction*(-1) + 3 ) / 2
// enemy = (direction + 3 ) / 2

int columns = 8; // config
int rows = 10;

// float A = 3.5;  // my players - his players
// float B = 20;  // townhall diff
// float C = 1; // enemy shootable diff
// float D = 0.5; // empty shootable diff
// float E = 0; // Attacking Man !! not required if handling unstable schenario
// float F = -1; // Avg Distance 
// float G = -1; // Min Distance


float A = 5;  // my players - his players
float B = 20;  // townhall diff
float C = 1; // enemy shootable diff
float D = 0.5; // empty shootable diff
float E = 0; // Attacking Man !! not required if handling unstable schenario
float F = -1; // Avg Distance 
float G = -0.5; // Min Distance


float win = 100; // win is multilpied to the terminal utitlity

// The parameters can be different as per onfig

int CAmoves = 0;
int CAmoves_avail = 0;
int Cmoves = 0;
int Cmoves_avail = 0;
int Amoves = 0;
int Amoves_avail = 0;
int Fmoves = 0;
int Fmoves_avail = 0;
int Bmoves = 0;
int Bmoves_avail = 0;

int bot;

vector<int> neighbour_x = {1,1,1,0,0,-1,-1,-1};
vector<int> neighbour_y = {1,0,-1,1,-1,1,0,-1};

vector<int> neighAtt_x = {1,1,0,-1,-1};
vector<int> neighAtt_y = {1,0,1,0,1};

vector<int> forward_x = {-1,0,1};
vector<int> forward_y = {1,1,1};

vector<int> attack_x = {-1,-1,0,1,1};
vector<int> attack_y = {0,1,1,0,1};

vector<int> support_x = {-1,-1,0,1,1};
vector<int> support_y = {0,-1,-1,0,-1};

vector<int> backward_x = {-2,0,2};
vector<int> backward_y = {-2,-2,-2};

vector<int> cannon_x = {-3,-3,-3,0,0,3,3,3};
vector<int> cannon_y = {-3,0,3,-3,3,-3,0,3};

vector<int> cannon_middle_x1 = {1,1,0,-1};
vector<int> cannon_middle_y1 = {0,1,1,1};
vector<int> cannon_middle_x2 = {-1,-1,0,1};
vector<int> cannon_middle_y2 = {0,-1,-1,-1};

vector<int> townhall_white_x = {0,2,4,6}; // config
vector<int> townhall_white_y = {0,0,0,0};
vector<int> townhall_black_x = {1,3,5,7};
vector<int> townhall_black_y = {9,9,9,9};

struct bmove{
    char type;
    int x1;
    int y1;
    int x2;
    int y2;
    string tp = ""; // change for ordering 
    bmove(){}
    bmove(char type, int x1, int y1, int x2, int y2){
        this->type = type;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    }
    bmove(char type, int x1, int y1, int x2, int y2, string move_type){ // 
        this->type = type;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->tp = move_type;  
    }
};

struct miniMaxAns{
    float utility;
    bmove move;
    miniMaxAns(){
        this->utility = 0.0;
        this->move = bmove('M',0,0,1,1); // just to check the end case remove this ***
    }
};

struct position{
    int x;
    int y;
    position(){
    }
    position(int x, int y){
        this->x = x;
        this->y = y;
    }
};

struct board_view{
    vector< vector <int> > board_view;
    vector< position> white_players;
    vector< position> black_players;
    int num_white_townhalls;
    int num_black_townhalls;
};



int sgn(int x)
{
    if(x > 0)
        return 1;
    else if(x < 0)
        return -1;
    else 
        return 0;
}

bool onBoard(int x, int y)
{
    if(x < columns && y < rows && x >= 0 && y >= 0)
        return true;
    else
        return false;
}

class board{
    public:
    board_view c_board;
    vector<board> next_configurations;
    bool max_chance;
    bool childrenComplete;
    int direction;

    board() {} // empty constructor;
    board(board_view bv,int x1, int y1,  char move_type , int x2, int y2, int direc){ // can be called to make a child ... Given a particular move
        // bool maxMoved will be true if the new board formed has chance of min and vice versa        
        //  direc will be direction of the kid or the new board
        this->c_board = bv;
        this->direction = direc;

        vector<position> moved; // just to help as variables 
        vector<position> killed;

        if (direc == 1) // white has to move
        {
            moved = this->c_board.black_players;
            killed = this->c_board.white_players;
        }
        else if(direc == -1) // black has to move 
        {
            moved = this->c_board.white_players;
            killed = this->c_board.black_players;
        }

        // cout << x1 << " " << y1 << " " << x2 << " " <<  y2 << " " << move_type << endl; 

        if(move_type == 'M') //  MOVE from x1,x2 to y1,y2
        {
            // cout << "HI_1" << endl;
            if(this->c_board.board_view[x2][y2] != 0) // somebody was killed Let's first update the killed ones
            {
                // cout << "HI_2" << endl;
                if(this->c_board.board_view[x2][y2] == -2)
                {
                    this->c_board.num_black_townhalls-=1;
                }
                else if(this->c_board.board_view[x2][y2] == -1)
                {
                    this->c_board.num_white_townhalls-=1;
                }

                else
                {
                    // cout << "HI_4" << endl;
                    for (int i = 0; i < killed.size(); i++)
                    {
                        if (killed[i].x == x2 && killed[i].y == y2)
                        {
                            killed.erase(killed.begin()+i);
                        }
                    }
                }
                
            }
            // Let's update the actual move
            this->c_board.board_view[x2][y2] = this->c_board.board_view[x1][y1];
            this->c_board.board_view[x1][y1] = 0;
            // update the player who just moved in the list aswell
            for (int i = 0; i < moved.size(); i++)
            {
                if (moved[i].x == x1 && moved[i].y == y1)
                {
                    moved[i].x = x2;
                    moved[i].y = y2;
                }
            }

        }
        else if(move_type == 'B') // somebody could have been killed 
        {
            if (this->c_board.board_view[x2][y2] != 0) // somebody was actually killed 
            {
                if(this->c_board.board_view[x2][y2] == -2)
                {
                    this->c_board.num_black_townhalls-=1;
                }
                else if(this->c_board.board_view[x2][y2] == -1)
                {
                    this->c_board.num_white_townhalls-=1;
                }
                else
                {
                    for (int i = 0; i < killed.size(); i++) // remove it from the list
                    {
                        if (killed[i].x == x2 && killed[i].y == y2)
                        {
                            killed.erase(killed.begin()+i);
                        }                    
                    }
                }
                this->c_board.board_view[x2][y2] = 0; // remove it from the board view
            }
        }
        else if(move_type == 'N') // NONE 
        {
        }

        if (direc == 1) // white has to move
        {
            this->c_board.black_players = moved ;
            this->c_board.white_players = killed;
        }
        else if(direc == -1) // black has to move 
        {
            this->c_board.white_players = moved;
            this->c_board.black_players = killed;
        }

    }

    board(board_view bv, int direc){ // can be called to make a child ... Given a particular move
        this->c_board = bv;
        this->direction = direc;
    }  

    bool enemyNearby(position pos){
        bool ans = false;
        for (int i = 0; i < neighbour_x.size(); i++)    
        {   
            int check_x = neighbour_x[i]+ pos.x;
            int check_y = neighbour_y[i]+pos.y ;
                if(onBoard(check_x,check_y))
                {
                    int enemy = (this->direction + 3) / 2;
                    if (this->c_board.board_view[check_x][check_y] == enemy)
                    {
                        ans = true;
                        return ans;
                    }                       
                }
        }
        return ans;
    }

    bool retreatAble(position pos) // check if enemy nearby that can attack us (Slight difference from enemy Nearby)
    {
        bool ans = false;
        int enemy = (this->direction+3)/2;
        for (size_t i = 0; i < neighAtt_x.size(); i++)
        {
            int check_x = ((this->direction)*neighAtt_x[i]) + pos.x;
            int check_y = ((this->direction)*neighAtt_y[i]) + pos.y ;
            if(onBoard(check_x,check_y))
            {
                if (this->c_board.board_view[check_x][check_y] == enemy)
                {
                    ans = true;
                    return ans;
                }                       
            }
        }
        return ans;
    }

    bool retreatAbleColour(position pos,bool black) // check if enemy nearby that can attack us (Slight difference from enemy Nearby)
    {
        bool ans = false;
        int enemy;
        int direction;
        if(black){
            enemy = 1;
            direction = -1;
        }
        else
        { 
            enemy = 2;
            direction = 1;
        }
        for(size_t i = 0; i < neighAtt_x.size(); i++)
        {
            int check_x = (direction*neighAtt_x[i]) + pos.x;
            int check_y = (direction*neighAtt_y[i]) + pos.y ;
            if(onBoard(check_x,check_y))
            {
                if (this->c_board.board_view[check_x][check_y] == enemy)
                {
                    ans = true;
                    return ans;
                }                       
            }
        }
        return ans;
    }

    bool support(position pos) // check if this position has support 
    {
        bool ans = false;
        int self = (this->direction*(-1)+3)/2;
        for (size_t i = 0; i < neighAtt_x.size(); i++)
        {
            int check_x = ((this->direction)*support_x[i]) + pos.x;
            int check_y = ((this->direction)*support_y[i]) + pos.y ;
            if(onBoard(check_x,check_y))
            {
                if (this->c_board.board_view[check_x][check_y] == self)
                {
                    ans = true;
                    return ans;
                }                       
            }
        }
        return ans;
    }

    bool supportColour(position pos,bool black) // check if this position has support 
    {

        bool ans = false;
        int self;
        int direction;
        if(black){
            self = 2;
            direction = -1;
        }
        else
        { 
            self = 1;
            direction = 1;
        }
        for (size_t i = 0; i < neighAtt_x.size(); i++)
        {
            int check_x = (direction*support_x[i]) + pos.x;
            int check_y = (direction*support_y[i]) + pos.y ;
            if(onBoard(check_x,check_y))
            {
                if (this->c_board.board_view[check_x][check_y] == self)
                {
                    ans = true;
                    return ans;
                }                       
            }
        }
        return ans;
    }

    bool self_at_pos(int x, int y)
    {
        int enemy = (this->direction + 3)/2;
        if (this->c_board.board_view[x][y] == enemy)
            return true;
        else 
            return false;        
    }

    vector<bmove> getCannonAttackMoves(position pos)  // give a vector of possible moves ... Can be a priority queue here according to som heuristics I mean node ordering to make good alpha beta ..
    {
        vector<bmove> moves;

        for (int i = 0; i < 4; i++) // for cannon shoots
        {
            int check_x_1 = pos.x + cannon_middle_x1[i];
            int check_x_2 = pos.x + cannon_middle_x2[i];
            int check_y_1 = pos.y + cannon_middle_y1[i];
            int check_y_2 = pos.y + cannon_middle_y2[i];

            int self = (this->direction*(-1) + 3)/2;
            if (onBoard(check_x_1,check_y_1) && onBoard(check_x_2,check_y_2))
            {
                for (int k = 3 ; k < 5; k++)
                {
                    int bomb_1_x = pos.x + k * cannon_middle_x1[i];
                    int bomb_2_x = pos.x + k * cannon_middle_x2[i];
                    int bomb_1_y = pos.y + k * cannon_middle_y1[i];
                    int bomb_2_y = pos.y + k * cannon_middle_y2[i];
                    int bomb_checkFront_1_x = pos.x + 2 * cannon_middle_x1[i];
                    int bomb_checkFront_2_x = pos.x + 2 * cannon_middle_x2[i];
                    int bomb_checkFront_1_y = pos.y + 2 * cannon_middle_y1[i];
                    int bomb_checkFront_2_y = pos.y + 2 * cannon_middle_y2[i];
                    if (this->c_board.board_view[check_x_1][check_y_1]== self && this->c_board.board_view[check_x_2][check_y_2] == self)
                    {
                        if (onBoard(bomb_1_x,bomb_1_y) && this->c_board.board_view[bomb_checkFront_1_x][bomb_checkFront_1_y] == 0 )
                        {
                            if(this->c_board.board_view[bomb_1_x][bomb_1_y]!=self && this->c_board.board_view[bomb_1_x][bomb_1_y]!= (-1)*self )  // abs because we cannot shoot our own cannon aswell
                            {
                                bmove* temp;
                                temp = new bmove('B',pos.x,pos.y,bomb_1_x,bomb_1_y,"CA"); // Change for ordering
                                moves.push_back(*temp);
                            }
                        }
                        if (onBoard(bomb_2_x,bomb_2_y) && this->c_board.board_view[bomb_checkFront_2_x][bomb_checkFront_2_y] == 0 )
                        {
                            if(this->c_board.board_view[bomb_2_x][bomb_2_y] != self && this->c_board.board_view[bomb_2_x][bomb_2_y] != (-1)*self )
                            {
                                bmove* temp;
                                temp = new bmove('B',pos.x,pos.y,bomb_2_x,bomb_2_y,"CA");  // Change for ordering
                                moves.push_back(*temp);
                            }
                        }
                    }
                }
            }
        }
        return moves;
    }

    vector<bmove> getCannonAttackMovesWithoutEmptyShoots(position pos)  // give a vector of possible moves ... Can be a priority queue here according to som heuristics I mean node ordering to make good alpha beta ..
    {
        vector<bmove> moves;

        for (int i = 0; i < 4; i++) // for cannon shoots
        {
            int check_x_1 = pos.x + cannon_middle_x1[i];
            int check_x_2 = pos.x + cannon_middle_x2[i];
            int check_y_1 = pos.y + cannon_middle_y1[i];
            int check_y_2 = pos.y + cannon_middle_y2[i];

            int self = (this->direction*(-1) + 3)/2;
            if (onBoard(check_x_1,check_y_1) && onBoard(check_x_2,check_y_2))
            {
                for (int k = 3 ; k < 5; k++)
                {
                    int bomb_1_x = pos.x + k * cannon_middle_x1[i];
                    int bomb_2_x = pos.x + k * cannon_middle_x2[i];
                    int bomb_1_y = pos.y + k * cannon_middle_y1[i];
                    int bomb_2_y = pos.y + k * cannon_middle_y2[i];
                    int bomb_checkFront_1_x = pos.x + 2 * cannon_middle_x1[i];
                    int bomb_checkFront_2_x = pos.x + 2 * cannon_middle_x2[i];
                    int bomb_checkFront_1_y = pos.y + 2 * cannon_middle_y1[i];
                    int bomb_checkFront_2_y = pos.y + 2 * cannon_middle_y2[i];
                    if (this->c_board.board_view[check_x_1][check_y_1]== self && this->c_board.board_view[check_x_2][check_y_2] == self)
                    {
                        if (onBoard(bomb_1_x,bomb_1_y) && this->c_board.board_view[bomb_checkFront_1_x][bomb_checkFront_1_y] == 0 )
                        {
                            if(this->c_board.board_view[bomb_1_x][bomb_1_y]!=self && this->c_board.board_view[bomb_1_x][bomb_1_y]!= (-1)*self && this->c_board.board_view[bomb_1_x][bomb_1_y]!= 0 )  // abs because we cannot shoot our own cannon aswell
                            {
                                bmove* temp;
                                temp = new bmove('B',pos.x,pos.y,bomb_1_x,bomb_1_y,"CA");  // Change for ordering
                                moves.push_back(*temp);
                            }
                        }
                        if (onBoard(bomb_2_x,bomb_2_y) && this->c_board.board_view[bomb_checkFront_2_x][bomb_checkFront_2_y] == 0 )
                        {
                            if(this->c_board.board_view[bomb_2_x][bomb_2_y] != self && this->c_board.board_view[bomb_2_x][bomb_2_y] != (-1)*self && this->c_board.board_view[bomb_2_x][bomb_2_y]!= 0 )
                            {
                                bmove* temp;
                                temp = new bmove('B',pos.x,pos.y,bomb_2_x,bomb_2_y,"CA");  // Change for ordering
                                moves.push_back(*temp);
                            }
                        }
                    }
                }
            }
        }
        return moves;
    }

    vector<bmove> getAttackMoves(position pos)  // give a vector of possible moves ... Can be a priority queue here according to som heuristics I mean node ordering to make good alpha beta ..
    {
        vector<bmove> moves;
        for (int i = 0; i < attack_x.size(); i++)
        {
            int enemy = (this->direction + 3)/2;
            int check_x = pos.x + attack_x[i];
            int check_y =  pos.y + (attack_y[i] * this->direction);
            if (onBoard(check_x,check_y))
            {
                if(this->c_board.board_view[check_x][check_y]  == enemy || this->c_board.board_view[check_x][check_y] == (-1)*enemy) //abs because -2 and 2 are both enemies ..  remember -2 is black townhall
                {
                    bmove* temp;
                    temp = new bmove('M',pos.x,pos.y,check_x,check_y,"A");  // Change for ordering
                    moves.push_back(*temp);
                }
            }            
        }
        return moves;
    }

    bool isSomeOneAttackable(){
        bool ans = false;

        int num_players;

        if(this->direction == 1) // my moves
            num_players = this->c_board.white_players.size();
        else
            num_players = this->c_board.black_players.size();

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getAttackMoves(this->c_board.white_players[i]);
            else
                temp =  this->getAttackMoves(this->c_board.black_players[i]);
            if (temp.size() > 0)
            return true;
        }

        board boardFromOtherSide = board(this->c_board,(this->direction*(-1)));
        if(boardFromOtherSide.direction == 1)
            num_players = boardFromOtherSide.c_board.white_players.size();
        else
            num_players = boardFromOtherSide.c_board.black_players.size();

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  boardFromOtherSide.getAttackMoves(boardFromOtherSide.c_board.white_players[i]);
            else
                temp =  boardFromOtherSide.getAttackMoves(boardFromOtherSide.c_board.black_players[i]);
            if (temp.size() > 0)
            return true;
        }
        
        return ans;
    }

    vector<bmove> getCannonMoveValidMoves(position pos)  // give a vector of possible moves ... Can be a priority queue here according to som heuristics I mean node ordering to make good alpha beta ..
    {
        vector<bmove> moves;

        // check cannon moves
        for (int i = 0; i < cannon_x.size(); i++)
        {
            int check_x = pos.x + cannon_x[i]; // no need to check / multiply with direction as cannon can move back and forth 
            int check_y = pos.y + cannon_y[i];
            if (onBoard(check_x,check_y) && this->c_board.board_view[check_x][check_y] == 0)
            {
                int temp_x_1 = pos.x + sgn(cannon_x[i]);
                int temp_y_1 = pos.y + sgn(cannon_y[i]);
                int temp_x_2 = temp_x_1 + sgn(cannon_x[i]);
                int temp_y_2 = temp_y_1 + sgn(cannon_y[i]);
                
                int self = (this->direction*(-1) + 3)/2;
                if (this->c_board.board_view[temp_x_1][temp_y_1] == self & this->c_board.board_view[temp_x_2][temp_y_2] == self)
                {
                    bmove* temp;
                    temp = new bmove('M',pos.x,pos.y,check_x,check_y,"C");  // Change for ordering
                    moves.push_back(*temp);
                }
            }
        }
        return moves;
    }


    vector<bmove> getForwardMoveValidMoves(position pos)  // give a vector of possible moves ... Can be a priority queue here according to som heuristics I mean node ordering to make good alpha beta ..
    {
        vector<bmove> moves;
        for (int i = 0; i < forward_x.size(); i++)
        {   
            int check_x = pos.x + forward_x[i];
            int check_y =  pos.y + (forward_y[i] * (this->direction));
            if (onBoard(check_x,check_y))
            {
                if(this->c_board.board_view[check_x][check_y] == 0)
                {
                    bmove* temp;
                    temp = new bmove('M',pos.x,pos.y,check_x,check_y,"F");  // Change for ordering
                    moves.push_back(*temp);
                }
            }
        }
       return moves;
    }


    vector<bmove> getBackwardMoveValidMoves(position pos)  // give a vector of possible moves ... Can be a priority queue here according to som heuristics I mean node ordering to make good alpha beta ..
    {
        vector<bmove> moves;
        int enemy = (this->direction + 3)/2;

        if (this->retreatAble(pos))
        {
            for (int i = 0; i < backward_x.size(); i++)
            {
                int check_x = pos.x + backward_x[i];
                int check_y =  pos.y + (backward_y[i] * this->direction);
                if (onBoard(check_x,check_y))
                {
                    if(this->c_board.board_view[check_x][check_y]  == 0 || this->c_board.board_view[check_x][check_y] == enemy)
                    {
                        bmove* temp;
                        temp = new bmove('M',pos.x,pos.y,check_x,check_y,"B");  // Change for ordering
                        moves.push_back(*temp);
                    }   
                }
            }
        }
        return moves;
    }

    vector<int> num_Cannon_shootable(bool blackCannon) // return my cannons if true else return opponents cannons
    {
        vector<int> ans = {0,0};
        vector<position> positions;
        int demand_id;
        if(blackCannon) // black
        {
            positions = this->c_board.black_players;
            demand_id = 2;
        }
        else // white 
        {
            positions = this->c_board.white_players;
            demand_id = 1;
        }

        for (int i = 0; i < positions.size(); i++)
        {
            position pos = positions[i];
            for (int i = 0; i < 4; i++) // for cannon shoots
            {
                int check_x_1 = pos.x + cannon_middle_x1[i];
                int check_x_2 = pos.x + cannon_middle_x2[i];
                int check_y_1 = pos.y + cannon_middle_y1[i];
                int check_y_2 = pos.y + cannon_middle_y2[i];

                if (onBoard(check_x_1,check_y_1) && onBoard(check_x_2,check_y_2))
                {
                    for (int k = 3 ; k < 5; k++)
                    {
                        int bomb_1_x = pos.x + k * cannon_middle_x1[i];
                        int bomb_2_x = pos.x + k * cannon_middle_x2[i];
                        int bomb_1_y = pos.y + k * cannon_middle_y1[i];
                        int bomb_2_y = pos.y + k * cannon_middle_y2[i];

                        int bomb_checkFront_1_x = pos.x + 2 * cannon_middle_x1[i];
                        int bomb_checkFront_2_x = pos.x + 2 * cannon_middle_x2[i];
                        int bomb_checkFront_1_y = pos.y + 2 * cannon_middle_y1[i];
                        int bomb_checkFront_2_y = pos.y + 2 * cannon_middle_y2[i];

                        if (this->c_board.board_view[check_x_1][check_y_1] == demand_id && this->c_board.board_view[check_x_2][check_y_2] == demand_id)
                        {
                            if (onBoard(bomb_1_x,bomb_1_y)  && this->c_board.board_view[bomb_checkFront_1_x][bomb_checkFront_1_y] == 0)
                            {
                                if(this->c_board.board_view[bomb_1_x][bomb_1_y]!=demand_id && this->c_board.board_view[bomb_1_x][bomb_1_y]!= (-1)*demand_id )  // abs because we cannot shoot our own cannon aswell
                                {
                                    if(this->c_board.board_view[bomb_1_x][bomb_1_y] == 0)
                                        ans[0]++;
                                    else
                                        ans[1]++;
                                }
                            }
                            if (onBoard(bomb_2_x,bomb_2_y)  && this->c_board.board_view[bomb_checkFront_2_x][bomb_checkFront_2_y] == 0 )
                            {
                                if(this->c_board.board_view[bomb_2_x][bomb_2_y] != demand_id && this->c_board.board_view[bomb_2_x][bomb_2_y] != (-1)*demand_id )
                                {
                                    if(this->c_board.board_view[bomb_2_x][bomb_2_y] == 0)
                                        ans[0]++;
                                    else
                                        ans[1]++;
                                }
                            }
                        }
                    }
                }
            }
        }
        return ans;
    }

    int minDistanceTownHall(bool Black,position pos)
    {
        int ans = rows; // config
        if (Black) // considering manhattan distance // check 
        {
            for (int i = 0; i < townhall_white_x.size(); i++)
            {
                if (this->c_board.board_view[townhall_white_x[i]][townhall_white_y[i]] < 0)
                {
                    int temp = abs(pos.y - townhall_white_y[i]) + abs(pos.x - townhall_white_x[i]);
                    if(temp < ans)
                    {
                        ans = temp;
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < townhall_black_x.size(); i++)
            {
                if (this->c_board.board_view[townhall_black_x[i]][townhall_black_y[i]] < 0)
                {
                    int temp = abs(pos.y - townhall_black_y[i]) + abs(pos.x - townhall_black_x[i]);
                    if(temp < ans)
                    {
                        ans = temp;
                    }
                }
            }
        }
        return ans;
    }
    
    vector<int> factors(bool black) // Factor of the terms relative to black  ...........JUST CONSIDER BLACK / WHITE HERE ...Assume black plays  as  the chance logic is dealt in the Utility function accordingly
    {
        vector<int> ans = {0,0,0,0,0,0,0,0}; //  Cannon shootable Empty |  Canon shootable someone | Normal Attackable Someone | Normal Attackable support | minTownHallDistance | avgMinTownHallDistance | Total positions
        vector<position> positions;
        
        int demand_id;
        int opposite_id;
        

        if(black) // black
        {
            positions = this->c_board.black_players;
            demand_id = 2;
            opposite_id = 1;
        }
        else // white 
        {
            positions = this->c_board.white_players;
            demand_id = 1;
            opposite_id = 2;
        }
        // Now we have the vectors and the demand_id

        int sumMinTownHallDistance = 0;
        int minTownHallDistance = rows;


        for (int i = 0; i < positions.size(); i++)
        {

            position pos = positions[i];

            int temp = 0;
            if (demand_id == 2) // black
            {
                temp = minDistanceTownHall(true,pos);
                
            }
            else if (demand_id == 1) // white
            {
                temp = minDistanceTownHall(false,pos);
            }
            sumMinTownHallDistance+=temp;
            if (temp < minTownHallDistance)
            {
                minTownHallDistance = temp;
            }
            

            if (retreatAbleColour(pos,black)) // means under attack ... utility based on possible attacks is highly based on whoose turn it is (Handled in the Utility function)
            {
                ans[2]++;
                if(supportColour(pos,black)) // Attack and have support ... Support might not be suffecient THINK
                {
                    ans[3]++;
                }
            }
            
            for (int i = 0; i < 4; i++) // for cannon shoots
            {
                int check_x_1 = pos.x + cannon_middle_x1[i];
                int check_x_2 = pos.x + cannon_middle_x2[i];
                int check_y_1 = pos.y + cannon_middle_y1[i];
                int check_y_2 = pos.y + cannon_middle_y2[i];

                if (onBoard(check_x_1,check_y_1) && onBoard(check_x_2,check_y_2))
                {
                    for (int k = 3 ; k < 5; k++)
                    {
                        int bomb_1_x = pos.x + k * cannon_middle_x1[i];
                        int bomb_2_x = pos.x + k * cannon_middle_x2[i];
                        int bomb_1_y = pos.y + k * cannon_middle_y1[i];
                        int bomb_2_y = pos.y + k * cannon_middle_y2[i];

                        int bomb_checkFront_1_x = pos.x + 2 * cannon_middle_x1[i];
                        int bomb_checkFront_2_x = pos.x + 2 * cannon_middle_x2[i];
                        int bomb_checkFront_1_y = pos.y + 2 * cannon_middle_y1[i];
                        int bomb_checkFront_2_y = pos.y + 2 * cannon_middle_y2[i];

                        if (this->c_board.board_view[check_x_1][check_y_1] == demand_id && this->c_board.board_view[check_x_2][check_y_2] == demand_id)
                        {
                            if (onBoard(bomb_1_x,bomb_1_y)  && this->c_board.board_view[bomb_checkFront_1_x][bomb_checkFront_1_y] == 0)
                            {
                                if(this->c_board.board_view[bomb_1_x][bomb_1_y]!=demand_id && this->c_board.board_view[bomb_1_x][bomb_1_y]!= (-1)*demand_id )  // abs because we cannot shoot our own cannon aswell
                                {
                                    if(this->c_board.board_view[bomb_1_x][bomb_1_y] == 0)
                                        ans[0]++;
                                    else
                                        ans[1]++;
                                }
                            }
                            if (onBoard(bomb_2_x,bomb_2_y)  && this->c_board.board_view[bomb_checkFront_2_x][bomb_checkFront_2_y] == 0 )
                            {
                                if(this->c_board.board_view[bomb_2_x][bomb_2_y] != demand_id && this->c_board.board_view[bomb_2_x][bomb_2_y] != (-1)*demand_id )
                                {
                                    if(this->c_board.board_view[bomb_2_x][bomb_2_y] == 0)
                                        ans[0]++;
                                    else
                                        ans[1]++;
                                }
                            }
                        }
                    }
                }
            }
        }

        ans[4] = minTownHallDistance;
        ans[5] = sumMinTownHallDistance;
        ans[6] = positions.size(); 

        return ans;
    }

    int attackableSodiers(){
        int num = 0;
        vector<position> b_positions = this->c_board.black_players;
        vector<position> w_positions = this->c_board.white_players;
        
        for (int i = 0; i < b_positions.size(); i++)
        {
            if (retreatAbleColour(b_positions[i],true)) // means under attack ... utility based on possible attacks is highly based on whoose turn it is (Handled in the Utility function)
            {
                num++;
            }
        }
        for (int i = 0; i < w_positions.size(); i++)
        {
            if (retreatAbleColour(w_positions[i],false)) // means under attack ... utility based on possible attacks is highly based on whoose turn it is (Handled in the Utility function)
            {
                num++;
            }
        }
        return num;
    }

    float utility(bool maxChance){ // MaxChance true if it is the chance of Wuji,The Bot ;)
        int myPlayers,opponent_players;
        int myTownHalls, opponent_townhalls;
        int myCannons, opponent_Cannons;

        int opponentCannonShootableEmpty; 
        int myCannonShootableEmpty;  

        int opponentCannonShootableSomeone; 
        int myCannonShootableSomeone;  

        int whiteSoldiersWhoAreNearyEnemy; // direct normal attack
        int blackSoldiersWhoAreNearyEnemy; // direct normal attack

        int whiteSoldiersWhoAreNearyEnemyWithSupport; // direct normal attack with support 
        int blackSoldiersWhoAreNearyEnemyWithSupport; // direct normal attack with support 

        int myMinTownHallDistance;
        int opponentMinTownHallDistance;

        int opponentAvgMinTownHallDistance;
        float myAvgMinTownHallDistance;

        int opponentSoldiers;
        int mySoldiers;

        vector<int> black_factors = this->factors(true);
        vector<int> white_factors = this->factors(false);

        whiteSoldiersWhoAreNearyEnemy = white_factors[2];            
        whiteSoldiersWhoAreNearyEnemyWithSupport = white_factors[3];           

        blackSoldiersWhoAreNearyEnemy = black_factors[2];            
        blackSoldiersWhoAreNearyEnemyWithSupport = black_factors[3];


        if(bot == 2) // ATTNETION white This convention is set as a universal variable in the beginning when server sends number  and not according to the convention that is generally follwed otherwise in the file
        {

            myCannonShootableEmpty =  white_factors[0];
            opponentCannonShootableEmpty  = black_factors[0];

            myCannonShootableSomeone =  white_factors[1];
            opponentCannonShootableSomeone  = black_factors[1];

            myMinTownHallDistance = white_factors[4]; 
            myAvgMinTownHallDistance = ((float)white_factors[5])/(float)white_factors[6]; 

            opponentMinTownHallDistance = black_factors[4]; 
            opponentAvgMinTownHallDistance = ((float)black_factors[5])/(float)black_factors[6]; 

            myPlayers  = this->c_board.white_players.size();
            opponent_players = this->c_board.black_players.size();
            myTownHalls = this->c_board.num_white_townhalls;
            opponent_townhalls = this->c_board.num_black_townhalls;

        }
        
        else if(bot == 1) // ATTNETION black This convention is set as a universal variable in the beginning when server sends number  and not according to the convention that is generally follwed otherwise in the file 
        {
            myCannonShootableEmpty =  black_factors[0];
            opponentCannonShootableEmpty  = white_factors[0];

            myCannonShootableSomeone =  black_factors[1];
            opponentCannonShootableSomeone  = white_factors[1];

            myMinTownHallDistance = black_factors[4]; 
            myAvgMinTownHallDistance = ((float)black_factors[5])/(float)black_factors[6]; 

            opponentMinTownHallDistance = white_factors[4]; 
            opponentAvgMinTownHallDistance = ((float)white_factors[5])/(float)white_factors[6]; 

            myPlayers  = this->c_board.black_players.size();
            opponent_players = this->c_board.white_players.size();
            myTownHalls = this->c_board.num_black_townhalls;
            opponent_townhalls = this->c_board.num_white_townhalls;
        }

        int attack = whiteSoldiersWhoAreNearyEnemy + blackSoldiersWhoAreNearyEnemy;

        int attackNeeded ; // 1 for yes -1 for no 
        if((this->direction == -1 && bot == 1) || (this->direction == 1 && bot == 2)) 
            attackNeeded = 1;
        else
            attackNeeded = -1;
        
        attack = attack * attackNeeded;

        float utility = (myPlayers - opponent_players)*A + (myTownHalls - opponent_townhalls)*B + (myCannonShootableSomeone - opponentCannonShootableSomeone)*C + (myCannonShootableEmpty- opponentCannonShootableEmpty)*D + attack*E  + (myAvgMinTownHallDistance - opponentAvgMinTownHallDistance)*F + (myMinTownHallDistance - opponentMinTownHallDistance)*G;
        
        return utility; // Update print utility function 
    }


    float utilityTerminal(bool maxChance){ // MaxChance true if it is the chance of Wuji,The Bot ;)
        int myTownHalls, opponent_townhalls;


        if(bot == 2) // ATTNETION white This convention is set as a universal variable in the beginning when server sends number  and not according to the convention that is generally follwed otherwise in the file
        {
            myTownHalls = this->c_board.num_white_townhalls;
            opponent_townhalls = this->c_board.num_black_townhalls;
        }
        
        else if(bot == 1) // ATTNETION black This convention is set as a universal variable in the beginning when server sends number  and not according to the convention that is generally follwed otherwise in the file 
        {
            myTownHalls = this->c_board.num_black_townhalls;
            opponent_townhalls = this->c_board.num_white_townhalls;
        }

        float utility = (myTownHalls - opponent_townhalls)*B*win ;

        return utility; // Update print utility function 
    }


    vector<bmove> getAllValidMoves(bool repeat)
    {
        vector<bmove> moves;
        int num_players;

        if(this->direction == 1)
            num_players = this->c_board.white_players.size();
        else
            num_players = this->c_board.black_players.size();

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getAttackMoves(this->c_board.white_players[i]);
            else
                temp =  this->getAttackMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
        }


        if (repeat) // no empty shoots in repeat
        {
            for (int i = 0; i < num_players; i++)
            {
                vector<bmove> temp;
                if(direction == 1)
                    temp =  this->getCannonAttackMovesWithoutEmptyShoots(this->c_board.white_players[i]);
                else
                    temp =  this->getCannonAttackMovesWithoutEmptyShoots(this->c_board.black_players[i]);
                moves.insert(moves.end(), temp.begin(), temp.end());
            }
            
        }
        else
        {
            for (int i = 0; i < num_players; i++)
            {
                vector<bmove> temp;
                if(direction == 1)
                    temp =  this->getCannonAttackMoves(this->c_board.white_players[i]);
                else
                    temp =  this->getCannonAttackMoves(this->c_board.black_players[i]);
                moves.insert(moves.end(), temp.begin(), temp.end());
            }
        }
        

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getCannonMoveValidMoves(this->c_board.white_players[i]);
            else
                temp =  this->getCannonMoveValidMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
        }

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getForwardMoveValidMoves(this->c_board.white_players[i]);
            else
                temp =  this->getForwardMoveValidMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
        }

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getBackwardMoveValidMoves(this->c_board.white_players[i]);
            else
                temp =  this->getBackwardMoveValidMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
        }

        return moves;
    }

    vector<bmove> getAllValidAttackMoves(bool repeat)
    {
        vector<bmove> moves;
        int num_players;

        if(this->direction == 1)
            num_players = this->c_board.white_players.size();
        else
            num_players = this->c_board.black_players.size();

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getAttackMoves(this->c_board.white_players[i]);
            else
                temp =  this->getAttackMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
        }


        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getCannonAttackMovesWithoutEmptyShoots(this->c_board.white_players[i]);
            else
                temp =  this->getCannonAttackMovesWithoutEmptyShoots(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
        }

        return moves;
    }

    vector<bmove> getAllValidMovesOnce(bool repeat) // change for ordering
    {
        vector<bmove> moves;
        int num_players;
        if(this->direction == 1)
            num_players = this->c_board.white_players.size();
        else
            num_players = this->c_board.black_players.size();

        if (repeat) // no empty shoots in repeat
        {
            for (int i = 0; i < num_players; i++)
            {
                vector<bmove> temp;
                if(direction == 1)
                    temp =  this->getCannonAttackMovesWithoutEmptyShoots(this->c_board.white_players[i]);
                else
                    temp =  this->getCannonAttackMovesWithoutEmptyShoots(this->c_board.black_players[i]);
                moves.insert(moves.end(), temp.begin(), temp.end());
                CAmoves_avail += (temp.size() > 0)?1:0;
            }
            
        }
        else
        {
            for (int i = 0; i < num_players; i++)
            {
                vector<bmove> temp;
                if(direction == 1)
                    temp =  this->getCannonAttackMoves(this->c_board.white_players[i]);
                else
                    temp =  this->getCannonAttackMoves(this->c_board.black_players[i]);
                moves.insert(moves.end(), temp.begin(), temp.end());
                CAmoves_avail += (temp.size() > 0)?1:0;
            }
        }
        
        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getAttackMoves(this->c_board.white_players[i]);
            else
                temp =  this->getAttackMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
            Amoves_avail += (temp.size() > 0)?1:0;
        }

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getCannonMoveValidMoves(this->c_board.white_players[i]);
            else
                temp =  this->getCannonMoveValidMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
            Cmoves_avail += (temp.size() > 0)?1:0;
        }

        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getForwardMoveValidMoves(this->c_board.white_players[i]);
            else
                temp =  this->getForwardMoveValidMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
            Fmoves_avail += (temp.size() > 0)?1:0;
        }


        for (int i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getBackwardMoveValidMoves(this->c_board.white_players[i]);
            else
                temp =  this->getBackwardMoveValidMoves(this->c_board.black_players[i]);
            moves.insert(moves.end(), temp.begin(), temp.end());
            Bmoves_avail += (temp.size() > 0)?1:0;
        }

        return moves;
    }


    bmove play(bool repeat){ // we don't have to take tention whether the current situation is a terminal state or not !! BUT WE HAVE TO IN THE SEARCH
        return  this->AplhaBetaSearchHeight(repeat);     
    }

    miniMaxAns MinValue(int depth, float alpha, float beta , int chances, bool repeat)
    {
        miniMaxAns ans;
        if (this->terminal() || (depth == 0))
        {
            ans.utility = this->utility(false);
            return ans;
        }
        ans.utility = FLT_MAX;
        vector<bmove> moves = this->getAllValidMoves(repeat);
        for (int i = 0; i < moves.size(); i++)
        {   
            board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
            float utility_temp = (config.MaxValue(depth-1,alpha,beta,chances,repeat)).utility;
            if(utility_temp < ans.utility)
            {
                ans.utility = utility_temp;
                ans.move = moves[i];
                if(ans.utility <= alpha)
                {
                    return ans;
                }
            }
            if (ans.utility < beta)
            {
                beta = ans.utility;
            }
        }
        return ans;
    }


    miniMaxAns MaxValue(int depth, float alpha, float beta, int chances,bool repeat)
    {
        miniMaxAns ans;
        if (this->terminal() || (depth == 0))
        {
            ans.utility = this->utility(true); // utility is a function of whoose turn it is . Isn't it natural
            return ans;
        }
        ans.utility = (-1) * FLT_MAX;
        vector<bmove> moves = this->getAllValidMoves(repeat);
        for (int i = 0; i < moves.size(); i++)
        {   
            board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
            float utility_temp = (config.MinValue(depth-1,alpha,beta,chances,repeat)).utility;
            if(utility_temp > ans.utility)
            {
                ans.utility = utility_temp;
                ans.move = moves[i];
                if(ans.utility >= beta)
                {
                    return ans;
                }
            }
            if (ans.utility >= alpha)
            {
                alpha = ans.utility;
            }
        }
        return ans;
    }

    miniMaxAns MinValueHeight(int height, float alpha, float beta , int chances, bool repeat,int min_height ,int max_height)
    {
        miniMaxAns ans;
        if (this->terminal())
        {
            ans.utility = this->utility(false);
            // ans.utility = this->utilityTerminal(false);
            return ans;
        }
        else if (height >= max_height)
        {
            ans.utility = this->utility(false);
            return ans;
        }
        else if(height > min_height)
        {
            vector<bmove> moves = this->getAllValidAttackMoves(repeat);
            if(moves.size() == 0)
            {
                ans.utility = this->utility(false);
                return ans;
            }
            
            ans.utility = FLT_MAX;
            for (int i = 0; i < moves.size(); i++)
            {   
                board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
                float utility_temp = (config.MaxValueHeight(height+1,alpha,beta,chances,repeat,min_height,max_height)).utility;
                if(utility_temp < ans.utility)
                {
                    ans.utility = utility_temp;
                    ans.move = moves[i];
                    if(ans.utility <= alpha)
                    {
                        return ans;
                    }
                }
                if (ans.utility < beta)
                {
                    beta = ans.utility;
                }
            }
            return ans;
        }
        ans.utility = FLT_MAX;
        vector<bmove> moves = this->getAllValidMoves(repeat);
        for (int i = 0; i < moves.size(); i++)
        {   
            board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
            float utility_temp = (config.MaxValueHeight(height+1,alpha,beta,chances,repeat,min_height,max_height)).utility;
            if(utility_temp < ans.utility)
            {
                ans.utility = utility_temp;
                ans.move = moves[i];
                if(ans.utility <= alpha)
                {
                    return ans;
                }
            }
            if (ans.utility < beta)
            {
                beta = ans.utility;
            }
        }
        return ans;
    }

    miniMaxAns MaxValueHeight(int height, float alpha, float beta, int chances,bool repeat, int min_height, int max_height)
    {
        miniMaxAns ans;
        if (this->terminal())
        {
            ans.utility = this->utility(true); // utility is a function of whoose turn it is . Isn't it natural
            // ans.utility = this->utilityTerminal(true); // utility is a function of whoose turn it is . Isn't it natural
            return ans;
        }
        else if(height >= max_height)
        {
            ans.utility = this->utility(true); // utility is a function of whoose turn it is . Isn't it natural
            return ans;   
        }
        else if(height > min_height)
        {
            ans.utility = (-1) * FLT_MAX;
            vector<bmove> moves = this->getAllValidAttackMoves(repeat);
            if(moves.size() == 0)
            {
                ans.utility = this->utility(false);
                return ans;
            }
            for (int i = 0; i < moves.size(); i++)
            {   
                board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
                float utility_temp = (config.MinValueHeight(height+1,alpha,beta,chances,repeat,min_height,max_height)).utility;
                if(utility_temp > ans.utility)
                {
                    ans.utility = utility_temp;
                    ans.move = moves[i];
                    if(ans.utility >= beta)
                    {
                        return ans;
                    }
                }
                if (ans.utility >= alpha)
                {
                    alpha = ans.utility;
                }
            }
            return ans;
        }

        ans.utility = (-1) * FLT_MAX;
        vector<bmove> moves = this->getAllValidMoves(repeat);
        for (int i = 0; i < moves.size(); i++)
        {   
            board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
            float utility_temp = (config.MinValueHeight(height+1,alpha,beta,chances,repeat,min_height,max_height)).utility;
            if(utility_temp > ans.utility)
            {
                ans.utility = utility_temp;
                ans.move = moves[i];
                if(ans.utility >= beta)
                {
                    return ans;
                }
            }
            if (ans.utility >= alpha)
            {
                alpha = ans.utility;
            }
        }
        return ans;
    }

    bmove AplhaBetaSearch(bool repeat)
    {
        int players_remaning = this->c_board.white_players.size() + this->c_board.black_players.size() ;
        int depth = 4;
        if (players_remaning < 22)
        {
            depth = 6;
        }
        if (players_remaning < 19)
        {
            depth = 7;
        }
        else if (players_remaning < 16)
        {
            depth = 8;
        }
        else if (players_remaning < 14)
        {
            depth = 10;
        }
        else if (players_remaning < 12)
        {
            depth = 12;
        }
        else if(players_remaning < 10)
        {
            depth = 14;
        }
        else if(players_remaning < 8)
        {
            depth = 16;
        }
        else if(players_remaning < 6)
        {
            depth = 18;
        }

        if (repeat)
        {
            depth = depth - 1;
        }

        vector<bmove> moves = this->getAllValidMovesOnce(repeat); // change for ordering

        // miniMaxAns ans = MaxValue(depth,INT_MIN,INT_MAX);
        miniMaxAns ans = MaxValue(depth, (-1)*FLT_MAX, FLT_MAX,1,repeat);
        // cout << "AlphaBeta move : " <<  ans.move.x1 << " " << ans.move.y1 << " " << ans.move.type << " " << ans.move.x2 << " " << ans.move.y2 << endl;
        return ans.move;
    }

    bmove AplhaBetaSearchHeight(bool repeat)
    {
        int players_remaning = this->c_board.white_players.size() + this->c_board.black_players.size() ;
        int min_height = 4;
        if (players_remaning < 22)
        {
            min_height = 5;
        }
        else if (players_remaning < 21)
        {
            min_height = 6;
        }
        else if (players_remaning < 19)
        {
            min_height = 7;
        }
        else if (players_remaning < 17)
        {
            min_height = 8;
        }
        else if (players_remaning < 14)
        {
            min_height = 9;
        }
        else if (players_remaning < 13)
        {
            min_height = 11;
        }
        else if (players_remaning < 12)
        {
            min_height = 12;
        }
        else if(players_remaning < 10)
        {
            min_height = 14;
        }
        else if(players_remaning < 9)
        {
            min_height = 15;
        }
        else if(players_remaning < 8)
        {
            min_height = 16;
        }
        else if(players_remaning < 6)
        {
            min_height = 18;
        }

        if (repeat)
        {
            min_height = min_height - 1;
        }

        // vector<bmove> moves = this->getAllValidMovesOnce(repeat); // change for ordering
        miniMaxAns ans;
        if(min_height >= 5)
            ans = MaxValueHeight(0, (-1)*FLT_MAX, FLT_MAX,1,repeat,min_height-1,min_height+10);
        else
            ans = MaxValueHeight(0, (-1)*FLT_MAX, FLT_MAX,1,repeat,min_height,min_height+10);

        // ans = MaxValueHeight(0, (-1)*FLT_MAX, FLT_MAX,1,repeat,min_height-1,min_height+10);

        // cout << "AlphaBeta move : " <<  ans.move.x1 << " " << ans.move.y1 << " " << ans.miove.type << " " << ans.move.x2 << " " << ans.move.y2 << endl;
        return ans.move;
    }


    bool terminal(){
        if(this->c_board.num_black_townhalls < 3 || this-> c_board.num_white_townhalls < 3 || this->c_board.white_players.size() == 0 || this->c_board.black_players.size() == 0) // config
            return true;
        else if(this->direction == 1){
            if (this->c_board.white_players.size() <= 3)
            {
                if(this->getAllValidMoves(false).size()==0)
                    return true;                /* code */
            }
        }
        else if(this->direction == -1){
            if(this->c_board.black_players.size() <= 3)
            {
                if(this->getAllValidMoves(false).size() == 0)
                    return true;
            }
        }
        return false;
    }
};

int main(int argc, char const *argv[])
{
    if (argc == 5) {
        A = atof(argv[1]);
        B = atof(argv[2]);
        C = atof(argv[3]);
        D = atof(argv[4]);
    }

    string a;
    getline(cin,a);
    vector<int> white_x = {1,1,1,3,3,3,5,5,5,7,7,7}; // config
    vector<int> white_y = {0,1,2,0,1,2,0,1,2,0,1,2};
    vector<int> black_x = {0,0,0,2,2,2,4,4,4,6,6,6};
    vector<int> black_y = {7,8,9,7,8,9,7,8,9,7,8,9};

    int intial_number_players = 12; // config

    vector< vector< int > >  board_cells(columns, vector<int> (rows, 0)); // config 
    vector<position> white_players;
    vector<position> black_players;
    for (int i = 0; i < intial_number_players; i++) // Initialize the board_cells
    {
        position temp_w;
        position temp_b;
        temp_w.x = white_x[i];
        temp_w.y = white_y[i];
        temp_b.x = black_x[i];
        temp_b.y = black_y[i];
        white_players.push_back(temp_w);
        black_players.push_back(temp_b);
        board_cells[white_x[i]][white_y[i]] = 1;
        board_cells[black_x[i]][black_y[i]] = 2;
    }
    for (int i = 0; i < townhall_white_x.size(); i++)
    {
        board_cells[townhall_white_x[i]][townhall_white_y[i]] = -1;
        board_cells[townhall_black_x[i]][townhall_black_y[i]] = -2;
    }

    // Now let's make the Intital Board
    board_view b_w;
    b_w.board_view = board_cells;
    b_w.white_players = white_players;
    b_w.black_players = black_players;
    b_w.num_black_townhalls = 4; // config
    b_w.num_white_townhalls = 4; // config

    board game;


    bool myTurn;
    game = board(b_w,0,0,'N',0,0,-1); // write this thing up 

    myTurn = 1;
    bot = 1;
    if ((int)(a[0]) == 50)
    {
        // Bot is white;
        myTurn = false;
        bot = 2;
    }
    else if((int)(a[0]) == 49)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
    {
        // Bot is black
        myTurn = true; 
        bot = 1;
    }
    else
    {
        cout << "ERROR INPUT" << endl;
    }

    string opponentMove1 = "1";
    string opponentMove2 = "2";
    string opponentMove3 = "3";
    string opponentMove4 = "4";

    bool repeatOpp = false;

    while (true){
        if (myTurn)
        {
            bmove BotMove;
            if (repeatOpp) // imporove this
                BotMove = game.play(true);
            else
                BotMove = game.play(false);
            
            string m_type = BotMove.tp; // change for ordering
            if (m_type == "C") // change for ordering 
            {
                Cmoves += 1;
            }
            else if (m_type == "CA")
            {
                CAmoves += 1;
            }
            else if (m_type == "F")
            {
                Fmoves += 1;
            }
            else if (m_type == "B")
            {
                Bmoves += 1;
            }
            else if (m_type == "A")
            {
                Amoves += 1;
            }
            else
            {
                cerr  << "Gadbad " << BotMove.tp << endl;
            }
            cout << "S " << BotMove.x1 << " " << BotMove.y1 << " " << BotMove.type << " " << BotMove.x2 << " " << BotMove.y2 << endl; 
            game = board(game.c_board,BotMove.x1,BotMove.y1,BotMove.type,BotMove.x2,BotMove.y2,game.direction*(-1));
        }
        else
        {
            char player;
            char Move;
            int x1,y1;
            int x2,y2;
            string move;
            getline(cin,move);

            player = move[0];
            x1 = (int)(move[2]) - 48;            
            y1 = (int)(move[4]) - 48;            
            Move = move[6];            
            x2 = (int)(move[8]) - 48;            
            y2 = (int)(move[10]) - 48;            

            game = board(game.c_board,x1,y1,Move,x2,y2,game.direction*(-1));

            repeatOpp == false;

            if(move == opponentMove3 && move == opponentMove2 && move == opponentMove1 && move == opponentMove4|| (move == opponentMove2 && move == opponentMove4))
                repeatOpp = true;
            
            opponentMove4 = opponentMove3;
            opponentMove3 = opponentMove2;
            opponentMove2 = opponentMove1;
            opponentMove1 = move;
        }
        myTurn=!myTurn;
        // ofstream myfile;
        // myfile.open ("Ordering.txt",std::ios::trunc);
        // myfile << "Cannon Attack Moves " << CAmoves << "/" << CAmoves_avail << " : "<<float(CAmoves)/(float)CAmoves_avail << endl;
        // myfile << "Cannon Moves " << Cmoves << "/" << Cmoves_avail << " : "<<float(Cmoves)/(float)Cmoves_avail << endl;
        // myfile << "Attack Moves " << Amoves << "/" << Amoves_avail << " : "<<float(Amoves)/(float)Amoves_avail << endl;
        // myfile << "Forward Moves " << Fmoves << "/" << Fmoves_avail << " : "<<float(Fmoves)/(float)Fmoves_avail << endl;
        // myfile << "Back Moves " << Bmoves << "/" << Bmoves_avail << " : "<<float(Bmoves)/(float)Bmoves_avail << endl;
        // myfile.close();

        vector<int> white_factors = game.factors(false);
        vector<int> black_factors = game.factors(true);

        for (int l = 0; l < white_factors.size(); l++)
        {
            cerr << white_factors[l] << " ";
        }
        cerr << endl;
        for (int l = 0; l < black_factors.size(); l++)
        {
            cerr << black_factors[l] << " ";
        }
        cerr << endl;

        cerr << "Utility : " << game.utility(true) << endl;

    }
    return 0;
}