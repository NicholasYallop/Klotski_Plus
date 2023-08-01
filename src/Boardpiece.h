#pragma once
#include "common.h"

extern SDL_Texture* DefaultBoardPieceTexture;

struct BoardPiece {
	int x, y;
	int w, h;
	SDL_Texture* texture;
	BoardPiece* next;
	void (*DoAction)(Tile*);
	BoardPiece();
	BoardPiece(const BoardPiece& piece);
	static void initBoardPieces();
	static void containingBoardpiece(int x, int y, int& i, int& j);
	static void BoardPieceInteraction_Blank(Tile* tile);
	static void BoardPieceInteraction_FlipVelocity(Tile* tile);
};