#include"search.h"
#include"search.h"
#include "book.h"

MoveSortStruct mvs;
SearchInfo searchInfo;
HashStruct HashTable[HASH_SIZE];
const bool NO_NULL = false; // "SearchPV()"�Ĳ������Ƿ��ֹ���Ųü�
using namespace std;


// ���֦����ȥ�ظ�����ͽϲ����
int prune(const Board& pos) {
	//����
	int vlRep = pos.RepStatus(1);
	//�����ظ����棬��ͬ�ظ������Ӧ��ͬ��ֵ
	if (vlRep > 0) {
		return pos.RepValue(vlRep);
	}
	//�������ظ�����
	return -MATE_VALUE;
}


// MVVÿ��������ֵ��
static int MVV_value[24] = { 0, 0, 0, 0, 0, 0, 0, 0,5, 1, 1, 3, 4, 3, 2, 0,5, 1, 1, 3, 4, 3, 2, 0 };

inline int value(int value){
	return (8 * MVV_value[searchInfo.board.chessBoard[getDST(value)]]) - MVV_value[searchInfo.board.chessBoard[getSRC(value)]];
}

inline bool Compare_MVV(int value_1, int value_2){
	if (value(value_1) > value(value_2))
		return true;
	else
		return false;
}

// ��̬����
int Quies(Board& pos, int Alpha, int Beta) {

	// �ﵽ������ȣ�ֱ�ӷ��أ�
	if (pos.distance == LIMIT_DEPTH) 
		return  pos.Evaluate();

	int val, Move_num = 0, val_best = -MATE_VALUE;
	int_16 mvs[MAX_GEN_MVS];

	// �ظ��ü������ظ�����ֱ�ӷ��ع�ֵ
	val = prune(pos);
	if (val > -MATE_VALUE) {
		return val;
	}
	if (val > Alpha)
		Alpha = val;
	//���ظ����棺

	// ����������:
	if (pos.lastCheck()) {
		Move_num = pos.gemove_num(mvs);
		std::sort(mvs, mvs + Move_num, CompareHistory);
	}
	// δ����������:
	else {
		val = pos.Evaluate();
		if (val >= Beta) 
			return val;
		val_best = val;
		if (val > Alpha) 
			Alpha = val;
		Move_num = pos.gemove_num(mvs, true);
		std::sort(mvs, mvs + Move_num, Compare_MVV);
	}
	// �������ŷ�A-B����
	for (int i = 0; i < Move_num; i++) {
		if (pos.makeMove(mvs[i])){	
			val = -Quies(pos, -Beta, -Alpha);
			if (val > val_best) {
				if (val >= Beta)
				{
					pos.undoMakeMove();
					return val;
				}
				val_best = val;
				if (val > Alpha)
					Alpha = val;
			}
			pos.undoMakeMove();
		}
	}

	return (val_best == -MATE_VALUE) ? pos.distance - MATE_VALUE : val_best;
}

