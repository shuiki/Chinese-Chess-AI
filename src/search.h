
#ifndef SEARCH
#define SEARCH

#include<iostream>
#include"BoardManipulate.h"
#include"hash.h"
//#include"tools.h"
#include<algorithm>
const int HASH_SIZE = 1 << 20; // �û����С int64
const int MAX_GEN_MOVES = 128;
using namespace std;

/***************
hash�û���ṹ��
**************/
//struct HashStruct {
//	bool Flag;	//�Ƿ��Ѽ�����������Ϣ
//	uint32_t dwLock0, dwLock1;	// ZobristУ����
//	uint16_t BestMove;	//����ŷ�
//};

extern ZobristTable zobristInfo;
/**********************
* �߷��洢����ṹ��
***********************/
enum State { HashState, AllSearchState, OdinaryState };
class MoveSortStruct {
public:
	uint16_t mvs[MAX_GEN_MOVES];           // ���е��߷�
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
	int mvResult;			//ĳһ�������õ��Ľ�� �洢��ʽ��H7-E7	(�ڣ�ƽ�����൱��һ��ȫ�ֱ���
	int alpha, beta;		//beta:����ڵ����ֵ alpha:��һ��ڵ����ֵ
	Board board;                // �������ľ���
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
		index = searchInfo.board.GenMoves(mvs);
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


#endif