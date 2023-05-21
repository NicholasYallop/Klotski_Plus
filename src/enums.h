enum struct INTERACTION_FLAG : __uint8_t{
    NONE = 0b0,
    DESTROY_TILE1 = 0b1,
    DESTROY_TILE2 = 0b01,
    SPAWN_GREY_TILE = 0b0010
};

INTERACTION_FLAG operator|(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs);

INTERACTION_FLAG operator&(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs);