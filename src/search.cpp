#include"search.h"

MoveSortStruct mvs;
SearchInfo searchInfo;
HashStruct HashTable[HASH_SIZE];
const bool NO_NULL = false; // "SearchPV()"的参数，是否禁止空着裁剪

// 重复裁剪
int RepPruning(const Board& pos, int vlBeta) {
	int vlRep = pos.RepStatus(1);
	if (vlRep > 0) {
		return pos.RepValue(vlRep);
	}
	return -MATE_VALUE;
}

// MVV/LVA每种子力的价值
static int value_MVV[24] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  5, 1, 1, 3, 4, 3, 2, 0,
  5, 1, 1, 3, 4, 3, 2, 0
};
 int get_value(int mv)
{
	return (value_MVV[searchInfo.board.chessBoard[getDST(mv)]] << 3) - value_MVV[searchInfo.board.chessBoard[getSRC(mv)]];
}

bool Compare_MVV(const int lpmv1, const int lpmv2) 
{
	if (get_value(lpmv1) > get_value(lpmv2))
		return true;
	else 
		return false;
}

// 静态搜索
 int Quies(Board& pos, int Alpha, int Beta) {

	 // 达到极限深度，直接返回局面评价值；
	 if (pos.distance == LIMIT_DEPTH) {
		 return  pos.Evaluate();
	 }

	int val, Move_num=0,val_best= -MATE_VALUE;
	int_16 mvs[MAX_GEN_MVS];

	// 重复裁剪；
	val = RepPruning(pos, Beta);
	if (val > -MATE_VALUE) {
		return val;
	}
	if (val > Alpha) 
		Alpha = val;
	// 被将军局面:
	if (pos.isChecked(pos.player)) {
		Move_num = pos.genMoves(mvs);
		std::sort(mvs, mvs + Move_num, CompareHistory);
	}
	// 未被将军局面:
	else {		
		val = pos.Evaluate();
		if (val >= Beta) {
			return val;
		}
		val_best = val;
		if (val > Alpha) {
			Alpha = val;
		}
		Move_num = pos.genMoves(mvs, true);
		std::sort(mvs, mvs + Move_num, Compare_MVV);
	}
	// 对生成着法进行A-B搜索
	for (int i = 0; i < Move_num; i++) {
		pos.makeMove(mvs[i]);
		//被将军局面则不进行搜索：
		if (pos.isChecked(pos.player)){
			pos.undoMakeMove();
			continue;
		}
		//未被将军局面：
		val = -Quies(pos, -Beta, -Alpha);
		if (val > val_best) {
			if (val >= Beta) 
				return val;			
			val_best = val;
			if (val > Alpha) 
				Alpha = val;		
		}
		pos.undoMakeMove();
	}

	return (val_best == -MATE_VALUE) ? pos.distance - MATE_VALUE : val_best;
	
}

