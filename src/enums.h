enum struct INTERACTION_FLAG : int{
    NONE = 0,
    DESTROY_TILE1 = 1,
    DESTROY_TILE2 = 2,
    SPAWN_GREY_TILE = 4,
    BOUNCE_RIGHT_TILE1 = 8,
    BOUNCE_LEFT_TILE1 = 16,
    BOUNCE_UP_TILE1 = 32,
    BOUNCE_DOWN_TILE1 = 64,
    BOUNCE_RIGHT_TILE2 = 128,
    BOUNCE_LEFT_TILE2 = 256,
    BOUNCE_UP_TILE2 = 512,
    BOUNCE_DOWN_TILE2 = 1024
};

enum struct EFFECT_RETURN_FLAG : __uint8_t{
    NONE = 0,
    END_EFFECT = 1
};

INTERACTION_FLAG operator|(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs);

INTERACTION_FLAG operator&(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs);

EFFECT_RETURN_FLAG operator|(EFFECT_RETURN_FLAG lhs, EFFECT_RETURN_FLAG rhs);

EFFECT_RETURN_FLAG operator&(EFFECT_RETURN_FLAG lhs, EFFECT_RETURN_FLAG rhs);