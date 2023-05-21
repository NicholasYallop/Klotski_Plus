#include "common.h"

extern App app;

void prepareScene(void)
{
	SDL_SetRenderDrawColor(app.renderer, 245, 182, 118, 0.8);
	SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
	SDL_RenderPresent(app.renderer);
}

SDL_Texture *loadTexture(char *filename)
{
	SDL_Texture *texture;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	texture = IMG_LoadTexture(app.renderer, filename);

	return texture;
}

void blit(SDL_Texture *texture, int x, int y){
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blit(SDL_Texture *texture, int x, int y, int w, int h)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;

	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blitInBoard(SDL_Texture *texture, int x, int y)
{
	SDL_Rect dest;
	SDL_Rect source;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	if (x < BOARD_SCREEN_OFFSET_X)
	{
		source.x = BOARD_SCREEN_OFFSET_X;
		source.w = dest.w - source.x;
	}
	else if (x > BOARD_SCREEN_OFFSET_X + BOARDPIECE_COUNT_X * BOARDPIECE_WIDTH)
	{
		source.x = x;
		source.w = BOARD_SCREEN_OFFSET_X + BOARDPIECE_COUNT_X * BOARDPIECE_WIDTH - source.x;
	}
	else
	{
		source.x = x;
		source.w = dest.w;
	}

	if (y < BOARD_SCREEN_OFFSET_Y)
	{
		source.y = BOARD_SCREEN_OFFSET_Y;
		source.h = dest.h - source.y;
	}
	else if (y > BOARD_SCREEN_OFFSET_Y + BOARDPIECE_COUNT_Y * BOARDPIECE_HEIGHT)
	{
		source.y = y;
		source.h = BOARD_SCREEN_OFFSET_Y + BOARDPIECE_COUNT_Y * BOARDPIECE_HEIGHT - source.y;
	}
	else
	{
		source.y = y;
		source.h = dest.h;
	}

	SDL_RenderCopy(app.renderer, texture, &dest, &source);
}

void blitInBoard(SDL_Texture *texture, int x, int y, int w, int h)
{
	SDL_Rect dest;
	SDL_Rect source;

	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;

	if (dest.x < BOARD_SCREEN_OFFSET_X)
	{
		source.x = BOARD_SCREEN_OFFSET_X;
		source.w = dest.w - source.x;
	}
	else if (dest.x + dest.w > BOARD_SCREEN_OFFSET_X + BOARDPIECE_COUNT_X * BOARDPIECE_WIDTH)
	{
		source.x = x;
		source.w = BOARD_SCREEN_OFFSET_X + BOARDPIECE_COUNT_X * BOARDPIECE_WIDTH - source.x;
	}
	else
	{
		source.x = x;
		source.w = dest.w;
	}

	if (dest.y < BOARD_SCREEN_OFFSET_Y)
	{
		source.y = BOARD_SCREEN_OFFSET_Y;
		source.h = dest.h - source.y;
	}
	else if (dest.y + dest.h> BOARD_SCREEN_OFFSET_Y + BOARDPIECE_COUNT_Y * BOARDPIECE_HEIGHT)
	{
		source.y = y;
		source.h = BOARD_SCREEN_OFFSET_Y + BOARDPIECE_COUNT_Y * BOARDPIECE_HEIGHT - source.y;
	}
	else
	{
		source.y = y;
		source.h = dest.h;
	}

	SDL_RenderCopy(app.renderer, texture, &dest, &source);
}