/*******************
限制宽度的深度优先搜索（迭代深度优先搜索的一步）用alpha-beta算法实现
根据searchInfo中mvs.mvs中存储的所有走法及顺序，
beta:本层节点最小值 alpha:上一层节点值
散列项里到底要保存什么值，并且当你要获取它时怎样来做。
答案是储存一个值，另加一个标志来说明这个值是什么含义。
**********************/
static int SearchPV(int depth, int alpha, int beta, bool bNoNull = false)
{
	int vl,vlBest,nHashFlag;
	int nNewDepth, nCurrTimer;
	uint16_t mvBest, mvHash, mv;
	MoveSortStruct MoveSort;
	// 1. 在叶子结点处调用静态搜索；
	if (depth <= 0)
		//return searchInfo.board.valueRed - searchInfo.board.valueBlack;
		return Quies(searchInfo.board, alpha, beta);
		//return Evaluate(searchInfo.board);
	
	// 2. 重复裁剪；
	vl = RepPruning(searchInfo.board, beta);
	if (vl > -MATE_VALUE) {//-MATE_VALUE是重复的特定返回值
		return vl;
	}
	// 3. 置换裁剪；如果该搜索节点在置换表中出现
	vl = probeHash(searchInfo.board, depth, alpha, beta, mvHash);
	if (searchInfo.bUseHash && vl > -MATE_VALUE) {
		// 由于PV结点不适用置换裁剪，所以不会发生PV路线中断的情况
		return vl;
	}
	// 4. 达到极限深度，直接返回评价值；
	if (searchInfo.board.distance == LIMIT_DEPTH) {
		return searchInfo.board.Evaluate();
	}
	// 5. 尝试空着裁剪；
	if (bNoNull){ //&& !searchInfo.board.isChecked() && searchInfo.board.nullOkay()) {
		searchInfo.board.nullMove();
		vl = -SearchPV(depth - NULL_DEPTH - 1 ,-beta, 1 - beta,NO_NULL);
		searchInfo.board.undoNullMove();

		if (vl >= beta) {
			return vl;
		}
	}
	// 6. 初始化；
	mvBest = 0;
	nHashFlag = HASH_ALPHA;
	vlBest = -MATE_VALUE;
	MoveSort.Init(mvHash);
	while ((mv = MoveSort.Next()) != 0) {
		searchInfo.board.makeMove(mv);
		//若被将军则不尝试
		if (searchInfo.board.isChecked(searchInfo.board.player))
		{
			searchInfo.board.undoMakeMove();
			continue;
		}
		int value = -SearchPV(depth - 1, -beta, -alpha);
		searchInfo.board.undoMakeMove();
		if (value > beta) {
			mvBest = mv;
			nHashFlag = HASH_BETA;
			break;
		}
				
		if (value > alpha)
		{
			mvBest = mv;
			nHashFlag = HASH_PV;
			alpha = vl;
		}
		
		nCurrTimer = (int)(GetTime() - searchInfo.llTime);
		if (nCurrTimer > searchInfo.nMaxTimer) {
			searchInfo.bStop = true;
		}
	}
	//searchInfo.alpha = alpha;
	//searchInfo.beta = beta;
	// 11. 更新置换表、历史表和杀手着法表。
	if (vlBest == -MATE_VALUE) {
		return searchInfo.board.distance - MATE_VALUE;
	}
	else {
		recordHash(searchInfo.board, nHashFlag, vlBest, depth, mvBest);
		if (mvBest != 0) {
			searchInfo.SetBestMove(mvBest, depth, searchInfo.wmvKiller[searchInfo.board.distance]);
		}
		return vlBest;
	}
	return alpha;
}
int SearchRoot(int depth) {
	int nNewDepth, vlBest, vl, mv, nCurrTimer;
	// 根结点搜索例程包括以下几个步骤：
	// 1. 初始化
	vlBest = -MATE_VALUE;
	mvs.Init(searchInfo.mvResult);
	// 2. 逐一搜索每个着法
	while ((mv = mvs.Next()) != 0) {
		searchInfo.board.makeMove(mv);
		//若被将军则不尝试
		if (searchInfo.board.isChecked(searchInfo.board.player))
		{
			searchInfo.board.undoMakeMove();
			continue;
		}
		// 3. 尝试选择性延伸(只考虑将军延伸)
		nNewDepth = (searchInfo.board.isChecked(searchInfo.board.player) ? depth : depth - 1);
		// 4. 主要变例搜索
		if (vlBest == -MATE_VALUE) {
			vl = -SearchPV(nNewDepth, -MATE_VALUE, MATE_VALUE,NO_NULL);
		}
		else {
			vl = -SearchPV(nNewDepth ,-vlBest - 1, -vlBest);
			if (vl > vlBest) { // 这里不需要" && vl < MATE_VALUE"了
				vl = -SearchPV(nNewDepth, -MATE_VALUE, -vlBest,  NO_NULL);
			}
		}
		searchInfo.board.undoMakeMove();
		if (searchInfo.bStop) {
			return vlBest;
		}
		// 5. Alpha-Beta边界判定("vlBest"代替了"SearchPV()"中的"vlAlpha")
		if (vl > vlBest) {
			// 6. 如果搜索到第一着法，那么"未改变最佳着法"的计数器加1，否则清零
			searchInfo.nUnchanged = (vlBest == -MATE_VALUE ? searchInfo.nUnchanged + 1 : 0);
			vlBest = vl;
			// 7. 搜索到最佳着法时记录主要变例
			searchInfo.mvResult = mv;
		}

		nCurrTimer = (int)(GetTime() - searchInfo.llTime);
		if (nCurrTimer > searchInfo.nMaxTimer) {
			searchInfo.bStop = true;
		}
	}
	/*printf("%d SSSSSSSSSSSSSSSSSSS:afer search\n",depth);
	fflush(stdout);
	searchInfo.board.drawBoard();*/
	recordHash(searchInfo.board, depth, vlBest, HASH_PV, searchInfo.mvResult);
	searchInfo.SetBestMove(searchInfo.mvResult, depth, searchInfo.wmvKiller[searchInfo.board.distance]);
	return vlBest;
}
/***********************
主搜索函数
思路：迭代加深搜索:主要深度优先遍历，利用已知结果
************************/
void SearchMain(int depth)
{
	//searchInfo.ClearHistory();
	//for (int i = 0; i < depth; i++)
	//{
	//	SearchRoot(i);

	//	// 搜索到杀棋则终止搜索
	//	if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE) {
	//		break;
	//	}
	//}
	int i, vl, vlLast;
	int nCurrTimer, nLimitTimer;
	int nBookMoves;
	//BookStruct bks[MAX_GEN_MOVES];
	// 主搜索例程包括以下几个步骤：


// 2. 从开局库中搜索着法
	//if (searchInfo.bUseBook) {
	//	// a. 获取开局库中的所有走法
	//	nBookMoves = GetBookMoves(searchInfo.board, bks);
	//	if (nBookMoves > 0) {
	//		vl = 0;
	//		for (i = 0; i < nBookMoves; i++) {
	//			vl += bks[i].wvl;
	//		}
	//		// b. 根据权重随机选择一个走法
	//		vl = searchInfo.rc4Random.NextLong() % (uint32_t)vl;
	//		for (i = 0; i < nBookMoves; i++) {
	//			vl -= bks[i].wvl;
	//			if (vl < 0) {
	//				break;
	//			}
	//		}
	//		// c. 如果开局库中的着法构成循环局面，那么不走这个着法
	//		searchInfo.board.makeMove(bks[i].wmv);
	//		if (searchInfo.board.RepStatus(3) == 0) {
	//			searchInfo.mvResult = bks[i].wmv;
	//			searchInfo.board.undoMakeMove();
	//			uint32_t result = MOVE_COORD(searchInfo.mvResult);
	//			printf("bestmove %.4s\n", (const char*)&result);
	//			fflush(stdout);
	//			if (searchInfo.bDebug)
	//				searchInfo.board.drawBoard();
	//			return;
	//		}
	//		searchInfo.board.undoMakeMove();
	//	}
	//}

	// 3. 如果深度为零则返回静态搜索值
	if (depth == 0) {
		vl = Quies(searchInfo.board, -MATE_VALUE, MATE_VALUE);
		//		vl = Evaluate(searchInfo.board);
		if (searchInfo.bDebug) {
			printf("info depth 0 score %d\n", vl);
			fflush(stdout);
		}
		return;
	}

	// 4. 初始化时间和计数器
	searchInfo.bStop = false;
	searchInfo.nUnchanged = 0;
	searchInfo.mvResult = 0;
	searchInfo.ClearKiller(searchInfo.wmvKiller);
	searchInfo.ClearHistory();
	memset(HashTable, 0, sizeof(HashTable));
	// 由于 ClearHash() 需要消耗一定时间，所以计时从这以后开始比较合理
	searchInfo.llTime = GetTime();
	vlLast = 0;
	nCurrTimer = 0;
	
	// 5. 做迭代加深搜索
	for (i = 1; i <= depth; i++) {
		// 6. 搜索根结点
		vl = SearchRoot(i);
		if (searchInfo.bStop) {
			if (vl > -MATE_VALUE) {
				vlLast = vl; // 跳出后，vlLast会用来判断认输或投降，所以需要给定最近一个值
			}
			break; // 没有跳出，则"vl"是可靠值
		}
		if (searchInfo.bDebug) {
			printf("info depth %d score %d\n", i, vl);
			fflush(stdout);
		}
		nCurrTimer = (int)(GetTime() - searchInfo.llTime);
		// 7. 如果搜索时间超过适当时限，则终止搜索
		nLimitTimer = searchInfo.nMaxTimer;
		// a. 如果当前搜索值没有落后前一层很多，那么适当时限减半
		nLimitTimer = (vl + DROPDOWN_VALUE >= vlLast ? nLimitTimer / 2 : nLimitTimer);
		// b. 如果最佳着法连续多层没有变化，那么适当时限减半
		nLimitTimer = (searchInfo.nUnchanged >= UNCHANGED_DEPTH ? nLimitTimer / 2 : nLimitTimer);
		if (nCurrTimer > nLimitTimer) {
			vlLast = vl;
			break; // 不管是否跳出，"vlLast"都已更新
		}

		vlLast = vl;

		// 8. 搜索到杀棋则终止搜索
		if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE) {
			break;
		}

	}
	//完成迭代加深搜索，获得结果
	//searchInfo.board.drawBoard();
	/*printf("SSSSSSSSSSSSSSSSSSS:after\n");
	fflush(stdout);
	searchInfo.board.drawBoard();*/
	searchInfo.board.makeMove(searchInfo.mvResult);
	char result[4];
	MOVE_COORD(searchInfo.mvResult,result);//将结果转化为可输出字符串 int->char*
	printf("bestmove %.4s\n", (const char*)&result);
	fflush(stdout);
	if (searchInfo.bDebug)
		searchInfo.board.drawBoard();
}
/*
ucci
isready
position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 1 moves h2e2 h9g7
go time 60000
*/