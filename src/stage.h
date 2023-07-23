#pragma once
#include "common.h"

extern SDL_Texture *boardPieceTexture;

struct Stage {
	Tile tileHead, * tileTail;
	BoardPiece pieceHead, * pieceTail;
	Round roundHead, * roundTail;
	Button buttonHead, * buttonTail;
	void spawnTile(Tile* tile);
	void spawnTileToGrid(int i, int j, TileType* tileType);
	void queueTileSpawnToGrid(int i, int j, TileType* tileType);
	void queueTileSpawn(Tile* tile);
	void spawnQueueTiles();
	void queueTileSpawnFromCollision(Tile* tile1, Tile* tile2, TileType* tileType);
	void doTileCollisions();
	void destroyOutOfBoundsTiles();
	void doRollingEffects();
	void doTiles();
	void EmptyTiles();
	void spawnRoundTiles();
	void spawnBoardPiece(int x, int y);
	void spawnBoardPiece(BoardPiece* piece);
	void doBoardPieces();
	void EmptyBoardPieces();
	void spawnRoundBoardPieces();
	void drawBoard();
	void addRounds(Round* round);
	template<typename First, typename ... Rounds>
	void addRounds(First arg, const Rounds&... rest);
	void initRounds();
};

void initStage();

#include "StageTemplates.h"
