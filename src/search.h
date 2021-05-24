
#ifndef SEARCH
#define SEARCH

#include<iostream>
#include"BoardManipulate.h"
#include"hash.h"
#include"tools.h"
#include<algorithm>
const int HASH_SIZE = 1 << 20; // 置换表大小 int64
const int MAX_GEN_MOVES = 128;
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
class SearchInfo {
public:
	MoveSortStruct mvs;				//根节点着法,目前可以选择的着法
	HashStruct HashTable[HASH_SIZE];	// 置换表HashTable[x]=搜索得到信息
	int nHistoryTable[65536];			// 历史表
	uint16_t wmvKiller[LIMIT_DEPTH][2]; // 杀手着法表
	int mvResult;			//某一步搜索得到的结果 存储方式：H7-E7	(炮８平５）相当于一个全局变量
	int alpha, beta;		//beta:本层节点最大值 alpha:上一层节点最大值
	int vlLast;
	Board board;                // 待搜索的局面
	int time;				//限制时间
	int nUnchanged;
	bool bQuit;						   // 是否收到退出指令
	bool bDebug;					   // 是否调试模式
	bool bStop;				            // 中止信号
	bool bUseHash, bUseBook;           // 是否使用置换表裁剪和开局库
	RC4 rc4Random;               // 随机数
	int nMaxTimer;					   // 最大使用时间
	char szBookFile[1024];             // 开局库
	bool CompareHistory(const int lpmv1, const int lpmv2);
	void ClearHistory();
	inline void ClearKiller(uint16_t(*lpwmvKiller)[2]);
	void SetBestMove(int mv, int nDepth, uint16_t* lpwmvKiller);
}searchInfo;

SearchInfo searchInfo;
extern ZobristTable zobristInfo;
/**********************
* 走法存储排序结构体
***********************/
enum State { HashState, PHASE_KILLER_1, PHASE_KILLER_2,AllSearchState, OdinaryState };
class MoveSortStruct {
public:
	uint16_t mvs[MAX_GEN_MOVES];           // 所有的走法
	int bestmv;	//计算出来的最好走法
	State state;
	int mvHash, mvKiller1, mvKiller2; // 置换表走法和两个杀手走法
	void Init(int mvHash_) { // 初始化，设定置换表走法和两个杀手走法
		mvHash = mvHash_;
		mvKiller1 = searchInfo.wmvKiller[searchInfo.board.distance][0];
		mvKiller2 = searchInfo.wmvKiller[searchInfo.board.distance][1];
		state = HashState;
	}
	int Next();
};

// "sort"按历史表排序的比较函数.排序方式：越好的越靠后
bool SearchInfo::CompareHistory(const int lpmv1, const int lpmv2) {
	return nHistoryTable[lpmv1] < nHistoryTable[lpmv2];
}
void SearchInfo::ClearHistory() {
	memset(nHistoryTable, 0, 65536 * sizeof(int));
}
// 清空杀手着法表
inline void SearchInfo::ClearKiller(uint16_t(*lpwmvKiller)[2]) {
	memset(lpwmvKiller, 0, LIMIT_DEPTH * sizeof(uint16_t[2]));
}
// 对最佳走法的处理
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
		// 2. 杀手着法启发(第二个杀手着法)，完成后立即进入下一阶段；
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