#include "common.h"

extern App app;
extern bool Quit;
static Stage stage;
static Round *currentRound;
SDL_Texture* boardPieceTexture;
TileType blueTile;
TileType redTile;
TileType greenTile;
TileType greyTile;
static Tile tileQueueHead, *tileQueueTail = &tileQueueHead;
static BoardPiece pieceQueueHead, *pieceQueueTail=&pieceQueueHead;

#pragma region tiles

void Stage::spawnTile(Tile *tile)
{
	Tile *deepCopy = new Tile(*tile);
	tileTail->next = deepCopy;
	tileTail = deepCopy;
}

void Stage::spawnTileToGrid(int i, int j, TileType *tileType)
{
	Tile *newTile = new Tile(i, j, tileType);

	tileTail->next = newTile;
	tileTail = newTile;
}

void Stage::queueTileSpawnToGrid(int i, int j, TileType* tileType)
{
	Tile *newTile = new Tile(i, j, tileType);

	tileQueueTail->next = newTile;
	tileQueueTail = newTile;
}

void Stage::queueTileSpawn(Tile *tile)
{
	Tile *deepCopy = new Tile(*tile);
	tileQueueTail->next = deepCopy;
	tileQueueTail = deepCopy;
}

void Stage::spawnQueueTiles()
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

void Stage::queueTileSpawnFromCollision(Tile *tile1, Tile *tile2, TileType *tileType)
{
	int contactX= std::max(tile1->x, tile2->x);
	int contactY= std::max(tile1->y, tile2->y);

	int i=0, j=0;
	BoardPiece::containingBoardpiece(contactX, contactY, i, j);
	queueTileSpawnToGrid(i, j, tileType);
}

void Stage::doTileCollisions()
{
	Tile *tile;
	Tile *comparisonTile;

	for (tile = static_cast<Tile*>(tileHead.next); tile != NULL; tile = static_cast<Tile*>(tile->next))
	{
		for (comparisonTile = static_cast<Tile*>(tile->next); comparisonTile != NULL; comparisonTile = static_cast<Tile*>(comparisonTile->next))
		{
			INTERACTION_FLAG flags;

			flags = RollingEffect::doTileInteraction(tile, comparisonTile);
			int tileRealignX, tileRealignY;
			int i, j;
			BoardPiece::containingBoardpiece(tile->x + tile->w/2, tile->y + tile->h/2, i, j);
			tileRealignX = BOARD_SCREEN_OFFSET_X + BOARDPIECE_WIDTH*(i+0.5) - 0.5*TILE_WIDTH;
			tileRealignY = BOARD_SCREEN_OFFSET_Y + BOARDPIECE_HEIGHT*(j+0.5) - 0.5*TILE_HEIGHT;
			int comparisonTileRealignX, comparisonTileRealignY;
			BoardPiece::containingBoardpiece(comparisonTile->x + comparisonTile->w/2, comparisonTile->y + comparisonTile->h/2, i, j);
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
				RollingEffect *effect = new RollingEffect(RollingEffect::realignRight, tileRealignX - tile->x);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_LEFT_TILE1))
			{
				RollingEffect *effect = new RollingEffect(RollingEffect::realignLeft, tile->x - tileRealignX);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_UP_TILE1))
			{
				RollingEffect *effect = new RollingEffect(RollingEffect::realignUp, tile->y - tileRealignY);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_DOWN_TILE1))
			{
				RollingEffect *effect = new RollingEffect(RollingEffect::realignDown, tileRealignY - tile->y);
				tile->rollingEffectTail->next = effect;
				tile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_RIGHT_TILE2))
			{
				RollingEffect *effect = new RollingEffect(RollingEffect::realignRight, comparisonTileRealignX - comparisonTile->x);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_LEFT_TILE2))
			{
				RollingEffect *effect = new RollingEffect(RollingEffect::realignLeft, comparisonTile->x - comparisonTileRealignX);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_UP_TILE2))
			{
				RollingEffect *effect = new RollingEffect(RollingEffect::realignUp, comparisonTile->y - comparisonTileRealignY);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}
			if (static_cast<int>(flags & INTERACTION_FLAG::BOUNCE_DOWN_TILE2))
			{
				RollingEffect *effect = new RollingEffect(RollingEffect::realignDown, comparisonTileRealignY - comparisonTile->y);
				comparisonTile->rollingEffectTail->next = effect;
				comparisonTile->rollingEffectTail = effect;
			}		
		}
	}

	Tile *prev = &tileHead; 
	for (tile = static_cast<Tile*>(tileHead.next); tile != NULL; tile = static_cast<Tile*>(tile->next))
	{
		if (tile->toDestroy)
		{
			if (tile == tileTail)
			{
				tileTail = prev;
			}
			if (tile == tileTail)
			{
				tileTail = static_cast<Tile*>(prev);
			}

			prev->next = tile->next;

			free(tile);

			tile = prev;
		}

		prev = tile;
	}
}

