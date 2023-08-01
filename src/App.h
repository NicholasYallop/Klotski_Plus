#pragma once
#include "common.h"

class App {
public:
	SDL_Renderer* renderer;
	SDL_Window* window;
	//Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
	int leftClickActive, rightClickActive;
	int leftClickHeld, rightClickHeld;

	Stage stage;

	App();

	void doTileClicks(int xMouse, int yMouse);
	void drawTiles();
	void drawBoard();
	void doButtonClicks(int xMouse, int yMouse);
	void drawButtons();
	void doClicks();
	void logic();
	void draw();
	void initStage();
};