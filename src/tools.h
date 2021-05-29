#ifndef TOOS
#define TOOS
#include <iostream>
#include <sys/timeb.h>


inline int64_t GetTime() {
    timeb tb;
    ftime(&tb);
    return (int64_t)tb.time * 1000 + tb.millitm;
}

const int SimpleValues[48] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  5, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 2, 2, 2, 2, 2,
  5, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 2, 2, 2, 2, 2,
};

inline int PopCnt8(uint8_t uc) {
  int n;
  n = ((uc >> 1) & 0x55) + (uc & 0x55);
  n = ((n >> 2) & 0x33) + (n & 0x33);
  return (n >> 4) + (n & 0x0f);
}

inline int PopCnt16(uint16_t w) {
  int n;
  n = ((w >> 1) & 0x5555) + (w & 0x5555);
  n = ((n >> 2) & 0x3333) + (n & 0x3333);
  n = ((n >> 4) & 0x0f0f) + (n & 0x0f0f);
  return (n >> 8) + (n & 0x00ff); 
}

inline int PopCnt32(uint32_t dw) {
  int n;
  n = ((dw >> 1) & 0x55555555) + (dw & 0x55555555);
  n = ((n >> 2) & 0x33333333) + (n & 0x33333333);
  n = ((n >> 4) & 0x0f0f0f0f) + (n & 0x0f0f0f0f);
  n = ((n >> 8) & 0x00ff00ff) + (n & 0x00ff00ff);
  return (n >> 16) + (n & 0x0000ffff);
}

inline uint32_t BOTH_BITPIECE(int nBitPiece) {
  return nBitPiece + (nBitPiece << 16);
}

inline int SIDE_TAG(int sd) {
  return 16 + (sd << 4);
}

inline int OPP_SIDE_TAG(int sd) {
  return 32 - (sd << 4);
}

inline int PIECE_INDEX(int pc) {
  return pc & 15;
}

inline int OPP_SIDE(int sd) {
  return 1 - sd;
}

inline int SQUARE_FORWARD(int sq, int sd) {
  return sq - 16 + (sd << 5);
}

inline int SQUARE_BACKWARD(int sq, int sd) {
  return sq + 16 - (sd << 5);
}

inline int RANK_Y(int sq) {
  return sq >> 4;
}

inline int FILE_X(int sq) {
  return sq & 15;
}

inline int SIDE_VALUE(int turn, int value){
    return (turn==1)?value:-value;
}

inline bool HOME_HALF(int sq, int sd) {
  return (sq & 0x80) != (sd << 7);
}

inline bool AWAY_HALF(int sq, int sd) {
  return (sq & 0x80) == (sd << 7);
}

inline bool SAME_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) == 0;
}

inline bool DIFF_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) != 0;
}

inline bool WHITE_HALF(int sq){
  return (sq>>7==1);
}

inline int RANK_DISP(int y) {
  return y << 4;
}

inline int FILE_DISP(int x) {
  return x;
}

inline int SIMPLE_VALUE(int pc) {
  return SimpleValues[pc];
}

inline uint32_t BIT_PIECE(int pc) {
  return 1 << (pc - 16);
}


#endif
