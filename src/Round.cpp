#include "common.h"

Round::Round() : tileHead(), tileTail(&tileHead), pieceHead(), pieceTail(&pieceHead), next(nullptr) {}

void Round::addTiles(Tile* tile)
{
	tileTail->next = tile;
	tileTail = tile;
}

void Round::addBoardPiece(BoardPiece* piece)
{
	pieceTail->next = piece;
	pieceTail = piece;
}

void Round::AddDefaultBoard() {
	int i, j;
	BoardPiece* piece;

	for (i = 0; i < BOARDPIECE_COUNT_X; i += 1)
	{
		for (j = 0; j < BOARDPIECE_COUNT_Y; j += 1)
		{
			piece = new BoardPiece();
			piece->x = BOARD_SCREEN_OFFSET_X + i * BOARDPIECE_WIDTH;
			piece->y = BOARD_SCREEN_OFFSET_Y + j * BOARDPIECE_HEIGHT;
			piece->w = BOARDPIECE_WIDTH;
			piece->h = BOARDPIECE_HEIGHT;
			piece->texture = DefaultBoardPieceTexture;
			piece->DoAction = BoardPiece::BoardPieceInteraction_Blank;
			pieceTail->next = piece;
			pieceTail = piece;
		}
	}
}