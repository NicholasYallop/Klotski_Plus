#pragma once
#include "common.h"

class Round {
public:
	Tile tileHead, * tileTail;
	BoardPiece pieceHead, * pieceTail;
	Round* next;
	Round();
};