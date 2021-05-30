#include"BoardManipulate.h"

using namespace std;

RC4 rander;
ZobristTable Zobrist;
PreEvalStruct PreEval;

const int position_val[7][256] = {
	// KING
	{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},
	// ADVISOR
	{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},
	// BISHOP
	{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 18,  0,  0,  0, 23,  0,  0,  0, 18,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},
	// KNIGHT
	{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
	0,  0,  0, 90, 96,103, 97, 94, 97,103, 96, 90,  0,  0,  0,  0,
	0,  0,  0, 92, 98, 99,103, 99,103, 99, 98, 92,  0,  0,  0,  0,
	0,  0,  0, 93,108,100,107,100,107,100,108, 93,  0,  0,  0,  0,
	0,  0,  0, 90,100, 99,103,104,103, 99,100, 90,  0,  0,  0,  0,
	0,  0,  0, 90, 98,101,102,103,102,101, 98, 90,  0,  0,  0,  0,
	0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,
	0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,
	0,  0,  0, 85, 90, 92, 93, 85, 93, 92, 90, 85,  0,  0,  0,  0,
	0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},
	// ROOK
	{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,206,208,207,213,214,213,207,208,206,  0,  0,  0,  0,
	0,  0,  0,206,212,209,216,233,216,209,212,206,  0,  0,  0,  0,
	0,  0,  0,206,208,207,214,216,214,207,208,206,  0,  0,  0,  0,
	0,  0,  0,206,213,213,216,216,216,213,213,206,  0,  0,  0,  0,
	0,  0,  0,208,211,211,214,215,214,211,211,208,  0,  0,  0,  0,
	0,  0,  0,208,212,212,214,215,214,212,212,208,  0,  0,  0,  0,
	0,  0,  0,204,209,204,212,214,212,204,209,204,  0,  0,  0,  0,
	0,  0,  0,198,208,204,212,212,212,204,208,198,  0,  0,  0,  0,
	0,  0,  0,200,208,206,212,200,212,206,208,200,  0,  0,  0,  0,
	0,  0,  0,194,206,204,212,200,212,204,206,194,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},
	// CANNON
	{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,100,100, 96, 91, 90, 91, 96,100,100,  0,  0,  0,  0,
	0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,
	0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,
	0,  0,  0, 96, 99, 99, 98,100, 98, 99, 99, 96,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 96, 96,100, 96, 96, 96, 96,  0,  0,  0,  0,
	0,  0,  0, 95, 96, 99, 96,100, 96, 99, 96, 95,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,
	0,  0,  0, 97, 96,100, 99,101, 99,100, 96, 97,  0,  0,  0,  0,
	0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	},
	// PAWN
	{
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
	0,  0,  0, 39, 49, 69, 84, 89, 84, 69, 49, 39,  0,  0,  0,  0,
	0,  0,  0, 39, 49, 64, 74, 74, 74, 64, 49, 39,  0,  0,  0,  0,
	0,  0,  0, 39, 46, 54, 59, 61, 59, 54, 46, 39,  0,  0,  0,  0,
	0,  0,  0, 29, 37, 41, 54, 59, 54, 41, 37, 29,  0,  0,  0,  0,
	0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
	0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}
};


Board::Board()
{
	memset(chessBoard, 0, sizeof(int_8) * 256);
	memset(chessView, 0, sizeof(int_8) * 48);
	player = RED;
	valueRed = valueBlack = 0;
	dwBitPiece = 0;
	wBitPiece[0] = wBitPiece[1] = 0;
	memset(pastMoves, 0, sizeof(int_8) * MAX_MOV_NUM);
	pastMoveNum = 0;
	distance = 0;
	chessNum = 0;
	zobr.init();
	rander.InitZero();
}

void Board::addPiece(int_8 pos, int_8 piece)
{
	if (piece >=48)
	{
		return;
	}
	chessBoard[pos] = piece;
	chessView[piece] = pos;
	dwBitPiece ^= BIT_PIECE(piece);
	if (checkSide(piece, RED))
	{
		valueRed += position_val[PieceType[piece]][pos];
		zobr.XOR(Zobrist.Table[PieceType[piece]][pos]);
		wBitPiece[0] ^= BIT_PIECE(piece);
	}
	else
	{
		valueBlack += position_val[PieceType[piece]][254-pos];
		zobr.XOR(Zobrist.Table[PieceType[piece] + 7][pos]);
		wBitPiece[1] ^= BIT_PIECE(piece - 16);
	}

}

