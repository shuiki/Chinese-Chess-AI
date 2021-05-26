#include"search.h"

MoveSortStruct mvs;
SearchInfo searchInfo;
HashStruct HashTable[HASH_SIZE];
const bool NO_NULL = false; // "SearchPV()"�Ĳ������Ƿ��ֹ���Ųü�

// �ظ��ü�
int RepPruning(const Board& pos, int vlBeta) {
	int vlRep = pos.RepStatus(1);
	if (vlRep > 0) {
		return pos.RepValue(vlRep);
	}
	return -MATE_VALUE;
}

// MVV/LVAÿ�������ļ�ֵ
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

// ��̬����
 int Quies(Board& pos, int Alpha, int Beta) {

	 // �ﵽ������ȣ�ֱ�ӷ��ؾ�������ֵ��
	 if (pos.distance == LIMIT_DEPTH) {
		 return  pos.Evaluate();
	 }

	int val, Move_num=0,val_best= -MATE_VALUE;
	int_16 mvs[MAX_GEN_MVS];

	// �ظ��ü���
	val = RepPruning(pos, Beta);
	if (val > -MATE_VALUE) {
		return val;
	}
	if (val > Alpha) 
		Alpha = val;
	// ����������:
	if (pos.isChecked(pos.player)) {
		Move_num = pos.genMoves(mvs);
		std::sort(mvs, mvs + Move_num, CompareHistory);
	}
	// δ����������:
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
	// �������ŷ�����A-B����
	for (int i = 0; i < Move_num; i++) {
		pos.makeMove(mvs[i]);
		//�����������򲻽���������
		if (pos.isChecked(pos.player)){
			pos.undoMakeMove();
			continue;
		}
		//δ���������棺
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
���ƿ�ȵ�������������������������������һ������alpha-beta�㷨ʵ��
����searchInfo��mvs.mvs�д洢�������߷���˳��
beta:����ڵ���Сֵ alpha:��һ��ڵ�ֵ
ɢ�����ﵽ��Ҫ����ʲôֵ�����ҵ���Ҫ��ȡ��ʱ����������
���Ǵ���һ��ֵ�����һ����־��˵�����ֵ��ʲô���塣
**********************/
static int SearchPV(int depth, int alpha, int beta, bool bNoNull = false)
{
	int vl,vlBest,nHashFlag;
	int nNewDepth, nCurrTimer;
	uint16_t mvBest, mvHash, mv;
	MoveSortStruct MoveSort;
	// 1. ��Ҷ�ӽ�㴦���þ�̬������
	if (depth <= 0)
		//return searchInfo.board.valueRed - searchInfo.board.valueBlack;
		return Quies(searchInfo.board, alpha, beta);
		//return Evaluate(searchInfo.board);
	
	// 2. �ظ��ü���
	vl = RepPruning(searchInfo.board, beta);
	if (vl > -MATE_VALUE) {//-MATE_VALUE���ظ����ض�����ֵ
		return vl;
	}
	// 3. �û��ü�������������ڵ����û����г���
	vl = probeHash(searchInfo.board, depth, alpha, beta, mvHash);
	if (searchInfo.bUseHash && vl > -MATE_VALUE) {
		// ����PV��㲻�����û��ü������Բ��ᷢ��PV·���жϵ����
		return vl;
	}
	// 4. �ﵽ������ȣ�ֱ�ӷ�������ֵ��
	if (searchInfo.board.distance == LIMIT_DEPTH) {
		return searchInfo.board.Evaluate();
	}
	// 5. ���Կ��Ųü���
	if (bNoNull){ //&& !searchInfo.board.isChecked() && searchInfo.board.nullOkay()) {
		searchInfo.board.nullMove();
		vl = -SearchPV(depth - NULL_DEPTH - 1 ,-beta, 1 - beta,NO_NULL);
		searchInfo.board.undoNullMove();

		if (vl >= beta) {
			return vl;
		}
	}
	// 6. ��ʼ����
	mvBest = 0;
	nHashFlag = HASH_ALPHA;
	vlBest = -MATE_VALUE;
	MoveSort.Init(mvHash);
	while ((mv = MoveSort.Next()) != 0) {
		searchInfo.board.makeMove(mv);
		//���������򲻳���
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
	// 11. �����û�����ʷ���ɱ���ŷ���
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
	// ������������̰������¼������裺
	// 1. ��ʼ��
	vlBest = -MATE_VALUE;
	mvs.Init(searchInfo.mvResult);
	// 2. ��һ����ÿ���ŷ�
	while ((mv = mvs.Next()) != 0) {
		searchInfo.board.makeMove(mv);
		//���������򲻳���
		if (searchInfo.board.isChecked(searchInfo.board.player))
		{
			searchInfo.board.undoMakeMove();
			continue;
		}
		// 3. ����ѡ��������(ֻ���ǽ�������)
		nNewDepth = (searchInfo.board.isChecked(searchInfo.board.player) ? depth : depth - 1);
		// 4. ��Ҫ��������
		if (vlBest == -MATE_VALUE) {
			vl = -SearchPV(nNewDepth, -MATE_VALUE, MATE_VALUE,NO_NULL);
		}
		else {
			vl = -SearchPV(nNewDepth ,-vlBest - 1, -vlBest);
			if (vl > vlBest) { // ���ﲻ��Ҫ" && vl < MATE_VALUE"��
				vl = -SearchPV(nNewDepth, -MATE_VALUE, -vlBest,  NO_NULL);
			}
		}
		searchInfo.board.undoMakeMove();
		if (searchInfo.bStop) {
			return vlBest;
		}
		// 5. Alpha-Beta�߽��ж�("vlBest"������"SearchPV()"�е�"vlAlpha")
		if (vl > vlBest) {
			// 6. �����������һ�ŷ�����ô"δ�ı�����ŷ�"�ļ�������1����������
			searchInfo.nUnchanged = (vlBest == -MATE_VALUE ? searchInfo.nUnchanged + 1 : 0);
			vlBest = vl;
			// 7. ����������ŷ�ʱ��¼��Ҫ����
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
����������
˼·��������������:��Ҫ������ȱ�����������֪���
************************/
void SearchMain(int depth)
{
	//searchInfo.ClearHistory();
	//for (int i = 0; i < depth; i++)
	//{
	//	SearchRoot(i);

	//	// ������ɱ������ֹ����
	//	if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE) {
	//		break;
	//	}
	//}
	int i, vl, vlLast;
	int nCurrTimer, nLimitTimer;
	int nBookMoves;
	//BookStruct bks[MAX_GEN_MOVES];
	// ���������̰������¼������裺


// 2. �ӿ��ֿ��������ŷ�
	//if (searchInfo.bUseBook) {
	//	// a. ��ȡ���ֿ��е������߷�
	//	nBookMoves = GetBookMoves(searchInfo.board, bks);
	//	if (nBookMoves > 0) {
	//		vl = 0;
	//		for (i = 0; i < nBookMoves; i++) {
	//			vl += bks[i].wvl;
	//		}
	//		// b. ����Ȩ�����ѡ��һ���߷�
	//		vl = searchInfo.rc4Random.NextLong() % (uint32_t)vl;
	//		for (i = 0; i < nBookMoves; i++) {
	//			vl -= bks[i].wvl;
	//			if (vl < 0) {
	//				break;
	//			}
	//		}
	//		// c. ������ֿ��е��ŷ�����ѭ�����棬��ô��������ŷ�
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

	// 3. ������Ϊ���򷵻ؾ�̬����ֵ
	if (depth == 0) {
		vl = Quies(searchInfo.board, -MATE_VALUE, MATE_VALUE);
		//		vl = Evaluate(searchInfo.board);
		if (searchInfo.bDebug) {
			printf("info depth 0 score %d\n", vl);
			fflush(stdout);
		}
		return;
	}

	// 4. ��ʼ��ʱ��ͼ�����
	searchInfo.bStop = false;
	searchInfo.nUnchanged = 0;
	searchInfo.mvResult = 0;
	searchInfo.ClearKiller(searchInfo.wmvKiller);
	searchInfo.ClearHistory();
	memset(HashTable, 0, sizeof(HashTable));
	// ���� ClearHash() ��Ҫ����һ��ʱ�䣬���Լ�ʱ�����Ժ�ʼ�ȽϺ���
	searchInfo.llTime = GetTime();
	vlLast = 0;
	nCurrTimer = 0;
	
	// 5. ��������������
	for (i = 1; i <= depth; i++) {
		// 6. ���������
		vl = SearchRoot(i);
		if (searchInfo.bStop) {
			if (vl > -MATE_VALUE) {
				vlLast = vl; // ������vlLast�������ж������Ͷ����������Ҫ�������һ��ֵ
			}
			break; // û����������"vl"�ǿɿ�ֵ
		}
		if (searchInfo.bDebug) {
			printf("info depth %d score %d\n", i, vl);
			fflush(stdout);
		}
		nCurrTimer = (int)(GetTime() - searchInfo.llTime);
		// 7. �������ʱ�䳬���ʵ�ʱ�ޣ�����ֹ����
		nLimitTimer = searchInfo.nMaxTimer;
		// a. �����ǰ����ֵû�����ǰһ��ܶ࣬��ô�ʵ�ʱ�޼���
		nLimitTimer = (vl + DROPDOWN_VALUE >= vlLast ? nLimitTimer / 2 : nLimitTimer);
		// b. �������ŷ��������û�б仯����ô�ʵ�ʱ�޼���
		nLimitTimer = (searchInfo.nUnchanged >= UNCHANGED_DEPTH ? nLimitTimer / 2 : nLimitTimer);
		if (nCurrTimer > nLimitTimer) {
			vlLast = vl;
			break; // �����Ƿ�������"vlLast"���Ѹ���
		}

		vlLast = vl;

		// 8. ������ɱ������ֹ����
		if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE) {
			break;
		}

	}
	//��ɵ���������������ý��
	//searchInfo.board.drawBoard();
	/*printf("SSSSSSSSSSSSSSSSSSS:after\n");
	fflush(stdout);
	searchInfo.board.drawBoard();*/
	searchInfo.board.makeMove(searchInfo.mvResult);
	char result[4];
	MOVE_COORD(searchInfo.mvResult,result);//�����ת��Ϊ������ַ��� int->char*
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