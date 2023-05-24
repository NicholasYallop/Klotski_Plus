#include "SDL2/SDL.h"
#include "enums.h"

struct Entity;

struct Entity {
	float x, y;
	int w, h;
	float dx, dy;
	int team;
	SDL_Texture *texture;
	Entity *next;
};

struct BoardPiece;

struct BoardPiece {
	float x, y;
	int w, h;
	SDL_Texture *texture;
	BoardPiece *next;
};

struct Tile;

struct TileType {
	int team;
	SDL_Texture *texture;
	INTERACTION_FLAG (*tileInteraction)(Tile*, Tile*);
	void (*clickInteraction)(Tile*);
};

struct RollingEffect;
struct RollingEffect
{
	RollingEffect *next;
	char* print;
};

struct Tile : Entity {
	TileType *tileType;
	RollingEffect rollingEffectHead, *rollingEffectTail;
	bool toDestroy = false;
	Tile() : Entity(), rollingEffectHead(), rollingEffectTail(&rollingEffectHead) {}
	Tile(int i,  int j, TileType *TileType) : Tile() 
	{
		x = BOARD_SCREEN_OFFSET_X + (i+1)*BOARDPIECE_WIDTH - TILE_WIDTH;
		y = BOARD_SCREEN_OFFSET_Y + (j+1)*BOARDPIECE_HEIGHT - TILE_HEIGHT;
		w = TILE_WIDTH;
		h = TILE_HEIGHT;
		tileType = TileType;
		texture = tileType->texture;
		team = tileType->team;
	}
};

struct Round;

struct Round {
	Tile tileHead, *tileTail;
	Round *next;
	Round() : tileHead(), tileTail(&tileHead){}
};

struct Button;

struct Button : Entity {
	void (*Click)(void);
};

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

struct Stage {
	Tile tileHead, *tileTail;
	BoardPiece pieceHead, *pieceTail;
	Round roundHead, *roundTail;
	Button buttonHead, *buttonTail;
};

typedef struct {
	SDL_Renderer *renderer;
	SDL_Window *window;
	Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
	int leftClickActive, rightClickActive;
} App;