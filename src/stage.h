#pragma once
#include "common.h"

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
};

void initStage(void);
