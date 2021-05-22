
#ifndef SEARCH
#define SEARCH

#include<iostream>
#include"BoardManipulate.h"
#include"hash.h"
//#include"tools.h"
#include<algorithm>
const int HASH_SIZE = 1 << 20; // 置换表大小 int64
const int MAX_GEN_MOVES = 128;
using namespace std;

/***************
hash置换表结构体
**************/
//struct HashStruct {
//	bool Flag;	//是否已检索过，有信息
//	uint32_t dwLock0, dwLock1;	// Zobrist校验锁
//	uint16_t BestMove;	//最佳着法
//};

extern ZobristTable zobristInfo;
/**********************
* 走法存储排序结构体
***********************/
enum State { HashState, AllSearchState, OdinaryState };
class MoveSortStruct {
public:
	uint16_t mvs[MAX_GEN_MOVES];           // 所有的走法
	int bestmv;	//计算出来的最好走法
	State state;
	/*MoveSortStruct(int Bestmv) {
		bestmv = Bestmv;
	}*/
	int Next();
};
/******************
集搜索信息于一体，记录整盘棋搜索得到的结果
*********************/
class SearchInfo {
public:
	MoveSortStruct mvs;				//根节点着法,目前可以选择的着法
	HashStruct HashTable[HASH_SIZE];	// 置换表HashTable[x]=搜索得到信息
	int nHistoryTable[65536];			// 历史表
	int time;
	int mvResult;			//某一步搜索得到的结果 存储方式：H7-E7	(炮８平５）相当于一个全局变量
	int alpha, beta;		//beta:本层节点最大值 alpha:上一层节点最大值
	Board board;                // 待搜索的局面
	bool bQuit;						   // 是否收到退出指令
	bool bDebug;					   // 是否调试模式
	bool bUseHash, bUseBook;           // 是否使用置换表裁剪和开局库
	RC4 rc4Random;               // 随机数
	int nMaxTimer;					   // 最大使用时间
	char szBookFile[1024];             // 开局库
}searchInfo;
// "sort"按历史表排序的比较函数.排序方式：越好的越靠后
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