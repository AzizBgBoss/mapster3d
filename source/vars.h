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
    float x, y, z;
} Tree;

Tree trees[MAX_TREES];

typedef struct
{
    NE_Camera *Camera;
    NE_Model *Model[NUM_MODELS];
    bool activeModel[NUM_MODELS];
} SceneData;

SceneData Scene = {0};

NE_Material *TerrainMaterial;
NE_Material *TreeMaterial;

float terrainVertices[TERRAIN_SIZE][TERRAIN_SIZE][3] = {0};
float terrainNormal[TERRAIN_SIZE - 1][TERRAIN_SIZE - 1][2][3] = {0};