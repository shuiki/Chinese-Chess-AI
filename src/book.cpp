#include "BoardManipulate.h"
#include "book.h"


int GetBookMoves(const Board& pos, BookStruct* lpbks) {
	BookStruct2 book_temp;
	Board posScan=pos;
	BookStruct bk;
	int nScan, nLow, nHigh, nPtr;
	int i, j, move_num;
	book_temp.init();
	// �۰���ҷ�������
	for (nScan = 0; nScan < 2; nScan++) {
		nPtr = nLow = 0;
		nHigh = book_temp.nLen - 1;
		while (nLow <= nHigh) {
			nPtr = (nLow + nHigh) / 2;
			book_temp.Read(bk, nPtr);
			if (BOOK_POS_CMP(bk, posScan) < 0) {
				nLow = nPtr + 1;
			}
			else if (BOOK_POS_CMP(bk, posScan) > 0) {
				nHigh = nPtr - 1;
			}
			else {
				break;
			}
		}
		if (nLow <= nHigh) {
			break;
		}
		// �������
		pos.mirror(posScan);
	}

	// �Ҳ������棻
	if (nScan == 2) {
		return 0;
	}

	// �ҵ����棻
	for (nPtr--; nPtr >= 0; nPtr--) {
		book_temp.Read(bk, nPtr);
		if (BOOK_POS_CMP(bk, posScan) < 0) {
			break;
		}
	}
	move_num = 0;
	for (nPtr++; nPtr < book_temp.nLen; nPtr++) {
		book_temp.Read(bk, nPtr);
		if (BOOK_POS_CMP(bk, posScan) > 0) {
			break;
		}
		if (posScan.isLegalMove(bk.wmv)) {
			// ����Ǿ�����������
			lpbks[move_num].nPtr = nPtr;
			lpbks[move_num].wmv = (nScan == 0 ? bk.wmv : MOVE_MIRROR(bk.wmv));
			lpbks[move_num].wvl = bk.wvl;
			move_num++;
			if (move_num == MAX_GEN_MVS) {
				break;
			}
		}
	}
	//����
	for (i = 0; i < move_num - 1; i++) {
		for (j = move_num - 1; j > i; j--) {
			if (lpbks[j - 1].wvl < lpbks[j].wvl) {
				std::swap(lpbks[j - 1], lpbks[j]);
			}
		}
	}
	return move_num;
}