void Board::delPiece(int_8 pos)
{
	int_8 pc = chessBoard[pos];
	chessView[pc] = 0;
	chessBoard[pos] = 0;
	dwBitPiece ^= BIT_PIECE(pc);
	if (checkSide(pc, RED))
	{
		valueRed -= position_val[PieceType[pc]][pos];
		zobr.XOR(Zobrist.Table[PieceType[pc]][pos]);
		wBitPiece[0] ^= BIT_PIECE(pc);
	}
	else
	{
		valueBlack -= position_val[PieceType[pc]][254-pos];
		zobr.XOR(Zobrist.Table[PieceType[pc] + 7][pos]);
		wBitPiece[1] ^= BIT_PIECE(pc - 16);
	}
}

void Board::changeSide()
{
	player = (Player)(1 - (int)player);
	zobr.XOR(Zobrist.PlayerZobr);
}

bool Board::makeMove(int_16 mv)//走一步棋，返回被吃掉的子
{
	if (!isLegalMove(mv))
		return false;
	int_8 src = getSRC(mv);
	int_8 dst = getDST(mv);
	int_8 sp = chessBoard[src];
	int_8 dp = chessBoard[dst];
	if (dp)
		delPiece(dst);
	delPiece(src);
	addPiece(dst, sp);
	if (isChecked(player)) {
		delPiece(dst);
		addPiece(src, sp);
		if (dp)
			addPiece(dst,dp);
		return false;
	}
	changeSide();
	checked = isChecked(player);
	pastMoves[pastMoveNum] = { mv,dp,checked,zobr };
	MOVE_COORD(mv, (char*)&pastMoves[pastMoveNum].show);
	pastMoveNum++;
	distance++;
	return true;
}

void Board::undoMakeMove()//撤销一步棋，参数是被吃掉的棋子
{
	int_16 mv = pastMoves[pastMoveNum - 1].move;
	int_8 dp = pastMoves[pastMoveNum - 1].captured;
	int_8 src = getSRC(mv);
	int_8 dst = getDST(mv);
	int_8 sp = chessBoard[dst];
	delPiece(dst);
	addPiece(src, sp);
	if (dp)
		addPiece(dst, dp);
	pastMoves[pastMoveNum - 1] = { 0,0,0,0 };
	pastMoveNum--;
	changeSide();
	checked = isChecked(player);
	distance--;
}


void Board::refreshBoard(const char* fen, int posLen, const char* moves, int movNum, char side)//根据ucci串更新棋盘
{
	clearBoard();
	int_8 boardPos = 51;
	int strPos = 0;
	int_8 blacks[7] = { 32,33,35,37,39,41,43 };
	int_8 reds[7] = { 16,17,19,21,23,25,27 };
	if (side == 'r'||side=='w')
		player = RED;
	else if (side == 'b')
		player = BLACK;
	for (int i = 0; i < posLen;i++)//在棋盘中填入fen串中的内容
	{
		char c = fen[strPos];
		int_8 curPiece = 0;
		if (c == '/')
		{
			int y = getY(boardPos) + 1;
			boardPos = coordXY(3, y);
			strPos++;
		}
		else if (c >= 'a' && c <= 'z')//黑子
		{
			curPiece = blacks[charToChessType(c)];
			blacks[charToChessType(c)]++;
			addPiece(boardPos, curPiece);
			boardPos++;
			strPos++;
		}
		else if (c >= 'A' && c <= 'Z')//红子
		{
			curPiece = reds[charToChessType(c)];
			reds[charToChessType(c)]++;
			addPiece(boardPos, curPiece);
			boardPos++;
			strPos++;
		}
		else if (c >= '0' && c <= '9')
		{
			boardPos += (c - '0');
			strPos++;
		}
	}
	strPos = 0;
	int_16 mv;
	int_8 src, dst;
	for (int i = 0; i < movNum; i++)
	{
		src = charToPos(moves[strPos], moves[strPos + 1]);
		dst = charToPos(moves[strPos + 2], moves[strPos + 3]);
		mv = getMV(src, dst);
		makeMove(mv);
		strPos += 5;
	}
	clearMoves();
	// PreEvaluate();
	//drawBoard();
}

