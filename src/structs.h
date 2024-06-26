#pragma once
#include "common.h"

struct Entity;
struct RollingEffect;
struct Round;
struct Button;
class BoardPiece;
class Tile;
struct Stage;

struct Entity {
	int x, y;
	int w, h;
	int dx, dy;
	int team;
	SDL_Texture *texture;
	Entity *next;
	Entity() : next(nullptr), texture(nullptr) {
		x = y = w = h = dx = dy = team = 0;
	}
	int isOutsideBoard() {
		return (
			(x + w) <= BOARD_SCREEN_OFFSET_X
			|| (x) >= BOARD_SCREEN_OFFSET_X + (BOARDPIECE_COUNT_X)*BOARDPIECE_WIDTH
			|| (y + h) <= BOARD_SCREEN_OFFSET_Y
			|| (y) >= BOARD_SCREEN_OFFSET_Y + (BOARDPIECE_COUNT_Y)*BOARDPIECE_HEIGHT
			);
	}
};

struct TileType {
	int team;
	SDL_Texture *texture;
	INTERACTION_FLAG (*tileInteraction)(Tile*, Tile*);
	void (*clickInteraction)(Tile*);
};

struct Button : Entity {
	void (Stage::*Click)(void);
};

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
} Delegate;