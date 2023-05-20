#include "common.h"

INTERACTION_FLAG operator|(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs) {
    return static_cast<INTERACTION_FLAG>(
        static_cast<__uint128_t>(lhs) |
        static_cast<__uint128_t>(rhs)
    );
}

INTERACTION_FLAG operator&(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs) {
    return static_cast<INTERACTION_FLAG>(
        static_cast<__uint128_t>(lhs) &
        static_cast<__uint128_t>(rhs)
    );
}