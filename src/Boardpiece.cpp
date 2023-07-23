#include "common.h"

BoardPiece::BoardPiece() : next(nullptr) {
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

void BoardPiece::containingBoardpiece(int x, int y, int& i, int& j)
{
	i = floor((x - BOARD_SCREEN_OFFSET_X) / BOARDPIECE_WIDTH);
	j = floor((y - BOARD_SCREEN_OFFSET_Y) / BOARDPIECE_HEIGHT);
}

void BoardPiece::BoardPieceInteraction_Blank(Tile* tile) {
	return;
}

void BoardPiece::BoardPieceInteraction_FlipVelocity(Tile* tile) {
	int dx = tile->dx;
	tile->dx = tile->dy;
	tile->dy = dx;
}