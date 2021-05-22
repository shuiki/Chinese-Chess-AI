#include"search.h"
/*******************
限制宽度的深度优先搜索（迭代深度优先搜索的一步）用alpha-beta算法实现
根据searchInfo中mvs.mvs中存储的所有走法及顺序，
beta:本层节点最小值 alpha:上一层节点值
**********************/
int RootSearch(int depth, int alpha, int beta)
{
	if (depth <= 0)
		return searchInfo.board.valueRed - searchInfo.board.valueBlack;
		//return Evaluate(searchInfo.board);
	MoveSortStruct MoveSort;
	int mv;
	while ((mv = MoveSort.Next()) != 0) {
		if (searchInfo.board.makeMove(mv))//不是被将着法，则尝试走
		{
			int value = -RootSearch(depth - 1, -beta, -alpha);
			searchInfo.board.undoMakeMove();
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
主搜索函数
思路：迭代加深搜索:主要深度优先遍历，利用已知结果
************************/
void SearchMain(int depth)
{
	for (int i = 0; i < depth; i++)
	{
		RootSearch(i,searchInfo.alpha,searchInfo.beta);

	}
	//完成迭代加深搜索，获得结果
	uint32_t result = MOVE_COORD(searchInfo.mvResult);//将结果转化为可输出字符串 int->char*
	printf("bestmove %.4s\n", (const char*)&result);
	fflush(stdout);
}