/*******************
���ƿ��ȵ�������������������������������һ������alpha-beta�㷨ʵ��
����searchInfo��mvs.mvs�д洢�������߷���˳��
beta:����ڵ���Сֵ alpha:��һ��ڵ�ֵ
ɢ�����ﵽ��Ҫ����ʲôֵ�����ҵ���Ҫ��ȡ��ʱ����������
���Ǵ���һ��ֵ������һ����־��˵�����ֵ��ʲô���塣
**********************/
static int SearchPV(int depth, int alpha, int beta, bool bNoNull = false)
{
	int vl, vlBest, nHashFlag;
	int nNewDepth, nCurrTimer;
	uint16_t mvBest, mvHash, mv;
	MoveSortStruct MoveSort;
	// 1. ��Ҷ�ӽ�㴦���þ�̬������
	if (depth <= 0)
		//return searchInfo.board.valueRed - searchInfo.board.valueBlack;
		return Quies(searchInfo.board, alpha, beta);
	//return Evaluate(searchInfo.board);

// 2. �ظ��ü���
	vl = prune(searchInfo.board);
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
	if (bNoNull && !searchInfo.board.lastCheck()) { //&& !searchInfo.board.isChecked() && searchInfo.board.nullOkay()) {
		searchInfo.board.nullMove();
		vl = -SearchPV(depth - NULL_DEPTH - 1, -beta, 1 - beta, NO_NULL);
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
	while ((mv = MoveSort.Next()) != 0) {/////////////////////�ڶ���genMove��������ظ���ͬһ����������ô����
		if (searchInfo.board.makeMove(mv))
		{
			/*
			//���������򲻳���
			if (searchInfo.board.isChecked(searchInfo.board.player))
			{
				searchInfo.board.undoMakeMove();
				continue;
			}
			*/
			//int value = -SearchPV(depth - 1, -beta, -alpha);
			nNewDepth = (searchInfo.board.lastCheck() ? depth : depth - 1);
			if (vlBest == -MATE_VALUE) {
				vl = -SearchPV(-beta, -alpha, nNewDepth);/////////////////repstatus�Ƕ������⣬���Ƿ���-20ƽ�֣�
			}
			else {
				vl = -SearchPV(-alpha - 1, -alpha, nNewDepth);
				if (vl > alpha&& vl < beta) {
					vl = -SearchPV(-beta, -alpha, nNewDepth);
				}
			}
			searchInfo.board.undoMakeMove();
			if (searchInfo.bStop)
				return mvBest;
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

		nCurrTimer = (int)(GetTime() - searchInfo.llTime);
		if (nCurrTimer > searchInfo.nMaxTimer) {
			searchInfo.bStop = true;
		}
	}
	//searchInfo.alpha = alpha;
	//searchInfo.beta = beta;
	// 11. �����û�������ʷ����ɱ���ŷ�����
	if (vlBest == -MATE_VALUE) {////////////////////////��󷵻ص�ɱ�����������һֱû���¹�vlBest
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
		if (searchInfo.board.makeMove(mv))
		{
			/*
			//���������򲻳���
			if (searchInfo.board.isChecked(searchInfo.board.player))
			{
				searchInfo.board.undoMakeMove();
				continue;
			}
			*/
			// 3. ����ѡ��������(ֻ���ǽ�������)
			nNewDepth = (searchInfo.board.isChecked(searchInfo.board.player) ? depth : depth - 1);
			// 4. ��Ҫ��������
			if (vlBest == -MATE_VALUE) {
				vl = -SearchPV(nNewDepth, -MATE_VALUE, MATE_VALUE, NO_NULL);
			}
			else {
				vl = -SearchPV(nNewDepth, -vlBest - 1, -vlBest);
				if (vl > vlBest) { // ���ﲻ��Ҫ" && vl < MATE_VALUE"��
					vl = -SearchPV(nNewDepth, -MATE_VALUE, -vlBest, NO_NULL);
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
		else {
			nCurrTimer = (int)(GetTime() - searchInfo.llTime);
			if (nCurrTimer > searchInfo.nMaxTimer) {
				searchInfo.bStop = true;
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
/***********************
����������
˼·��������������:��Ҫ������ȱ�����������֪���
************************/
void SearchMain(int dep)
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
	BookStruct bks[MAX_GEN_MVS];
	// ���������̰������¼������裺


// 2. �ӿ��ֿ��������ŷ�
	if (searchInfo.bUseBook) {
		// a. ��ȡ���ֿ��е������߷�
		nBookMoves = GetBookMoves(searchInfo.board, bks);
		if (nBookMoves > 0) {
			vl = 0;
			for (i = 0; i < nBookMoves; i++) {
				vl += bks[i].wvl;
			}
			// b. ����Ȩ�����ѡ��һ���߷�
			vl = searchInfo.rc4Random.NextLong() % (uint32_t)vl;
			for (i = 0; i < nBookMoves; i++) {
				vl -= bks[i].wvl;
				if (vl < 0) {
					break;
				}
			}
			// c. ������ֿ��е��ŷ�����ѭ�����棬��ô��������ŷ�
			searchInfo.board.makeMove(bks[i].wmv);
			if (searchInfo.board.RepStatus(3) == 0) {
				searchInfo.mvResult = bks[i].wmv;
				searchInfo.board.undoMakeMove();
				char result[4];
				MOVE_COORD(searchInfo.mvResult, result);//�����ת��Ϊ������ַ��� int->char*
				printf("bestmove %.4s\n", (const char*)&result);
				fflush(stdout);
				if (searchInfo.bDebug)
					searchInfo.board.drawBoard();
				return;
			}
			searchInfo.board.undoMakeMove();
		}
	}

	// 3. ������Ϊ���򷵻ؾ�̬����ֵ
	if (dep == 0) {
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
	for (i = 1; i <= dep; i++)
	{
		// 6. ���������
		vl = SearchRoot(i);
		if (searchInfo.bStop) {
			if (vl > -MATE_VALUE) {
				vlLast = vl; // ������vlLast�������ж������Ͷ����������Ҫ�������һ��ֵ
			}
			break; // û����������"vl"�ǿɿ�ֵ
		}
		if (searchInfo.bDebug) {
			printf("info depth %d mv %d score %d\n", i, searchInfo.mvResult, vl);
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
		if (vlLast > WIN_VALUE || vlLast < -WIN_VALUE)
		{
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
	MOVE_COORD(searchInfo.mvResult, result);//�����ת��Ϊ������ַ��� int->char*
	printf("bestmove %.4s\n", (const char*)&result);
	//printf("%d,%d,%d\n",searchInfo.mvResult,i,searchInfo.bStop);
	fflush(stdout);
	if (searchInfo.bDebug)
		searchInfo.board.drawBoard();
}

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