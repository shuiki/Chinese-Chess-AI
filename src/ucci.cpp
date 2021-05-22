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
CommEnum BootLine(void) {
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
CommEnum IdleLine(UCCIComm& UcciComm, int bDebug) {
	//���bDebugָ��Ϊ1�������ǰ������ָ��
	char Line_Str[INPUT_MAX];
	char* lp;
	static long Coord_List[MAX_MOVE_NUM];
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
	//�������positionָ�����ParsePosָ��ŷ��б�����UcciComm��
	else if (strncmp(lp, "position ", 9) == 0) {
		lp += 9;
		if (strncmp(lp, "startpos", 8) == 0) {
			UcciComm.position.Fen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
		}
		else if (strncmp(lp, "midgamepos", 10) == 0) {
			UcciComm.position.Fen = "2bakab1r/6r2/1cn4c1/p1p1p3p/9/2P3p2/PC2P1n1P/2N1B1NC1/R4R3/3AKAB2 w - - 0 1";
		}
		else if (strncmp(lp, "checkmatepos", 12) == 0) {
			UcciComm.position.Fen = "4kar2/4a2rn/4bc3/RN1c5/2bC5/9/4p4/9/4p4/3p1K3 w - - 0 1";
		}
		else if (strncmp(lp, "zugzwangpos", 11) == 0) {
			UcciComm.position.Fen = "3k5/4PP3/4r4/3P5/9/9/9/9/9/5K3 w - - 0 1";
		}
		else if (strncmp(lp, "endgamepos", 10) == 0) {
			UcciComm.position.Fen = "4k4/4a4/4P4/9/9/9/9/4B4/9/4K4 w - - 0 1";
			// Ȼ���ж��Ƿ�ָ����FEN��
		}
		else if (strncmp(lp, "fen ", 4) == 0) {
			UcciComm.position.Fen = lp + 4;
			// ������߶����ǣ�����������
		}
		else {
			return Comm_none;
		}
		// Ȼ��Ѱ���Ƿ�ָ���˺����ŷ������Ƿ���"moves"�ؼ���
		while (*lp != '\0') {
			if (strncmp(lp, " moves ", 7) == 0) {
				//�Թ�"moves"
				lp += 7;
				*(lp - strlen(" moves ")) = '\0';
				UcciComm.position.Move_Num = min((int)(strlen(lp) + 1) / 5, MAX_MOVE_NUM); 	// "moves"�����ÿ���ŷ�����1���ո��4���ַ�
				for (int i = 0; i < UcciComm.position.Move_Num; i++) {					
					Coord_List[i] = *(long*)lp; // 4���ַ���ת��Ϊһ��"long"���洢�ʹ�����������
					lp += 5;// lp�����ĸ��ַ���һ���ո�
				}
				UcciComm.position.CoordList = Coord_List;
				return Comm_position;
			}
			lp++;
		}
		UcciComm.position.Move_Num = 0;
		return Comm_position;
	}
	// 3. "go [ponder | draw] <mode>"ָ��
	else if (strncmp(lp, "go time ", 8) == 0) {
		lp += 8;
		int nRet=0,nt=0;
		if (sscanf_s(lp, "%d", &nRet) > 0) {
			if (nRet > 0)
				UcciComm.search.TIME = nRet < 3600000000 ? nRet : 3600000000;
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
CommEnum BusyLine(UCCIComm& UcciComm, int bDebug) {
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