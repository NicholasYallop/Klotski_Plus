#include "common.h"

extern App app;
extern bool Quit;
static Stage stage;
static Round *currentRound;
static SDL_Texture *boardPieceTexture;
static TileType blueTile;
static TileType redTile;
static TileType greenTile;
static TileType greyTile;
static Tile tileQueueHead, *tileQueueTail = &tileQueueHead;
static BoardPiece pieceQueueHead, *pieceQueueTail=&pieceQueueHead;

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

static void queueTileSpawnToGrid(int i, int j, TileType *tileType)
{
	Tile *newTile = new Tile(i, j, tileType);

	tileQueueTail->next = newTile;
	tileQueueTail = newTile;
}

static void queueTileSpawn(Tile *tile)
{
	Tile *deepCopy = new Tile(*tile);
	tileQueueTail->next = deepCopy;
	tileQueueTail = deepCopy;
}

static void spawnQueueTiles()
{
	Entity *tile;

	while(tileQueueHead.next)
	{
		tile = tileQueueHead.next;
		spawnTile(static_cast<Tile*>(tile));
		tileQueueHead.next = tileQueueHead.next->next;
		free(tile);
	}
	tileQueueTail = &tileQueueHead;
}

static EFFECT_RETURN_FLAG realignLeft(Tile *tile, double& parameter){
	if (parameter==0)
	{
		tile->isRealigning = false;
		tile->dx = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	tile->isRealigning = true;
	if (parameter<=RECOIL_SPEED_X)
	{
		tile->dx = - int(parameter);
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
		tile->isRealigning = false;
		tile->dx = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	tile->isRealigning = true;
	if (parameter<=RECOIL_SPEED_X)
	{
		tile->dx = int(parameter);
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
		tile->isRealigning = false;
		tile->dy = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	tile->isRealigning = true;
	if (parameter<=RECOIL_SPEED_Y)
	{
		tile->dy = int(parameter);
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
		tile->isRealigning = false;
		tile->dy = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	tile->isRealigning = true;
	if (parameter<=RECOIL_SPEED_Y)
	{
		tile->dy = -int(parameter);
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
	i = floor((x-BOARD_SCREEN_OFFSET_X)/BOARDPIECE_WIDTH);
	j = floor((y-BOARD_SCREEN_OFFSET_Y)/BOARDPIECE_HEIGHT);
}

static void queueTileSpawnFromCollision(Tile *tile1, Tile *tile2, TileType *tileType)
{
	int contactX= std::max(tile1->x, tile2->x);
	int contactY= std::max(tile1->y, tile2->y);

	int i=0, j=0;
	containingTile(contactX, contactY, i, j);
	queueTileSpawnToGrid(i, j, tileType);
}

static void doTileCollisions()
{
	Tile *tile;
	Tile *comparisonTile;

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
				queueTileSpawnFromCollision(tile, comparisonTile, &greyTile);
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::SPAWN_GREEN_TILE)){
				Tile *green = new Tile(0, 0, &greenTile);
				if (static_cast<int>(flags & INTERACTION_FLAG::DESTROY_TILE1))
				{
					green->x = int(tile->x);
					green->y = int(tile->y);
					queueTileSpawn(green);
				}
				if (static_cast<int>(flags & INTERACTION_FLAG::DESTROY_TILE2))
				{
					green->x = int(comparisonTile->x);
					green->y = int(comparisonTile->y);
					queueTileSpawn(green);
				}
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::SPAWN_RED_TILE)){
				queueTileSpawnFromCollision(tile, comparisonTile, &redTile);
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_RIGHT_TILE1))
			{
				RollingEffect *effect = new RollingEffect(realignRight, tileRealignX - tile->x);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_LEFT_TILE1))
			{
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
				RollingEffect *effect = new RollingEffect(realignRight, comparisonTileRealignX - comparisonTile->x);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_LEFT_TILE2))
			{
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

	Tile *prev = &stage.tileHead; 
	for (tile = static_cast<Tile*>(stage.tileHead.next); tile != NULL; tile = static_cast<Tile*>(tile->next))
	{
		if (tile->toDestroy)
		{
			if (tile == stage.tileTail)
			{
				stage.tileTail = prev;
			}
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
			if (tile == stage.tileTail)
			{
				stage.tileTail = static_cast<Tile*>(prev);
			}
			prev->next = tile->next;

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
				if (effect == tile->rollingEffectHead.next)
				{
					tile->rollingEffectHead.next = effect->next;
				}
				if (effect == tile->rollingEffectTail)
				{
					tile->rollingEffectTail = prev;
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

static void addTilesToRound(Round* round, Tile* tile)
{
	round->tileTail->next = tile;
	round->tileTail = tile;
}

template<typename First, typename ... Tiles>
static void addTilesToRound(Round* round, First arg, const Tiles&... rest)
{
	round->tileTail->next = arg;
	round->tileTail = arg;
	addTilesToRound(round, rest...);
}

static void EmptyTiles(void) {
	Entity* tile;

	while (stage.tileHead.next)
	{
		tile = stage.tileHead.next;
		stage.tileHead.next = stage.tileHead.next->next;
		free(tile);
	}
	stage.tileTail = &stage.tileHead;
}

static void spawnRoundTiles(void)
{
	Tile* tile;
	for (tile = static_cast<Tile*>(currentRound->tileHead.next);
		tile != NULL;
		tile = static_cast<Tile*>(tile->next))
	{
		queueTileSpawn(tile);
	}
}

static void doTileClicks(int xMouse, int yMouse)
{
	Tile* tile;

	for (tile = static_cast<Tile*>(stage.tileHead.next); tile != NULL; tile = static_cast<Tile*>(tile->next))
	{
		if (!tile->isRealigning
			&& collision(xMouse, yMouse, CLICK_HEIGHT, CLICK_WIDTH, tile->x, tile->y, tile->w, tile->h))
		{
			tile->tileType->clickInteraction(tile);
		}
	}
}

static void drawTiles(void)
{
	Entity* e;

	for (e = stage.tileHead.next; e != NULL; e = e->next) {
		if (!e->h || !e->w) {
			blitInBoard(e->texture, e->x, e->y);
		}
		else {
			blitInBoard(e->texture, e->x, e->y, e->w, e->h);
		}
	}
}

#pragma endregion tiles

#pragma region boardPieces

static void spawnBoardPiece(int x, int y)
{
	BoardPiece* newPiece = new BoardPiece();

	newPiece->x = x;
	newPiece->y = y;
	newPiece->w = BOARDPIECE_WIDTH;
	newPiece->h = BOARDPIECE_HEIGHT;
	newPiece->texture = boardPieceTexture;

	stage.pieceTail->next = newPiece;
	stage.pieceTail = newPiece;
}

static void spawnBoardPiece(BoardPiece *piece)
{
	BoardPiece* newPiece = new BoardPiece(*piece);

	stage.pieceTail->next = newPiece;
	stage.pieceTail = newPiece;
}

static void doBoardPieces() {
	Entity* tile;
	BoardPiece* boardpiece;

	for (tile = stage.tileHead.next; tile != NULL; tile = tile->next) {
		for (boardpiece = stage.pieceHead.next; boardpiece != NULL; boardpiece = boardpiece->next) {
			if ((std::abs((tile->x - boardpiece->x) - (BOARDPIECE_WIDTH - TILE_WIDTH)) < BOARDPIECE_TILE_INTERACTION_PRECISION) &&
				(std::abs((tile->y - boardpiece->y) - (BOARDPIECE_HEIGHT - TILE_HEIGHT)) < BOARDPIECE_TILE_INTERACTION_PRECISION))
			{
				boardpiece->DoAction(static_cast<Tile*>(tile));
			}
		}
	}
}

static void EmptyBoardPieces(void) {
	BoardPiece* boardpiece;

	while (stage.pieceHead.next)
	{
		boardpiece = stage.pieceHead.next;
		stage.pieceHead.next = stage.pieceHead.next->next;
		free(boardpiece);
	}
	stage.pieceTail = &stage.pieceHead;
}

static void spawnRoundBoardPieces(void) {
	BoardPiece* boardpiece;

	for (boardpiece = currentRound->pieceHead.next; boardpiece != NULL; boardpiece = boardpiece->next) {
		spawnBoardPiece(boardpiece);
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

static void BoardPieceInteraction_Blank(Tile* tile) {
	return;
}

static void BoardPieceInteraction_FlipVelocity(Tile* tile) {
	int dx = tile->dx;
	tile->dx = tile->dy;
	tile->dy = dx;
}

static void addBoardPieceToRound(Round* round, BoardPiece* piece)
{
	round->pieceTail->next = piece;
	round->pieceTail = piece;
}

template<typename First, typename ... Tiles>
static void addBoardPieceToRound(Round* round, First arg, const Tiles&... rest)
{
	round->pieceTail->next = arg;
	round->pieceTail = arg;
	addTilesToRound(round, rest...);
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

static void AddDefaultBoardToRound(Round *round) {
	int i, j;
	BoardPiece* piece;

	for (i = 0; i < BOARDPIECE_COUNT_X; i += 1)
	{
		for (j = 0; j < BOARDPIECE_COUNT_Y; j += 1)
		{
			piece = new BoardPiece();
			piece->x = BOARD_SCREEN_OFFSET_X + i * BOARDPIECE_WIDTH;
			piece->y = BOARD_SCREEN_OFFSET_Y + j * BOARDPIECE_HEIGHT;
			piece->w = BOARDPIECE_WIDTH;
			piece->h = BOARDPIECE_HEIGHT;
			piece->texture = boardPieceTexture;
			piece->DoAction = BoardPieceInteraction_Blank;
			round->pieceTail->next = piece;
			round->pieceTail = piece;
		}
	}
}

static void initRounds(void)
{
	Round *round0 = new Round();
	addTilesToRound(
		round0,
		new Tile(1, 1, &blueTile),
		new Tile(0, 1, &blueTile)
	);
	AddDefaultBoardToRound(round0);
	BoardPiece *piece;
	for (piece = round0->pieceHead.next; piece != NULL; piece = piece->next) {
		if (((piece->x - BOARD_SCREEN_OFFSET_X) / BOARDPIECE_WIDTH == 2) &&
			((piece->y - BOARD_SCREEN_OFFSET_Y) / BOARDPIECE_HEIGHT == 1)) {
			piece->texture = greenTile.texture;
			piece->DoAction = BoardPieceInteraction_FlipVelocity;
		}
	}

	Round *round1 = new Round();
	addTilesToRound(
		round1,
		new Tile(0, 0, &blueTile),
		new Tile(1, 0, &redTile),
		new Tile(2, 0, &redTile),
		new Tile(3, 0, &redTile),
		new Tile(1, 1, &blueTile),
		new Tile(2, 1, &blueTile)
	);
	AddDefaultBoardToRound(round1);

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
	AddDefaultBoardToRound(round2);

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
	AddDefaultBoardToRound(round3);

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
	AddDefaultBoardToRound(round4);

	Round *round5 = new Round();
	addTilesToRound(round5, 
		new Tile(0, 0, &redTile),
		new Tile(0, 1, &blueTile),
		new Tile(1, 1, &blueTile),
		new Tile(2, 1, &blueTile),
		new Tile(3, 1, &greenTile),
		new Tile(4, 1, &greenTile));
	AddDefaultBoardToRound(round5);

	Round *round6 = new Round();
	addTilesToRound(round6,
		new Tile(5, 1, &redTile),
		new Tile(6, 1, &redTile),
		new Tile(5, 2, &blueTile),
		new Tile(6, 2, &greenTile),
		new Tile(5, 3, &blueTile),
		new Tile(6, 3, &blueTile));
	AddDefaultBoardToRound(round6);

	Round *round7 = new Round();
	addTilesToRound(round7,
		new Tile(5, 1, &redTile),
		new Tile(6, 1, &redTile),
		new Tile(4, 2, &blueTile),
		new Tile(5, 2, &greenTile),
		new Tile(6, 2, &blueTile),
		new Tile(5, 3, &redTile),
		new Tile(6, 3, &blueTile),
		new Tile(5, 4, &blueTile),
		new Tile(6, 4, &greenTile));
	AddDefaultBoardToRound(round7);

	addRoundsToStage(round0, round1, round2, round3, round4, round5, round6, round7);
}

static void resetRound()
{
	EmptyBoardPieces();
	EmptyTiles();

	spawnRoundBoardPieces();
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
	if (app.leftClickActive && !app.leftClickHeld)
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
		EmptyBoardPieces();
		spawnRoundBoardPieces();
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

static INTERACTION_FLAG GreyInteractions(Tile *callingTile, Tile *interactingTile)
{
	return INTERACTION_FLAG::NONE;
}

static void GreyClick(Tile *callingTile)
{
	return;
}

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
	if (interactingTile->tileType->team == greenTile.team)
	{
		if (collided)
		{
			return INTERACTION_FLAG::DESTROY_TILE1 | INTERACTION_FLAG::SPAWN_GREEN_TILE
				| bounce(callingTile->x, callingTile->y, callingTile->w, callingTile->h,
					interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);
		}
	}

	return INTERACTION_FLAG::NONE;
}

static void BlueClick(Tile *callingTile)
{
	callingTile->dx = CLICK_SPEED_X;
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
			return INTERACTION_FLAG::DESTROY_TILE1 | INTERACTION_FLAG::DESTROY_TILE2 | INTERACTION_FLAG::SPAWN_GREY_TILE;
		}
	}
	if (interactingTile->tileType->team == greenTile.team)
	{
		if (collided)
		{
			return INTERACTION_FLAG::DESTROY_TILE1 | INTERACTION_FLAG::DESTROY_TILE2 | INTERACTION_FLAG::SPAWN_RED_TILE;
		}
	}

	return INTERACTION_FLAG::NONE;
}

static void RedClick(Tile *callingTile)
{
	callingTile->dy = CLICK_SPEED_Y;
}

static INTERACTION_FLAG GreenInteractions(Tile *callingTile, Tile *interactingTile)
{
	int collided = collision(callingTile->x, callingTile->y, callingTile->w, callingTile->h,
					interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);

	if (interactingTile->tileType->team == greenTile.team)
	{
		if (collided)
		{
			return bounce(callingTile->x, callingTile->y, callingTile->w, callingTile->h,
					interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);
		}
	}
	if (interactingTile->tileType->team == blueTile.team)
	{
		if (collided)
		{
			return INTERACTION_FLAG::DESTROY_TILE2 | INTERACTION_FLAG::SPAWN_GREEN_TILE
				| bounce(callingTile->x, callingTile->y, callingTile->w, callingTile->h,
					interactingTile->x, interactingTile->y, interactingTile->w, interactingTile->h);
		}
	}
	if (interactingTile->tileType->team == redTile.team)
	{
		if (collided)
		{
			return INTERACTION_FLAG::DESTROY_TILE1 | INTERACTION_FLAG::DESTROY_TILE2 | INTERACTION_FLAG::SPAWN_RED_TILE;
		}
	}
	return INTERACTION_FLAG::NONE;
}

static void GreenClick(Tile *callingTile)
{
	callingTile->dx = -CLICK_SPEED_X;
}

static void initColours(void)
{
	greyTile = TileType();
	greyTile.team = 0;
	greyTile.texture = loadTexture("gfx/grey.png");
	greyTile.tileInteraction = GreyInteractions;
	greyTile.clickInteraction = GreyClick;

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

	greenTile = TileType();
	greenTile.team = 3;
	greenTile.texture = loadTexture("gfx/green.jpg");
	greenTile.tileInteraction = GreenInteractions;
	greenTile.clickInteraction = GreenClick;
}

#pragma endregion Colours

static void logic(void)
{
	doClicks();

	doTiles();

	doBoardPieces();

	doTileCollisions();

	spawnQueueTiles();

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
	// don't think we can do linked list initialisation in constructor as stage is static
	stage.roundTail = &stage.roundHead;
	stage.tileTail = &stage.tileHead;
	stage.pieceTail = &stage.pieceHead;
	stage.buttonTail = &stage.buttonHead;

	initButtons();
	initColours();
	initRounds();
	play();
}