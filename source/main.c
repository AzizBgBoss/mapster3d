/*
Dear future me, I hope your fucking idea works

Yours truly, past AzizBgBoss

7/13/2026
*/

#include <NEMain.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

#include "tree_bin.h"
#include "tree_texture.h"
#include "plant_0_bin.h"
#include "plant_0_texture.h"
#include "plant_1_bin.h"
#include "plant_1_texture.h"
#include "plant_2_bin.h"
#include "plant_2_texture.h"
#include "npc_bin.h"
#include "npc_texture.h"
#include "apple_bin.h"
#include "apple_texture.h"
#include "orange_bin.h"
#include "orange_texture.h"

#include "terrain_texture.h"

#include "font.h"

#include "defs.h"
#include "vars.h"
#include "mathutils.h"
#include "gameutils.h"
#include "bgutils.h"

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
        NE_FOG_ENABLE);

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

            if (!isInSight(camX, camY, camZ, dirX, dirY, dirZ, centerX, centerY, centerZ))
                continue;

            NE_PolyNormal(terrainNormal[x][z][0][0], terrainNormal[x][z][0][1], terrainNormal[x][z][0][2]);
            float baseU = WRAP_U(x * TEX_SCALE);
            float nextU = baseU + TEX_SCALE;

            float baseV = WRAP_V(z * TEX_SCALE);
            float nextV = baseV + TEX_SCALE;

            NE_PolyNormal(terrainNormal[x][z][0][0], terrainNormal[x][z][0][1], terrainNormal[x][z][0][2]);

            NE_PolyTexCoord(baseU, baseV);
            NE_PolyVertex(terrainVertices[x][z][0], terrainVertices[x][z][1], terrainVertices[x][z][2]);

            NE_PolyTexCoord(nextU, baseV);
            NE_PolyVertex(terrainVertices[x + 1][z][0], terrainVertices[x + 1][z][1], terrainVertices[x + 1][z][2]);

            NE_PolyTexCoord(baseU, nextV);
            NE_PolyVertex(terrainVertices[x][z + 1][0], terrainVertices[x][z + 1][1], terrainVertices[x][z + 1][2]);

            NE_PolyNormal(terrainNormal[x][z][1][0], terrainNormal[x][z][1][1], terrainNormal[x][z][1][2]);

            NE_PolyTexCoord(baseU, nextV);
            NE_PolyVertex(terrainVertices[x][z + 1][0], terrainVertices[x][z + 1][1], terrainVertices[x][z + 1][2]);

            NE_PolyTexCoord(nextU, baseV);
            NE_PolyVertex(terrainVertices[x + 1][z][0], terrainVertices[x + 1][z][1], terrainVertices[x + 1][z][2]);

            NE_PolyTexCoord(nextU, nextV);
            NE_PolyVertex(terrainVertices[x + 1][z + 1][0], terrainVertices[x + 1][z + 1][1], terrainVertices[x + 1][z + 1][2]);
        }
    }

    NE_PolyEnd();

    VisibleModel visible[NUM_MODELS];
    int visibleCount = 0;

    for (int i = 0; i < NUM_MODELS; i++)
    {
        if (!Scene->activeModel[i])
            continue;

        float mx = Scene->Model[i]->x / 4096.0f;
        float my = Scene->Model[i]->y / 4096.0f;
        float mz = Scene->Model[i]->z / 4096.0f;

        if (!isInSight(camX, camY, camZ, dirX, dirY, dirZ, mx, my, mz))
            continue;

        float dx = mx - camX;
        float dy = my - camY;
        float dz = mz - camZ;

        visible[visibleCount].model = Scene->Model[i];
        visible[visibleCount].distSq = dx * dx + dy * dy + dz * dz;
        visibleCount++;
    }

    qsort(visible, visibleCount, sizeof(VisibleModel), compareModelDist);

    for (int i = 0; i < visibleCount; i++)
    {
        if (highlightedModelID != -1 && visible[i].model == Scene->Model[highlightedModelID])
            NE_PolyFormat(
                31,
                0,
                NE_LIGHT_ALL,
                NE_CULL_FRONT,
                NE_FOG_ENABLE);
        else
            NE_PolyFormat(
                31,
                0,
                NE_LIGHT_ALL,
                NE_CULL_NONE,
                NE_FOG_ENABLE);
        NE_ModelDraw(visible[i].model);
    }

    printf("\nPolys: %d %d%%       \nVertices: %d %d%%       \nCPU: %d%%       ",
           NE_GetPolygonCount(), NE_GetPolygonCount() / 2048 * 100, NE_GetVertexCount(), NE_GetVertexCount() / 2048 * 100, NE_GetCPUPercent());
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    videoSetModeSub(MODE_0_2D);

    vramSetPrimaryBanks(VRAM_A_MAIN_BG, VRAM_B_LCD,
                        VRAM_C_SUB_BG, VRAM_D_LCD);

    bgsub = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);

    dmaCopy(fontTiles, bgGetGfxPtr(bgsub), fontTilesLen);
    dmaFillHalfWords(0, bgGetMapPtr(bgsub), 32 * 32);
    dmaCopy(fontPal, BG_PALETTE_SUB, fontPalLen);

    int seed = rando(0, 10000);
    for (int x = 0; x < TERRAIN_SIZE; x++)
    {
        for (int z = 0; z < TERRAIN_SIZE; z++)
        {
            terrainVertices[x][z][0] = (x - TERRAIN_SIZE / 2.0f) * SCALE;
            terrainVertices[x][z][1] = fractalPerlin2D(x * 0.1f * SCALE / 0.25f, z * 0.1f * SCALE / 0.25f, 1, 0.5f, 1.0f, seed) * 0.75f;
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

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_SetFov(60);
    NE_ClippingPlanesSet(0.05, 40);
    NE_ClearColorSet(RGB15(10, 20, 31), 63, 0);
    NE_FogEnable(5, RGB15(10, 20, 31), 31, 5, 0x7C00); // shift, color, density, mass, depth

    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);

    Scene.Camera = NE_CameraCreate();

    NE_CameraSet(Scene.Camera,
                 0.1, 0.2, 0.1,
                 0, 0, 0,
                 0, 1, 0);

    for (int i = 0; i < SIZE_MATERIALS_REF; i++)
    {
        materials[i].mat = NE_MaterialCreate();
        materials[i].pal = NE_PaletteCreate();

        NE_MaterialTexLoad(materials[i].mat, NE_PAL256, TEX_WIDTH, TEX_HEIGHT, NE_TEXGEN_TEXCOORD,
                           materialsRef[i].Bitmap);
        NE_PaletteLoad(materials[i].pal, materialsRef[i].Pal, TEX_WIDTH, NE_PAL256);
        NE_MaterialSetPalette(materials[i].mat, materials[i].pal);
    }

    TerrainMaterial = materials[0].mat;
    TreeMaterial = materials[1].mat;
    NpcMaterial = materials[2].mat;

    itemModels[1] = (ModelRef){apple_bin, materials[3].mat};
    itemModels[2] = (ModelRef){orange_bin, materials[4].mat};

    Plant0Material = materials[5].mat;
    Plant1Material = materials[6].mat;
    Plant2Material = materials[7].mat;

    HighlightMaterial = NE_MaterialCreate();
    NE_MaterialSetProperties(HighlightMaterial,
                             RGB15(0, 0, 0),    // Diffuse
                             RGB15(31, 31, 31), // Ambient
                             RGB15(0, 0, 0),    // Specular
                             RGB15(0, 0, 0),    // Emission
                             false, false);     // Vertex color, use shininess table

    // Allocate space for everything
    for (int i = 0; i < NUM_MODELS; i++)
    {
        Scene.Model[i] = NE_ModelCreate(NE_Static);
        Scene.activeModel[i] = false;
    }

    for (int i = 0; i < MAX_TREES; i++)
        createTree(frando(-TERRAIN_SIZE / 2.0f, TERRAIN_SIZE / 2.0f - 1.0f) * SCALE, frando(-TERRAIN_SIZE / 2.0f, TERRAIN_SIZE / 2.0f - 1.0f) * SCALE, rando(ITEM_APPLE, ITEM_ORANGE + 1));
    for (int i = 0; i < 0; i++)
        createItem(frando(-TERRAIN_SIZE / 2.0f, TERRAIN_SIZE / 2.0f - 1.0f) * SCALE, frando(-TERRAIN_SIZE / 2.0f, TERRAIN_SIZE / 2.0f - 1.0f) * SCALE, rando(ITEM_APPLE, ITEM_ORANGE + 1), 1);
    for (int i = 0; i < 1; i++)
    {
        spawnNpc(frando(-TERRAIN_SIZE / 2.0f, TERRAIN_SIZE / 2.0f - 1.0f) * SCALE, frando(-TERRAIN_SIZE / 2.0f, TERRAIN_SIZE / 2.0f - 1.0f) * SCALE);
    }
    /*
    int fpscount = 0;
    int oldsec = 0;
    int seconds = 0;
    */
    timerStart(0, ClockDivider_1024, 0, NULL); // more precise than time()
    oldTime = timerElapsed(0) / (float)(BUS_CLOCK / 1024);

    while (1)
    {
        delta = (timerElapsed(0) / (float)(BUS_CLOCK / 1024) - oldTime) / FPS_TIME;
        oldTime = timerElapsed(0) / (float)(BUS_CLOCK / 1024);
        NE_WaitForVBL(0);

        // ========================= Update selection ================================

        selectedModel = -1;
        selectionType = -1;
        selectionParam = 0;
        for (int i = 0; i < MAX_ITEMS; i++)
        {
            if (items[i].active && isInPlayerRange(items[i].x, items[i].z, 0.2f))
            {
                selectedModel = items[i].modelID;
                selectionType = SELECTION_ITEM;
                selectionParam = i;
                break;
            }
        }

        if (selectedModel == -1)
            for (int i = 0; i < MAX_NPCS; i++)
            {
                if (npcs[i].active && isInPlayerRange(npcs[i].x, npcs[i].z, 0.2f) &&
                    (npcs[i].inventory.quantity > 0 || player.inventory.quantity > 0)) // no need to highlight if there's nothing to take or give
                {
                    selectedModel = npcs[i].modelID;
                    selectionType = SELECTION_NPC;
                    selectionParam = i;
                    break;
                }
            }

        if (selectedModel != -1)
            setHighlightedModel(selectedModel);
        else
            setHighlightedModel(-1);

        // ========================= Controls ========================================

        scanKeys();
        uint32_t keys = keysHeld();

        if (keys & KEY_LEFT)
            player.yaw += 0.05f * delta;
        else if (keys & KEY_RIGHT)
            player.yaw -= 0.05f * delta;
        if (keys & KEY_UP)
            player.pitch += 0.05f * delta;
        else if (keys & KEY_DOWN)
            player.pitch -= 0.05f * delta;

        if (player.pitch > 1.5f)
            player.pitch = 1.5f;
        else if (player.pitch < -1.5f)
            player.pitch = -1.5f;

        if (keys & KEY_A)
        {
            moveForward(&player.x, &player.z, player.yaw, player.speed, -1);
        }
        if (keys & KEY_B)
        {
            moveForward(&player.x, &player.z, player.yaw, -player.speed, -1);
        }

        keys = keysDown();

        if (keys & KEY_L)
        {
            if (selectionType == SELECTION_ITEM) // Pickup item
            {
                if (player.inventory.itemID == ITEM_NONE || items[selectionParam].inventory.itemID == player.inventory.itemID)
                {
                    if (player.inventory.quantity < 3)
                        addItemQuantity(selectionParam, -giveInventory(&player.inventory, items[selectionParam].inventory.itemID, items[selectionParam].inventory.quantity));
                    else
                        alert("You can't hold more items!");
                }
                else
                    alert("You can't pick up a different item from the one you're holding!");
            }
            else if (selectionType == SELECTION_NPC) // Take from NPC
            {
                if (player.inventory.itemID == ITEM_NONE || player.inventory.itemID == npcs[selectionParam].inventory.itemID)
                {
                    if (player.inventory.quantity < 3)
                        giveInventory(&npcs[selectionParam].inventory, npcs[selectionParam].inventory.itemID, -giveInventory(&player.inventory, npcs[selectionParam].inventory.itemID, npcs[selectionParam].inventory.quantity));
                    else
                        alert("You can't hold more items!");
                }
                else
                    alert("You can't take a different item from the one you're holding!");
            }
        }

        if (keys & KEY_R)
        {
            if (selectionType == SELECTION_NPC) // Give to NPC
            {
                if (npcs[selectionParam].inventory.itemID == ITEM_NONE || npcs[selectionParam].inventory.itemID == player.inventory.itemID)
                {
                    if (npcs[selectionParam].inventory.quantity < 3)
                        giveInventory(&player.inventory, player.inventory.itemID, -giveInventory(&npcs[selectionParam].inventory, player.inventory.itemID, player.inventory.quantity));
                    else
                        alert("The NPC can't hold more items!");
                }
                else
                    alert("You can't give the NPC a different item from the one they're holding!");
            }
            else if (player.inventory.itemID != ITEM_NONE) // Drop item
            {
                createItem(Scene.Model[player.inventory.modelID]->x / 4096.0f, Scene.Model[player.inventory.modelID]->z / 4096.0f, player.inventory.itemID, player.inventory.quantity);
                giveInventory(&player.inventory, player.inventory.itemID, -player.inventory.quantity);
            }
        }

        // ========================= Update Player ===================================

        player.yaw = fmodf(player.yaw, 2.0f * (float)M_PI);
        player.y = getHeightAt(player.x, player.z);

        NE_CameraSet(Scene.Camera,
                     player.x,
                     player.y + 0.1f,
                     player.z,

                     player.x + cosf(player.pitch) * sinf(player.yaw),
                     player.y + 0.1f + sinf(player.pitch),
                     player.z + cosf(player.pitch) * cosf(player.yaw),

                     0, 1, 0);

        syncHeldItem(player.x, player.y, player.z, player.yaw, player.pitch, player.inventory.modelID);

        // ========================= Update NPCs =====================================

        for (int i = 0; i < MAX_NPCS; i++)
        {
            if (npcs[i].active)
            {
                updateNpc(&npcs[i], i);
                syncHeldItem(npcs[i].x, npcs[i].y, npcs[i].z, npcs[i].yaw, 0, npcs[i].inventory.modelID);
            }
        }

        // ========================= Update Trees ====================================

        for (int i = 0; i < MAX_TREES; i++)
        {
            if (trees[i].active)
            {
                updateTree(&trees[i], i);
            }
        }

        // ========================= Update Game Logic ===============================

        if (highlightedModelID != -1)
        {
            NE_ModelSetCoord(Scene.Model[highlightedModelID], Scene.Model[highlightedModel]->x / 4096.0f, Scene.Model[highlightedModel]->y / 4096.0f, Scene.Model[highlightedModel]->z / 4096.0f);
            NE_ModelSetRot(Scene.Model[highlightedModelID], Scene.Model[highlightedModel]->rx, Scene.Model[highlightedModel]->ry, Scene.Model[highlightedModel]->rz);
        }

        // ========================= Update Bottom Screen UI =========================

        clearPrint();
        printSmart(0, 0, "Holding ");
        if (player.inventory.quantity > 0 && player.inventory.itemID != ITEM_NONE)
        {
            printValDirect(player.inventory.quantity);
            printSmartDirect(" ");
        }
        printSmartDirect(itemNames[player.inventory.itemID]);
        if (player.inventory.quantity > 1 && player.inventory.itemID != ITEM_NONE)
        {
            printSmartDirect("s");
        }

        printSmartDirect("\n");

        if (selectionType == SELECTION_ITEM)
        {
            printSmartDirect("\nPress L to pickup ");
            printValDirect(items[selectionParam].inventory.quantity);
            printSmartDirect(" ");
            printSmartDirect(itemNames[items[selectionParam].inventory.itemID]);
            if (items[selectionParam].inventory.quantity > 1 && items[selectionParam].inventory.itemID != ITEM_NONE)
            {
                printSmartDirect("s");
            }
        }
        else if (selectionType == SELECTION_NPC)
        {
            if (npcs[selectionParam].inventory.itemID == ITEM_NONE || npcs[selectionParam].inventory.itemID == player.inventory.itemID)
            {
                printSmartDirect("\nPress R to give ");
                printValDirect(player.inventory.quantity);
                printSmartDirect(" ");
                printSmartDirect(itemNames[player.inventory.itemID]);
                if (player.inventory.quantity > 1 && player.inventory.itemID != ITEM_NONE)
                {
                    printSmartDirect("s");
                }
                printSmartDirect(" to ");
                printSmartDirect(npcs[selectionParam].name);
            }
            if (player.inventory.itemID == ITEM_NONE || player.inventory.itemID == npcs[selectionParam].inventory.itemID)
            {
                printSmartDirect("\nPress L to take ");
                printValDirect(npcs[selectionParam].inventory.quantity);
                printSmartDirect(" ");
                printSmartDirect(itemNames[npcs[selectionParam].inventory.itemID]);
                if (npcs[selectionParam].inventory.quantity > 1 && npcs[selectionParam].inventory.itemID != ITEM_NONE)
                {
                    printSmartDirect("s");
                }
                printSmartDirect(" from ");
                printSmartDirect(npcs[selectionParam].name);
            }
        }

        if (selectionType != SELECTION_NPC && player.inventory.itemID != ITEM_NONE)
        {
            printSmartDirect("\nPress R to drop ");
            printValDirect(player.inventory.quantity);
            printSmartDirect(" ");
            printSmartDirect(itemNames[player.inventory.itemID]);
            if (player.inventory.quantity > 1 && player.inventory.itemID != ITEM_NONE)
            {
                printSmartDirect("s");
            }
        }

        if (time(NULL) < alertTime)
        {
            printSmartDirect("\n\n");
            printSmartDirect(alertText);
        }

        print(0, 11, "Mapster3D v" VERSION " by AzizBgBoss");

        // ========================= Render Scene ====================================

        NE_ProcessArg(Draw3DScene, &Scene);
        // fpscount++;
        frames++;
    }

    return 0;
}