/*#include <cstdio>
//#include "tools.h"
#include "ucci.h"
//#include"BoardManipulate.h"
#include "hash.h"
//#include "search.h"


int main() {
	UCCIComm UcciComm;
	if (BootLine() != Comm_ucci) {
		return 0;
	}
	//InitZobrist();*/
	/*searchInfo.board.refreshBoard("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR\0", "\0", 0, 'r');
	searchInfo.board.distance = 0;
	searchInfo.bQuit = false;
	searchInfo.bDebug = true;
	searchInfo.bUseHash = true;*/
/*	printf("%s\n", "ucciok");
	fflush(stdout);
	while (true) {//!searchInfo.bQuit) {
		switch (IdleLine(UcciComm, 1)) {//searchInfo.bDebug)) {
		case Comm_isready:
			printf("%s\n", "readyok");
			fflush(stdout);
			break;
		case Comm_position:
			//searchInfo.board.refreshBoard(UcciComm.position.pos, UcciComm.position.CoordList, UcciComm.position.Move_Num, UcciComm.position.player);
			//searchInfo.board.distance = 0;
			break;
		case Comm_go:
			//searchInfo.nMaxTimer = (int)(0.95 * UcciComm.search.TIME);
			//SearchMain(MaxDepth);
			break;
		case Comm_quit:
			//searchInfo.bQuit = true;
			break;
		default:
			break;
		}
	}
	printf("%s\n", "bye");
	fflush(stdout);
	return 0;
}*/
#include <iostream>
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
	searchInfo.board.distance = 0;
	searchInfo.bQuit = false;
	searchInfo.bDebug = true;
	searchInfo.bUseHash = true;
	searchInfo.bUseBook = true;
	while (true) {//!searchInfo.bQuit) {
		switch (IdleLine(UcciComm, 1)) {//searchInfo.bDebug)) {
		case Comm_isready:
			printf("%s\n", "readyok");
			fflush(stdout);
			break;
		case Comm_position:

			searchInfo.board.refreshBoard(UcciComm.position.pos, UcciComm.position.CoordList, UcciComm.position.Move_Num, UcciComm.position.player);
			searchInfo.board.distance = 0;
			break;
		case Comm_go:
			searchInfo.nMaxTimer = (int)(0.95 * UcciComm.search.TIME);
			SearchMain(MaxDepth);
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
	return 0;
}