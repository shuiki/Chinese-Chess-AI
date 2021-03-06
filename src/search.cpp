#include"search.h"
#include "book.h"

MoveSortStruct mvs;
SearchInfo searchInfo;
HashStruct HashTable[HASH_SIZE];
const bool NO_NULL = false; // 禁止空着
using namespace std;

int max_depth, pv_num, quies_num;

int prune(const Board& pos) {
	//查找
	int vlRep = pos.RepStatus(1);
	//存在重复局面，不同重复局面对应不同估值
	if (vlRep > 0) {
		return pos.RepValue(vlRep);
	}
	//不存在重复局面
	return -MAX_VALUE;
}


// MVV每种子力价值：
static int MVV_value[24] = { 0, 0, 0, 0, 0, 0, 0, 0,5, 1, 1, 3, 4, 3, 2, 0,5, 1, 1, 3, 4, 3, 2, 0 };

inline int value(int value) {
	return (8 * MVV_value[searchInfo.board.chessBoard[getDST(value)]]) - MVV_value[searchInfo.board.chessBoard[getSRC(value)]];
}

inline bool Compare_MVV(int value_1, int value_2) {
	if (value(value_1) > value(value_2))
		return true;
	else
		return false;
}
void checkTime() {
	int64_t CurTime=0;
	CurTime = (GetTime() - searchInfo.Timer);
	//if(searchInfo.bDebug)
	//cout<< searchInfo.TimeMax << endl;
	if (CurTime > searchInfo.TimeMax*1000)
		searchInfo.bStop = true;
	
	//printf("%d ", CurTime);
	//cout << searchInfo.bStop<<' ';
	//cout << CurTime << ' '<<searchInfo.TimeMax<<endl;
}
// 静态搜索
int Quies(Board& board, int Alpha, int Beta) {

	quies_num++;
	// 达到极限深度，直接返回；
	if (board.distance == LIMIT_DEPTH)
		return  board.Evaluate();
	//时间判断，时间到了则返回
	checkTime();
	if (searchInfo.bStop)
		return board.Evaluate();
	int val, Move_num = 0, val_best = -MAX_VALUE;
	int_16 mvs[MAX_GEN_MVS];

	// 重复裁剪；有重复局面直接返回估值
	val = prune(board);
	if (val > -MAX_VALUE) {
		return val;
	}
	if (val > Alpha)
		Alpha = val;

	// 被将军
	if (board.lastCheck()) {
		Move_num = board.gemove_num(mvs);
		std::sort(mvs, mvs + Move_num, CompareHistory);
	}
	else {
		val = board.Evaluate();
		if (val >= Beta)
			return val;
		val_best = val;
		if (val > Alpha)
			Alpha = val;
		Move_num = board.gemove_num(mvs, true);
		std::sort(mvs, mvs + Move_num, Compare_MVV);
	}
	// alpha-beta
	for (int i = 0; i < Move_num; i++) {
		if (board.makeMove(mvs[i])) {
			val = -Quies(board, -Beta, -Alpha);
			if (val > val_best) {
				if (val >= Beta)
				{
					board.undoMakeMove();
					return val;
				}
				val_best = val;
				if (val > Alpha)
					Alpha = val;
			}
			board.undoMakeMove();
		}
	}

	return (val_best == -MAX_VALUE) ? board.distance - MAX_VALUE : val_best;
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
	pv_num++;
	int vl, vlBest, nHashFlag;
	int newDepth;
	uint16_t mvBest, mvHash, mv;
	MoveSortStruct MoveSort;
	// 在叶子结点处调用静态搜索；
	if (depth <= 0)
		//return searchInfo.board.valueRed - searchInfo.board.valueBlack;
		return Quies(searchInfo.board, alpha, beta);
	//return Evaluate(searchInfo.board);

		//超过数组大小
	if (searchInfo.board.distance == LIMIT_DEPTH) {
		return searchInfo.board.Evaluate();
	}

	// 去重
	vl = prune(searchInfo.board);
	if (vl > -MAX_VALUE) {//-MAX_VALUE是重复的特定返回值
		return vl;
	}
	// 哈希置换表查找
	vl = probeHash(searchInfo.board, depth, alpha, beta, mvHash);
	if (searchInfo.bUseHash && vl > -MAX_VALUE) {
		return vl;
	}


	// 空着；
	if (bNoNull && !searchInfo.board.lastCheck()) { //&& !searchInfo.board.isChecked() && searchInfo.board.nullOkay()) {
		searchInfo.board.nullMove();
		vl = -SearchPV(depth - NULL_DEPTH - 1, -beta, 1 - beta, NO_NULL);
		searchInfo.board.undoNullMove();

		if (vl >= beta) {
			return vl;
		}
	}
	if (0&&searchInfo.bDebug)
	{
		char result[4];
		MOVE_COORD(searchInfo.nHistoryTable[searchInfo.board.distance], result);//将结果转化为可输出字符串 int->char*
		printf("*PVmax_depth: %d depth: %d move %.4s\n", max_depth, depth, (const char*)&result);
		//printf("%d\n",searchInfo.mvResult);
		fflush(stdout);
	}
	// 初始化；
	mvBest = 0;
	nHashFlag = HASH_ALPHA;
	vlBest = -MAX_VALUE;
	MoveSort.Init(mvHash);
	while ((mv = MoveSort.Next()) != 0) {/////////////////////第二层genMove后面大量重复的同一步，看看怎么回事
		checkTime();
		if (searchInfo.bStop == true) {
			return mvBest;
		}
		if (searchInfo.board.makeMove(mv))
		{

			newDepth = (searchInfo.board.lastCheck() ? depth : depth - 1);
			if (vlBest == -MAX_VALUE) {
				vl = -SearchPV(newDepth, -beta, -alpha);/////////////////repstatus那儿有问题，老是返回-20平局！
			}
			else {
				vl = -SearchPV(newDepth, -alpha - 1, -alpha);
				if (vl > alpha && vl < beta) {
					vl = -SearchPV(newDepth, -beta, -alpha);
				}
			}
			searchInfo.board.undoMakeMove();

			if (vl > vlBest)
			{
				vlBest = vl;
				if (vl > beta) {
					mvBest = mv;
					nHashFlag = HASH_BETA;
					break;
				}

				if (vl > alpha)
				{
					mvBest = mv;
					nHashFlag = HASH_PV;
					alpha = vl;
				}
			}
		}


	}
	//searchInfo.alpha = alpha;
	//searchInfo.beta = beta;
	// 更新board
	if (vlBest == -MAX_VALUE) {////////////////////////最后返回的杀棋来自这儿，一直没更新过vlBest
		return searchInfo.board.distance - MAX_VALUE;
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
	int newDepth, vlBest, vl, mv;
	// 根结点搜索例程包括以下几个步骤：
	// 初始化
	vlBest = -MAX_VALUE;
	mvs.Init(searchInfo.mvResult);
	// 搜索
	while ((mv = mvs.Next()) != 0) {
		checkTime();
		if (searchInfo.bStop == true) {
			if (searchInfo.bDebug)
				cout << "Time Out search depth = " << max_depth << endl;
			break;
		}
		if (searchInfo.board.makeMove(mv))
		{
			if (0&&searchInfo.bDebug)
			{
				char result[4];
				MOVE_COORD(mv, result);//将结果转化为可输出字符串 int->char*
				printf("ROOT max_depth: %d depth: %d move %.4s\n", max_depth, max_depth - depth, (const char*)&result);
				//printf("%d\n",searchInfo.mvResult);
				fflush(stdout);
			}
			
			// 选择延伸
			newDepth = (searchInfo.board.isChecked(searchInfo.board.player) ? depth : depth - 1);

			if (vlBest == -MAX_VALUE) {
				vl = -SearchPV(newDepth, -MAX_VALUE, MAX_VALUE, NO_NULL);
			}
			else {
				vl = -SearchPV(newDepth, -vlBest - 1, -vlBest);
				if (vl > vlBest) { // 这里不需要" && vl < MAX_VALUE"了
					vl = -SearchPV(newDepth, -MAX_VALUE, -vlBest, NO_NULL);
				}
			}
			searchInfo.board.undoMakeMove();

			if (vl > vlBest) {

				vlBest = vl;
				searchInfo.mvResult = mv;
			}

		}
		
	}
	/*printf("%d SSSSSSSSSSSSSSSSSSS:afer search\n",depth);
	fflush(stdout);
	searchInfo.board.drawBoard();*/
	recordHash(searchInfo.board, depth, vlBest, HASH_PV, searchInfo.mvResult);
	searchInfo.SetBestMove(searchInfo.mvResult, depth, searchInfo.wmvKiller[searchInfo.board.distance]);
	return vlBest;
}
/*SearchMain用到的初始化函数*/
void Initial() {

	searchInfo.bStop = false;
	searchInfo.mvResult = 0;
	searchInfo.ClearKiller(searchInfo.wmvKiller);
	searchInfo.ClearHistory();
	memset(HashTable, 0, sizeof(HashTable));
	//计时
	searchInfo.Timer = GetTime();
}
/*SearchMain用到的开局库函数，返回是否找到着法*/
int TryBookMv() {
	int i, vl, mvs;
	BookStruct bks[MAX_GEN_MVS];
	mvs = GetBookMoves(searchInfo.board, bks);
	if (mvs > 0) {
		vl = 0;
		for (i = 0; i < mvs; i++) {
			vl += bks[i].wvl;
		}
		vl = searchInfo.rc4Random.NextLong() % (uint32_t)vl;
		for (i = 0; i < mvs; i++) {
			vl -= bks[i].wvl;
			if (vl < 0) {
				break;
			}
		}
		// 去循环
		searchInfo.board.makeMove(bks[i].wmv);
		if (searchInfo.board.RepStatus(3) == 0) {
			searchInfo.mvResult = bks[i].wmv;
			searchInfo.board.undoMakeMove();
			char result[4];
			MOVE_COORD(searchInfo.mvResult, result);//将结果转化为可输出字符串 int->char*
			printf("bestmove %.4s\n", (const char*)&result);
			fflush(stdout);
			if (searchInfo.bDebug)
				searchInfo.board.drawBoard();
			return 1;
		}
		searchInfo.board.undoMakeMove();
	}
	return 0;
}

/***********************
主搜索函数
思路：迭代加深搜索:主要深度优先遍历，利用已知结果
************************/
void SearchMain(int dep)
{
	int i, vl, vlLast;
	int CurTime;

	// 主搜索例程包括以下几个步骤：

	// 开局库
	if (searchInfo.bUseBook) {
		if (TryBookMv())
			return;
	}

	// 初始化
	Initial();
	vlLast = 0;
	CurTime = 0;

	// 做迭代加深搜索
	for (i = 1; i <= dep; i++)
	{
		max_depth = i;
		pv_num = quies_num = 0;
		vl = SearchRoot(i);
		if (searchInfo.bStop) {
			if (vl > -MAX_VALUE) {
				vlLast = vl; // 跳出后，vlLast会用来判断认输或投降，所以需要给定最近一个值
			}
			break; // 没有跳出，则"vl"是可靠值
		}
		if (searchInfo.bDebug) {
			printf("info depth %d mv %d score %d\n", i, searchInfo.mvResult, vl);
			fflush(stdout);
		}

		checkTime();
		if (searchInfo.bStop) {
			vlLast = vl;
			break; // 不管是否跳出，"vlLast"都已更新
		}

		vlLast = vl;

		// 8. 搜索到杀棋则终止搜索
		if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE)
		{
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
	MOVE_COORD(searchInfo.mvResult, result);//将结果转化为可输出字符串 int->char*
	printf("bestmove %.4s\n", (const char*)&result);
	if (searchInfo.bDebug)
		cout << "PV_NUM= " << pv_num << ", QUIES_NUM= " << quies_num << endl;
	//printf("%d,%d,%d\n",searchInfo.mvResult,i,searchInfo.bStop);
	fflush(stdout);
	if (searchInfo.bDebug)
	{
		searchInfo.board.drawBoard();
	}
}
/*
ucci
isready
position startpos moves b2e2 b9c7 b0c2 a9b9 a0b0 h9g7 g3g4 c6c5 b0b6 b7a7 b6c6 a7a8 h0g2 f9e8 h2i2 i9h9 i0h0 a8c8 c6d6 c7b5 d6d8 c8c3 c0a2 h7h3 e3e4 c5c4 a2c4 c3g3
go time 60 movestogo 985 opptime 60 oppmovestogo 985
*/
/*
position fen 1n1k1abnr/9/1c5c1/8p/9/p8/P5p1P/BC3CN1R/2p6/1p1AK4 w - - 0 28 moves
go time 60000
*/

/*
position fen 3akabnr/9/P1c1b4/4p1p1p/9/2p1P4/6PcP/N3K2CR/9/R1Br5 r - - 2 14 moves a7a8 d0e0
go time 60000
bestmove a7a8
17235
*/


/*
ucci
ucciok
isready
readyok
position startpos r - - 0 1 moves
go time 60000
bestmove a3a4
position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 2 2 moves a3a4 c9e7
go time 60000
bestmove a4a5
position fen rn1akabnr/9/1c2b2c1/2p1p1p1p/p8/9/2P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 3 moves
go time 60000
bestmove c3c4
position fen rn1akabnr/9/1c2b2c1/2p1p1p1p/p8/9/2P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 4 moves c3c4 a5a4
go time 60000
bestmove c4c5
*/

/*
ucci
isready
position fen rnbakabnr/9/1c7/p1p1p1p1p/9/9/P1P1P1P1P/1C5CR/9/RNBAKABc1 r - - 0 2 moves
go time 60000
bestmove h0k0
bestmove g0e2
42953
*/

/*
ucci
isready
position startpos r - - 0 1 moves
go time 60000
position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r - - 2 2 moves a3a4 c9e7
go time 60000
position fen rn1akabnr/9/1c2b2c1/2p1p1p1p/p8/9/2P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 3 moves
go time 60000
position fen rn1akabnr/9/1c2b2c1/2p1p1p1p/p8/9/2P1P1P1P/1C5C1/9/RNBAKABNR r - - 0 4 moves c3c4 a5a4
go time 60000
*/
