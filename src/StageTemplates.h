#pragma once
#include "common.h"

template<typename First, typename ... Rounds>
void Stage::addRounds(First arg, const Rounds&... rest)
{
	stage.roundTail->next = arg;
	stage.roundTail = arg;
	addRounds(rest...);
}