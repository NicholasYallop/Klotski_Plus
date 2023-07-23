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
	Entity() : next(nullptr){
		x = y = w = h = dx = dy = team = 0;
	}
};

struct TileType {
	int team;
	SDL_Texture *texture;
	INTERACTION_FLAG (*tileInteraction)(Tile*, Tile*);
	void (*clickInteraction)(Tile*);
};

struct Button : Entity {
	void (*Click)(void);
};

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

typedef struct {
	SDL_Renderer *renderer;
	SDL_Window *window;
	Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
	int leftClickActive, rightClickActive;
	int leftClickHeld, rightClickHeld;
} App;