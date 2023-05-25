#include "common.h"

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (std::max(x1, x2) < std::min(x1 + w1, x2 + w2)) && (std::max(y1, y2) < std::min(y1 + h1, y2 + h2));
}

INTERACTION_FLAG bounce(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int xDiff = x1 - x2;
	int yDiff = y1 - y2;
	if(0<xDiff && xDiff<w2)
	{
		return INTERACTION_FLAG::BOUNCE_RIGHT;
	}
	if(0>xDiff && xDiff>-w1)
	{
		return INTERACTION_FLAG::BOUNCE_LEFT;
	}
	if(0<yDiff && yDiff<h2)
	{
		return INTERACTION_FLAG::BOUNCE_DOWN;
	}
	if(0>yDiff && yDiff>-h1)
	{
		return INTERACTION_FLAG::BOUNCE_UP;
	}
	return INTERACTION_FLAG::NONE;
}