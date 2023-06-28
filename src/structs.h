#include "SDL.h"
#include "enums.h"

struct Entity;

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

struct BoardPiece;

struct BoardPiece {
	int x, y;
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
	double parameter;
	EFFECT_RETURN_FLAG (*action)(Tile *tile, double& parameter);
	RollingEffect() : next(0){}
	RollingEffect(EFFECT_RETURN_FLAG (action)(Tile*,double&), double parameter) : RollingEffect()
	{
		RollingEffect::action = action;
		RollingEffect::parameter = parameter;
	}
};

struct Tile : Entity {
	TileType *tileType;
	RollingEffect rollingEffectHead, *rollingEffectTail;
	bool toDestroy = false, isRealigning = false;
	Tile() : 
		Entity(), 
		rollingEffectHead(), rollingEffectTail(&rollingEffectHead){}
	Tile(const Tile& tile) : Tile()
	{
		x = tile.x;
		y = tile.y;
		w = TILE_WIDTH;
		h = TILE_HEIGHT;
		dx = tile.dx;
		dy = tile.dy;
		tileType = tile.tileType;
		texture = tileType->texture;
		team = tileType->team;
		RollingEffect *effect;
		for(effect=tile.rollingEffectHead.next; effect!=NULL; effect=effect->next)
		{
			rollingEffectTail->next = effect;
			rollingEffectTail = effect;
		}
	}
	Tile(int i,  int j, TileType *TileType) : Tile()
	{
		x = BOARD_SCREEN_OFFSET_X + (i+0.5)*BOARDPIECE_WIDTH - 0.5*TILE_WIDTH;
		y = BOARD_SCREEN_OFFSET_Y + (j+0.5)*BOARDPIECE_HEIGHT - 0.5*TILE_HEIGHT;
		w = TILE_WIDTH;
		h = TILE_HEIGHT;
		dx = dy = 0;
		tileType = TileType;
		texture = tileType->texture;
		team = tileType->team;
	}
	Tile(int i, int j, TileType *tileType, RollingEffect *effect) : Tile(i, j, tileType)
	{
		rollingEffectTail->next = effect;
		rollingEffectTail = effect;
	}
	template<typename First, typename ... Effects>
	Tile(int i, int j, TileType *tileType, First arg, const Effects&... rest): Tile(i, j, tileType, rest...)
	{
		rollingEffectTail->next = arg;
		rollingEffectTail = arg;
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
	int leftClickHeld, rightClickHeld;
} App;