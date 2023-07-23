#pragma once
#include "common.h"

struct BoardPiece {
	int x, y;
	int w, h;
	SDL_Texture* texture;
	BoardPiece* next;
	void (*DoAction)(Tile*);
	BoardPiece();
	BoardPiece(const BoardPiece& piece);
};