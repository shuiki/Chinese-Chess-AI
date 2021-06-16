

#ifndef SEARCH_H
#define SEARCH_H

#include<iostream>
#include"BoardManipulate.h"
#include"hash.h"
#include"tools.h"
#include<algorithm>
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
enum State { HashState, PHASE_KILLER_1, PHASE_KILLER_2, AllSearchState, OdinaryState };

class SearchInfo {
public:
	//MoveSortStruct* mvs;				//���ڵ��ŷ�,Ŀǰ����ѡ����ŷ�
	//HashStruct HashTable[HASH_SIZE];	// �û���HashTable[x]=�����õ���Ϣ
	int nHistoryTable[65536];			// ��ʷ��
	uint16_t wmvKiller[LIMIT_DEPTH][2]; // ɱ���ŷ���
	int64_t Timer;                     // ��ʱ��
	uint32_t mvResult;					// �߷�
	bool bStop;				            // ͣflag
	int vlLast;
	Board board;					// ����
	bool bQuit;						   // �˳�flag
	bool bDebug;					   // debug flag
	bool bUseHash, bUseBook;           // �û���Ϳ��ֿ�
	RC4 rc4Random;               // �����
	int64_t TimeMax;					   // ����ʱ��
	char szBookFile[1024];             // ���ֿ�
	//bool CompareHistory(const int lpmv1, const int lpmv2);
	void ClearHistory();
	inline void ClearKiller(uint16_t(*lpwmvKiller)[2]);
	void SetBestMove(int mv, int nDepth, uint16_t* lpwmvKiller);

};

extern HashStruct HashTable[HASH_SIZE];
extern SearchInfo searchInfo;

class MoveSortStruct {
public:
	uint16_t mvs[MAX_GEN_MVS];           // ���е��߷�
	State state;
	int mvHash, mvKiller1, mvKiller2; // �û����߷�������ɱ���߷�
	int index;
	void Init(int mvHash_) { // ��ʼ�����趨�û����߷�������ɱ���߷�
		mvHash = mvHash_;
		index = 0;
		mvKiller1 = searchInfo.wmvKiller[searchInfo.board.distance][0];
		mvKiller2 = searchInfo.wmvKiller[searchInfo.board.distance][1];
		state = HashState;
	}
	int Next();
};
/**********************
* �߷��洢����ṹ��
***********************/
extern MoveSortStruct mvs;
// "sort"����ʷ������ıȽϺ���.����ʽ��Խ�õ�Խ����
inline bool CompareHistory(const int lpmv1, const int lpmv2) {
	return searchInfo.nHistoryTable[lpmv1] < searchInfo.nHistoryTable[lpmv2];
}
extern ZobristTable zobristInfo;

void SearchMain(int nDepth);

inline void SearchInfo::ClearHistory() {
	memset(nHistoryTable, 0, 65536 * sizeof(int));
}
// ���ɱ���ŷ���
inline void SearchInfo::ClearKiller(uint16_t(*lpwmvKiller)[2]) {
	memset(lpwmvKiller, 0, LIMIT_DEPTH * sizeof(uint16_t[2]));
}
// ������߷��Ĵ���
inline void SearchInfo::SetBestMove(int mv, int nDepth, uint16_t* lpwmvKiller) {
	nHistoryTable[mv] += nDepth * nDepth;
	if (lpwmvKiller[0] != mv) {
		lpwmvKiller[1] = lpwmvKiller[0];
		lpwmvKiller[0] = mv;
	}
}

//bool CompareHistory(const int lpmv1, const int lpmv2);
inline int MoveSortStruct::Next() {
	//index = 0;
	switch (state) {
	case HashState:
		state = PHASE_KILLER_1;
		if (mvHash != 0)
			return mvHash;
	case PHASE_KILLER_1:
		state = PHASE_KILLER_2;
		if (mvKiller1 != mvHash && mvKiller1 != 0 && searchInfo.board.isLegalMove(mvKiller1)) {
			return mvKiller1;
		}
	case PHASE_KILLER_2:
		state = AllSearchState;
		if (mvKiller2 != mvHash && mvKiller2 != 0 && searchInfo.board.isLegalMove(mvKiller2)) {
			return mvKiller2;
		}
	case AllSearchState:
		state = OdinaryState;
		index = searchInfo.board.gemove_num(mvs);
		sort(mvs, mvs + index, CompareHistory);
	case OdinaryState:
		while (index > 0) {
			int mv = mvs[--index];
			if (mv != mvHash && mv != mvKiller1 && mv != mvKiller2)
				return mv;
		}
	default:
		return 0;
	}

}
#endif