bool Board::isLegalMove(int_16 mv)//判断一步棋是否合法
{
	int_8 src = getSRC(mv);
	int_8 sp = chessBoard[src];
	int_8 dst = getDST(mv);
	int_8 dp = chessBoard[dst];
	int delta = 0;
	int kp = 0;
	if (!inBoard(src) || !inBoard(dst))
		return false;
	if (!checkSide(sp, player))//准备移动的不是当前持方的棋子
		return false;
	if (dp && checkSide(dp, player))//落子处有当前持方的棋子
		return false;
	switch (pieceIndex(sp))
	{
	case(KING_FROM):
		return inBox(dst, player) && kingLegal(src, dst);
	case(ADVISOR_FROM):
	case(ADVISOR_TO):
		return inBox(dst, player) && advisorLegal(src, dst);
	case(BISHOP_FROM):
	case(BISHOP_TO):
		return (!crossRiver(player, dst)) && bishopLegal(src, dst) &&
			chessBoard[(src + dst) >> 1] == 0;
	case(KNIGHT_FROM):
	case(KNIGHT_TO):
		kp = knightPin(src, dst);
		return kp != src && chessBoard[kp] == 0;
	case(ROOK_FROM):
	case(ROOK_TO):
		if (sameX(src, dst))
			delta = src > dst ? -16 : 16;
		else if (sameY(src, dst))
			delta = src > dst ? -1 : 1;
		src += delta;
		while (src != dst && chessBoard[src] == 0)
			src += delta;
		return src == dst;
	case(CANNON_FROM):
	case(CANNON_TO):
		if (sameX(src, dst))
			delta = src > dst ? -16 : 16;
		else if (sameY(src, dst))
			delta = src > dst ? -1 : 1;
		src += delta;
		while (src != dst && chessBoard[src] == 0)
			src += delta;
		if (src == dst)
			return true;
		else if (dp == 0)
			return false;
		src += delta;
		while (src != dst && chessBoard[src] == 0)
			src += delta;
		if (src == dst)
			return true;
		else
			return false;
	default://兵卒
		int delta = player == BLACK ? 16 : -16;
		int diff = dst - src;
		if (crossRiver(player, src))
			return diff == delta || diff == 1 || diff == -1;
		else
			return diff == delta;
	}

}

void Board::mirror(Board& mirror) const {
	int sq, pc;
	mirror.clearBoard();
	for (sq = 0; sq < 256; sq++) {
		pc = chessBoard[sq];
		if (pc != 0) {
			mirror.addPiece(MIRROR_SQUARE(sq), pc);
		}
	}
	if (player == BLACK) {
		mirror.changeSide();
	}
	mirror.clearMoves();
}

bool Board::isChecked(Player player)//判断某玩家是否被将军
{
	Player side = player;
	int_8 kingIndex = player == RED ? 16 : 32;
	int_8 kingPos = chessView[kingIndex];
	//判断是否被马将军
	for (int i = 0; i < 8; i++)
	{
		int delta = KNIGHT_DELTA[i];
		int_8 src = kingPos, dst = kingPos + delta;
		if (inBoard(dst) && PieceType[chessBoard[dst]] == KNIGHT_TYPE &&
			checkSide(chessBoard[dst], rival(player)) &&
			chessBoard[src + KNIGHT_CHECK[i]] == 0)
			return true;
	}
	//判断是否被车或炮将军或将帅对脸
	for (int i = 0; i < 4; i++)
	{
		int delta = KING_DELTA[i];
		int_8 src = kingPos, dst = kingPos + delta;
		while (inBoard(dst) && chessBoard[dst] == 0)
			dst += delta;
		if ((PieceType[chessBoard[dst]] == ROOK_TYPE || PieceType[chessBoard[dst]] == KING_TYPE) &&
			checkSide(chessBoard[dst], rival(player)))
			return true;
		dst += delta;
		while (inBoard(dst) && chessBoard[dst] == 0)
			dst += delta;
		if (PieceType[chessBoard[dst]] == CANNON_TYPE && checkSide(chessBoard[dst], rival(player)))
			return true;
	}
	//判断是否被兵卒将军
	for (int i = 0; i < 4; i++)
	{
		int delta = KING_DELTA[i];
		int_8 src = kingPos, dst = kingPos + delta;
		if (PieceType[chessBoard[dst]] == PAWN_TYPE && checkSide(chessBoard[dst], rival(player)))
			return true;
	}
	return false;
}


void Board::clearBoard()
{
	memset(chessBoard, 0, sizeof(int_8) * 256);
	memset(chessView, 0, sizeof(int_8) * 48);
	memset(pastMoves, 0, sizeof(int_8) * MAX_MOV_NUM);
	chessNum = 0;
	distance = 0;
	dwBitPiece = 0;
	zobr.init();
	clearMoves();
}

