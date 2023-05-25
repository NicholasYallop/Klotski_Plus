#include "common.h"

extern App app;
extern bool Quit;
static Stage stage;
static Round *currentRound;
static SDL_Texture *boardPieceTexture;
static TileType blueTile;
static TileType redTile;
static TileType greyTile;

#pragma region tiles

static void spawnTile(Tile *tile)
{
	Tile *deepCopy = new Tile(*tile);
	stage.tileTail->next = deepCopy;
	stage.tileTail = deepCopy;
}

static void spawnTileToGrid(int i, int j, TileType *tileType)
{
	Tile *newTile = new Tile(i, j, tileType);

	stage.tileTail->next = newTile;
	stage.tileTail = newTile;
}

static EFFECT_RETURN_FLAG realignLeft(Tile *tile, double& parameter){
	if (parameter==0)
	{
		tile->dx = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	
	if (parameter<=RECOIL_SPEED_X)
	{
		tile->dx = -parameter;
		parameter = 0;
		return EFFECT_RETURN_FLAG::NONE;
	}
	tile->dx = -RECOIL_SPEED_X;
	parameter -= RECOIL_SPEED_X;
	return EFFECT_RETURN_FLAG::NONE;
}

static EFFECT_RETURN_FLAG realignRight(Tile *tile, double& parameter){
	if (parameter==0)
	{
		tile->dx = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	
	if (parameter<=RECOIL_SPEED_X)
	{
		tile->dx = parameter;
		parameter = 0;
		return EFFECT_RETURN_FLAG::NONE;
	}
	tile->dx = RECOIL_SPEED_X;
	parameter -= RECOIL_SPEED_X;
	return EFFECT_RETURN_FLAG::NONE;
}

static EFFECT_RETURN_FLAG realignDown(Tile *tile, double& parameter){
	if (parameter==0)
	{
		tile->dy = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	
	if (parameter<=RECOIL_SPEED_Y)
	{
		tile->dy = parameter;
		parameter = 0;
		return EFFECT_RETURN_FLAG::NONE;
	}
	tile->dy = RECOIL_SPEED_Y;
	parameter -= RECOIL_SPEED_Y;
	return EFFECT_RETURN_FLAG::NONE;
}

static EFFECT_RETURN_FLAG realignUp(Tile *tile, double& parameter){
	if (parameter==0)
	{
		tile->dy = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	
	if (parameter<=RECOIL_SPEED_Y)
	{
		tile->dy = -parameter;
		parameter = 0;
		return EFFECT_RETURN_FLAG::NONE;
	}
	tile->dy = -RECOIL_SPEED_Y;
	parameter -= RECOIL_SPEED_Y;
	return EFFECT_RETURN_FLAG::NONE;
}

static INTERACTION_FLAG doTileInteraction(Tile *tile1, Tile *tile2)
{
	return (tile1->tileType->tileInteraction(tile1, tile2));
}

static void containingTile(int x, int y , int& i, int& j)
{
	i = std::floor((x-BOARD_SCREEN_OFFSET_X)/BOARDPIECE_WIDTH);
	j = std::floor((y-BOARD_SCREEN_OFFSET_Y)/BOARDPIECE_HEIGHT);
}

static void spawnTileFromCollision(Tile *tile1, Tile *tile2, TileType *tileType)
{
	int contactX= std::max(tile1->x, tile2->x);
	int contactY= std::max(tile1->y, tile2->y);

	int i=0, j=0;
	containingTile(contactX, contactY, i, j);
	spawnTileToGrid(i, j, tileType);
}

static void doTileCollisions()
{
	Tile *tile;
	Tile *comparisonTile;
	Tile *prev = &stage.tileHead; 

	for (tile = static_cast<Tile*>(stage.tileHead.next); tile != NULL; tile = static_cast<Tile*>(tile->next))
	{
		for (comparisonTile = static_cast<Tile*>(tile->next); comparisonTile != NULL; comparisonTile = static_cast<Tile*>(comparisonTile->next))
		{
			INTERACTION_FLAG flags;

			flags = doTileInteraction(tile, comparisonTile);
			int tileRealignX, tileRealignY;
			int i, j;
			containingTile(tile->x + tile->w/2, tile->y + tile->h/2, i, j);
			tileRealignX = BOARD_SCREEN_OFFSET_X + BOARDPIECE_WIDTH*(i+0.5) - 0.5*TILE_WIDTH;
			tileRealignY = BOARD_SCREEN_OFFSET_Y + BOARDPIECE_HEIGHT*(j+0.5) - 0.5*TILE_HEIGHT;
			int comparisonTileRealignX, comparisonTileRealignY;
			containingTile(comparisonTile->x + comparisonTile->w/2, comparisonTile->y + comparisonTile->h/2, i, j);
			comparisonTileRealignX = BOARD_SCREEN_OFFSET_X + BOARDPIECE_WIDTH*(i+0.5) - 0.5*TILE_WIDTH;
			comparisonTileRealignY = BOARD_SCREEN_OFFSET_Y + BOARDPIECE_HEIGHT*(j+0.5) - 0.5*TILE_HEIGHT;
			if (static_cast<int>(flags & INTERACTION_FLAG::DESTROY_TILE1)){
				tile->toDestroy = true;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::DESTROY_TILE2)){
				comparisonTile->toDestroy = true;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::SPAWN_GREY_TILE)){
				spawnTileFromCollision(tile, comparisonTile, &greyTile);
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_RIGHT_TILE1))
			{
				printf("tile right: %i\n", tileRealignX - tile->x);
				RollingEffect *effect = new RollingEffect(realignRight, tileRealignX - tile->x);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_LEFT_TILE1))
			{
				printf("tile left: %i\n", tile->x - tileRealignX);
				RollingEffect *effect = new RollingEffect(realignLeft, tile->x - tileRealignX);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_UP_TILE1))
			{
				RollingEffect *effect = new RollingEffect(realignUp, tile->y - tileRealignY);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_DOWN_TILE1))
			{
				RollingEffect *effect = new RollingEffect(realignDown, tileRealignY - tile->y);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_RIGHT_TILE2))
			{
				printf("comparison right: %i\n", comparisonTileRealignX - comparisonTile->x);
				RollingEffect *effect = new RollingEffect(realignRight, comparisonTileRealignX - comparisonTile->x);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_LEFT_TILE2))
			{
				printf("comparison left: %i\n", comparisonTile->x - comparisonTileRealignX);//comparisonTileRealignX - comparisonTile->x);
				RollingEffect *effect = new RollingEffect(realignLeft, comparisonTile->x - comparisonTileRealignX);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_UP_TILE2))
			{
				RollingEffect *effect = new RollingEffect(realignUp, comparisonTile->y - comparisonTileRealignY);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_DOWN_TILE2))
			{
				RollingEffect *effect = new RollingEffect(realignDown, comparisonTileRealignY - comparisonTile->y);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}
		
		}
	}
	for (tile = static_cast<Tile*>(stage.tileHead.next); tile != NULL; tile = static_cast<Tile*>(tile->next))
	{
		if (tile->toDestroy)
		{
			if (tile == stage.tileTail)
			{
				stage.tileTail = prev;
			}

			prev->next = tile->next;

			free(tile);

			tile = prev;
		}

		prev = tile;
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

static void destroyOutOfBoundsTiles(void)
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

static void doRollingEffects()
{
	Tile *tile;
	for(tile=static_cast<Tile*>(stage.tileHead.next)
		; tile!=NULL
		; tile = static_cast<Tile*>(tile->next))
	{
		RollingEffect *prev = &tile->rollingEffectHead;
		RollingEffect *effect;
		for (effect=tile->rollingEffectHead.next
			; effect!=NULL
			; effect=effect->next)
		{
			EFFECT_RETURN_FLAG flags = effect->action(tile, effect->parameter);
			if (static_cast<uint8_t>(flags & EFFECT_RETURN_FLAG::END_EFFECT))
			{
				if (effect == tile->rollingEffectTail)
				{
					tile->rollingEffectTail=prev;
				}
				prev->next = effect->next;
				free(effect);
				effect = prev;
			}

			prev = effect;
		}
	}
}

static void doTiles(void)
{
	destroyOutOfBoundsTiles();
	doRollingEffects();
}

static void addTilesToRound(Round *round, Tile *tile)
{
	round->tileTail->next = tile;
	round->tileTail = tile;
}

template<typename First, typename ... Tiles>
static void addTilesToRound(Round *round, First arg, const Tiles&... rest)
{
	round->tileTail->next = arg;
	round->tileTail = arg;
	addTilesToRound(round, rest...);
}

static void spawnRoundTiles(void)
{
	Tile *tile;

	for (tile = static_cast<Tile*>(currentRound->tileHead.next); 
		tile != NULL; 
		tile = static_cast<Tile*>(tile->next))
	{
		spawnTile(tile);
	}
}

static void doTileClicks(int xMouse, int yMouse)
{
	Entity *tile;

		for (tile = stage.tileHead.next; tile != NULL; tile = tile->next)
		{
			if (collision(xMouse, yMouse, CLICK_HEIGHT, CLICK_WIDTH, tile->x, tile->y, tile->w, tile->h))
			{
				static_cast<Tile*>(tile)->tileType->clickInteraction(static_cast<Tile*>(tile));
			}
		}
}

static void drawTiles(void)
{
	Entity *e;

	for (e=stage.tileHead.next; e != NULL; e = e->next){
		if (!e->h || !e->w){
			blitInBoard(e->texture, e->x, e->y);
		}
		else{
			blitInBoard(e->texture, e->x, e->y, e->w, e->h);
		}
	}
}

#pragma endregion tiles

#pragma region boardPieces

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

#pragma endregion boardPieces

#pragma region Rounds

static void addRoundsToStage()
{
}

template<typename First, typename ... Rounds>
static void addRoundsToStage(First arg, const Rounds&... rest)
{
	stage.roundTail->next = arg;
	stage.roundTail = arg;
	addRoundsToStage(rest...);
}

static void initRounds(void)
{
	Round *round0 = new Round();
	addTilesToRound(
		round0,
		new Tile(1, 1, &blueTile),
		new Tile(0, 1, &blueTile)
	);

	Round *round1 = new Round();
	addTilesToRound(
		round1,
		new Tile(0, 0, &blueTile),
		new Tile(1, 0, &redTile),
		new Tile(2, 0, &redTile),
		new Tile(1, 1, &blueTile),
		new Tile(2, 1, &blueTile)
	);

	Round *round2 = new Round();
	addTilesToRound(
		round2,
		new Tile(7, 1, &redTile),
		new Tile(5, 2, &blueTile),
		new Tile(6, 2, &redTile),
		new Tile(7, 2, &blueTile),
		new Tile(8, 2, &redTile),
		new Tile(6, 3, &blueTile),
		new Tile(7, 3, &redTile)
	);

	Round *round3 = new Round();
	addTilesToRound(
		round3,
		new Tile(0, 0, &blueTile),
		new Tile(1, 1, &blueTile),
		new Tile(2, 2, &blueTile),
		new Tile(3, 3, &blueTile),
		new Tile(4, 4, &blueTile),
		new Tile(5, 5, &blueTile),

		new Tile(0, 5, &redTile),
		new Tile(1, 4, &redTile),
		new Tile(2, 3, &redTile),
		new Tile(3, 2, &redTile),
		new Tile(4, 1, &redTile),
		new Tile(5, 0, &redTile)
	);

	Round *round4 = new Round();
	addTilesToRound(
		round4,
		new Tile(4, 2, &blueTile),
		new Tile(5, 3, &redTile),
		new Tile(6, 2, &blueTile),
		new Tile(6, 1, &redTile),
		new Tile(5, 0, &blueTile),
		new Tile(4, 0, &redTile),
		new Tile(3, 0, &blueTile),
		new Tile(2, 1, &redTile),
		new Tile(2, 2, &blueTile),
		new Tile(2, 3, &redTile),
		new Tile(2, 4, &blueTile),
		new Tile(3, 5, &redTile),
		new Tile(4, 5, &blueTile),
		new Tile(5, 5, &redTile),
		new Tile(6, 5, &blueTile),
		new Tile(7, 5, &redTile),
		new Tile(8, 4, &blueTile),
		new Tile(8, 3, &redTile),
		new Tile(8, 2, &blueTile),
		new Tile(8, 1, &redTile),
		new Tile(8, 0, &blueTile)

	);

	Round *round5 = new Round();
	addTilesToRound(round5, 
		new Tile(0, 1, &redTile),
		new Tile(0, 0, &redTile),
		new Tile(1, 1, &blueTile),
		new Tile(2, 1, &blueTile));

	addRoundsToStage(/*round0, round1, round2, round3, round4,*/ round5);
}

static void resetRound()
{
	Entity *tile;

	while(stage.tileHead.next)
	{
		tile = stage.tileHead.next;
		stage.tileHead.next = stage.tileHead.next->next;
		free(tile);
	}
	stage.tileTail = &stage.tileHead;

	spawnRoundTiles();
}

#pragma endregion Rounds

#pragma region Buttons

static void doButtonClicks(int xMouse, int yMouse)
{
	Entity *button;

		for (button = stage.buttonHead.next; button != NULL; button=button->next)
		{
			if (collision(xMouse, yMouse, CLICK_HEIGHT, CLICK_WIDTH, button->x, button->y, button->w, button->h))
			{
				static_cast<Button*>(button)->Click();
			}
		}
}

static void drawButtons(void)
{
	Entity *button;

	for(button = stage.buttonHead.next; button != NULL; button=button->next)
	{
		blit(button->texture, button->x, button->y, button->w, button->h);
	}
}

static void quit(void);

static void initButtons()
{	
	Button *quitButton = new Button();
	quitButton->x = SCREEN_WIDTH - 35;
	quitButton->y = 10;
	quitButton->w = 15;
	quitButton->h = 15;
	quitButton->texture = loadTexture("gfx/red_cross.png");
	quitButton->Click = quit;

	stage.buttonTail->next = quitButton;
	stage.buttonTail = quitButton;

	Button *resetButton = new Button();
	resetButton->x = 10;
	resetButton->y = 10;
	resetButton->w = 40;
	resetButton->h = 40;
	resetButton->texture = loadTexture("gfx/restart.png");
	resetButton->Click = resetRound;

	stage.buttonTail->next = resetButton;
	stage.buttonTail = resetButton;
}

#pragma endregion Buttons

#pragma region Play

static void doClicks()
{
	if (app.leftClickActive)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);

		doTileClicks(x, y);

		doButtonClicks(x, y);
	}
}

