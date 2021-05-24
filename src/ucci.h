#ifndef UCCI_H
#define UCCI_H

/* �ļ���ucci.h
 */
const int MaxDepth = 32; // UCCI����˼���������

enum CommEnum {
    Comm_none, Comm_ucci,
    Comm_isready, Comm_position,
    Comm_go, Comm_quit
};  // UCCIָ������

// UCCIָ����Խ��ͳ������������Ľṹ
union UCCIComm {

    /* 1. "position"ָ��ݵ���Ϣ���ʺ���"Comm_position"ָ������
     *    "position"ָ���������þ��棬������ʼ������ͬ�����ŷ����ɵľ���
     *    ���磬position startpos moves h2e2 h9g8��FEN������"startpos"�����FEN�����ŷ���(Move_Num)����2
     */
    struct {
        const char* Fen; // FEN�����������(��"startpos"��)Ҳ�ɽ���������ת����FEN��
        int Move_Num;        // �����ŷ���
        char* CoordList;    // �����ŷ���ָ�����"IdleLine()"�е�һ����̬���飬�����԰�"CoordList"����������;
        char player;
        char* pos;
        //�����ν�ʱ�����ǵ��޸�
    } position;

    /* 2. "go"ָ��ݵ���Ϣ���ʺ���"e_CommGo"ָ������
     *    "go"ָ��������˼��(����)��ͬʱ�趨˼��ģʽ�����̶���ȡ�ʱ���ƻ��Ǽ�ʱ��
     */
    struct {
        int TIME;
    }search;
};

// ��������������������UCCIָ��������ڲ�ͬ����
CommEnum BootLine(void);                                      // UCCI���������ĵ�һ��ָ�ֻ����"ucci"
CommEnum IdleLine(UCCIComm& Command, int /* bool */ Debug); // �������ʱ����ָ��
CommEnum BusyLine(UCCIComm& Command, int /* bool */ Debug);                      // ����˼��ʱ����ָ��

#endif
