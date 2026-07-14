/*
Dear future me, I hope your fucking idea works

Yours truly, past AzizBgBoss

7/13/2026
*/

#include <NEMain.h>
#include <time.h>
#include <math.h>

#include "mathutils.h"

#define TERRAIN_SIZE 32
#define SCALE 0.25f
#define RENDER 5.0f

typedef struct
{
    NE_Camera *Camera;
} SceneData;

NE_Material *TerrainMaterial;

typedef struct
{
    float x, y, z;
    float pitch, yaw;
    float speed;
} Player;

Player player = {0, 0, 0, 0, 0, 0.05f};

float terrainVertices[TERRAIN_SIZE][TERRAIN_SIZE][3] = {0};
float terrainNormal[TERRAIN_SIZE - 1][TERRAIN_SIZE - 1][2][3] = {0};

float getHeightAt(float x, float z)
{
    float gx = x / SCALE + TERRAIN_SIZE / 2.0f;
    float gz = z / SCALE + TERRAIN_SIZE / 2.0f;

    int x0 = (int)gx;
    int z0 = (int)gz;

    if (x0 < 0 || z0 < 0 || x0 >= TERRAIN_SIZE - 1 || z0 >= TERRAIN_SIZE - 1)
        return 0;

    float tx = gx - x0;
    float tz = gz - z0;

    float h00 = terrainVertices[x0][z0][1];
    float h10 = terrainVertices[x0+1][z0][1];
    float h01 = terrainVertices[x0][z0+1][1];
    float h11 = terrainVertices[x0+1][z0+1][1];

    float h0 = h00 + (h10 - h00) * tx;
    float h1 = h01 + (h11 - h01) * tx;

    return h0 + (h1 - h0) * tz;
}

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_CameraUse(Scene->Camera);

    NE_LightSet(0, NE_White, 1, -2, 1);

    NE_PolyFormat(
        31,
        0,
        NE_LIGHT_ALL,
        NE_CULL_NONE,
        NE_MODULATION);

    NE_MaterialUse(TerrainMaterial);

    float camX = player.x;
    float camY = player.y;
    float camZ = player.z;

    float dirX = cosf(player.pitch) * sinf(player.yaw);
    float dirY = sinf(player.pitch);
    float dirZ = cosf(player.pitch) * cosf(player.yaw);

    float len = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);
    if (len > 0.00001f)
    {
        dirX /= len;
        dirY /= len;
        dirZ /= len;
    }

    NE_PolyBegin(GL_TRIANGLES);

    for (int x = 0; x < TERRAIN_SIZE - 1; x++)
    {
        for (int z = 0; z < TERRAIN_SIZE - 1; z++)
        {
            float centerX = (terrainVertices[x][z][0] +
                             terrainVertices[x + 1][z + 1][0]) *
                            0.5f;

            float centerY = (terrainVertices[x][z][1] +
                             terrainVertices[x + 1][z + 1][1]) *
                            0.5f;

            float centerZ = (terrainVertices[x][z][2] +
                             terrainVertices[x + 1][z + 1][2]) *
                            0.5f;

            float dx = centerX - camX;
            float dy = centerY - camY;
            float dz = centerZ - camZ;

            float distSq = dx * dx + dy * dy + dz * dz;
            if (distSq > RENDER * RENDER)
                continue;
            float dist = sqrtf(distSq);
            float dot;
            if (dist > 0.00001f)
                dot = (dx * dirX + dy * dirY + dz * dirZ) / dist;
            else
                dot = 1.0f;

            if (dot < -1.0f)
                continue;

            NE_PolyNormal(terrainNormal[x][z][0][0], terrainNormal[x][z][0][1], terrainNormal[x][z][0][2]);

            NE_PolyVertex(terrainVertices[x][z][0], terrainVertices[x][z][1], terrainVertices[x][z][2]);
            NE_PolyVertex(terrainVertices[x + 1][z][0], terrainVertices[x + 1][z][1], terrainVertices[x + 1][z][2]);
            NE_PolyVertex(terrainVertices[x][z + 1][0], terrainVertices[x][z + 1][1], terrainVertices[x][z + 1][2]);

            NE_PolyNormal(terrainNormal[x][z][1][0], terrainNormal[x][z][1][1], terrainNormal[x][z][1][2]);

            NE_PolyVertex(terrainVertices[x][z + 1][0], terrainVertices[x][z + 1][1], terrainVertices[x][z + 1][2]);
            NE_PolyVertex(terrainVertices[x + 1][z][0], terrainVertices[x + 1][z][1], terrainVertices[x + 1][z][2]);
            NE_PolyVertex(terrainVertices[x + 1][z + 1][0], terrainVertices[x + 1][z + 1][1], terrainVertices[x + 1][z + 1][2]);
        }
    }

    NE_PolyEnd();
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    consoleDemoInit();

    int seed = rando(0, 10000);
    for (int x = 0; x < TERRAIN_SIZE; x++)
    {
        for (int z = 0; z < TERRAIN_SIZE; z++)
        {
            terrainVertices[x][z][0] = (x - TERRAIN_SIZE / 2.0f) * SCALE;
            terrainVertices[x][z][1] = fractalPerlin2D(x * 0.1f, z * 0.1f, 1, 0.5f, 1.0f, seed) * 3.0f * SCALE;
            terrainVertices[x][z][2] = (z - TERRAIN_SIZE / 2.0f) * SCALE;
        }
    }
    for (int x = 0; x < TERRAIN_SIZE - 1; x++)
    {
        for (int z = 0; z < TERRAIN_SIZE - 1; z++)
        {
            CalculateNormal(
                terrainVertices[x][z][0], terrainVertices[x][z][1], terrainVertices[x][z][2],
                terrainVertices[x + 1][z][0], terrainVertices[x + 1][z][1], terrainVertices[x + 1][z][2],
                terrainVertices[x][z + 1][0], terrainVertices[x][z + 1][1], terrainVertices[x][z + 1][2],
                &terrainNormal[x][z][0][0], &terrainNormal[x][z][0][1], &terrainNormal[x][z][0][2]);
            CalculateNormal(
                terrainVertices[x + 1][z][0], terrainVertices[x + 1][z][1], terrainVertices[x + 1][z][2],
                terrainVertices[x + 1][z + 1][0], terrainVertices[x + 1][z + 1][1], terrainVertices[x + 1][z + 1][2],
                terrainVertices[x][z + 1][0], terrainVertices[x][z + 1][1], terrainVertices[x][z + 1][2],
                &terrainNormal[x][z][1][0], &terrainNormal[x][z][1][1], &terrainNormal[x][z][1][2]);
        }
    }

    player.y = getHeightAt(player.x, player.z);

    SceneData Scene = {0};

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_ClearColorSet(RGB15(10, 20, 31), 63, 0);

    Scene.Camera = NE_CameraCreate();

    NE_CameraSet(Scene.Camera,
                 0.1, 0.2, 0.1,
                 0, 0, 0,
                 0, 1, 0
    );

    TerrainMaterial = NE_MaterialCreate();

    NE_MaterialSetProperties(
        TerrainMaterial,
        RGB15(0, 31, 0),
        RGB15(0, 10, 0),
        RGB15(0, 0, 0),
        RGB15(0, 0, 0),
        false,
        false);

    {
        float lx = 1.0f, ly = -2.0f, lz = 1.0f;
        float llen = sqrtf(lx * lx + ly * ly + lz * lz);
        lx /= llen; ly /= llen; lz /= llen;
        NE_LightSet(0, NE_White, lx, ly, lz);
    }

    int fpscount = 0;
    int oldsec = 0;
    int seconds = 0;

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();
        uint32_t keys = keysHeld();

        if (keys & KEY_LEFT)
            player.yaw += 0.05f;
        else if (keys & KEY_RIGHT)
            player.yaw -= 0.05f;
        if (keys & KEY_UP)
            player.pitch += 0.05f;
        else if (keys & KEY_DOWN)
            player.pitch -= 0.05f;

        if (player.pitch > 1.5f)
            player.pitch = 1.5f;
        else if (player.pitch < -1.5f)
            player.pitch = -1.5f;

        if (keys & KEY_A)
        {
            player.x += sinf(player.yaw) * player.speed;
            player.z += cosf(player.yaw) * player.speed;
        }
        if (keys & KEY_B)
        {
            player.x -= sinf(player.yaw) * player.speed;
            player.z -= cosf(player.yaw) * player.speed;
        }

        if (player.x > (TERRAIN_SIZE / 2.0f) * SCALE)
            player.x = (TERRAIN_SIZE / 2.0f) * SCALE;
        else if (player.x < -(TERRAIN_SIZE / 2.0f) * SCALE)
            player.x = -(TERRAIN_SIZE / 2.0f) * SCALE;

        if (player.z > (TERRAIN_SIZE / 2.0f) * SCALE)
            player.z = (TERRAIN_SIZE / 2.0f) * SCALE;
        else if (player.z < -(TERRAIN_SIZE / 2.0f) * SCALE)
            player.z = -(TERRAIN_SIZE / 2.0f) * SCALE;

        player.y = getHeightAt(player.x, player.z);

        NE_CameraSet(Scene.Camera,
                     player.x,
                     player.y + 0.2f,
                     player.z,

                     player.x + cosf(player.pitch) * sinf(player.yaw),
                     player.y + 0.2f + sinf(player.pitch),
                     player.z + cosf(player.pitch) * cosf(player.yaw),

                     0, 1, 0);

        time_t unixTime = time(NULL);
        struct tm *timeStruct = gmtime((const time_t *)&unixTime);
        seconds = timeStruct->tm_sec;

        if (seconds != oldsec)
        {
            oldsec = seconds;
            printf("\x1b[10;0HFPS: %d", fpscount);
            fpscount = 0;
        }

        printf("\x1b[0;0HPad: Rotate.\nA/B: Move forward/back.");
        printf("\nPolys: %d\nVertices: %d\nCPU: %d%%",
               NE_GetPolygonCount(), NE_GetVertexCount(), NE_GetCPUPercent());
        printf("\nSeed: %d, Keys: %08X", seed, keys);
        printf("\x1b[15;0HPos: %.2f %.2f %.2f", player.x, player.y, player.z);

        NE_ProcessArg(Draw3DScene, &Scene);
        fpscount++;
    }

    return 0;
}