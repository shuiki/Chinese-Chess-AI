#include <iostream>
#include  <stdlib.h>
#include <cstdio>
#include "search.h"
#include "ucci.h"
#include"BoardManipulate.h"
using namespace std;

int main() {
	UCCIComm UcciComm;
	if (BootLine() != Comm_ucci) {
		return 0;
	}
	printf("%s\n", "ucciok");
	fflush(stdout);
	//extern SearchInfo searchInfo;
	InitZobrist();
	searchInfo.board.clearBoard();
	searchInfo.board.clearMoves();
	searchInfo.board.distance = 0;
	searchInfo.bQuit = false;
	searchInfo.bDebug = !true;
	searchInfo.bUseHash = true;
	searchInfo.bUseBook = true;
	while (1) {//!searchInfo.bQuit) {
		switch (IdleLine(UcciComm, searchInfo.bDebug)) {//searchInfo.bDebug)) {
		case Comm_isready:
			printf("%s\n", "readyok");
			fflush(stdout);
			break;
		case Comm_position:
			searchInfo.board.refreshBoard(UcciComm.position.pos, UcciComm.position.posLen, UcciComm.position.CoordList, UcciComm.position.Move_Num, UcciComm.position.player);
			searchInfo.board.distance = 0;
			//searchInfo.board.drawBoard();
			//char result[4];
			//MOVE_COORD(17235, result);//将结果转化为可输出字符串 int->char*
			//printf("17235: %.4s\n", (const char*)&result);
			//searchInfo.board.drawBoard();
			break;
		case Comm_go:
			searchInfo.nMaxTimer = (int)(0.95 * UcciComm.search.TIME);
			SearchMain(MaxDepth);
			//SearchMain(3);
			break;
		case Comm_quit:
			searchInfo.bQuit = true;
			break;
		default:
			break;
		}
	}
	printf("%s\n", "bye");
	fflush(stdout);
	system("PAUSE");
	getchar();
	getchar();
	return 0;
}