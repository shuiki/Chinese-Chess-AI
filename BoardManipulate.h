#pragma once
#ifndef BOARD_MANIPULATE
#define BOARD_MANIPULATE
//���̲���

//���ݱ�ʾ����16x16���̣�ʹ��9x10�Ĳ��֣���3~11����3~12��
typedef unsigned char int_8;//������ʾ���ӡ�λ��
typedef unsigned int int_32;//������ʾmove�����+�յ㣩

enum Player {BLACK,RED};

const int MAX_MOV_NUM = 256;

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

struct MoveStruct {
	int_32 move;
	int_8 captured;
	bool checked;
};

class Board {
public:
	int_8 chessBoard[256];//����״̬
	int_8 chessView[48];//ÿ���ӵ�״̬
	Player player;//���
	int_32 pastMoves[MAX_MOV_NUM];//�ϴγ�����������ʷ�߷�
	int pastMoveNum;//��ʷ�߷�����
	int chessNum;//ʣ��������
	
	Board();
	void refreshBoard(const char* fen, const char* moves);//����ucci����������
	bool isLegalMove(int_32 mv);//�ж�һ�����Ƿ�Ϸ�
	bool isChecked(Player player);//�ж�ĳ����Ƿ񱻽���
	int_8 makeMove(int_32 mv);//��һ���壬���ر��Ե�����
	void unMakeMove(int_32 mv,int_8 captured);//����һ���壬�����Ǳ��Ե�������
	void clearBoard();
	void clearMoves();
};


#endif