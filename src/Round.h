#include "common.h"

struct Round {
	Tile tileHead, * tileTail;
	BoardPiece pieceHead, * pieceTail;
	Round* next;
	Round();
};