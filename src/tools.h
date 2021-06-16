#ifndef TOOLS
#define TOOLS
#include <iostream>
#include <sys/timeb.h>

inline int64_t GetTime() {
    timeb tb;
    ftime(&tb);
    return (int64_t)tb.time * 1000 + tb.millitm;
}

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

inline int RANK_Y(int sq) {
  return sq >> 4;
}

inline int FILE_X(int sq) {
  return sq & 15;
}

inline int SIDE_VALUE(int turn, int value){
    return (turn==1)?value:-value;
}

inline uint32_t BIT_PIECE(int pc) {
  return 1 << (pc - 16);
}

#endif
