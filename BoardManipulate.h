#pragma once
#ifndef BOARD_MANIPULATE
#define BOARD_MANIPULATE
//棋盘操作

//数据表示法：16x16棋盘，使用9x10的部分（横3~11，纵3~12）
typedef unsigned char int_8;//用来表示棋子、位置
typedef unsigned int int_32;//用来表示move（起点+终点）

enum Player {BLACK,RED};

const int MAX_MOV_NUM = 256;

const int KING_FROM = 0;
const int KING_TO = 0;//将、帅
const int ADVISOR_FROM = 1;//士、仕
const int ADVISOR_TO = 2;
const int BISHOP_FROM = 3;//相、象
const int BISHOP_TO = 4;
const int KNIGHT_FROM = 5;
const int KNIGHT_TO = 6;//马
const int ROOK_FROM = 7;
const int ROOK_TO = 8;//车
const int CANNON_FROM = 9;
const int CANNON_TO = 10;
const int PAWN_FROM = 11;
const int PAWN_TO = 15;

const int RED_INDEX = 16;//红方子从16开始
const int BLACK_INDEX = 32;//黑方子从32开始 

struct MoveStruct {
	int_32 move;
	int_8 captured;
	bool checked;
};

class Board {
public:
	int_8 chessBoard[256];//棋盘状态
	int_8 chessView[48];//每个子的状态
	Player player;//玩家
	int_32 pastMoves[MAX_MOV_NUM];//上次吃子以来的历史走法
	int pastMoveNum;//历史走法长度
	int chessNum;//剩余棋子数
	
	Board();
	void refreshBoard(const char* fen, const char* moves);//根据ucci串更新棋盘
	bool isLegalMove(int_32 mv);//判断一步棋是否合法
	bool isChecked(Player player);//判断某玩家是否被将军
	int_8 makeMove(int_32 mv);//走一步棋，返回被吃掉的子
	void unMakeMove(int_32 mv,int_8 captured);//撤销一步棋，参数是被吃掉的棋子
	void clearBoard();
	void clearMoves();
};


#endif