#pragma once
#include "common.h"

class Stage {
public:
	Round roundHead, * roundTail;
	Round* currentRound;

	Tile tileHead, * tileTail;
	Tile tileQueueHead, * tileQueueTail;

	BoardPiece pieceHead, * pieceTail;
	BoardPiece pieceQueueHead, * pieceQueueTail;

	Button buttonHead, * buttonTail;

	Stage() :
		roundHead(), roundTail(&roundHead),
		currentRound(&roundHead),
		tileHead(), tileTail(&tileHead),
		tileQueueHead(), tileQueueTail(&tileQueueHead),
		pieceHead(), pieceTail(&pieceHead),
		pieceQueueHead(), pieceQueueTail(&pieceQueueHead),
		buttonHead(), buttonTail(&buttonHead) {};

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
	void addRounds(Round* round);
	template<typename First, typename ... Rounds>
	void addRounds(First arg, const Rounds&... rest);
	void initRounds();
	void resetRound();
	void initButtons();
	void quit();
	void play();
	void playerWins();
};

void initStage();

#include "StageTemplates.h"
