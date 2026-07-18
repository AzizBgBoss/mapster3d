const char *itemNames[ITEMS] =
    {
        "Nothing",
        "Apple",
        "Orange",
};

typedef struct
{
    uint8_t itemID;
    uint8_t quantity;
    int modelID;
} Inventory;

typedef struct
{
    bool active;
    int modelID;
    float x, y, z;
    Inventory inventory;
} Item;

Item items[MAX_ITEMS] = {0};

typedef struct
{
    float x, y, z;
    float pitch, yaw;
    float speed;
    Inventory inventory;
} Player;

Player player = {0, 0, 0, 0, 0, 0.02f, {ITEM_NONE, 0, -1}};

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
    char name[16];
    Inventory inventory;
} Npc;

Npc npcs[MAX_NPCS] = {0};

const char names[][16] = {
    "Jeremy",
    "Lamar",
    "Alyssa",
    "Gabriel",
    "Natalie",
    "Jamal",
    "Jared",
    "AzizBgBoss",
    "Marcus",
    "Priya",
    "Diego",
    "Kaitlyn",
    "Trevor",
    "Amara",
    "Chelsea",
    "Malik",
    "Sofia",
    "Brandon",
    "Nadia",
    "Ethan",
    "Cody",
    "Leila",
    "Tyrese",
    "Megan",
    "Rashid",
    "Paige",
    "Devon",
    "Kratos",
};

#define NAMES sizeof(names) / sizeof(names[0])

typedef struct
{
    NE_Camera *Camera;
    NE_Model *Model[NUM_MODELS];
    uint8_t *modelsRef[NUM_MODELS];
    bool activeModel[NUM_MODELS];
} SceneData;

SceneData Scene = {0};

typedef struct
{
    NE_Model *model;
    float distSq;
} VisibleModel;

typedef struct
{
    uint8_t *model;
    NE_Material *mat;
} ModelRef;

typedef struct
{
    unsigned int *Bitmap;
    unsigned short *Pal;
} MaterialRef;

typedef struct
{
    NE_Material *mat;
    NE_Palette *pal;
} Material;

uint32_t frames = 0;

NE_Material *TerrainMaterial, *TreeMaterial, *NpcMaterial, *AppleMaterial, *OrangeMaterial;
NE_Palette *TerrainPalette, *TreePalette, *NpcPalette, *ApplePalette, *OrangePalette;

NE_Material *HighlightMaterial;

const MaterialRef materialsRef[] = {
    {terrain_textureBitmap, terrain_texturePal},
    {tree_textureBitmap, tree_texturePal},
    {npc_textureBitmap, npc_texturePal},
    {apple_textureBitmap, apple_texturePal},
    {orange_textureBitmap, orange_texturePal},
};

#define SIZE_MATERIALS_REF (sizeof(materialsRef) / sizeof(MaterialRef))

Material materials[SIZE_MATERIALS_REF] = {0};

ModelRef itemModels[ITEMS];

int bgsub;

uint32_t oldTime;
float delta;

float terrainVertices[TERRAIN_SIZE][TERRAIN_SIZE][3] = {0};
float terrainNormal[TERRAIN_SIZE - 1][TERRAIN_SIZE - 1][2][3] = {0};

int highlightedModel = -1;
int highlightedModelID = -1;

char alertText[64];
uint32_t alertTime = 0;

int selectedModel = -1;
uint8_t selectionType = -1;
uint8_t selectionParam = 0;