void Stage::destroyOutOfBoundsTiles()
{
	Entity *tile;
	Entity *prev = &tileHead;

	for (tile = tileHead.next; tile != NULL; tile = tile->next)
	{
		if (tile->isOutsideBoard())
		{
			if (tile == tileHead.next)
			{
				tileHead.next = tile->next;
			}
			if (tile == tileTail)
			{
				tileTail = static_cast<Tile*>(prev);
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

void Stage::doRollingEffects()
{
	Tile *tile;
	for(tile=static_cast<Tile*>(tileHead.next)
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

void Stage::doTiles()
{
	destroyOutOfBoundsTiles();
	doRollingEffects();
}

void Stage::EmptyTiles() {
	Entity* tile;

	while (tileHead.next)
	{
		tile = tileHead.next;
		tileHead.next = tileHead.next->next;
		free(tile);
	}
	tileTail = &tileHead;
}

void Stage::spawnRoundTiles()
{
	Tile* tile;
	for (tile = static_cast<Tile*>(currentRound->tileHead.next);
		tile != NULL;
		tile = static_cast<Tile*>(tile->next))
	{
		queueTileSpawn(tile);
	}
}

// this belongs in app
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

//this belongs in app
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

void Stage::spawnBoardPiece(int x, int y)
{
	BoardPiece* newPiece = new BoardPiece();

	newPiece->x = x;
	newPiece->y = y;
	newPiece->w = BOARDPIECE_WIDTH;
	newPiece->h = BOARDPIECE_HEIGHT;
	newPiece->texture = boardPieceTexture;

	pieceTail->next = newPiece;
	pieceTail = newPiece;
}

void Stage::spawnBoardPiece(BoardPiece *piece)
{
	BoardPiece* newPiece = new BoardPiece(*piece);

	pieceTail->next = newPiece;
	pieceTail = newPiece;
}

void Stage::doBoardPieces() {
	Entity* tile;
	BoardPiece* boardpiece;

	for (tile = tileHead.next; tile != NULL; tile = tile->next) {
		for (boardpiece = pieceHead.next; boardpiece != NULL; boardpiece = boardpiece->next) {
			if ((std::abs((tile->x - boardpiece->x) - (BOARDPIECE_WIDTH - TILE_WIDTH)) < BOARDPIECE_TILE_INTERACTION_PRECISION) &&
				(std::abs((tile->y - boardpiece->y) - (BOARDPIECE_HEIGHT - TILE_HEIGHT)) < BOARDPIECE_TILE_INTERACTION_PRECISION))
			{
				boardpiece->DoAction(static_cast<Tile*>(tile));
			}
		}
	}
}

void Stage::EmptyBoardPieces() {
	BoardPiece* boardpiece;

	while (pieceHead.next)
	{
		boardpiece = pieceHead.next;
		pieceHead.next = pieceHead.next->next;
		free(boardpiece);
	}
	pieceTail = &pieceHead;
}

void Stage::spawnRoundBoardPieces() {
	BoardPiece* boardpiece;

	for (boardpiece = currentRound->pieceHead.next; boardpiece != NULL; boardpiece = boardpiece->next) {
		spawnBoardPiece(boardpiece);
	}
}

void Stage::drawBoard(void)
{
	BoardPiece *b;

	for (b=pieceHead.next; b != NULL; b = b->next){
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

void Stage::addRounds(Round* round)
{
	roundTail->next = round;
	roundTail = round;
}

void Stage::initRounds(void)
{
	Round *round0 = new Round();
	round0->addTiles(
		new Tile(1, 1, &blueTile),
		new Tile(0, 1, &blueTile)
	);
	round0->AddDefaultBoard();
	BoardPiece *piece;
	for (piece = round0->pieceHead.next; piece != NULL; piece = piece->next) {
		if (((piece->x - BOARD_SCREEN_OFFSET_X) / BOARDPIECE_WIDTH == 2) &&
			((piece->y - BOARD_SCREEN_OFFSET_Y) / BOARDPIECE_HEIGHT == 1)) {
			piece->texture = greenTile.texture;
			piece->DoAction = BoardPiece::BoardPieceInteraction_FlipVelocity;
		}
	}

	Round *round1 = new Round();
	round1->addTiles(
		new Tile(0, 0, &blueTile),
		new Tile(1, 0, &redTile),
		new Tile(2, 0, &redTile),
		new Tile(3, 0, &redTile),
		new Tile(1, 1, &blueTile),
		new Tile(2, 1, &blueTile)
	);
	round1->AddDefaultBoard();

	Round *round2 = new Round();
	round2->addTiles(
		new Tile(7, 1, &redTile),
		new Tile(5, 2, &blueTile),
		new Tile(6, 2, &redTile),
		new Tile(7, 2, &blueTile),
		new Tile(8, 2, &redTile),
		new Tile(6, 3, &blueTile),
		new Tile(7, 3, &redTile)
	);
	round2->AddDefaultBoard();

	Round *round3 = new Round();
	round3->addTiles(
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
	round3->AddDefaultBoard();

	Round *round4 = new Round();
	round4->addTiles(
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
	round4->AddDefaultBoard();

	Round *round5 = new Round();
	round5->addTiles( 
		new Tile(0, 0, &redTile),
		new Tile(0, 1, &blueTile),
		new Tile(1, 1, &blueTile),
		new Tile(2, 1, &blueTile),
		new Tile(3, 1, &greenTile),
		new Tile(4, 1, &greenTile));
	round5->AddDefaultBoard();

	Round *round6 = new Round();
	round6->addTiles(
		new Tile(5, 1, &redTile),
		new Tile(6, 1, &redTile),
		new Tile(5, 2, &blueTile),
		new Tile(6, 2, &greenTile),
		new Tile(5, 3, &blueTile),
		new Tile(6, 3, &blueTile));
	round6->AddDefaultBoard();

	Round *round7 = new Round();
	round7->addTiles(
		new Tile(5, 1, &redTile),
		new Tile(6, 1, &redTile),
		new Tile(4, 2, &blueTile),
		new Tile(5, 2, &greenTile),
		new Tile(6, 2, &blueTile),
		new Tile(5, 3, &redTile),
		new Tile(6, 3, &blueTile),
		new Tile(5, 4, &blueTile),
		new Tile(6, 4, &greenTile));
	round7->AddDefaultBoard();

	addRounds(round0, round1, round2, round3, round4, round5, round6, round7);
}

void Stage::resetRound()
{
	EmptyBoardPieces();
	EmptyTiles();

	spawnRoundBoardPieces();
	spawnRoundTiles();
}

#pragma endregion Rounds

#pragma region Buttons

// belongs in app
static void doButtonClicks(int xMouse, int yMouse)
{
	Entity *button;

		for (button = stage.buttonHead.next; button != NULL; button=button->next)
		{
			if (collision(xMouse, yMouse, CLICK_HEIGHT, CLICK_WIDTH, button->x, button->y, button->w, button->h))
			{
				(&stage->*(static_cast<Button*>(button))->Click)();
			}
		}
}

// belongs in app
static void drawButtons(void)
{
	Entity *button;

	for(button = stage.buttonHead.next; button != NULL; button=button->next)
	{
		blit(button->texture, button->x, button->y, button->w, button->h);
	}
}

void Stage::quit()
{
	Quit = true;
}

void Stage::initButtons()
{	
	Button *quitButton = new Button();
	quitButton->x = SCREEN_WIDTH - 35;
	quitButton->y = 10;
	quitButton->w = 15;
	quitButton->h = 15;
	quitButton->texture = loadTexture("gfx/red_cross.png");
	quitButton->Click = &Stage::quit;

	buttonTail->next = quitButton;
	buttonTail = quitButton;

	Button *resetButton = new Button();
	resetButton->x = 10;
	resetButton->y = 10;
	resetButton->w = 40;
	resetButton->h = 40;
	resetButton->texture = loadTexture("gfx/restart.png");
	resetButton->Click = &Stage::resetRound;

	buttonTail->next = resetButton;
	buttonTail = resetButton;
}

#pragma endregion Buttons

#pragma region Play

// belongs in app
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

void Stage::play()
{
	currentRound = &roundHead;
}

void Stage::playerWins()
{
	if (currentRound->next != NULL)
	{
		currentRound = currentRound->next;
		tileTail = &tileHead;
		EmptyBoardPieces();
		spawnRoundBoardPieces();
		spawnRoundTiles();
	}
	else
	{
		Quit=true;
	}
}

#pragma endregion Play

#pragma region Colours

#pragma endregion Colours

// belongs in app
static void logic(void)
{
	doClicks();

	stage.doTiles();

	stage.doBoardPieces();

	stage.doTileCollisions();

	stage.spawnQueueTiles();

	if (stage.tileHead.next == NULL)
	{
		stage.playerWins();
	}
}

// belongs in app
static void draw(void)
{
	stage.drawBoard();

	drawButtons();

	drawTiles();
}

// mostly just a stage constructor
void initStage()
{
	boardPieceTexture = loadTexture("gfx/dark_brown.jpg");

	app.delegate.logic = logic;
	app.delegate.draw = draw;

	stage = Stage();
	stage.roundTail = &stage.roundHead;
	stage.tileTail = &stage.tileHead;
	stage.pieceTail = &stage.pieceHead;
	stage.buttonTail = &stage.buttonHead;

	stage.initButtons();
	Colours::initColours();
	stage.initRounds();
	stage.play();
}