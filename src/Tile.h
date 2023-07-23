#pragma once
#include "structs.h"

class Tile : public Entity {
public:
	TileType* tileType;
	RollingEffect rollingEffectHead, *rollingEffectTail;
	bool toDestroy = false, isRealigning = false;

	Tile();

	Tile(const Tile& tile);

	Tile(int i, int j, TileType* TileType);

	Tile(int i, int j, TileType* tileType, RollingEffect* effect);

	template<typename First, typename ... Effects>
	Tile(int i, int j, TileType* tileType, First arg, const Effects&... rest);
};