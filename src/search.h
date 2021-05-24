
#ifndef SEARCH
#define SEARCH

#include<iostream>
#include"BoardManipulate.h"
#include"hash.h"
#include"tools.h"
#include<algorithm>
const int HASH_SIZE = 1 << 20; // �û����С int64
const int MAX_GEN_MOVES = 128;
const int LIMIT_DEPTH = 64;       // �����ļ������
const int SORT_VALUE_MAX = 65535; // �ŷ��������ֵ
const int NULL_DEPTH = 2;    // ���Ųü������
const int IID_DEPTH = 2;         // �ڲ�������������
const int SMP_DEPTH = 6;         // �������������
const int UNCHANGED_DEPTH = 4;   // δ�ı�����ŷ������

const int DROPDOWN_VALUE = 20;   // ���ķ�ֵ
const int RESIGN_VALUE = 300;    // ����ķ�ֵ
const int DRAW_OFFER_VALUE = 40; // ��͵ķ�ֵ
using namespace std;

/***************
hash�û���ṹ��
**************/
//struct HashStruct {
//	bool Flag;	//�Ƿ��Ѽ�����������Ϣ
//	uint32_t dwLock0, dwLock1;	// ZobristУ����
//	uint16_t BestMove;	//����ŷ�
//};

/******************
��������Ϣ��һ�壬��¼�����������õ��Ľ��
*********************/
class SearchInfo {
public:
	MoveSortStruct mvs;				//���ڵ��ŷ�,Ŀǰ����ѡ����ŷ�
	HashStruct HashTable[HASH_SIZE];	// �û���HashTable[x]=�����õ���Ϣ
	int nHistoryTable[65536];			// ��ʷ��
	uint16_t wmvKiller[LIMIT_DEPTH][2]; // ɱ���ŷ���
	int mvResult;			//ĳһ�������õ��Ľ�� �洢��ʽ��H7-E7	(�ڣ�ƽ�����൱��һ��ȫ�ֱ���
	int alpha, beta;		//beta:����ڵ����ֵ alpha:��һ��ڵ����ֵ
	int vlLast;
	Board board;                // �������ľ���
	int time;				//����ʱ��
	int nUnchanged;
	bool bQuit;						   // �Ƿ��յ��˳�ָ��
	bool bDebug;					   // �Ƿ����ģʽ
	bool bStop;				            // ��ֹ�ź�
	bool bUseHash, bUseBook;           // �Ƿ�ʹ���û���ü��Ϳ��ֿ�
	RC4 rc4Random;               // �����
	int nMaxTimer;					   // ���ʹ��ʱ��
	char szBookFile[1024];             // ���ֿ�
	bool CompareHistory(const int lpmv1, const int lpmv2);
	void ClearHistory();
	inline void ClearKiller(uint16_t(*lpwmvKiller)[2]);
	void SetBestMove(int mv, int nDepth, uint16_t* lpwmvKiller);
}searchInfo;

SearchInfo searchInfo;
extern ZobristTable zobristInfo;
/**********************
* �߷��洢����ṹ��
***********************/
enum State { HashState, PHASE_KILLER_1, PHASE_KILLER_2,AllSearchState, OdinaryState };
class MoveSortStruct {
public:
	uint16_t mvs[MAX_GEN_MOVES];           // ���е��߷�
	int bestmv;	//�������������߷�
	State state;
	int mvHash, mvKiller1, mvKiller2; // �û����߷�������ɱ���߷�
	void Init(int mvHash_) { // ��ʼ�����趨�û����߷�������ɱ���߷�
		mvHash = mvHash_;
		mvKiller1 = searchInfo.wmvKiller[searchInfo.board.distance][0];
		mvKiller2 = searchInfo.wmvKiller[searchInfo.board.distance][1];
		state = HashState;
	}
	int Next();
};

// "sort"����ʷ������ıȽϺ���.����ʽ��Խ�õ�Խ����
bool SearchInfo::CompareHistory(const int lpmv1, const int lpmv2) {
	return nHistoryTable[lpmv1] < nHistoryTable[lpmv2];
}
void SearchInfo::ClearHistory() {
	memset(nHistoryTable, 0, 65536 * sizeof(int));
}
// ���ɱ���ŷ���
inline void SearchInfo::ClearKiller(uint16_t(*lpwmvKiller)[2]) {
	memset(lpwmvKiller, 0, LIMIT_DEPTH * sizeof(uint16_t[2]));
}
// ������߷��Ĵ���
void SearchInfo::SetBestMove(int mv, int nDepth, uint16_t* lpwmvKiller) {
	nHistoryTable[mv] += nDepth * nDepth;
	if (lpwmvKiller[0] != mv) {
		lpwmvKiller[1] = lpwmvKiller[0];
		lpwmvKiller[0] = mv;
	}
}

//bool CompareHistory(const int lpmv1, const int lpmv2);
int MoveSortStruct::Next() {
	static int index = 0;
	switch (state) {
	case HashState:
		state = AllSearchState;
		return bestmv;
	case PHASE_KILLER_1:
		state = PHASE_KILLER_2;
		if (mvKiller1 != mvHash && mvKiller1 != 0 && searchInfo.board.isLegalMove(mvKiller1)) {
			return mvKiller1;
		}
		// 2. ɱ���ŷ�����(�ڶ���ɱ���ŷ�)����ɺ�����������һ�׶Σ�
	case PHASE_KILLER_2:
		state = AllSearchState;
		if (mvKiller2 != mvHash && mvKiller2 != 0 && searchInfo.board.isLegalMove(mvKiller2)) {
			return mvKiller2;
		}
	case AllSearchState:
		state = OdinaryState;
		index = searchInfo.board.genMoves(mvs);
		sort(mvs, mvs + index, SearchInfo::CompareHistory);
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