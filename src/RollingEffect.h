#pragma once
#include "common.h"

struct RollingEffect
{
	RollingEffect* next;
	double parameter;
	EFFECT_RETURN_FLAG(*action)(Tile* tile, double& parameter);

	RollingEffect();
	RollingEffect(EFFECT_RETURN_FLAG(action)(Tile*, double&), double parameter);

	static EFFECT_RETURN_FLAG realignLeft(Tile* tile, double& parameter);
	static EFFECT_RETURN_FLAG realignRight(Tile* tile, double& parameter);
	static EFFECT_RETURN_FLAG realignDown(Tile* tile, double& parameter);
	static EFFECT_RETURN_FLAG realignUp(Tile* tile, double& parameter);
	static INTERACTION_FLAG doTileInteraction(Tile* tile1, Tile* tile2);
};