#pragma once
#include "common.h"
extern TileType blueTile;
extern TileType redTile;
extern TileType greenTile;
extern TileType greyTile;

class Colours {
public:
	static INTERACTION_FLAG GreyInteractions(Tile* callingTile, Tile* interactingTile);
	static void GreyClick(Tile* callingTile);
	static INTERACTION_FLAG BlueInteractions(Tile* callingTile, Tile* interactingTile);
	static void BlueClick(Tile* callingTile);
	static INTERACTION_FLAG RedInteractions(Tile* callingTile, Tile* interactingTile);
	static void RedClick(Tile* callingTile);
	static INTERACTION_FLAG GreenInteractions(Tile* callingTile, Tile* interactingTile);
	static void GreenClick(Tile* callingTile);
	static void initColours(void);
};