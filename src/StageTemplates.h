#pragma once
#include "common.h"

template<typename First, typename ... Rounds>
void Stage::addRounds(First arg, const Rounds&... rest)
{
	roundTail->next = arg;
	roundTail = arg;
	addRounds(rest...);
}