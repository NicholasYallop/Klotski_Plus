#include "common.h"

extern App app;
static Stage stage;
static SDL_Texture *tileTexture;

static void spawnTile(int x, int y)
{
	Entity *newTile = new Entity();
	
	newTile->x = x;
	newTile->y = y;
	newTile->w = TILE_WIDTH;
	newTile->h = TILE_HEIGHT;
	newTile->texture = tileTexture;

	stage.tileTail->next = newTile;
	stage.tileTail = newTile;
}

static void doTileClicks()
{
	if (app.leftClickActive)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);

		Entity *tile;

		for (tile = stage.tileHead.next; tile != NULL; tile = tile->next)
		{
			if (collision(x, y, CLICK_HEIGHT, CLICK_WIDTH, tile->x, tile->y, tile->w, tile->h))
			{
				tile->dx = 4;
			}
		}
	}
}

static void doTiles(void)
{
	Entity *tile;

	for (tile = stage.tileHead.next; tile != NULL; tile = tile->next)
	{
		tile->x += tile->dx;
	}
}

static void logic(void)
{
	doTileClicks();

	doTiles();
}

static void drawTiles(void)
{
	Entity *e;

	for (e=stage.tileHead.next; e != NULL; e = e->next){
		if (!e->h || !e->w){
			blit(e->texture, e->x, e->y);
		}
		else{
			blit(e->texture, e->x, e->y, e->w, e->h);
		}
	}
}

static void draw(void)
{
	drawTiles();
}

static void initBoard(void)
{
	spawnTile(100, 100);
}

void initStage(void)
{
	tileTexture = loadTexture("gfx/eye_test.jpg");

	app.delegate.logic = logic;
	app.delegate.draw = draw;

	stage = Stage();
	stage.tileTail = &stage.tileHead;

	initBoard();
}