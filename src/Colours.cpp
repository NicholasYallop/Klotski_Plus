#pragma once
#include "common.h"

INTERACTION_FLAG Colours::GreyInteractions(Tile* callingTile, Tile* interactingTile)
{
	return INTERACTION_FLAG::NONE;
}

 void Colours::GreyClick(Tile* callingTile)
{
	return;
}

INTERACTION_FLAG Colours::BlueInteractions(Tile* callingTile, Tile* interactingTile)
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

void Colours::BlueClick(Tile* callingTile)
{
	callingTile->dx = CLICK_SPEED_X;
}

INTERACTION_FLAG Colours::RedInteractions(Tile* callingTile, Tile* interactingTile)
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

void Colours::RedClick(Tile* callingTile)
{
	callingTile->dy = CLICK_SPEED_Y;
}

INTERACTION_FLAG Colours::GreenInteractions(Tile* callingTile, Tile* interactingTile)
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

void Colours::GreenClick(Tile* callingTile)
{
	callingTile->dx = -CLICK_SPEED_X;
}

void Colours::initColours(void)
{
	greyTile = TileType();
	greyTile.team = 0;
	greyTile.texture = loadTexture("gfx/grey.png");
	greyTile.tileInteraction = Colours::GreyInteractions;
	greyTile.clickInteraction = Colours::GreyClick;

	blueTile = TileType();
	blueTile.team = 1;
	blueTile.texture = loadTexture("gfx/blue.png");
	blueTile.tileInteraction = Colours::BlueInteractions;
	blueTile.clickInteraction = Colours::BlueClick;

	redTile = TileType();
	redTile.team = 2;
	redTile.texture = loadTexture("gfx/red.png");
	redTile.tileInteraction = Colours::RedInteractions;
	redTile.clickInteraction = Colours::RedClick;

	greenTile = TileType();
	greenTile.team = 3;
	greenTile.texture = loadTexture("gfx/green.jpg");
	greenTile.tileInteraction = Colours::GreenInteractions;
	greenTile.clickInteraction = Colours::GreenClick;
}