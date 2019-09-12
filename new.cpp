#include <iostream>
#include <bits/stdc++.h>

using namespace std;

// To DO 
// *** Follow cannon moves on Piazza
// winning score is important ... not just winning


// Can we do something like we always think .. I mean even while the opponent is thinking in that case we can have a parallel thread
// Also can we used the previous thought moves ... again to play the next move
// For example if we have thought 5 levels we don't need to think 4 levels to reach a depth of 4 we can directly start our algorithm

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

int columns = 8;
int rows = 8;

int bot;

vector<int> neighbour_x = {1,1,1,0,0,-1,-1,-1};
vector<int> neighbour_y = {1,0,-1,1,-1,1,0,-1};

vector<int> forward_x = {-1,0,1};
vector<int> forward_y = {1,1,1};

vector<int> attack_x = {-1,-1,0,1,1};
vector<int> attack_y = {0,1,1,0,1};

vector<int> backward_x = {-2,0,2};
vector<int> backward_y = {-2,-2,-2};

vector<int> cannon_x = {-3,-3,-3,0,0,3,3,3};
vector<int> cannon_y = {-3,0,3,-3,3,-3,0,3};

vector<int> cannon_middle_x1 = {1,1,0,-1};
vector<int> cannon_middle_y1 = {0,1,1,1};
vector<int> cannon_middle_x2 = {-1,-1,0,1};
vector<int> cannon_middle_y2 = {0,-1,-1,-1};

vector<int> townhall_white_x = {0,2,4,6};
vector<int> townhall_white_y = {0,0,0,0};
vector<int> townhall_black_x = {1,3,5,7};
vector<int> townhall_black_y = {7,7,7,7};
struct bmove{
    char type;
    int x1;
    int y1;
    int x2;
    int y2;
    bmove(){}
    bmove(char type, int x1, int y1, int x2, int y2){
        this->type = type;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    }
};

