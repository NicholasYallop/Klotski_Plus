#include "common.h"

RollingEffect::RollingEffect() : next(0) {}

RollingEffect::RollingEffect(EFFECT_RETURN_FLAG(action)(Tile*, double&), double parameter) : RollingEffect()
{
	RollingEffect::action = action;
	RollingEffect::parameter = parameter;
}

EFFECT_RETURN_FLAG RollingEffect::realignLeft(Tile* tile, double& parameter) {
	if (parameter == 0)
	{
		tile->isRealigning = false;
		tile->dx = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	tile->isRealigning = true;
	if (parameter <= RECOIL_SPEED_X)
	{
		tile->dx = -int(parameter);
		parameter = 0;
		return EFFECT_RETURN_FLAG::NONE;
	}
	tile->dx = -RECOIL_SPEED_X;
	parameter -= RECOIL_SPEED_X;
	return EFFECT_RETURN_FLAG::NONE;
}

EFFECT_RETURN_FLAG RollingEffect::realignRight(Tile* tile, double& parameter) {
	if (parameter == 0)
	{
		tile->isRealigning = false;
		tile->dx = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	tile->isRealigning = true;
	if (parameter <= RECOIL_SPEED_X)
	{
		tile->dx = int(parameter);
		parameter = 0;
		return EFFECT_RETURN_FLAG::NONE;
	}
	tile->dx = RECOIL_SPEED_X;
	parameter -= RECOIL_SPEED_X;
	return EFFECT_RETURN_FLAG::NONE;
}

EFFECT_RETURN_FLAG RollingEffect::realignDown(Tile* tile, double& parameter) {
	if (parameter == 0)
	{
		tile->isRealigning = false;
		tile->dy = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	tile->isRealigning = true;
	if (parameter <= RECOIL_SPEED_Y)
	{
		tile->dy = int(parameter);
		parameter = 0;
		return EFFECT_RETURN_FLAG::NONE;
	}
	tile->dy = RECOIL_SPEED_Y;
	parameter -= RECOIL_SPEED_Y;
	return EFFECT_RETURN_FLAG::NONE;
}

EFFECT_RETURN_FLAG RollingEffect::realignUp(Tile* tile, double& parameter) {
	if (parameter == 0)
	{
		tile->isRealigning = false;
		tile->dy = 0;
		return EFFECT_RETURN_FLAG::END_EFFECT;
	}
	tile->isRealigning = true;
	if (parameter <= RECOIL_SPEED_Y)
	{
		tile->dy = -int(parameter);
		parameter = 0;
		return EFFECT_RETURN_FLAG::NONE;
	}
	tile->dy = -RECOIL_SPEED_Y;
	parameter -= RECOIL_SPEED_Y;
	return EFFECT_RETURN_FLAG::NONE;
}

INTERACTION_FLAG RollingEffect::doTileInteraction(Tile* tile1, Tile* tile2)
{
	return (tile1->tileType->tileInteraction(tile1, tile2));
}