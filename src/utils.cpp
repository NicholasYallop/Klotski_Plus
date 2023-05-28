#include "common.h"

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	bool test = std::max(x1, x2) < std::min(x1 + w1, x2 + w2) && (std::max(y1, y2) < std::min(y1 + h1, y2 + h2));
	if (test)
	{
		printf("collision | x1=%i ; y1=%i; x2=%i ; y2=%i\n", x1, y1, x2, y2);
		return true;
	}
	return false;
}

INTERACTION_FLAG bounce(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int xDiff = x1 - x2;
	int yDiff = y1 - y2;
	if(0<xDiff && xDiff<=w1)
	{
		return INTERACTION_FLAG::BOUNCE_RIGHT_TILE1 | INTERACTION_FLAG::BOUNCE_LEFT_TILE2;
	}
	if(0>xDiff && xDiff>=-w2)
	{
		return INTERACTION_FLAG::BOUNCE_LEFT_TILE1 | INTERACTION_FLAG::BOUNCE_RIGHT_TILE2;
	}
	if(0<yDiff && yDiff<=h1)
	{
		return INTERACTION_FLAG::BOUNCE_DOWN_TILE1 | INTERACTION_FLAG::BOUNCE_UP_TILE2;
	}
	if(0>yDiff && yDiff>=-h2)
	{
		return INTERACTION_FLAG::BOUNCE_UP_TILE1 | INTERACTION_FLAG::BOUNCE_DOWN_TILE2;
	}
	printf("no bounce return | xDiff=%i ; yDiff=%i\n", xDiff, yDiff);
	return INTERACTION_FLAG::NONE;
}