struct miniMaxAns{
    int utility;
    bmove move;
    miniMaxAns(){
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
            // cout << "HI_5" << endl;
            // cout << endl;
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

    bool enemy_at_pos(int x, int y)
    {
        int enemy = (this->direction + 3)/2;
        if (this->c_board.board_view[x][y] == enemy)
            return true;
        else 
            return false;        
    }
    vector<bmove> getValidMoves(position pos)  // give a vector of possible moves ... Can be a priority queue here according to som heuristics I mean node ordering to make good alpha beta ..
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
                                temp = new bmove('B',pos.x,pos.y,bomb_1_x,bomb_1_y); 
                                moves.push_back(*temp);
                            }
                        }
                        if (onBoard(bomb_2_x,bomb_2_y) && this->c_board.board_view[bomb_checkFront_2_x][bomb_checkFront_2_y] == 0 )
                        {
                            if(this->c_board.board_view[bomb_2_x][bomb_2_y] != self && this->c_board.board_view[bomb_2_x][bomb_2_y] != (-1)*self )
                            {
                                bmove* temp;
                                temp = new bmove('B',pos.x,pos.y,bomb_2_x,bomb_2_y); 
                                moves.push_back(*temp);
                            }
                        }
                    }
                }
            }
        }
        
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
                    temp = new bmove('M',pos.x,pos.y,check_x,check_y); 
                    moves.push_back(*temp);
                }
            }            
        }

        for (int i = 0; i < forward_x.size(); i++)
        {   
            int check_x = pos.x + forward_x[i];
            int check_y =  pos.y + (forward_y[i] * (this->direction));
            if (onBoard(check_x,check_y))
            {
                if(this->c_board.board_view[check_x][check_y] == 0)
                {
                    bmove* temp;
                    temp = new bmove('M',pos.x,pos.y,check_x,check_y); 
                    moves.push_back(*temp);
                }
            }
        }


        if (this->enemyNearby(pos))
        {
            for (int i = 0; i < backward_x.size(); i++)
            {
                int check_x = pos.x + backward_x[i];
                int check_y =  pos.y + (backward_y[i] * this->direction);
                if (onBoard(check_x,check_y))
                {
                    if(this->c_board.board_view[check_x][check_y]  == 0)
                    {
                        bmove* temp;
                        temp = new bmove('M',pos.x,pos.y,check_x,check_y); 
                        moves.push_back(*temp);
                    }   
                }
            }
        }

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
                    temp = new bmove('M',pos.x,pos.y,check_x,check_y); 
                    moves.push_back(*temp);
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

    int utility(){
        int myPlayers,opponent_players;
        int myTownHalls, opponent_townhalls;
        int myCannons, opponent_Cannons;

        int opponentCannonShootableEmpty; 
        int myCannonShootableEmpty;  

        int opponentCannonShootableSomeone; 
        int myCannonShootableSomeone;  

        if (bot == 2) // white
        {
            myCannonShootableEmpty =  this->num_Cannon_shootable(false)[0];
            opponentCannonShootableEmpty  = this->num_Cannon_shootable(true)[0];

            myCannonShootableSomeone =  this->num_Cannon_shootable(false)[1];
            opponentCannonShootableSomeone  = this->num_Cannon_shootable(true)[1];


            myPlayers  = this->c_board.white_players.size();
            opponent_players = this->c_board.black_players.size();
            myTownHalls = this->c_board.num_white_townhalls;
            opponent_townhalls = this->c_board.num_black_townhalls;
        }
        else if(bot == 1) // black This convention is set as a universal variable in the beginning when server sends number of 
        {
            myCannonShootableEmpty =  this->num_Cannon_shootable(true)[0];
            opponentCannonShootableEmpty  = this->num_Cannon_shootable(false)[0];

            myCannonShootableSomeone =  this->num_Cannon_shootable(true)[1];
            opponentCannonShootableSomeone  = this->num_Cannon_shootable(false)[1];

            myPlayers  = this->c_board.black_players.size();
            opponent_players = this->c_board.white_players.size();
            myTownHalls = this->c_board.num_black_townhalls;
            opponent_townhalls = this->c_board.num_white_townhalls;
        }
        
        int A = 1 ; // soldier factor 
        int B = 10; // townhall factor
        int C = 5; // number 
        int D = 2;
        int utility = myPlayers*A+ myTownHalls*B - opponent_players*A - opponent_townhalls*B + ( myCannonShootableEmpty- opponentCannonShootableEmpty)*D + (myCannonShootableSomeone - opponentCannonShootableSomeone) * C;
        return utility;
    }

    void printUtility(){ // just copy the Utility function here for debugging
       int myPlayers,opponent_players;
        int myTownHalls, opponent_townhalls;
        int myCannons, opponent_Cannons;

        int opponentCannonShootableEmpty; 
        int myCannonShootableEmpty;  

        int opponentCannonShootableSomeone; 
        int myCannonShootableSomeone;  

        if (bot == 2) // white
        {
            myCannonShootableEmpty =  this->num_Cannon_shootable(false)[0];
            opponentCannonShootableEmpty  = this->num_Cannon_shootable(true)[0];

            myCannonShootableSomeone =  this->num_Cannon_shootable(false)[1];
            opponentCannonShootableSomeone  = this->num_Cannon_shootable(true)[1];


            myPlayers  = this->c_board.white_players.size();
            opponent_players = this->c_board.black_players.size();
            myTownHalls = this->c_board.num_white_townhalls;
            opponent_townhalls = this->c_board.num_black_townhalls;
        }
        else if(bot == 1) // black This convention is set as a universal variable in the beginning when server sends number of 
        {
            myCannonShootableEmpty =  this->num_Cannon_shootable(true)[0];
            opponentCannonShootableEmpty  = this->num_Cannon_shootable(false)[0];

            myCannonShootableSomeone =  this->num_Cannon_shootable(true)[1];
            opponentCannonShootableSomeone  = this->num_Cannon_shootable(false)[1];

            myPlayers  = this->c_board.black_players.size();
            opponent_players = this->c_board.white_players.size();
            myTownHalls = this->c_board.num_black_townhalls;
            opponent_townhalls = this->c_board.num_white_townhalls;
        }
        
        int A = 1 ; // soldier factor 
        int B = 10; // townhall factor
        int C = 5; // number 
        int D = 2;
        int utility = myPlayers*A+ myTownHalls*B - opponent_players*A - opponent_townhalls*B + ( myCannonShootableEmpty- opponentCannonShootableEmpty)*D + (myCannonShootableSomeone - opponentCannonShootableSomeone) * C;
        cout << "Utility : " << utility << endl;
        cout << "Player diff and score : " << myPlayers-opponent_players << " " << (myPlayers-opponent_players)*A  << endl; 
        cout << "Town Hall diff and score : " << myTownHalls - opponent_townhalls << " " << (myTownHalls - opponent_townhalls)*B  << endl; 
        cout << "Cannon shootable me(empty and someone) , his(empty ans someone) : " << myCannonShootableEmpty << " " << opponentCannonShootableEmpty << " "  << myCannonShootableSomeone << " " << opponentCannonShootableSomeone << endl;


    }

    vector<bmove> getAllValidMoves()
    {
        vector<bmove> moves;
        int num_players;
        if(this->direction == 1)
            num_players = this->c_board.white_players.size();
        else
            num_players = this->c_board.black_players.size();

        for (size_t i = 0; i < num_players; i++)
        {
            vector<bmove> temp;
            if(direction == 1)
                temp =  this->getValidMoves(this->c_board.white_players[i]);
            else
                temp =  this->getValidMoves(this->c_board.black_players[i]);

            moves.insert(moves.end(), temp.begin(), temp.end());
        }
        return moves;
    }

    bmove play(){ // we don't have to take tention whether the current situation is a terminal state or not !! BUT WE HAVE TO IN THE SEARCH
    //    return (this->miniMaxCutoff(4,true)).move;   
        return  this->AplhaBetaSearch();     
    }

    miniMaxAns miniMaxCutoff(int depth,bool max)
    {
        miniMaxAns ans;

        if(this->terminal() || depth == 0)
        {
            ans.utility = this->utility();
            return ans;
        }
        
        vector<bmove> moves = this->getAllValidMoves();
        if(max)
            ans.utility = INT_MIN;
        else
        {
            ans.utility = INT_MAX;
        }
        
        for (int i = 0; i < moves.size(); i++)
        {   
            board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
            int utility_temp = config.miniMaxCutoff(depth-1,!max).utility;
            if(( utility_temp > ans.utility && max) || (( utility_temp < ans.utility && (!max))))
            {
                ans.utility = config.utility();
                ans.move = moves[i];
            }
        }
        return ans;
    }

    miniMaxAns MinValue(int depth, int aplha, int beta)
    {
        miniMaxAns ans;
        if (this->terminal() || (depth == 0))
        {
            ans.utility = this->utility();
            return ans;
        }
        ans.utility = INT_MAX;
        vector<bmove> moves = this->getAllValidMoves();
        for (int i = 0; i < moves.size(); i++)
        {   
            board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
            int utility_temp = (config.MaxValue(depth-1,aplha,beta)).utility;
            if(utility_temp < ans.utility)
            {
                ans.utility = utility_temp;
                ans.move = moves[i];
                if(ans.utility <= aplha)
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

    miniMaxAns MaxValue(int depth, int aplha, int beta)
    {
        miniMaxAns ans;
        if (this->terminal() || (depth == 0))
        {
            ans.utility = this->utility();
            return ans;
        }
        ans.utility = INT_MIN;
        vector<bmove> moves = this->getAllValidMoves();
        for (int i = 0; i < moves.size(); i++)
        {   
            board config = board(this->c_board,moves[i].x1,moves[i].y1,moves[i].type,moves[i].x2,moves[i].y2,(this->direction*(-1)));
            int utility_temp = (config.MinValue(depth-1,aplha,beta)).utility;
            if(utility_temp > ans.utility)
            {
                ans.utility = utility_temp;
                ans.move = moves[i];
                if(ans.utility >= beta)
                {
                    return ans;
                }
            }
            if (ans.utility >= aplha)
            {
                aplha = ans.utility;
            }
        }
        return ans;
    }

    bmove AplhaBetaSearch()
    {
        int players_remaning = this->c_board.white_players.size() + this->c_board.black_players.size() ;
        int depth = 4;
        if ( players_remaning < 15 )
        {
            depth = 5;
        }
        else if ( players_remaning < 7 )
        {
            depth = 6;
        }
        
        miniMaxAns ans = MaxValue(depth,INT_MIN,INT_MAX);
        // cout << "AlphaBeta move : " <<  ans.move.x1 << " " << ans.move.y1 << " " << ans.move.type << " " << ans.move.x2 << " " << ans.move.y2 << endl;
        return ans.move;
    }

    bool terminal(){
        if(this->c_board.num_black_townhalls < 3 || this-> c_board.num_white_townhalls < 3)
            return true;
        else 
            return false;
    }

    void printAllMoves(){
        vector<bmove> moves = this->getAllValidMoves();
        for (int i = 0; i < moves.size(); i++)
        {
            cout << moves[i].x1 << " " << moves[i].y1 << " " << moves[i].type << " " << moves[i].x2 << " " << moves[i].y2 << endl ;
        }
        
    }
};

int main(int argc, char const *argv[])
{

    string a;
    getline(cin,a);
    // cout << a << endl;
    vector<int> white_x = {1,1,1,3,3,3,5,5,5,7,7,7};
    vector<int> white_y = {0,1,2,0,1,2,0,1,2,0,1,2};
    vector<int> black_x = {0,0,0,2,2,2,4,4,4,6,6,6};
    vector<int> black_y = {5,6,7,5,6,7,5,6,7,5,6,7};

    int intial_number_players = 12;

    vector< vector< int > >  board_cells(columns, vector<int> (rows, 0));
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
    b_w.num_black_townhalls = 4;
    b_w.num_white_townhalls = 4;

    board game;

    bool myTurn;
    game = board(b_w,0,0,'N',0,0,-1); // write this thing up 

    myTurn = 1;
    bot = 1;
    if ((int)(a[0]) == 50)
    {
        // cout << "Bot is white" << endl;
        myTurn = false;
        bot = 2;
    }
    else if((int)(a[0]) == 49)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
    {
        // cout << "Bot is black" << endl;
        myTurn = true; 
        bot = 1;
    }
    else
    {
        cout << "ERROR INPUT" << endl;
    }

    // cout << "INITIAL CONFIGURATION" << endl;

    // for (int i = 0; i < 8; i++)
    // {
    //     for (int j = 0; j < 8; j++)
    //     {
    //         if (game.c_board.board_view[j][i] == 0)
    //         {
    //             cout << "O ";
    //         }
    //         else if (game.c_board.board_view[j][i] == 1)
    //         {
    //             cout << "W ";
    //         }
    //         else if (game.c_board.board_view[j][i] == 2)
    //         {
    //             cout << "B ";
    //         }
    //         else if (game.c_board.board_view[j][i] == -1 )
    //         {
    //             cout << "H ";
    //         }
    //         else if (game.c_board.board_view[j][i] == -2 )
    //         {
    //             cout << "T ";
    //         }
    //     }
    //     cout << endl;
    // }
    
    // game.printAllMoves();    


    while (true){
        if (myTurn)
        {
                       
            // cout << "Bot will move:" << endl;
            bmove BotMove = game.play(); //  ........
            cout << "S " << BotMove.x1 << " " << BotMove.y1 << " " << BotMove.type << " " << BotMove.x2 << " " << BotMove.y2 << endl; 
            game = board(game.c_board,BotMove.x1,BotMove.y1,BotMove.type,BotMove.x2,BotMove.y2,game.direction*(-1));
        }
        else
        {
            // cout << "Man will move:" << endl;
            char player;
            char Move;
            int x1,y1;
            int x2,y2;
            string move;
            getline(cin,move);
            // cout << move << endl;
            player = move[0];
            x1 = (int)(move[2]) - 48;            
            y1 = (int)(move[4]) - 48;            
            Move = move[6];            
            x2 = (int)(move[8]) - 48;            
            y2 = (int)(move[10]) - 48;            

            // cout << player << " " << Move << endl;
            game = board(game.c_board,x1,y1,Move,x2,y2,game.direction*(-1));
            // cout << "hi";

        }
        myTurn=!myTurn;

        // for (int i = 0; i < 8; i++)
        // {
        //     for (int j = 0; j < 8; j++)
        //     {
        //         if (game.c_board.board_view[j][i] == 0)
        //         {
        //             cout << "O ";
        //         }
        //         else if (game.c_board.board_view[j][i] == 1)
        //         {
        //             cout << "W ";
        //         }
        //         else if (game.c_board.board_view[j][i] == 2)
        //         {
        //             cout << "B ";
        //         }
        //         else if (game.c_board.board_view[j][i] == -1 )
        //         {
        //             cout << "H ";
        //         }
        //         else if (game.c_board.board_view[j][i] == -2 )
        //         {
        //             cout << "T ";
        //         }
        //     }
        //     cout << endl;
        // }

        // game.printUtility();
    }
    return 0;
}