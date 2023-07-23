#pragma once
#include "common.h"

template<typename First, typename ... Tiles>
void Round::addTiles(First arg, const Tiles&... rest)
{
	tileTail->next = arg;
	tileTail = arg;
	addTiles(rest...);
}

template<typename First, typename ... Tiles>
static void addBoardPiece(First arg, const Tiles&... rest)
{
	pieceTail->next = arg;
	pieceTail = arg;
	addBoardPiece(rest...);
}