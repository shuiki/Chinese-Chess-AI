#include"search.h"
#include"tools.h"
const bool NO_NULL = false; // "SearchPV()"�Ĳ������Ƿ��ֹ���Ųü�
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
	int mvBest, mvHash;
	int_16 mv;
	MoveSortStruct MoveSort;
	// 1. ��Ҷ�ӽ�㴦���þ�̬������
	if (depth <= 0)
		return searchInfo.board.valueRed - searchInfo.board.valueBlack;
		//return SearchQuiesc(searchInfo.board, alpha, beta);
		//return Evaluate(searchInfo.board);
	
	// 2. �ظ��ü���
	vl = RepPruning(searchInfo.board, beta);
	if (vl > -MATE_VALUE) {//-MATE_VALUE���ظ����ض�����ֵ
		return vl;
	}
	// 3. �û��ü�������������ڵ����û����г���
	vl = probeHash(searchInfo.board, alpha, beta, depth, mv);
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
	MoveSortStruct MoveSort;
	int mv;
	while ((mv = MoveSort.Next()) != 0) {
		if (searchInfo.board.makeMove(mv))//���Ǳ����ŷ���������
		{
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
		}
		nCurrTimer = (int)(GetTime() - searchInfo.time);
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
	searchInfo.mvs.Init(searchInfo.mvResult);
	// 2. ��һ����ÿ���ŷ�
	while ((mv = searchInfo.mvs.Next()) != 0) {
		if (searchInfo.board.makeMove(mv)) {
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

			nCurrTimer = (int)(GetTime() - searchInfo.time);
			if (nCurrTimer > searchInfo.nMaxTimer) {
				searchInfo.bStop = true;
			}

		}
	}
	recordHash(searchInfo.board, HASH_PV, vlBest, depth, searchInfo.mvResult);
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
	//if (Search.bUseBook) {
	//	// a. ��ȡ���ֿ��е������߷�
	//	nBookMoves = GetBookMoves(Search.pos, bks);
	//	if (nBookMoves > 0) {
	//		vl = 0;
	//		for (i = 0; i < nBookMoves; i++) {
	//			vl += bks[i].wvl;
	//		}
	//		// b. ����Ȩ�����ѡ��һ���߷�
	//		vl = Search.rc4Random.NextLong() % (uint32_t)vl;
	//		for (i = 0; i < nBookMoves; i++) {
	//			vl -= bks[i].wvl;
	//			if (vl < 0) {
	//				break;
	//			}
	//		}
	//		// c. ������ֿ��е��ŷ�����ѭ�����棬��ô��������ŷ�
	//		Search.pos.MakeMove(bks[i].wmv);
	//		if (Search.pos.RepStatus(3) == 0) {
	//			Search2.mvResult = bks[i].wmv;
	//			Search.pos.UndoMakeMove();
	//			uint32_t result = MOVE_COORD(Search2.mvResult);
	//			printf("bestmove %.4s\n", (const char*)&result);
	//			fflush(stdout);
	//			if (Search.bDebug)
	//				Search.pos.DrawBoard(Search2.mvResult);
	//			return;
	//		}
	//		Search.pos.UndoMakeMove();
	//	}
	//}

	// 3. ������Ϊ���򷵻ؾ�̬����ֵ
	if (depth == 0) {
		vl = SearchQuiesc(searchInfo.board, -MATE_VALUE, MATE_VALUE);
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
	memset(searchInfo.HashTable, 0, sizeof(searchInfo.HashTable));
	// ���� ClearHash() ��Ҫ����һ��ʱ�䣬���Լ�ʱ�����Ժ�ʼ�ȽϺ���
	searchInfo.time = GetTime();
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
		nCurrTimer = (int)(GetTime() - searchInfo.time);
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
	uint32_t result = MOVE_COORD(searchInfo.mvResult);//�����ת��Ϊ������ַ��� int->char*
	printf("bestmove %.4s\n", (const char*)&result);
	fflush(stdout);
}