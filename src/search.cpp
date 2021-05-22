#include"search.h"
/*******************
���ƿ�ȵ�������������������������������һ������alpha-beta�㷨ʵ��
����searchInfo��mvs.mvs�д洢�������߷���˳��
beta:����ڵ���Сֵ alpha:��һ��ڵ�ֵ
**********************/
int RootSearch(int depth, int alpha, int beta)
{
	if (depth <= 0)
		return searchInfo.board.valueRed - searchInfo.board.valueBlack;
		//return Evaluate(searchInfo.board);
	MoveSortStruct MoveSort;
	int mv;
	while ((mv = MoveSort.Next()) != 0) {
		if (searchInfo.board.makeMove(mv))//���Ǳ����ŷ���������
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
����������
˼·��������������:��Ҫ������ȱ�����������֪���
************************/
void SearchMain(int depth)
{
	for (int i = 0; i < depth; i++)
	{
		RootSearch(i,searchInfo.alpha,searchInfo.beta);

	}
	//��ɵ���������������ý��
	uint32_t result = MOVE_COORD(searchInfo.mvResult);//�����ת��Ϊ������ַ��� int->char*
	printf("bestmove %.4s\n", (const char*)&result);
	fflush(stdout);
}