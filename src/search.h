

#ifndef SEARCH_H
#define SEARCH_H

#include<iostream>
#include"BoardManipulate.h"
#include"hash.h"
#include"tools.h"
#include<algorithm>
const int LIMIT_DEPTH = 64;       // 搜索的极限深度
const int SORT_VALUE_MAX = 65535; // 着法序列最大值
const int NULL_DEPTH = 2;    // 空着裁剪的深度
const int IID_DEPTH = 2;         // 内部迭代加深的深度
const int SMP_DEPTH = 6;         // 并行搜索的深度
const int UNCHANGED_DEPTH = 4;   // 未改变最佳着法的深度

const int DROPDOWN_VALUE = 20;   // 落后的分值
const int RESIGN_VALUE = 300;    // 认输的分值
const int DRAW_OFFER_VALUE = 40; // 提和的分值
using namespace std;

/***************
hash置换表结构体
**************/
//struct HashStruct {
//	bool Flag;	//是否已检索过，有信息
//	uint32_t dwLock0, dwLock1;	// Zobrist校验锁
//	uint16_t BestMove;	//最佳着法
//};

/******************
集搜索信息于一体，记录整盘棋搜索得到的结果
*********************/
enum State { HashState, PHASE_KILLER_1, PHASE_KILLER_2, AllSearchState, OdinaryState };

class SearchInfo {
public:
	//MoveSortStruct* mvs;				//根节点着法,目前可以选择的着法
	//HashStruct HashTable[HASH_SIZE];	// 置换表HashTable[x]=搜索得到信息
	int nHistoryTable[65536];			// 历史表
	uint16_t wmvKiller[LIMIT_DEPTH][2]; // 杀手着法表
	int64_t Timer;                     // 计时器
	uint32_t mvResult;					// 走法
	bool bStop;				            // 停flag
	int vlLast;
	Board board;					// 局面
	bool bQuit;						   // 退出flag
	bool bDebug;					   // debug flag
	bool bUseHash, bUseBook;           // 置换表和开局库
	RC4 rc4Random;               // 随机数
	int64_t TimeMax;					   // 限制时间
	char szBookFile[1024];             // 开局库
	//bool CompareHistory(const int lpmv1, const int lpmv2);
	void ClearHistory();
	inline void ClearKiller(uint16_t(*lpwmvKiller)[2]);
	void SetBestMove(int mv, int nDepth, uint16_t* lpwmvKiller);

};

extern HashStruct HashTable[HASH_SIZE];
extern SearchInfo searchInfo;

class MoveSortStruct {
public:
	uint16_t mvs[MAX_GEN_MVS];           // 所有的走法
	State state;
	int mvHash, mvKiller1, mvKiller2; // 置换表走法和两个杀手走法
	int index;
	void Init(int mvHash_) { // 初始化，设定置换表走法和两个杀手走法
		mvHash = mvHash_;
		index = 0;
		mvKiller1 = searchInfo.wmvKiller[searchInfo.board.distance][0];
		mvKiller2 = searchInfo.wmvKiller[searchInfo.board.distance][1];
		state = HashState;
	}
	int Next();
};
/**********************
* 走法存储排序结构体
***********************/
extern MoveSortStruct mvs;
// "sort"按历史表排序的比较函数.排序方式：越好的越靠后
inline bool CompareHistory(const int lpmv1, const int lpmv2) {
	return searchInfo.nHistoryTable[lpmv1] < searchInfo.nHistoryTable[lpmv2];
}
extern ZobristTable zobristInfo;

void SearchMain(int nDepth);

inline void SearchInfo::ClearHistory() {
	memset(nHistoryTable, 0, 65536 * sizeof(int));
}
// 清空杀手着法表
inline void SearchInfo::ClearKiller(uint16_t(*lpwmvKiller)[2]) {
	memset(lpwmvKiller, 0, LIMIT_DEPTH * sizeof(uint16_t[2]));
}
// 对最佳走法的处理
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