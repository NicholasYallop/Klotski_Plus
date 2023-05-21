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

struct TileType;

struct Tile : Entity {
	TileType *tileType;
};

struct TileType {
	int team;
	SDL_Texture *texture;
	INTERACTION_FLAG (*tileInteraction)(Tile*, Tile*);
	void (*clickInteraction)(Tile*);
};

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

typedef struct {
	Tile tileHead, *tileTail;
	BoardPiece pieceHead, *pieceTail;
} Stage;

typedef struct {
	SDL_Renderer *renderer;
	SDL_Window *window;
	Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
	int leftClickActive, rightClickActive;
} App;