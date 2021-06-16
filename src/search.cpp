#include"search.h"
#include "book.h"

MoveSortStruct mvs;
SearchInfo searchInfo;
HashStruct HashTable[HASH_SIZE];
const bool NO_NULL = false; // "SearchPV()"�Ĳ������Ƿ��ֹ���Ųü�
using namespace std;

int max_depth, pv_num, quies_num;
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
	CurTime = (GetTime() - searchInfo.llTime);
	//if(searchInfo.bDebug)
	//cout<< searchInfo.nMaxTimer << endl;
	if (CurTime > searchInfo.nMaxTimer*1000)
		searchInfo.bStop = true;
	
	//printf("%d ", CurTime);
	//cout << searchInfo.bStop<<' ';
	//cout << CurTime << ' '<<searchInfo.nMaxTimer<<endl;
}
// ��̬����
int Quies(Board& board, int Alpha, int Beta) {

	quies_num++;
	// �ﵽ������ȣ�ֱ�ӷ��أ�
	if (board.distance == LIMIT_DEPTH)
		return  board.Evaluate();
	//ʱ���жϣ�ʱ�䵽���򷵻�
	checkTime();
	if (searchInfo.bStop)
		return board.Evaluate();
	int val, Move_num = 0, val_best = -MATE_VALUE;
	int_16 mvs[MAX_GEN_MVS];

	// �ظ��ü������ظ�����ֱ�ӷ��ع�ֵ
	val = prune(board);
	if (val > -MATE_VALUE) {
		return val;
	}
	if (val > Alpha)
		Alpha = val;
	//���ظ����棺

	// ����������:
	if (board.lastCheck()) {
		Move_num = board.gemove_num(mvs);
		std::sort(mvs, mvs + Move_num, CompareHistory);
	}
	// δ����������:
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
	// �������ŷ�A-B����
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

	return (val_best == -MATE_VALUE) ? board.distance - MATE_VALUE : val_best;
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
	pv_num++;
	int vl, vlBest, nHashFlag;
	int newDepth;
	uint16_t mvBest, mvHash, mv;
	MoveSortStruct MoveSort;
	// 1. ��Ҷ�ӽ�㴦���þ�̬������
	if (depth <= 0)
		//return searchInfo.board.valueRed - searchInfo.board.valueBlack;
		return Quies(searchInfo.board, alpha, beta);
	//return Evaluate(searchInfo.board);

		// 4. �ﵽ������ȣ�ֱ�ӷ�������ֵ��
	if (searchInfo.board.distance == LIMIT_DEPTH) {
		return searchInfo.board.Evaluate();
	}

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


	// 5. ���Կ��Ųü���
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
		MOVE_COORD(searchInfo.nHistoryTable[searchInfo.board.distance], result);//�����ת��Ϊ������ַ��� int->char*
		printf("*PVmax_depth: %d depth: %d move %.4s\n", max_depth, depth, (const char*)&result);
		//printf("%d\n",searchInfo.mvResult);
		fflush(stdout);
	}
	// 6. ��ʼ����
	mvBest = 0;
	nHashFlag = HASH_ALPHA;
	vlBest = -MATE_VALUE;
	MoveSort.Init(mvHash);
	while ((mv = MoveSort.Next()) != 0) {/////////////////////�ڶ���genMove��������ظ���ͬһ����������ô����
		checkTime();
		if (searchInfo.bStop == true) {
			return mvBest;
		}
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
			newDepth = (searchInfo.board.lastCheck() ? depth : depth - 1);
			if (vlBest == -MATE_VALUE) {
				vl = -SearchPV(newDepth, -beta, -alpha);/////////////////repstatus�Ƕ������⣬���Ƿ���-20ƽ�֣�
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
	// 11. �����û�����ʷ���ɱ���ŷ���
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
	int newDepth, vlBest, vl, mv;
	// ������������̰������¼������裺
	// 1. ��ʼ��
	vlBest = -MATE_VALUE;
	mvs.Init(searchInfo.mvResult);
	// 2. ��һ����ÿ���ŷ�
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
				MOVE_COORD(mv, result);//�����ת��Ϊ������ַ��� int->char*
				printf("ROOT max_depth: %d depth: %d move %.4s\n", max_depth, max_depth - depth, (const char*)&result);
				//printf("%d\n",searchInfo.mvResult);
				fflush(stdout);
			}
			/*
			//���������򲻳���
			if (searchInfo.board.isChecked(searchInfo.board.player))
			{
				searchInfo.board.undoMakeMove();
				continue;
			}
			*/
			// 3. ����ѡ��������(ֻ���ǽ�������)
			newDepth = (searchInfo.board.isChecked(searchInfo.board.player) ? depth : depth - 1);
			// 4. ��Ҫ��������
			if (vlBest == -MATE_VALUE) {
				vl = -SearchPV(newDepth, -MATE_VALUE, MATE_VALUE, NO_NULL);
			}
			else {
				vl = -SearchPV(newDepth, -vlBest - 1, -vlBest);
				if (vl > vlBest) { // ���ﲻ��Ҫ" && vl < MATE_VALUE"��
					vl = -SearchPV(newDepth, -MATE_VALUE, -vlBest, NO_NULL);
				}
			}
			searchInfo.board.undoMakeMove();
			// 5. Alpha-Beta�߽��ж�("vlBest"������"SearchPV()"�е�"vlAlpha")
			if (vl > vlBest) {

				vlBest = vl;
				// 7. ����������ŷ�ʱ��¼��Ҫ����
				searchInfo.mvResult = mv;
			}

		}
		// 6. �����������һ�ŷ�����ô"δ�ı�����ŷ�"�ļ�������1����������
				//searchInfo.nUnchanged = (vlBest == -MATE_VALUE ? searchInfo.nUnchanged + 1 : 0);
	}
	/*printf("%d SSSSSSSSSSSSSSSSSSS:afer search\n",depth);
	fflush(stdout);
	searchInfo.board.drawBoard();*/
	recordHash(searchInfo.board, depth, vlBest, HASH_PV, searchInfo.mvResult);
	searchInfo.SetBestMove(searchInfo.mvResult, depth, searchInfo.wmvKiller[searchInfo.board.distance]);
	return vlBest;
}
/*SearchMain�õ��ĳ�ʼ������*/
void Initial() {
	// ��ʼ��ʱ��ͼ�����
	searchInfo.bStop = false;
	//searchInfo.nUnchanged = 0;
	searchInfo.mvResult = 0;
	searchInfo.ClearKiller(searchInfo.wmvKiller);
	searchInfo.ClearHistory();
	memset(HashTable, 0, sizeof(HashTable));
	// ���� ClearHash() ��Ҫ����һ��ʱ�䣬���Լ�ʱ�����Ժ�ʼ�ȽϺ���
	searchInfo.llTime = GetTime();
}
/*SearchMain�õ��Ŀ��ֿ⺯���������Ƿ��ҵ��ŷ�*/
int TryBookMv() {
	int i, vl, mvs;
	BookStruct bks[MAX_GEN_MVS];
	// a. ��ȡ���ֿ��е������߷�
	mvs = GetBookMoves(searchInfo.board, bks);
	if (mvs > 0) {
		vl = 0;
		for (i = 0; i < mvs; i++) {
			vl += bks[i].wvl;
		}
		// b. ����Ȩ�����ѡ��һ���߷�
		vl = searchInfo.rc4Random.NextLong() % (uint32_t)vl;
		for (i = 0; i < mvs; i++) {
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
			return 1;
		}
		searchInfo.board.undoMakeMove();
	}
	return 0;
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
	int CurTime;

	// ���������̰������¼������裺

	// 2. �ӿ��ֿ��������ŷ�
		if (searchInfo.bUseBook) {
			if (TryBookMv())
				return;
		}

		// 4.��ʼ��ʱ��ͼ�����
	Initial();
	vlLast = 0;
	CurTime = 0;

	// 5. ��������������
	for (i = 1; i <= dep; i++)
	{
		max_depth = i;
		pv_num = quies_num = 0;
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

		checkTime();
		if (searchInfo.bStop) {
			vlLast = vl;
			break; // �����Ƿ�������"vlLast"���Ѹ���
		}
#if 0
		CurTime = (int)(GetTime() - searchInfo.llTime);
		// 7. �������ʱ�䳬���ʵ�ʱ�ޣ�����ֹ����
		nLimitTimer = searchInfo.nMaxTimer;
		// a. �����ǰ����ֵû�����ǰһ��ܶ࣬��ô�ʵ�ʱ�޼���
		nLimitTimer = (vl + DROPDOWN_VALUE >= vlLast ? nLimitTimer / 2 : nLimitTimer);
		// b. �������ŷ��������û�б仯����ô�ʵ�ʱ�޼���
		nLimitTimer = (searchInfo.nUnchanged >= UNCHANGED_DEPTH ? nLimitTimer / 2 : nLimitTimer);
		if (CurTime > nLimitTimer) {
			vlLast = vl;
			break; // �����Ƿ�������"vlLast"���Ѹ���
		}
#endif

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
