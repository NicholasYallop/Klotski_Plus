#include "common.h"

INTERACTION_FLAG operator|(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs) {
    return static_cast<INTERACTION_FLAG>(
        static_cast<int>(lhs) |
        static_cast<int>(rhs)
    );
}

INTERACTION_FLAG operator&(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs) {
    return static_cast<INTERACTION_FLAG>(
        static_cast<int>(lhs) &
        static_cast<int>(rhs)
    );
}

EFFECT_RETURN_FLAG operator|(EFFECT_RETURN_FLAG lhs, EFFECT_RETURN_FLAG rhs) {
    return static_cast<EFFECT_RETURN_FLAG>(
        static_cast<Uint8>(lhs) |
        static_cast<Uint8>(rhs)
    );
}

EFFECT_RETURN_FLAG operator&(EFFECT_RETURN_FLAG lhs, EFFECT_RETURN_FLAG rhs) {
    return static_cast<EFFECT_RETURN_FLAG>(
        static_cast<Uint8>(lhs) &
        static_cast<Uint8>(rhs)
    );
}