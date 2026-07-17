#define NUM_MODELS 32
#define MAX_TREES 16
#define MAX_NPCS 16

#define TARGET_FPS 60
#define FPS_TIME ((1.0f / TARGET_FPS) * 1.0f)

#define TERRAIN_SIZE 32
#define SCALE 0.25f
#define RENDER 4.0f
#define SIGHT_BACKUP 0.3f  // this pulls back your sight, useful when you don't see stuff when looking down

#define TEX_SCALE 256
#define TEX_WIDTH  256.0f
#define TEX_HEIGHT 256.0f

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