static void play()
{
	currentRound = &stage.roundHead;
}

static void playerWins(void)
{
	if (currentRound->next != NULL)
	{
		currentRound = currentRound->next;
		stage.tileTail = &stage.tileHead;
		spawnRoundTiles();
	}
	else
	{
		Quit=true;
	}
}

static void quit()
{
	Quit=true;
}

#pragma endregion Play

#pragma region Colours

static INTERACTION_FLAG BlueInteractions(Tile *callingTile, Tile *interactingTile)
{
	int collided = collision(callingTile->x, callingTile->y, callingTile->w, callingTile->h, interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);

	if (interactingTile->tileType->team == blueTile.team)
	{
		if (collided)
		{
			return bounce(callingTile->x, callingTile->y, callingTile->w, callingTile->h, interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);
		}
	}
	if (interactingTile->tileType->team == redTile.team)
	{
		if (collided)
		{
			return INTERACTION_FLAG::DESTROY_TILE1 | INTERACTION_FLAG::DESTROY_TILE2 | INTERACTION_FLAG::SPAWN_GREY_TILE;
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

	if (interactingTile->tileType->team == redTile.team)
	{
		if (collided)
		{
			return bounce(callingTile->x, callingTile->y, callingTile->w, callingTile->h, interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);
		}
	}
	if (interactingTile->tileType->team == blueTile.team)
	{
		if (collided)
		{
			return INTERACTION_FLAG::DESTROY_TILE1 | INTERACTION_FLAG::DESTROY_TILE2 | INTERACTION_FLAG::DESTROY_TILE2 | INTERACTION_FLAG::SPAWN_GREY_TILE;
		}
	}

	return INTERACTION_FLAG::NONE;
}

static void RedClick(Tile *callingTile)
{
	callingTile->dy = 4;
}

static INTERACTION_FLAG greyInteractions(Tile *callingTile, Tile *interactingTile)
{
	return INTERACTION_FLAG::NONE;
}

static void greyClick(Tile *callingTile)
{
	return;
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

	greyTile = TileType();
	greyTile.team = 3;
	greyTile.texture = loadTexture("gfx/grey.png");
	greyTile.tileInteraction = greyInteractions;
	greyTile.clickInteraction = greyClick;
}

#pragma endregion Colours

static void logic(void)
{
	doTileCollisions();

	doClicks();

	doTiles();

	if (stage.tileHead.next == NULL)
	{
		playerWins();
	}
}

static void draw(void)
{
	drawBoard();

	drawButtons();

	drawTiles();
}

void initStage(void)
{
	boardPieceTexture = loadTexture("gfx/dark_brown.jpg");

	app.delegate.logic = logic;
	app.delegate.draw = draw;

	stage = Stage();
	// can't do linked list initialisation in constructor as stage is static
	stage.roundTail = &stage.roundHead;
	stage.tileTail = &stage.tileHead;
	stage.pieceTail = &stage.pieceHead;
	stage.buttonTail = &stage.buttonHead;

	initButtons();
	initColours();
	initRounds();
	initBoard();
	play();
}