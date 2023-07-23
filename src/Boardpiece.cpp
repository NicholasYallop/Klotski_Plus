#include "common.h"

BoardPiece::BoardPiece() : next(nullptr), texture(nullptr) {
	x = y = w = h = 0;
}

BoardPiece::BoardPiece(const BoardPiece& piece) : BoardPiece() {
	x = piece.x;
	y = piece.y;
	w = piece.w;
	h = piece.h;
	texture = piece.texture;
	DoAction = piece.DoAction;
}