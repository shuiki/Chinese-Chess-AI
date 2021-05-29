
#pragma once
#ifndef BOARD_MANIPULATE
#define BOARD_MANIPULATE
#include<algorithm>
#include<iostream>
#include "tools.h"

//���̲���
//���ݱ�ʾ����16x16���̣�ʹ��9x10�Ĳ��֣���3~11����3~12��
typedef unsigned char int_8;//������ʾ���ӡ�λ��
typedef unsigned int int_32;//������ʾУ�����
typedef unsigned short int_16;//������ʾmove�����+�յ㣩

const char* const cszPieceBytesInChineseBlack[7] = { "��","ʿ","��","�a","܇","�h","��" };
const char* const cszPieceBytesInChineseRed[7] = { "˧","��","��","��","��","��","��" };

enum Player { BLACK, RED, NUL };

const int MAX_MOV_NUM = 300;
const int MAX_GEN_MVS = 300;

////////////////////
const int DRAW_VALUE = 20;              // ����ʱ���صķ���(ȡ��ֵ)
///////////////////

/////��ȡ
const int MATE_VALUE = 10000;           // ��߷�ֵ���������ķ�ֵ
const int WIN_VALUE = MATE_VALUE - 100; // ������ʤ���ķ�ֵ���ޣ�������ֵ��˵���Ѿ�������ɱ����


const int KING_TYPE = 0;
const int ADVISOR_TYPE = 1;
const int BISHOP_TYPE = 2;
const int KNIGHT_TYPE = 3;
const int ROOK_TYPE = 4;
const int CANNON_TYPE = 5;
const int PAWN_TYPE = 6;

const int KING_FROM = 0;
const int KING_TO = 0;//����˧
const int ADVISOR_FROM = 1;//ʿ����
const int ADVISOR_TO = 2;
const int BISHOP_FROM = 3;//�ࡢ��
const int BISHOP_TO = 4;
const int KNIGHT_FROM = 5;
const int KNIGHT_TO = 6;//��
const int ROOK_FROM = 7;
const int ROOK_TO = 8;//��
const int CANNON_FROM = 9;
const int CANNON_TO = 10;
const int PAWN_FROM = 11;
const int PAWN_TO = 15;

const int RED_INDEX = 16;//�췽�Ӵ�16��ʼ
const int BLACK_INDEX = 32;//�ڷ��Ӵ�32��ʼ 


const int LegalSpan[100] = {//��־��17��ʿ18����19����Ŀ�����
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

class RC4 {//��������zobristУ��������
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
	int_32 key, lock_1, lock_2;//zobristУ�����У����
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
	ZobristNode Table[14][256];//ÿ�����Ӷ�Ӧ��ÿ��״̬��zobristУ��ֵ
};

extern ZobristTable Zobrist;

// ��ʼ��Zobrist��
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
	int valueAdvanced;         // ���воַ�ֵ�����κ���
	uint8_t ucvlWhitePieces[7][256];
	uint8_t ucvlBlackPieces[7][256];
};

extern PreEvalStruct PreEval;

class Board {
public:
	int_8 chessBoard[256];//����״̬
	int_8 chessView[48];//ÿ���ӵ�״̬
	uint32_t dwBitPiece;
	uint16_t wBitPiece[2];
	Player player;//���
	MoveStruct pastMoves[MAX_MOV_NUM];//�ϴγ�����������ʷ�߷�,�������ж��ظ�
	int distance;//����ڵ�ľ���
	int pastMoveNum;//��ʷ�߷�����
	int chessNum;//ʣ��������
	ZobristNode zobr;//�û���У��
	bool checked;//��ǰ��������־
	int valueRed, valueBlack; // ����ͺ����������ֵ
	Board();
	void refreshBoard(const char* fen, int posLen,const char* moves, int movNum, char side);//����ucci����������
	bool isLegalMove(int_16 mv);//�ж�һ�����Ƿ�Ϸ�
	bool isChecked(Player player);//�ж�ĳ����Ƿ񱻽���
	bool makeMove(int_16 mv);//��һ���壬���ر��Ե�����
	void undoMakeMove();//������һ����
	void nullMove(void) {                       // ��һ���ղ�
		uint32_t dwKey;
		dwKey = zobr.key;
		changeSide();
		pastMoves[pastMoveNum].Set(0, 0, false, dwKey);
		pastMoveNum++;
		distance++;
	}
	void undoNullMove(void) {                   // ������һ���ղ�
		distance--;
		pastMoveNum--;
		changeSide();
	}
	int gemove_num(int_16* mvs, bool captureOnly = false);//�����߷��������߷���
	void clearBoard();
	void clearMoves();
	void mirror(Board& mirror) const;
	//void drawBoard();
	int RepStatus(int limit)const;
	bool lastCheck()
	{
		return pastMoves[pastMoveNum - 1].checked;
	}
	/*�ж��ظ��������
		A. ����0����ʾû���ظ����棻
		B.����1����ʾ�����ظ����棬��˫�����޳���(�к�)��
		C.����3(= 1 + 2)����ʾ�����ظ����棬���������泤��(�б�����)��
		D.����5(= 1 + 4)����ʾ�����ظ����棬�Է������泤��(�жԷ���)��
		E.����7(= 1 + 2 + 4)����ʾ�����ظ����棬˫������(�к�)��*/
		// �����йغ���
	void PreEvaluate(void);
	int Material(void)
	{
		return SIDE_VALUE(player, valueRed - valueBlack) + PreEval.valueAdvanced;
	}

	int Evaluate(void)
	{
		// ֻ��������ƽ�⣬�������������Ρ�ǣ�ơ���������ԡ�����谭
		return this->Material();
	}

	int DrawValue(void) const {                 // �����ֵ
		return (distance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
	}

	int RepValue(int nRepStatus) const {        // �ظ������ֵ
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

////////��ع��ߺ���///////////
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

inline bool inBoard(int pos)//λ��������
{
	return InBoard[pos];
}

inline bool inBox(int pos, Player player)//λ��ĳ����С�Ź�����
{
	return InFort[pos];
}

inline int_8 getDST(int_16 mv)//һ�����յ�
{
	return mv >> 8;
}

inline int_8 getSRC(int_16 mv)//һ�������
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

// ��ת����
inline int SQUARE_FLIP(int sq) {
	return 254 - sq;
}

// ������ˮƽ����
inline int FILE_FLIP(int x) {
	return 14 - x;
}

// �����괹ֱ����
inline int RANK_FLIP(int y) {
	return 15 - y;
}

inline int MIRROR_SQUARE(int sq) {
	return coordXY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

inline int_8 charToChessType(char c)
{
	c = (c >= 'a') ? c : 'a' + c - 'A';//ת��ΪСд
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

inline void MOVE_COORD(int mv,char* rslt) {      // ���ŷ�ת�����ַ���
	rslt[0] = FILE_X(getSRC(mv)) - 3 + 'a';
	rslt[1] = '9' - RANK_Y(getSRC(mv)) + 3;
	rslt[2] = FILE_X(getDST(mv)) - 3 + 'a';
	rslt[3] = '9' - RANK_Y(getDST(mv)) + 3;
}

#endif

