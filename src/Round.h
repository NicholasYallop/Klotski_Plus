#pragma once
#include "common.h"

class Round {
public:
	Tile tileHead, * tileTail;
	BoardPiece pieceHead, * pieceTail;
	Round* next;
	Round();
	void addTiles(Tile* tile);
	template<typename First, typename ...Tiles>
	void addTiles(First arg, const Tiles & ...rest);
	void addBoardPiece(BoardPiece* piece);
	template<typename First, typename ... BoardPieces>
	void addBoardPiece(First arg, const BoardPieces&... rest);
	void AddDefaultBoard();
};

#include "RoundTemplates.h"