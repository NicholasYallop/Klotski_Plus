#include "common.h"

extern App app;
static Stage stage;
static SDL_Texture *boardPieceTexture;
static TileType blueTile;
static TileType redTile;

static void spawnTile(int x, int y, TileType *tileType)
{
	Tile *newTile = new Tile();
	
	newTile->x = x;
	newTile->y = y;
	newTile->w = TILE_WIDTH;
	newTile->h = TILE_HEIGHT;
	newTile->texture = tileType->texture;
	newTile->tileType = tileType;

	stage.tileTail->next = newTile;
	stage.tileTail = newTile;
}

static void spawnBoardPiece(int x, int y)
{
	BoardPiece *newPiece = new BoardPiece();
	
	newPiece->x = x;
	newPiece->y = y;
	newPiece->w = BOARDPIECE_WIDTH;
	newPiece->h = BOARDPIECE_HEIGHT;
	newPiece->texture = boardPieceTexture;

	stage.pieceTail->next = newPiece;
	stage.pieceTail = newPiece;
}

static void doTileCollisions()
{
	Entity *tile;
	Entity *comparisonTile;
	Entity *prev = &stage.tileHead;

	for (tile = stage.tileHead.next; tile != NULL; tile = tile->next)
	{
		Entity *innerPrev = tile;
		bool tileCollided = false;

		for (comparisonTile = tile->next; comparisonTile != NULL; comparisonTile = comparisonTile->next)
		{
			if (collision(tile->x, tile->y, tile->w, tile->h, comparisonTile->x, comparisonTile->y, comparisonTile->w, comparisonTile->h))
			{
				tileCollided = true;

				if (comparisonTile == tile->next)
				{
					tile->next = comparisonTile->next;
				}

				innerPrev->next = comparisonTile->next;

				free(comparisonTile);

				comparisonTile = innerPrev;
			}

			innerPrev = comparisonTile;
		}

		if (tileCollided)
		{
			tileCollided = true;

			if (tile == stage.tileTail)
			{
				stage.tileTail = static_cast<Tile*>(prev);
			}

			prev->next = tile->next;

			free(tile);

			tile = prev;
		}

		prev = tile;
	}
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
	doTileCollisions();

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

static void drawBoard(void)
{
	BoardPiece *b;

	for (b=stage.pieceHead.next; b != NULL; b = b->next){
		if (!b->h || !b->w){
			blit(b->texture, b->x, b->y);
		}
		else{
			blit(b->texture, b->x, b->y, b->w, b->h);
		}
	}
}

static void draw(void)
{
	drawBoard();

	drawTiles();
}

static void initColours(void)
{
	blueTile = TileType();
	blueTile.team = 1;
	blueTile.texture = loadTexture("gfx/blue.png");

	redTile = TileType();
	redTile.team = 2;
	redTile.texture = loadTexture("gfx/red.png");
}

static void initTiles(void)
{
	spawnTile(110, 110, &blueTile);
	spawnTile(210, 110, &redTile);
}

static void initBoard(void)
{
	int i, j;

	for (i = 0 ; i<11 ; i += 1)
	{
		for (j = 0 ; j<6 ; j += 1)
		{
			spawnBoardPiece(BOARD_SCREEN_OFFSET_X + i * BOARDPIECE_WIDTH, BOARD_SCREEN_OFFSET_Y + j * BOARDPIECE_HEIGHT);
		}
	}
}

void initStage(void)
{
	boardPieceTexture = loadTexture("gfx/dark_brown.jpg");

	app.delegate.logic = logic;
	app.delegate.draw = draw;

	stage = Stage();
	stage.tileTail = &stage.tileHead;
	stage.pieceTail = &stage.pieceHead;

	initColours();
	initTiles();
	initBoard();
}