int Board::RepStatus(int limit) const {
	bool selfSide, bPerpCheck, bOppPerpCheck;
	const MoveStruct* tMv;

	selfSide = false;
	bPerpCheck = bOppPerpCheck = true;	// 长将标记
	tMv = pastMoves + pastMoveNum - 1;	// 指向历史表中最后节点
	while (tMv->move != 0 && tMv->captured == 0) {
		if (selfSide) {
			bPerpCheck = bPerpCheck && tMv->checked;
			if (tMv->zobr.key == zobr.key) {
				limit--;
				if (limit == 0) {
					return 1 + (bPerpCheck ? 2 : 0) + (bOppPerpCheck ? 4 : 0);
				}
			}
		}
		else {
			bOppPerpCheck = bOppPerpCheck && tMv->checked;
		}
		selfSide = !selfSide;
		tMv--;
	}
	return 0;
}

void Board::clearMoves()
{
	pastMoveNum = 1;
	pastMoves[0] = { 0,0,checked,zobr };
}

int Board::gemove_num(int_16* mvs, bool captureOnly)//生成走法，返回走法数
{
	int_8 curPos, curPiece, curType;
	int mvNum = 0;
	for (curPos = 51; curPos <= 203; curPos++)
	{
		curPiece = chessBoard[curPos];
		if (!checkSide(curPiece, player))
			continue;
		curType = pieceIndex(curPiece);
		switch (curType)
		{
		case(KING_FROM):
			for (int i = 0; i < 4; i++)
			{
				int_8 src = curPos, dst = curPos + KING_DELTA[i];
				if (inBox(dst, player) && !checkSide(chessBoard[dst], player))
				{
					if (!captureOnly || checkSide(chessBoard[dst], rival(player)))
					{
						mvs[mvNum] = getMV(src, dst);
						mvNum++;
					}
				}
			}
			break;
		case(ADVISOR_FROM):
		case(ADVISOR_TO):
			for (int i = 0; i < 4; i++)
			{
				int_8 src = curPos, dst = curPos + ADVISOR_DELTA[i];
				if (inBox(dst, player) && !checkSide(chessBoard[dst], player))
				{
					if (!captureOnly || checkSide(chessBoard[dst], rival(player)))
					{
						mvs[mvNum] = getMV(src, dst);
						mvNum++;
					}
				}
			}
			break;
		case(BISHOP_FROM):
		case(BISHOP_TO):
			for (int i = 0; i < 4; i++)
			{
				int_8 src = curPos, dst = curPos + BISHOP_DELTA[i];
				if (inBoard(dst)&&!crossRiver(player, dst) && !checkSide(chessBoard[dst], player) && chessBoard[(src + dst) >> 1] == 0)
				{
					if (!captureOnly || checkSide(chessBoard[dst], rival(player)))
					{
						mvs[mvNum] = getMV(src, dst);
						mvNum++;
					}
				}
			}
			break;
		case(KNIGHT_FROM):
		case(KNIGHT_TO):
			for (int i = 0; i < 8; i++)
			{
				int delta = KNIGHT_DELTA[i];
				int_8 src = curPos, dst = curPos + delta;
				if (inBoard(dst) && !checkSide(chessBoard[dst], player) && chessBoard[src + LegalSpan[delta + 64]] == 0)
				{
					if (!captureOnly || checkSide(chessBoard[dst], rival(player)))
					{
						mvs[mvNum] = getMV(src, dst);
						mvNum++;
					}
				}
			}
			break;
		case(ROOK_FROM):
		case(ROOK_TO):
			for (int i = 0; i < 4; i++)
			{
				int delta = KING_DELTA[i];
				int_8 src = curPos, dst = curPos + delta;
				while (inBoard(dst) && chessBoard[dst] == 0 && !captureOnly)
				{
					mvs[mvNum] = getMV(src, dst);
					mvNum++;
					dst += delta;
				}
				if (checkSide(chessBoard[dst], rival(player)))
				{
					mvs[mvNum] = getMV(src, dst);
					mvNum++;
				}
			}
			break;
		case(CANNON_FROM):
		case(CANNON_TO):
			for (int i = 0; i < 4; i++)
			{
				int delta = KING_DELTA[i];
				int_8 src = curPos, dst = curPos + delta;
				while (inBoard(dst) && chessBoard[dst] == 0 && !captureOnly)
				{
					mvs[mvNum] = getMV(src, dst);
					mvNum++;
					dst += delta;
				}
				dst += delta;
				while (inBoard(dst) && chessBoard[dst] == 0)
					dst += delta;
				if (checkSide(chessBoard[dst], rival(player)))
				{
					mvs[mvNum] = getMV(src, dst);
					mvNum++;
				}
			}
			break;
		default://兵卒
			int delta = player == BLACK ? 16 : -16;
			int_8 src = curPos, dst = curPos + delta;
			if (!checkSide(chessBoard[dst], player))
			{
				if (!captureOnly || checkSide(chessBoard[dst], rival(player)))
				{
					mvs[mvNum] = getMV(src, dst);
					mvNum++;
				}
			}
			if (crossRiver(player, src))
			{
				if (inBoard(src + 1) && !checkSide(chessBoard[src + 1], player))
				{
					if (!captureOnly || checkSide(chessBoard[dst], rival(player)))
					{
						mvs[mvNum] = getMV(src, src+1);
						mvNum++;
					}
				}
				if (inBoard(src - 1) && !checkSide(chessBoard[src - 1], player))
				{
					if (!captureOnly || checkSide(chessBoard[dst], rival(player)))
					{
						mvs[mvNum] = getMV(src, src-1);
						mvNum++;
					}
				}
			}
		}
	}
	return mvNum;
}

