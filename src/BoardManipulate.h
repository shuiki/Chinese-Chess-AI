#pragma once
#ifndef BOARD_MANIPULATE
#define BOARD_MANIPULATE
#include<algorithm>
#include<iostream>

//���̲���

//���ݱ�ʾ����16x16���̣�ʹ��9x10�Ĳ��֣���3~11����3~12��
typedef unsigned char int_8;//������ʾ���ӡ�λ��
typedef unsigned int int_32;//������ʾУ�����
typedef unsigned short int_16;//������ʾmove�����+�յ㣩

enum Player {BLACK,RED,NUL};

const int MAX_MOV_NUM = 256;
const int MAX_GEN_MVS = 256;

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

const int PieceType[48] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6,
  0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 6, 6, 6
};

const int KING_DELTA[4] = { -16,16,-1,1 };
const int ADVISOR_DELTA[4] = { -17,17,-15,15 };
const int BISHOP_DELTA[4] = { -34,34,-30,30 };
const int KNIGHT_DELTA[8] = { -33, -31, -18, 14, -14, 18, 31, 33 };

class RC4 {//��������zobristУ��������
public:
	int_8 s[256];
	int x, y;

	void InitZero(void) {
		int i, j;
		int_8 uc;

		x = y = j = 0;
		for (i = 0; i < 256; i++) {
			s[i] = i;
		}
		for (i = 0; i < 256; i++) {
			j = (j + s[i]) & 255;
			uc = s[i];
			s[i] = s[j];
			s[j] = uc;
		}
	}

	int_8 NextByte(void) {
		x = (x + 1) & 255;
		y = (y + s[x]) & 255;
		std::swap(s[x], s[y]);
		return s[(s[x] + s[y]) & 255];
	}

	int_32 NextLong(void) {
		union {
			int_8 uc[4];
			int_32 dw;
		} Ret;
		Ret.uc[0] = NextByte();
		Ret.uc[1] = NextByte();
		Ret.uc[2] = NextByte();
		Ret.uc[3] = NextByte();
		return Ret.dw;
	}
};

RC4 rander;

class ZobristNode {
public:
	int_32 key, lock_1, lock_2;//zobristУ�����У����
	void init() { key = 0; lock_1 = 0; lock_2 = 0; }
	void initWithRC4(RC4 src) { key = src.NextLong(); lock_1 = src.NextLong(); lock_2 = src.NextLong(); }
	void XOR(ZobristNode a) { key = key ^ a.key; lock_1 = lock_1 ^ a.lock_1; lock_2 = lock_2 ^ a.lock_2; }
};

struct ZobristTable {
	ZobristNode PlayerZobr;
	ZobristNode Table[14][256];//ÿ�����Ӷ�Ӧ��ÿ��״̬��zobristУ��ֵ
};

ZobristTable Zobrist;

struct MoveStruct {
	int_16 move;
	int_8 captured;
	bool checked;
	ZobristNode zobr;
};



class Board {
public:
	int_8 chessBoard[256];//����״̬
	int_8 chessView[48];//ÿ���ӵ�״̬
	Player player;//���
	MoveStruct pastMoves[MAX_MOV_NUM];//�ϴγ�����������ʷ�߷�,�������ж��ظ�
	int distance;//����ڵ�ľ���
	int pastMoveNum;//��ʷ�߷�����
	int chessNum;//ʣ��������
	ZobristNode zobr;//�û���У��
	bool checked;//��ǰ��������־
	Board();
	void refreshBoard(const char* fen, const char* moves, char side);//����ucci����������
	bool isLegalMove(int_16 mv);//�ж�һ�����Ƿ�Ϸ�
	bool isChecked(Player player);//�ж�ĳ����Ƿ񱻽���
	int_8 makeMove(int_16 mv);//��һ���壬���ر��Ե�����
	void undoMakeMove();//������һ����
	int genMoves(int_16* mvs,bool captureOnly=false);//�����߷��������߷���
	void clearBoard();
	void clearMoves();
	void drawBoard();
private:
	void addPiece(int_8 pos,int_8 piece);
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
	return (pos >= 51) && (pos <= 203);
}

inline bool inBox(int pos,Player player)//λ��ĳ����С�Ź�����
{
	switch (player) {
	case(BLACK):
		return pos >= 54 && pos <= 88;
	case(RED):
		return pos >= 163 && pos <= 200;
	}
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

inline bool crossRiver(Player side,int_8 pos)
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

#endif