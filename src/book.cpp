#include "BoardManipulate.h"
#include "book.h"


const uint8_t cucsqMirrorTab[256] = {
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0, 0, 0, 0,
  0, 0, 0, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0, 0, 0, 0,
  0, 0, 0, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55, 0x54, 0x53, 0, 0, 0, 0,
  0, 0, 0, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64, 0x63, 0, 0, 0, 0,
  0, 0, 0, 0x7b, 0x7a, 0x79, 0x78, 0x77, 0x76, 0x75, 0x74, 0x73, 0, 0, 0, 0,
  0, 0, 0, 0x8b, 0x8a, 0x89, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0, 0, 0, 0,
  0, 0, 0, 0x9b, 0x9a, 0x99, 0x98, 0x97, 0x96, 0x95, 0x94, 0x93, 0, 0, 0, 0,
  0, 0, 0, 0xab, 0xaa, 0xa9, 0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0, 0, 0, 0,
  0, 0, 0, 0xbb, 0xba, 0xb9, 0xb8, 0xb7, 0xb6, 0xb5, 0xb4, 0xb3, 0, 0, 0, 0,
  0, 0, 0, 0xcb, 0xca, 0xc9, 0xc8, 0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
  0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
};

inline uint8_t SQUARE_MIRROR(int sq) {
	return cucsqMirrorTab[sq];
}

inline int MOVE(int sqSrc, int sqDst) {   
	return sqSrc + (sqDst << 8);
}
inline int MOVE_MIRROR(int mv) {          
	return MOVE(SQUARE_MIRROR(getSRC(mv)), SQUARE_MIRROR(getDST(mv)));
}

int GetBookMoves(const Board& pos, BookStruct* lpbks) {
	BookStruct2 book_temp;
	Board posScan=pos;
	BookStruct bk;
	int nScan, nLow, nHigh, nPtr;
	int i, j, move_num;
	book_temp.init();
	// 折半查找法搜索；
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
		// 镜像局面
		pos.mirror(posScan);
	}

	// 找不到局面；
	if (nScan == 2) {
		return 0;
	}

	// 找到局面；
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
			// 如果是镜像，做镜像处理：
			lpbks[move_num].nPtr = nPtr;
			lpbks[move_num].wmv = (nScan == 0 ? bk.wmv : MOVE_MIRROR(bk.wmv));
			lpbks[move_num].wvl = bk.wvl;
			move_num++;
			if (move_num == MAX_GEN_MVS) {
				break;
			}
		}
	}
	//排序
	for (i = 0; i < move_num - 1; i++) {
		for (j = move_num - 1; j > i; j--) {
			if (lpbks[j - 1].wvl < lpbks[j].wvl) {
				std::swap(lpbks[j - 1], lpbks[j]);
			}
		}
	}
	return move_num;
}
