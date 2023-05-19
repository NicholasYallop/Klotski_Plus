#include "SDL2/SDL.h"

struct Entity;

struct Entity {
	float x, y;
	int w, h;
	float dx, dy;
	int team;
	SDL_Texture *texture;
	Entity *next;
};

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

typedef struct {
	Entity tileHead, *tileTail;
} Stage;

typedef struct {
	SDL_Renderer *renderer;
	SDL_Window *window;
	Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
} App;