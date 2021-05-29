
#pragma once
#ifndef BOARD_MANIPULATE
#define BOARD_MANIPULATE
#include<algorithm>
#include<iostream>
#include "tools.h"

//棋盘操作
//数据表示法：16x16棋盘，使用9x10的部分（横3~11，纵3~12）
typedef unsigned char int_8;//用来表示棋子、位置
typedef unsigned int int_32;//用来表示校验码等
typedef unsigned short int_16;//用来表示move（起点+终点）

const char* const cszPieceBytesInChineseBlack[7] = { "将","士","象","碼","車","砲","卒" };
const char* const cszPieceBytesInChineseRed[7] = { "帅","仕","相","马","车","炮","兵" };

enum Player { BLACK, RED, NUL };

const int MAX_MOV_NUM = 300;
const int MAX_GEN_MVS = 300;

////////////////////
const int DRAW_VALUE = 20;              // 和棋时返回的分数(取负值)
///////////////////

/////暂取
const int MATE_VALUE = 10000;           // 最高分值，即将死的分值
const int WIN_VALUE = MATE_VALUE - 100; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了


const int KING_TYPE = 0;
const int ADVISOR_TYPE = 1;
const int BISHOP_TYPE = 2;
const int KNIGHT_TYPE = 3;
const int ROOK_TYPE = 4;
const int CANNON_TYPE = 5;
const int PAWN_TYPE = 6;

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


const int LegalSpan[100] = {//标志将17、士18、相19、马的可行性
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	19,-16,0,-16,19,0,0,0,0,0,
	0,0,0,0,0,0,-1,18,17,18,
	1,0,0,0,0,0,0,0,0,0,
	0,0,0,17,0,17,0,0,0,0,
	0,0,0,0,0,0,0,0,-1,18,
	17,18,1,0,0,0,0,0,0,0,
	0,0,0,0,19,16,0,16,19,0,
};

