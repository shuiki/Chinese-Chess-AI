#include<iostream>
#include"BoardManipulate.h"
#include<algorithm>
#define MAX_GEN_MOVES 256
const int HASH_SIZE = 1 << 20; // �û����С int64
using namespace std;

/***************
hash�û���ṹ��
**************/
struct HashStruct {
	bool Flag;	//�Ƿ��Ѽ�����������Ϣ
	uint32_t dwLock0, dwLock1;	// ZobristУ����
	uint16_t BestMove;	//����ŷ�
};

extern ZobristInfo zobristInfo;
/**********************
* �߷��洢����ṹ��
***********************/
enum State { HashState, AllSearchState, OdinaryState };
class MoveSortStruct {
public:
	int_16 mvs[MAX_GEN_MOVES];           // ���е��߷�
	int bestmv;	//�������������߷�
	State state;
	/*MoveSortStruct(int Bestmv) {
		bestmv = Bestmv;
	}*/
	int Next();
};
/******************
��������Ϣ��һ�壬��¼�����������õ��Ľ��
*********************/
class SearchInfo {
public:
	MoveSortStruct mvs;				//���ڵ��ŷ�,Ŀǰ����ѡ����ŷ�
	HashStruct HashTable[HASH_SIZE];	// �û���HashTable[x]=�����õ���Ϣ
	int nHistoryTable[65536];			// ��ʷ��
	int time;
	int_16 mvResult;			//ĳһ�������õ��Ľ�� �洢��ʽ��H7-E7	(�ڣ�ƽ�����൱��һ��ȫ�ֱ���
	int alpha, beta;		//beta:����ڵ����ֵ alpha:��һ��ڵ����ֵ
	Board pos;                // �������ľ���
	bool bQuit;						   // �Ƿ��յ��˳�ָ��
	bool bDebug;					   // �Ƿ����ģʽ
	bool bUseHash, bUseBook;           // �Ƿ�ʹ���û���ü��Ϳ��ֿ�
	RC4 rc4Random;               // �����
	int nMaxTimer;					   // ���ʹ��ʱ��
	char szBookFile[1024];             // ���ֿ�
}searchInfo;
// "sort"����ʷ������ıȽϺ���.����ʽ��Խ�õ�Խ����
bool CompareHistory(const int lpmv1, const int lpmv2) {
	return searchInfo.nHistoryTable[lpmv1] < searchInfo.nHistoryTable[lpmv2];
}


//bool CompareHistory(const int lpmv1, const int lpmv2);
int MoveSortStruct::Next() {
	static int index = 0;
	switch (state) {
	case HashState:
		state = AllSearchState;
		return bestmv;
	case AllSearchState:
		state = OdinaryState;
		index = searchInfo.pos.genMoves(mvs);
		sort(mvs, mvs + index, CompareHistory);
	case OdinaryState:
		while (index > 0) {
			int mv = mvs[--index];
			if (mv != bestmv)
				return mv;
		}
	default:
		return 0;
	}

}


/*******************
���ƿ�ȵ�������������������������������һ������alpha-beta�㷨ʵ��
����searchInfo��mvs.mvs�д洢�������߷���˳��
beta:����ڵ���Сֵ alpha:��һ��ڵ�ֵ
**********************/
int RootSearch(int depth, int alpha, int beta)
{
	if (depth <= 0)
		return searchInfo.pos.Evaluate();
	MoveSortStruct MoveSort;
	int_16 mv;
	while ((mv = MoveSort.Next()) != 0) {
		if (searchInfo.pos.makeMove(mv))//���Ǳ����ŷ���������
		{
			int value = -RootSearch(depth - 1, -beta, -alpha);
			searchInfo.pos.undoMakeMove();
			if (value > beta)
				return beta;
			if (value > alpha)
				alpha = value;
		}
	}
	searchInfo.alpha = alpha;
	searchInfo.beta = beta;
	return alpha;
}
/***********************
����������
˼·��������������:��Ҫ������ȱ�����������֪���
************************/
void SearchMain(int depth)
{
	for (int i = 0; i < depth; i++)
	{
		RootSearch(i, searchInfo.alpha, searchInfo.beta);

	}
	//��ɵ���������������ý��
	uint32_t result = MOVE_COORD(searchInfo.mvResult);//�����ת��Ϊ������ַ��� int->char*
	printf("bestmove %.4s\n", (const char*)&result);
	fflush(stdout);
}