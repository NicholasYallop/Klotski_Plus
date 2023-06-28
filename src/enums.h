enum struct INTERACTION_FLAG : int{
    NONE = 0,
    DESTROY_TILE1 = 1,
    DESTROY_TILE2 = 2,
    BOUNCE_RIGHT_TILE1 = 4,
    BOUNCE_LEFT_TILE1 = 8,
    BOUNCE_UP_TILE1 = 16,
    BOUNCE_DOWN_TILE1 = 32,
    BOUNCE_RIGHT_TILE2 = 64,
    BOUNCE_LEFT_TILE2 = 128,
    BOUNCE_UP_TILE2 = 256,
    BOUNCE_DOWN_TILE2 = 512,
    SPAWN_GREY_TILE = 1024,
    SPAWN_BLUE_TILE = 2048,
    SPAWN_GREEN_TILE = 4096,
    SPAWN_RED_TILE = 8192
};

enum struct EFFECT_RETURN_FLAG : Uint8{
    NONE = 0,
    END_EFFECT = 1
};

INTERACTION_FLAG operator|(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs);

INTERACTION_FLAG operator&(INTERACTION_FLAG lhs, INTERACTION_FLAG rhs);

EFFECT_RETURN_FLAG operator|(EFFECT_RETURN_FLAG lhs, EFFECT_RETURN_FLAG rhs);

EFFECT_RETURN_FLAG operator&(EFFECT_RETURN_FLAG lhs, EFFECT_RETURN_FLAG rhs);