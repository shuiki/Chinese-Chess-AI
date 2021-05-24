#pragma once
#ifndef HASH_H
#define HASH_H
#include "BoardManipulate.h"

const int HASH_SIZE = 1 << 20;
const int HASH_ALPHA = 1;
const int HASH_BETA = 2;
const int HASH_PV = 3;

typedef struct {
	int_32 lock_1, lock_2;//校验锁
	int depth;//深度
	int flag;//节点类型标志
	int_16 bestmv;//最优走法
	int val;//估值
}HashStruct;

HashStruct HashTable[HASH_SIZE];

void recordHash(const Board&board,int depth,int val,int flag,int_16 mv)
{
	HashStruct ht = HashTable[board.zobr.key & (HASH_SIZE - 1)];
	if (ht.depth > depth)
		return;
	ht.lock_1 = board.zobr.lock_1;
	ht.lock_2 = board.zobr.lock_2;
	ht.depth = depth;
	ht.flag = flag;
	ht.val = val;
	if (val > WIN_VALUE)
	{
		ht.val += board.distance;
	}
	else if (val < -WIN_VALUE)
	{
		ht.val -= board.distance;
	}
	HashTable[board.zobr.key & (HASH_SIZE - 1)] = ht;
}

int probeHash(const Board& board, int depth, int alpha, int beta, int_16& mv)
{
	HashStruct ht= HashTable[board.zobr.key & (HASH_SIZE - 1)];
	if (ht.bestmv == 0)
	{
		mv = 0;
		return -MATE_VALUE;
	}
	if (ht.lock_1 != board.zobr.lock_1 || ht.lock_2 != board.zobr.lock_2)
	{
		mv = 0;
		return -MATE_VALUE;
	}
	int res = ht.val;
	mv = ht.bestmv;
	bool toCheck=false;
	if (ht.val > WIN_VALUE)
	{
		res -= board.distance;
		toCheck = true;
	}
	else if (ht.val < -WIN_VALUE)
	{
		res += board.distance;
		toCheck = true;
	}
	if (ht.depth >= depth || toCheck)
	{
		if (ht.flag == HASH_ALPHA)
		{
			return res <= alpha ? res : -MATE_VALUE;
		}
		if (ht.flag == HASH_BETA)
		{
			return res >= beta ? res : -MATE_VALUE;
		}
		return res;
	}
	return -MATE_VALUE;
}

#endif // !HASH_H
