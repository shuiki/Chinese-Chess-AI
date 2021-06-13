#ifndef UCCI_H
#define UCCI_H

// UCCI引擎思考极限深度
const int MaxDepth = 32;

// UCCI指令类型
enum CommEnum {
    Comm_none, Comm_ucci,
    Comm_isready, Comm_position,
    Comm_go, Comm_quit
};  

union UCCIComm {
    struct {
        const char* Fen; 
        int move_step_Num;       
        char* CoordList;   
        char player;
        char pos[8192];
        int posLen;
    } position;
    struct {
        int TIME;
    }search;
};

CommEnum BootLine(void);                                     
CommEnum IdleLine(UCCIComm& Command); 
CommEnum BusyLine(UCCIComm& Command);                      

#endif
