#include "common.h"

Tile::Tile() :
	Entity(),
	rollingEffectHead(), rollingEffectTail(&rollingEffectHead) {}

Tile::Tile(const Tile& tile) : Tile()
{
	x = tile.x;
	y = tile.y;
	w = TILE_WIDTH;
	h = TILE_HEIGHT;
	dx = tile.dx;
	dy = tile.dy;
	tileType = tile.tileType;
	texture = tileType->texture;
	team = tileType->team;
	RollingEffect* effect;
	for (effect = tile.rollingEffectHead.next; effect != NULL; effect = effect->next)
	{
		rollingEffectTail->next = effect;
		rollingEffectTail = effect;
	}
}

Tile::Tile(int i, int j, TileType* TileType) : Tile()
{
	x = BOARD_SCREEN_OFFSET_X + (i + 0.5) * BOARDPIECE_WIDTH - 0.5 * TILE_WIDTH;
	y = BOARD_SCREEN_OFFSET_Y + (j + 0.5) * BOARDPIECE_HEIGHT - 0.5 * TILE_HEIGHT;
	w = TILE_WIDTH;
	h = TILE_HEIGHT;
	dx = dy = 0;
	tileType = TileType;
	texture = tileType->texture;
}


Tile::Tile(int i, int j, TileType* tileType, RollingEffect* effect) : Tile(i, j, tileType)
{
	rollingEffectTail->next = effect;
	rollingEffectTail = effect;
}

template<typename First, typename ... Effects>
Tile::Tile(int i, int j, TileType* tileType, First arg, const Effects&... rest) : Tile(i, j, tileType, rest...)
{
	rollingEffectTail->next = arg;
	rollingEffectTail = arg;
}