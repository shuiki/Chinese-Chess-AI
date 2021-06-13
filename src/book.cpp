#include "BoardManipulate.h"
#include "book.h"
/**********************************
在eleeye 的 BOOK.DAT提取开局库
#include<iostream>
#include <sstream>
#include <fstream>
using namespace std;
struct BookStruct {
	union {
		uint32_t dwZobristLock;
		int nPtr;
	};
	uint16_t wmv, wvl;
};

int main()
{
	FILE* fp;
	int j;
	BookStruct book;
	int num = 0;
	fopen_s(&fp, "BOOK.DAT", "rb");
	ofstream out("结果.txt", ios::ate);
	if (fp == NULL)
		printf("file open error");
	int pchData[100] = { 0 };
	uint16_t temp_3[100] = { 0 };
	while (!feof(fp)) {

		fread(pchData, 4, 1, fp);
		fread(temp_3, 2, 2, fp);
		num++;
		out << "{uint32_t(" << pchData[0] << "),uint16_t(" << temp_3[0] << "),uint16_t(" << temp_3[1] << ")},\n";

	}
	out << num;
	out.close();
	fclose(fp);
	return 0;
}
********************************************/


int GetBookMoves(const Board& pos, BookStruct* lpbks) {
	BookStruct bk;
	Board pos_temp=pos;
	int number,nPtr,l, r;
	int i, j, move_num;
	// 二分查找：
	for (number = 0; number < 2; number++){
		nPtr = l = 0;
		r =nLen - 1;
		while (l <= r){
			nPtr = (l + r) / 2;
			Read(bk, nPtr);
			if (BOOK_compare(bk, pos_temp) < 0) 
				l = nPtr + 1;
			else if (BOOK_compare(bk, pos_temp) > 0) 
				r = nPtr - 1;
			else 
				break;
		}
		if (l <= r) 
			break;
		// 对称
		pos.mirror(pos_temp);
	}

	// 找不到:
	if (number == 2) 
		return 0;

	// 找到:
	for (nPtr--; nPtr >= 0; nPtr--) {
		Read(bk, nPtr);
		if (!BOOK_compare(bk, pos_temp)) 
			break;
	}
	move_num = 0;
	for (nPtr++; nPtr < nLen; nPtr++) {
		Read(bk, nPtr);
		if (BOOK_compare(bk, pos_temp)) 
			break;

		// 对称
		if (pos_temp.isLegalMove(bk.wmv)) {
			lpbks[move_num].nPtr = nPtr;
			lpbks[move_num].wmv = (number == 0 ? bk.wmv : mirror_move(bk.wmv));
			lpbks[move_num].wvl = bk.wvl;
			move_num++;
			if (move_num == MAX_GEN_MVS) 
				break;
		}
	}
	//排序
	for (i = 0; i < move_num - 1; i++) 
		for (j = move_num - 1; j > i; j--) 
			if (lpbks[j - 1].wvl < lpbks[j].wvl) 
				std::swap(lpbks[j - 1], lpbks[j]);

	return move_num;
}
