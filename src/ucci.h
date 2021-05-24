#ifndef UCCI_H
#define UCCI_H

/* 文件：ucci.h
 */
const int MaxDepth = 32; // UCCI引擎思考极限深度

enum CommEnum {
    Comm_none, Comm_ucci,
    Comm_isready, Comm_position,
    Comm_go, Comm_quit
};  // UCCI指令类型

// UCCI指令可以解释成以下这个抽象的结构
union UCCIComm {

    /* 1. "position"指令传递的信息，适合于"Comm_position"指令类型
     *    "position"指令用来设置局面，包括初始局面连同后续着法构成的局面
     *    例如，position startpos moves h2e2 h9g8，FEN串就是"startpos"代表的FEN串，着法数(Move_Num)就是2
     */
    struct {
        const char* Fen; // FEN串，特殊局面(如"startpos"等)也由解释器最终转换成FEN串
        int Move_Num;        // 后续着法数
        char* CoordList;    // 后续着法，指向程序"IdleLine()"中的一个静态数组，但可以把"CoordList"本身看成数组;
        char player;
        char* pos;
        //后续衔接时变量记得修改
    } position;

    /* 2. "go"指令传递的信息，适合于"e_CommGo"指令类型
     *    "go"指令让引擎思考(搜索)，同时设定思考模式，即固定深度、时段制还是加时制
     */
    struct {
        int TIME;
    }search;
};

// 下面三个函数用来解释UCCI指令，但适用于不同场合
CommEnum BootLine(void);                                      // UCCI引擎启动的第一条指令，只接收"ucci"
CommEnum IdleLine(UCCIComm& Command, int /* bool */ Debug); // 引擎空闲时接收指令
CommEnum BusyLine(UCCIComm& Command, int /* bool */ Debug);                      // 引擎思考时接收指令

#endif
