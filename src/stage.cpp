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

static INTERACTION_FLAG doTileInteraction(Tile *tile1, Tile *tile2)
{
	return (tile1->tileType->tileInteraction(tile1, tile2));
}

static void doTileCollisions()
{
	Entity *tile;
	Entity *comparisonTile;
	Entity *prev = &stage.tileHead;

	for (tile = stage.tileHead.next; tile != NULL; tile = tile->next)
	{
		Entity *innerPrev = tile;
		bool DestoryOuterTile = false;

		for (comparisonTile = tile->next; comparisonTile != NULL; comparisonTile = comparisonTile->next)
		{
			bool DestroyComparisonTile = false;
			INTERACTION_FLAG flags;

			flags = doTileInteraction(static_cast<Tile*>(tile), static_cast<Tile*>(comparisonTile));

			printf("%u \n", flags);

			if (static_cast<uint8_t>(flags & INTERACTION_FLAG::DESTROY_TILE1)){
				DestoryOuterTile = true;
			}
			if (static_cast<uint8_t>(flags & INTERACTION_FLAG::DESTORY_TILE2)){
				DestroyComparisonTile = true;
			}

			if (DestroyComparisonTile)
			{
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

		if (DestoryOuterTile)
		{
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
				static_cast<Tile*>(tile)->tileType->clickInteraction(static_cast<Tile*>(tile));
			}
		}
	}
}

static int isOutsideBoard(Entity *entity)
{
	return (
		(entity->x + entity->w) <= BOARD_SCREEN_OFFSET_X 
		|| (entity->x) >= BOARD_SCREEN_OFFSET_X + (BOARDPIECE_COUNT_X) * BOARDPIECE_WIDTH
		|| (entity->y + entity->h) <= BOARD_SCREEN_OFFSET_Y 
		|| (entity->y) >= BOARD_SCREEN_OFFSET_Y + (BOARDPIECE_COUNT_Y) * BOARDPIECE_HEIGHT
	);
}

static void doTiles(void)
{
	Entity *tile;
	Entity *prev = &stage.tileHead;

	for (tile = stage.tileHead.next; tile != NULL; tile = tile->next)
	{
		if (isOutsideBoard(tile))
		{
			if (tile == stage.tileHead.next)
			{
				stage.tileHead.next = tile->next;
			}

			prev->next = tile -> next;

			free(tile);

			tile = prev;
		}
		else
		{
			tile->x += tile->dx;
			tile->y += tile->dy;
		}

		prev = tile;
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

static INTERACTION_FLAG BlueInteractions(Tile *callingTile, Tile *interactingTile)
{
	int collided = collision(callingTile->x, callingTile->y, callingTile->w, callingTile->h, interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);

	if (interactingTile->tileType->team == redTile.team)
	{
		if (collided)
		{
			return INTERACTION_FLAG::DESTROY_TILE1 | INTERACTION_FLAG::DESTORY_TILE2;
		}
	}

	return INTERACTION_FLAG::NONE;
}

static void BlueClick(Tile *callingTile)
{
	callingTile->dx = 4;
}

static INTERACTION_FLAG RedInteractions(Tile *callingTile, Tile *interactingTile)
{
	int collided = collision(callingTile->x, callingTile->y, callingTile->w, callingTile->h, interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);

	if (interactingTile->tileType->team == blueTile.team)
	{
		if (collided)
		{
			return INTERACTION_FLAG::DESTROY_TILE1 | INTERACTION_FLAG::DESTORY_TILE2;
		}
	}

	return INTERACTION_FLAG::NONE;
}

static void RedClick(Tile *callingTile)
{
	callingTile->dy = 4;
}

static void initColours(void)
{
	blueTile = TileType();
	blueTile.team = 1;
	blueTile.texture = loadTexture("gfx/blue.png");
	blueTile.tileInteraction = BlueInteractions;
	blueTile.clickInteraction = BlueClick;

	redTile = TileType();
	redTile.team = 2;
	redTile.texture = loadTexture("gfx/red.png");
	redTile.tileInteraction = RedInteractions;
	redTile.clickInteraction = RedClick;
}

static void initTiles(void)
{
	spawnTile(110, 110, &blueTile);
	spawnTile(210, 110, &redTile);
	spawnTile(310, 110, &redTile);
}

static void initBoard(void)
{
	int i, j;

	for (i = 0 ; i<BOARDPIECE_COUNT_X ; i += 1)
	{
		for (j = 0 ; j<BOARDPIECE_COUNT_Y ; j += 1)
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