#pragma once
#include "common.h"

App::App() : 
	renderer(), window(),
	keyboard(), 
	leftClickActive(0), rightClickActive(0),
	leftClickHeld(0), rightClickHeld(0),
	stage()
{};

void App::doTileClicks(int xMouse, int yMouse)
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

void App::drawTiles()
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

void App::drawBoard()
{
	BoardPiece* b;

	for (b = stage.pieceHead.next; b != NULL; b = b->next) {
		if (!b->h || !b->w) {
			blit(b->texture, b->x, b->y);
		}
		else {
			blit(b->texture, b->x, b->y, b->w, b->h);
		}
	}
}

void App::doButtonClicks(int xMouse, int yMouse)
{
	Entity* button;

	for (button = stage.buttonHead.next; button != NULL; button = button->next)
	{
		if (collision(xMouse, yMouse, CLICK_HEIGHT, CLICK_WIDTH, button->x, button->y, button->w, button->h))
		{
			(&stage->*(static_cast<Button*>(button))->Click)();
		}
	}
}

void App::drawButtons()
{
	Entity* button;

	for (button = stage.buttonHead.next; button != NULL; button = button->next)
	{
		blit(button->texture, button->x, button->y, button->w, button->h);
	}
}

void App::doClicks()
{
	if (leftClickActive && !leftClickHeld)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);

		doTileClicks(x, y);

		doButtonClicks(x, y);
	}
}

void App::logic()
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

void App::draw()
{
	drawBoard();

	drawButtons();

	drawTiles();
}

void App::initStage()
{
	//delegate.logic = logic;
	//delegate.draw = draw;

	BoardPiece::initBoardPieces();
	Colours::initColours();
	stage.initButtons();
	stage.initRounds();
	stage.play();
}