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

/*文件：ucci.cpp
 *
 * 其中第一个解释器"BootLine()"接收引擎启动后的第一行指令
 * 输入"ucci"，返回"Comm_ucci"，其他返回"Comm_none"
 * 前两个解释器等待是否有输入，若没有输入则执行待机指令"Idle()"
 * 第三个解释器"BusyLine()"，只用在引擎思考时,没有输入时直接返回"Comm_none"
 */

const int INPUT_MAX = 8192;
const int MAX_MOVE_NUM = 1024;

#ifdef _WIN32
//#include <windows.h>
inline void Idle(void) {
	Sleep(1);
}
#else
//#include <time.h>
inline void Idle(void) {
	timespec tv;
	tv.tv_sec = 0;
	tv.tv_nsec = 1000000;
	nanosleep(&tv, NULL);
}
#endif

//收引擎启动后的第一行指令,如果是ucci，则返回Comm_ucci
CommEnum BootLine(void)
{
	char Line_Str[INPUT_MAX];
	while (!std::cin.getline(Line_Str, INPUT_MAX)) {
		Idle();
	}
	if (strcmp(Line_Str, "ucci") == 0)
		return Comm_ucci;
	else
		return Comm_none;
}

//"IdleLine()"是最复杂的UCCI指令解释器，大多数的UCCI指令都由它来解释
CommEnum IdleLine(UCCIComm& UcciComm, int bDebug)
{
	//如果bDebug指令为1，输出当前读到的指令
	char Line_Str[INPUT_MAX];
	char* lp;
	while (!std::cin.getline(Line_Str, INPUT_MAX)) {
		//输入一个指令
		Idle();
	}
	lp = Line_Str;
	if (bDebug) {
		printf("info idleline [%s]\n", lp);
		fflush(stdout);
	}
	// 1. "isready"指令
	if (strcmp(lp, "isready") == 0) {
		return Comm_isready;
	}
	// 2. "position {<special_position> | fen <fen_string>} [moves <move_list>]"指令
	//如果读到position指令，着法列表保存在UcciComm里
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
		// 然后判断是否指定了FEN串
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
		// 如果两者都不是，就立即返回
		else {
			return Comm_none;
		}
		// 然后寻找是否指定了后续着法，即是否有"moves"关键字
		if (strncmp(lp, "moves ", 6) == 0) {
			//略过" moves "
			lp += 6;
			UcciComm.position.Move_Num = min((int)(strlen(lp) + 1) / 5, MAX_MOVE_NUM); 	// "moves"后面的每个着法都是1个空格和4个字符
			UcciComm.position.CoordList = lp;
			return Comm_position;
		}
		return Comm_position;
	}
	// 3. "go [ponder | draw] <mode>"指令
	else if (strncmp(lp, "go time ", 8) == 0) 
	{
		lp += 8;
		int nRet = 0, nt = 0;
		if (sscanf_s(lp, "%d", &nRet) > 0) {
			if (nRet > 0)
				UcciComm.search.TIME = nRet < 60000000 ? nRet : 60000000;
			else
				UcciComm.search.TIME = 0;
		}
		else
			UcciComm.search.TIME = 0;
		return Comm_go;
	}
	// 4. "quit"指令
	else if (strcmp(lp, "quit") == 0) {
		return Comm_quit;
	}
	// 5. 无法识别的指令
	else {
		return Comm_none;
	}
}

//第三个解释器只用在引擎思考时，没有输入时直接返回"Comm_none"
CommEnum BusyLine(UCCIComm& UcciComm, int bDebug)
{
	char Line_Str[INPUT_MAX];
	char* lp;
	if (std::cin.getline(Line_Str, INPUT_MAX)) {
		if (bDebug) {
			printf("info busyline [%s]\n", Line_Str);
			fflush(stdout);
			// "BusyLine"只能接收"isready"、"quit"这两条指令
			if (strcmp(Line_Str, "isready") == 0) {
				return Comm_isready;
			}
			else if (strcmp(Line_Str, "quit") == 0) {
				return Comm_quit;
			}
			else
				return Comm_none;
		}
	}
	else
		return Comm_none;
}