void InitZobrist(void) {
	int i, j;
	RC4 rc4;

	rc4.InitZero();
	Zobrist.PlayerZobr.initWithRC4(rc4);
	for (i = 0; i < 14; i++) {
		for (j = 0; j < 256; j++) {
			Zobrist.Table[i][j].initWithRC4(rc4);
		}
	}
}

void RC4::InitZero(void) {
	int i, j;
	int_8 uc;

	x = y = j = 0;
	for (i = 0; i < 256; i++) {
		s[i] = i;
	}
	for (i = 0; i < 256; i++) {
		j = (j + s[i]) & 255;
		uc = s[i];
		s[i] = s[j];
		s[j] = uc;
	}
}

int_8 RC4::NextByte(void) {
	this->x = (this->x + 1) & 255;
	this->y = (this->y + s[x]) & 255;
	std::swap(s[x], s[y]);
	return s[(s[x] + s[y]) & 255];
}

int_32 RC4::NextLong(void) {
	union {
		int_8 uc[4];
		int_32 dw;
	} Ret;
	Ret.uc[0] = NextByte();
	Ret.uc[1] = NextByte();
	Ret.uc[2] = NextByte();
	Ret.uc[3] = NextByte();
	return Ret.dw;
}



inline const char* PIECE_BYTE_IN_CHINESE(int pt, bool type) {
	if (type == true)
		return cszPieceBytesInChineseRed[pt];
	else
		return cszPieceBytesInChineseBlack[pt];
}

void Board::drawBoard()
{
	int i, j, pc;
	const char* c;
	printf("   a b c d e f g h i\n");
	for (i = 3; i <= 12; i++) {
		printf(" %d ", 12 - i);
		for (j = 3; j <= 11; j++) {
			pc = chessBoard[coordXY(j, i)];
			if (pc == 0) {
				printf(" .");
			}
			else {
				c = PIECE_BYTE_IN_CHINESE(PieceType[pc], pc < 32);
				printf("%s", c);
			}
		}
		printf("\n");
	}

}


//rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR
//h2e2 h9g7
/*
int main()
{
	Board myboard;
	myboard.refreshBoard("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR\0","h2e2 h9g7 \0",'r');
	myboard.drawBoard();
	int_16 mv;
	int_8 src, dst;
	char c[5];
	int_16 mvs[MAX_GEN_MVS];
	scanf_s("%s", c, 5);
	while ((c[0]) != 'q')
	{
		if (c[0] == 'u')
		{
			myboard.undoMakeMove();
			myboard.drawBoard();
		}
		else if (c[0] == 't')
		{
			printf("%d\n", myboard.gemove_num(mvs));
		}
		else if (c[0] == 's')
		{
			printf("%d\n", myboard.gemove_num(mvs,true));
		}
		else
		{
			src = charToPos(c[0], c[1]);
			dst = charToPos(c[2], c[3]);
			mv = getMV(src, dst);
			if (myboard.isLegalMove(mv))
			{
				myboard.makeMove(mv);
				myboard.drawBoard();
			}
			else
				printf("illegal move!\n");
		}
		scanf_s("%s",c,5);
	}
	system("pause");
	return 0;
}
*/

//h2e2 h9g7
/*
position fen rnbakabnr/4c4/9/9/2PP2P2/P7P/9/1C3C3/9/1RBAKABNR b - - 0 13 moves
go time 60000
*/