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

/*�ļ���ucci.cpp
 *
 * ���е�һ��������"BootLine()"��������������ĵ�һ��ָ��
 * ����"ucci"������"Comm_ucci"����������"Comm_none"
 * ǰ�����������ȴ��Ƿ������룬��û��������ִ�д���ָ��"Idle()"
 * ������������"BusyLine()"��ֻ��������˼��ʱ,û������ʱֱ�ӷ���"Comm_none"
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

//������������ĵ�һ��ָ��,�����ucci���򷵻�Comm_ucci
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

//"IdleLine()"����ӵ�UCCIָ����������������UCCIָ�����������
CommEnum IdleLine(UCCIComm& UcciComm, int bDebug)
{
	//���bDebugָ��Ϊ1�������ǰ������ָ��
	char Line_Str[INPUT_MAX];
	char* lp;
	while (!std::cin.getline(Line_Str, INPUT_MAX)) {
		//����һ��ָ��
		Idle();
	}
	lp = Line_Str;
	if (bDebug) {
		printf("info idleline [%s]\n", lp);
		fflush(stdout);
	}
	// 1. "isready"ָ��
	if (strcmp(lp, "isready") == 0) {
		return Comm_isready;
	}
	// 2. "position {<special_position> | fen <fen_string>} [moves <move_list>]"ָ��
	//�������positionָ��ŷ��б�����UcciComm��
	else if (strncmp(lp, "position ", 9) == 0) {
		lp += 9;
		if (strncmp(lp, "startpos", 8) == 0) {
			lp += 8;
			UcciComm.position.Fen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";
			UcciComm.position.player = 'r';
			int i;
			for (i = 0; i < *(UcciComm.position.Fen + i) != ' '; i++) {
				if (*(UcciComm.position.Fen + i) != ' ') {
					*(UcciComm.position.pos + i) = *(UcciComm.position.Fen + i);
				}
			}
			*(UcciComm.position.pos + i) = '\0';

		}
		// Ȼ���ж��Ƿ�ָ����FEN��
		else if (strncmp(lp, "fen ", 4) == 0) {
			lp += 4;
			UcciComm.position.Fen = lp;
			int l; int space_num = 0;
			for (l = 0; space_num < 2; lp++, l++) {
				if (*lp != ' ') {
					if (space_num == 0) {
						*(UcciComm.position.pos + l) = *lp;
					}
					else if (space_num == 1)
						UcciComm.position.player = *lp;
				}
				else if (*lp == ' ')
					space_num++;
			}
			*(UcciComm.position.pos + l) = '\0';
			lp += 7;
		}
		// ������߶����ǣ�����������
		else {
			return Comm_none;
		}
		// Ȼ��Ѱ���Ƿ�ָ���˺����ŷ������Ƿ���"moves"�ؼ���
		if (strncmp(lp, " moves ", 7) == 0) {
			//�Թ�" moves "
			lp += 7;
			UcciComm.position.Move_Num = min((int)(strlen(lp) + 1) / 5, MAX_MOVE_NUM); 	// "moves"�����ÿ���ŷ�����1���ո��4���ַ�
			UcciComm.position.CoordList = lp;
			return Comm_position;
		}
		return Comm_position;
	}
	// 3. "go [ponder | draw] <mode>"ָ��
	else if (strncmp(lp, "go time ", 8) == 0) {
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
	// 4. "quit"ָ��
	else if (strcmp(lp, "quit") == 0) {
		return Comm_quit;
	}
	// 5. �޷�ʶ���ָ��
	else {
		return Comm_none;
	}
}

//������������ֻ��������˼��ʱ��û������ʱֱ�ӷ���"Comm_none"
CommEnum BusyLine(UCCIComm& UcciComm, int bDebug)
{
	char Line_Str[INPUT_MAX];
	char* lp;
	if (std::cin.getline(Line_Str, INPUT_MAX)) {
		if (bDebug) {
			printf("info busyline [%s]\n", Line_Str);
			fflush(stdout);
			// "BusyLine"ֻ�ܽ���"isready"��"quit"������ָ��
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