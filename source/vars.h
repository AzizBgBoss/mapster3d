typedef struct
{
    float x, y, z;
    float pitch, yaw;
    float speed;
} Player;

Player player = {0, 0, 0, 0, 0, 0.02f};

typedef struct
{
    bool active;
    int modelID;
    float x, y, z;
} Tree;

Tree trees[MAX_TREES] = {0};

typedef struct
{
    bool active;
    int modelID;
    float x, y, z;
    float yaw;
    float speed;
    uint8_t target; 
} Npc;

Npc npcs[MAX_NPCS] = {0};

typedef struct
{
    NE_Camera *Camera;
    NE_Model *Model[NUM_MODELS];
    bool activeModel[NUM_MODELS];
} SceneData;

SceneData Scene = {0};

typedef struct {
    NE_Model *model;
    float distSq;
} VisibleModel;

uint32_t frames = 0;

NE_Material *TerrainMaterial, *TreeMaterial, *NpcMaterial;
NE_Palette *TerrainPalette, *TreePalette, *NpcPalette;

uint32_t oldTime;
float delta;

float terrainVertices[TERRAIN_SIZE][TERRAIN_SIZE][3] = {0};
float terrainNormal[TERRAIN_SIZE - 1][TERRAIN_SIZE - 1][2][3] = {0};