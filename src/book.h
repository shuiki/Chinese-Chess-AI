#include <cstdio>
#include "buffer.h"
#include"BoardManipulate.h"

#ifndef BOOK_H
#define BOOK_H


inline int BOOK_POS_CMP(const BookStruct& bk, const Board& pos) {
	return bk.dwZobristLock <((uint32_t) pos.zobr.lock_2) ? -1 :
		bk.dwZobristLock >((uint32_t)pos.zobr.lock_2) ? 1 : 0;
}

struct BookStruct2 {
	int nLen;
	void init() {
		nLen = 12081;
	}
	void Read(BookStruct& bk, int nPtr) const {
		bk = book[nPtr];
	}
};

// 获取开局库着法
int GetBookMoves(const Board& pos, BookStruct* lpbks);

#endif
