#pragma once
#include "SDL.h"
#include "enums.h"

struct Entity;
struct BoardPiece;
struct RollingEffect;
struct Round;
struct Button;
struct Stage;
class Tile;

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

struct BoardPiece {
	int x, y;
	int w, h;
	SDL_Texture *texture;
	BoardPiece *next;
	void (*DoAction)(Tile*);
	BoardPiece() : next(nullptr), texture(nullptr) {
		x = y = w = h = 0;
	}

	BoardPiece(const BoardPiece& piece) : BoardPiece() {
		x = piece.x;
		y = piece.y;
		w = piece.w;
		h = piece.h;
		texture = piece.texture;
		DoAction = piece.DoAction;
	}
};

struct TileType {
	int team;
	SDL_Texture *texture;
	INTERACTION_FLAG (*tileInteraction)(Tile*, Tile*);
	void (*clickInteraction)(Tile*);
};

struct RollingEffect
{
	RollingEffect *next;
	double parameter;
	EFFECT_RETURN_FLAG (*action)(Tile *tile, double& parameter);
	RollingEffect() : next(0){}
	RollingEffect(EFFECT_RETURN_FLAG (action)(Tile*,double&), double parameter) : RollingEffect()
	{
		RollingEffect::action = action;
		RollingEffect::parameter = parameter;
	}
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