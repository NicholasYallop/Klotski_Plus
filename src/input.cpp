#include "common.h"

extern App app;

void doKeyUp(SDL_KeyboardEvent *event)
{
	if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		app.keyboard[event->keysym.scancode] = 0;
	}
}

void doKeyDown(SDL_KeyboardEvent *event)
{
	if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		app.keyboard[event->keysym.scancode] = 1;
	}
}

void doMouseDown(SDL_MouseButtonEvent *event)
{
	if (event->button == SDL_BUTTON_LEFT)
	{
		app.leftClickActive = 1;
	}
	if (event->button == SDL_BUTTON_RIGHT)
	{
		app.rightClickActive = 1;
	}
}

void doMouseUp(SDL_MouseButtonEvent *event)
{
	if (event->button == SDL_BUTTON_LEFT)
	{
		app.leftClickActive = 0;
		app.leftClickHeld = 0;
	}
	if (event->button == SDL_BUTTON_RIGHT)
	{
		app.rightClickActive = 0;
		app.rightClickHeld = 0;
	}
}

void doInput(void)
{
	SDL_Event event;

	bool mouseDown = false;
	if (app.leftClickActive)
	{
		app.leftClickHeld = 1;
	}
	if (app.rightClickActive)
	{
		app.rightClickHeld = 1;
	}

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				exit(0);
				break;

			case SDL_KEYDOWN:
				doKeyDown(&event.key);
				break;

			case SDL_KEYUP:
				doKeyUp(&event.key);
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouseDown = true;
				doMouseDown(&event.button);

			case SDL_MOUSEBUTTONUP:
				if (!mouseDown)
				{
					doMouseUp(&event.button);
				}
			
			default:
				break;
		}
	}
}