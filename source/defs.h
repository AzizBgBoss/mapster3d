#define MAX_TREES 16
#define MAX_NPCS 16
#define MAX_ITEMS 32

#define NUM_MODELS (MAX_TREES + MAX_NPCS * 2 + MAX_ITEMS + 1 + 1) // * 2 since each NPC can hold and item; + 1 for the item we're holding; + 1 for the higlighted model

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192
#define TARGET_FPS 60
#define FPS_TIME ((1.0f / TARGET_FPS) * 1.0f)

#define TERRAIN_SIZE 32
#define SCALE 0.25f
#define MODEL_SCALE 0.125f
#define RENDER 4.0f
#define SIGHT_BACKUP 0.3f  // this pulls back your sight, useful when you don't see stuff when looking down

#define TEX_SCALE 256
#define TEX_WIDTH  128.0f
#define TEX_HEIGHT 128.0f

#define WRAP_U(v) fmodf((v), TEX_WIDTH)
#define WRAP_V(v) fmodf((v), TEX_HEIGHT)

#define F_SECOND 60
#define F_MINUTE (F_SECOND * 60)

#define RAD2ANG(rad) (((int)((rad) * (512.0f / (2.0f * M_PI))) % 512 + 512) % 512)

enum // NPC targets
{
    TARGET_PLAYER,
    TARGET_RANDOM,

    TARGETS,
};

enum // Item IDs
{
    ITEM_NONE,
    ITEM_APPLE,
    ITEM_ORANGE,

    ITEMS,
};