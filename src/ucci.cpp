#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <sys/timeb.h>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "ucci.h"  
#pragma comment(lib, "Shlwapi.lib")

const int MAX = 1024;

#ifdef _WIN32
//#include <windows.h>
inline void Idle(void) {
	Sleep(1);
}

#else
//#include <time.h>
inline void Idle(void) {
}
#endif


CommEnum BootLine(void)
{
	char Line_Str[MAX];
	while (!std::cin.getline(Line_Str, MAX)) {
		Idle();
	}
	if (strcmp(Line_Str, "ucci") == 0)
		return Comm_ucci;
	else
		return Comm_none;
}


CommEnum IdleLine(UCCIComm& UcciComm)
{
	char Line_Str[MAX];
	char* lp;
	while (!std::cin.getline(Line_Str, MAX)) {
		Idle();
	}

	lp = Line_Str;
	if (strcmp(lp, "isready") == 0) {
		return Comm_isready;
	}

	//"position {<special_position> | fen <fen_string>} [move_steps <move_step_list>]"指令,着法列表保存在UcciComm里
	else if (strncmp(lp, "position ", 9) == 0) {
		lp += 9;
		if (strncmp(lp, "startpos ", 8) == 0) {
			lp += 9;
			UcciComm.position.Fen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR r";
			UcciComm.position.player = 'r';
			int i;
			for (i = 0; *(UcciComm.position.Fen + i) != ' '; i++) {
				if (*(UcciComm.position.Fen + i) != ' ') {
					UcciComm.position.pos [i] = *(UcciComm.position.Fen + i);
				}
			}
			UcciComm.position.pos[i]='\0';
			UcciComm.position.posLen = i;
			//UcciComm.position.pos[i] = '\0';

		}
		// 指定FEN串
		else if (strncmp(lp, "fen ", 4) == 0) {
			lp += 4;
			UcciComm.position.Fen = lp;
			int l; int space_num = 0;
			for (l = 0; space_num < 6; lp++) {
				if (*lp != ' '|| *lp == '\n') {
					if (space_num == 0) {
						UcciComm.position.pos [l] = *lp;
						l++;
					}
					else if (space_num == 1)
						UcciComm.position.player = *lp;
				}
				else if (*lp == ' '|| *lp == '\n')
					space_num++;
			}
			/*printf_s("******: %s", UcciComm.position.pos);
			fflush(stdout);
			system("pause");*/
			UcciComm.position.pos [l] = '\0';
			UcciComm.position.posLen = l;
		}
		// 返回
		else {
			return Comm_none;
		}

		//后续着法
		if (strncmp(lp, "move_steps ", 6) == 0) {
			lp += 6;
			UcciComm.position.move_step_Num = min((int)(strlen(lp) + 1) / 5, MAX); 
			UcciComm.position.CoordList = lp;
			return Comm_position;
		}
		return Comm_position;
	}
	//go 指令
	else if (strncmp(lp, "go time ", 8) == 0) {
		lp += 8;
		int temp_time = 0, nt = 0;
		if (sscanf_s(lp, "%d", &temp_time) > 0) {
			if (temp_time > 0)
				UcciComm.search.TIME = temp_time < 60000000 ? temp_time : 60000000;
			else
				UcciComm.search.TIME = 0;
		}
		else
			UcciComm.search.TIME = 0;
		return Comm_go;
	}
	// quit指令
	else if (strcmp(lp, "quit") == 0) 
		return Comm_quit;
	//其他
	else 
		return Comm_none;
}

CommEnum BusyLine(UCCIComm& UcciComm)
{
	char Line_Str[MAX];
	char* lp;
	if (std::cin.getline(Line_Str, MAX)) {

		if (strcmp(Line_Str, "isready") == 0) 
			return Comm_isready;

		else if (strcmp(Line_Str, "quit") == 0)
			return Comm_quit;

		else
			return Comm_none;		
	}
	else
		return Comm_none;
}