static const bool InBoard[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const bool InFort[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const int PieceType[48] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6
};

const int KING_DELTA[4] = { -16,16,-1,1 };
const int ADVISOR_DELTA[4] = { -17,17,-15,15 };
const int BISHOP_DELTA[4] = { -34,34,-30,30 };
const int KNIGHT_DELTA[8] = { -33, -31, -18, 14, -14, 18, 31, 33 };
const int KNIGHT_CHECK[8] = { -17,-15,-17,15,-15,17,15,17 };

class RC4 {//生成用于zobrist校验的随机码
public:
	int_8 s[256];
	int x, y;

	void InitZero(void);

	int_8 NextByte(void);

	int_32 NextLong(void);
};

extern RC4 rander;

class ZobristNode {
public:
	int_32 key, lock_1, lock_2;//zobrist校验码和校验锁
	void init() { key = 0; lock_1 = 0; lock_2 = 0; }
	void initWithRC4(RC4 &src) 
	{ 
		key = src.NextLong(); 
		lock_1 = src.NextLong(); 
		lock_2 = src.NextLong(); 
	}
	void XOR(ZobristNode a) { key = key ^ a.key; lock_1 = lock_1 ^ a.lock_1; lock_2 = lock_2 ^ a.lock_2; }
};

struct ZobristTable {
	ZobristNode PlayerZobr;
	ZobristNode Table[14][256];//每个格子对应的每种状态的zobrist校验值
};

extern ZobristTable Zobrist;

// 初始化Zobrist表
void InitZobrist(void);

struct MoveStruct {
	int_16 move;
	int_8 captured;
	bool checked;
	ZobristNode zobr;
	char show[4];
	void Set(int mv, int pcCaptured, bool bCheck, uint32_t dwKey_) {
		move = mv;
		captured = pcCaptured;
		checked = bCheck;
		zobr.key = dwKey_;
	}
};

struct PreEvalStruct {
	bool bPromotion;
	int valueAdvanced;         // 开中残局分值，二次函数
	uint8_t ucvlWhitePieces[7][256];
	uint8_t ucvlBlackPieces[7][256];
};

extern PreEvalStruct PreEval;

class Board {
public:
	int_8 chessBoard[256];//棋盘状态
	int_8 chessView[48];//每个子的状态
	uint32_t dwBitPiece;
	uint16_t wBitPiece[2];
	Player player;//玩家
	MoveStruct pastMoves[MAX_MOV_NUM];//上次吃子以来的历史走法,可用来判断重复
	int distance;//与根节点的距离
	int pastMoveNum;//历史走法长度
	int chessNum;//剩余棋子数
	ZobristNode zobr;//置换表校验
	bool checked;//当前被将军标志
	int valueRed, valueBlack; // 黑棋和红棋的子力价值
	Board();
	void refreshBoard(const char* fen, int posLen,const char* moves, int movNum, char side);//根据ucci串更新棋盘
	bool isLegalMove(int_16 mv);//判断一步棋是否合法
	bool isChecked(Player player);//判断某玩家是否被将军
	bool makeMove(int_16 mv);//走一步棋，返回被吃掉的子
	void undoMakeMove();//撤销上一步棋
	void nullMove(void) {                       // 走一步空步
		uint32_t dwKey;
		dwKey = zobr.key;
		changeSide();
		pastMoves[pastMoveNum].Set(0, 0, false, dwKey);
		pastMoveNum++;
		distance++;
	}
	void undoNullMove(void) {                   // 撤消走一步空步
		distance--;
		pastMoveNum--;
		changeSide();
	}
	int gemove_num(int_16* mvs, bool captureOnly = false);//生成走法，返回走法数
	void clearBoard();
	void clearMoves();
	void mirror(Board& mirror) const;
	//void drawBoard();
	int RepStatus(int limit)const;
	bool lastCheck()
	{
		return pastMoves[pastMoveNum - 1].checked;
	}
	/*判断重复情况：　
		A. 返回0，表示没有重复局面；
		B.返回1，表示存在重复局面，但双方都无长将(判和)；
		C.返回3(= 1 + 2)，表示存在重复局面，本方单方面长将(判本方负)；
		D.返回5(= 1 + 4)，表示存在重复局面，对方单方面长将(判对方负)；
		E.返回7(= 1 + 2 + 4)，表示存在重复局面，双方长将(判和)。*/
		// 评估有关函数
	void PreEvaluate(void);
	int Material(void)
	{
		return SIDE_VALUE(player, valueRed - valueBlack) + PreEval.valueAdvanced;
	}

	int Evaluate(void)
	{
		// 只考虑子力平衡，不考虑特殊棋形、牵制、车的灵活性、马的阻碍
		return this->Material();
	}

	int DrawValue(void) const {                 // 和棋分值
		return (distance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
	}

	int RepValue(int nRepStatus) const {        // 重复局面分值
		int vlReturn;
		vlReturn = ((nRepStatus & 2) == 0 ? 0 : distance - MATE_VALUE) +
			((nRepStatus & 4) == 0 ? 0 : MATE_VALUE - distance);
		return vlReturn == 0 ? DrawValue() : vlReturn;
	}
	void drawBoard();

private:
	void addPiece(int_8 pos, int_8 piece);
	void delPiece(int_8 pos);
	void changeSide();
};

////////相关工具函数///////////
inline int coordXY(int x, int y)
{
	return x + (y << 4);
}

inline int getX(int pos)
{
	return pos & 15;
}

inline int getY(int pos)
{
	return pos >> 4;
}

inline bool inBoard(int pos)//位于棋盘中
{
	return InBoard[pos];
}

inline bool inBox(int pos, Player player)//位于某方的小九宫格中
{
	return InFort[pos];
}

inline int_8 getDST(int_16 mv)//一步的终点
{
	return mv >> 8;
}

inline int_8 getSRC(int_16 mv)//一步的起点
{
	return mv & 255;
}

inline int_16 getMV(int_8 src, int_8 dst)
{
	int_16 result = ((int_16)dst) << 8;
	result += (int_16)src;
	return result;
}

inline int pieceIndex(int_8 piece)
{
	return piece & 15;
}

inline Player getSide(int_8 piece)
{
	//if (piece >= 16 && piece < 32)
	//else if (piece >= 32 && piece < 48)
	if (piece & 16)
		return RED;
	else if (piece & 32)
		return BLACK;
	else
		return NUL;
}

inline bool checkSide(int_8 piece, Player player)
{
	return getSide(piece) == player;
}

inline bool crossRiver(Player side, int_8 pos)
{
	switch (side)
	{
	case(RED):
		return inBoard(pos) && getY(pos) <= 7;
	case(BLACK):
		return inBoard(pos) && getY(pos) >= 8;
	}
}

inline bool kingLegal(int_8 src, int_8 dst)
{
	return LegalSpan[dst - src + 64] == 17;
}

inline bool advisorLegal(int_8 src, int_8 dst)
{
	return LegalSpan[dst - src + 64] == 18;
}

inline bool bishopLegal(int_8 src, int_8 dst)
{
	return LegalSpan[dst - src + 64] == 19;
}

inline int_8 knightPin(int_8 src, int_8 dst)
{
	int delta = LegalSpan[dst - src + 64];
	if (delta <= 16)
		return src + delta;
	return src;
}

inline bool sameX(int_8 src, int_8 dst)
{
	return getX(src) == getX(dst);
}

inline bool sameY(int_8 src, int_8 dst)
{
	return getY(src) == getY(dst);
}

// 翻转格子
inline int SQUARE_FLIP(int sq) {
	return 254 - sq;
}

// 纵坐标水平镜像
inline int FILE_FLIP(int x) {
	return 14 - x;
}

// 横坐标垂直镜像
inline int RANK_FLIP(int y) {
	return 15 - y;
}

inline int MIRROR_SQUARE(int sq) {
	return coordXY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

inline int_8 charToChessType(char c)
{
	c = (c >= 'a') ? c : 'a' + c - 'A';//转化为小写
	switch (c) {
	case('k'):
		return KING_TYPE;
	case('a'):
		return ADVISOR_TYPE;
	case('b'):
		return BISHOP_TYPE;
	case('n'):
		return KNIGHT_TYPE;
	case('r'):
		return ROOK_TYPE;
	case('c'):
		return CANNON_TYPE;
	case('p'):
		return PAWN_TYPE;
	}
}

inline int_8 charToPos(char a, char b)
{
	int x, y;
	x = 3 + a - 'a';
	y = 3 + '9' - b;
	return coordXY(x, y);
}

inline Player rival(Player player)
{
	return (Player)(1 - (int)player);
}

inline void MOVE_COORD(int mv,char* rslt) {      // 把着法转换成字符串
	rslt[0] = FILE_X(getSRC(mv)) - 3 + 'a';
	rslt[1] = '9' - RANK_Y(getSRC(mv)) + 3;
	rslt[2] = FILE_X(getDST(mv)) - 3 + 'a';
	rslt[3] = '9' - RANK_Y(getDST(mv)) + 3;
}

#endif

