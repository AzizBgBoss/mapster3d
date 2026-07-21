/*
Dear future me, I hope your fucking idea works

Yours truly, past AzizBgBoss

13/7/2026

Dear past me, yes my idea will work!

Yours truly, present AzizBgBoss

19/7/2026
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
#include "seed_pack_bin.h"
#include "apple_seed_pack_texture.h"
#include "orange_seed_pack_texture.h"
#include "watering_can_bin.h"
#include "watering_can_texture.h"

#include "terrain_texture.h"

#include "font.h"

#include "defs.h"
#include "vars.h"
#include "mathutils.h"
#include "gameutils.h"
#include "bgutils.h"
#include "menuutils.h"

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
        else if (placementModelID != -1 && visible[i].model == Scene->Model[placementModelID])
            NE_PolyFormat(
                16,
                0,
                NE_LIGHT_ALL,
                NE_CULL_BACK,
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

    // thePrint("\nPolys: %d %d%%       \nVertices: %d %d%%       \nCPU: %d%%       ", NE_GetPolygonCount(), NE_GetPolygonCount() / 2048 * 100, NE_GetVertexCount(), NE_GetVertexCount() / 2048 * 100, NE_GetCPUPercent());
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
    itemModels[3] = (ModelRef){seed_pack_bin, materials[8].mat};
    itemModels[4] = (ModelRef){seed_pack_bin, materials[9].mat};
    itemModels[5] = (ModelRef){watering_can_bin, materials[10].mat};

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

    PlacementMaterial = NE_MaterialCreate();
    NE_MaterialSetProperties(PlacementMaterial,
                             RGB15(5, 5, 5),  // Diffuse
                             RGB15(5, 5, 20), // Ambient
                             RGB15(0, 0, 0),  // Specular
                             RGB15(0, 0, 0),  // Emission
                             false, false);   // Vertex color, use shininess table

    // Allocate space for everything
    for (int i = 0; i < NUM_MODELS; i++)
    {
        Scene.Model[i] = NE_ModelCreate(NE_Static);
        Scene.activeModel[i] = false;
    }

    // Set up menus
    for (int i = 0; i < sizeof(buyItems) / sizeof(ShopItem); i++)
    {
        sprintf(menus[3].items[i], "%s - %d$", itemNames[buyItems[i].itemID], buyItems[i].price);
        menus[3].itemCount++;
    }
    for (int i = 0; i < sizeof(sellItems) / sizeof(ShopItem); i++)
    {
        sprintf(menus[4].items[i], "%s - %d$", itemNames[sellItems[i].itemID], sellItems[i].price);
        menus[4].itemCount++;
    }

    createItem(0, 0, 0, ITEM_WATERING_CAN, 1);

    /*
    int fpscount = 0;
    int oldsec = 0;
    int seconds = 0;
    */
    timerStart(0, ClockDivider_1024, 0, NULL); // more precise than time()
    oldTime = timerElapsed(0) / (float)(BUS_CLOCK / 1024);

    alert("Welcome! Don't forget to check the help in the pause menu if you need any!");

    while (1)
    {
        delta = (timerElapsed(0) / (float)(BUS_CLOCK / 1024) - oldTime) / FPS_TIME;
        oldTime = timerElapsed(0) / (float)(BUS_CLOCK / 1024);
        NE_WaitForVBL(0);

        if (placementModelID != -1)
        {
            Scene.activeModel[placementModelID] = false;
            placementModelID = -1; // Always keep it off unless syncPlacement() is called
        }

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
            for (int i = 0; i < MAX_TREES; i++)
            {
                if (trees[i].active && isInPlayerRange(trees[i].x, trees[i].z, 0.2f))
                {
                    selectedModel = trees[i].modelID;
                    selectionType = SELECTION_TREE;
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

        if (activeMenu == -1)
        {
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
                moveForward(&player.x, &player.z, player.yaw, player.speed * -0.5f, -1);
            }

            if (keys & KEY_R)
            {
                if (selectionType == SELECTION_TREE) // Water tree
                {
                    if (player.inventory.itemID == ITEM_WATERING_CAN)
                    {
                        if (trees[selectionParam].water < (trees[selectionParam].level + 1) * TREE_TRANSITION_TIME / 4)
                        {
                            trees[selectionParam].water += 0.02f * delta;
                        }
                    }
                }
            }

            keys = keysDown();

            if (keys & KEY_X)
                activeMenu = 0;

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
                    if (npcs[selectionParam].inventory.itemID != ITEM_NONE)
                    {
                        if (npcs[selectionParam].inventory.quantity > 0 && (player.inventory.itemID == ITEM_NONE || player.inventory.itemID == npcs[selectionParam].inventory.itemID))
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
                else if (selectionType == SELECTION_TREE) // Take from tree
                {
                    if (trees[selectionParam].inventory.quantity > 0)
                    {
                        if (player.inventory.itemID == ITEM_NONE || player.inventory.itemID == trees[selectionParam].inventory.itemID)
                        {
                            if (player.inventory.quantity < 3)
                                giveInventoryTree(&trees[selectionParam], trees[selectionParam].inventory.itemID, -giveInventory(&player.inventory, trees[selectionParam].inventory.itemID, trees[selectionParam].inventory.quantity));
                            else
                                alert("You can't hold more items!");
                        }
                        else
                            alert("You can't take a different item from the one you're holding!");
                    }
                }
                else if (player.inventory.itemID >= ITEM_APPLE_SEED_PACK && player.inventory.itemID <= ITEM_ORANGE_SEED_PACK) // Plant
                {
                    syncPlacement(tree_bin);
                    if (isLegal(F32TOF(Scene.Model[placementModelID]->x), F32TOF(Scene.Model[placementModelID]->z)))
                    {
                        if (createTree(F32TOF(Scene.Model[placementModelID]->x), F32TOF(Scene.Model[placementModelID]->z), player.inventory.itemID - ITEM_APPLE_SEED_PACK + ITEM_APPLE) != -1)
                            giveInventory(&player.inventory, player.inventory.itemID, -1);
                        else
                            alert("Maximum planted trees reached (%d)!", MAX_TREES);
                    }
                    else
                        alert("You can't place here!");
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
                else if (selectionType == SELECTION_TREE) // Give to tree
                {
                }
                else if (player.inventory.itemID != ITEM_NONE) // Drop item
                {
                    if (createItem(Scene.Model[player.inventory.modelID]->x / 4096.0f, Scene.Model[player.inventory.modelID]->z / 4096.0f, player.yaw, player.inventory.itemID, player.inventory.quantity) != -1)
                        giveInventory(&player.inventory, player.inventory.itemID, -player.inventory.quantity);
                    else
                        alert("Maximum dropped items limit reached (%d)!", MAX_ITEMS);
                }
            }
        }
        else
        {
            scanKeys();
            uint32_t keys = keysDown();

            if (keys & KEY_UP)
                menus[activeMenu].choice = (menus[activeMenu].choice + menus[activeMenu].itemCount - 1) % menus[activeMenu].itemCount;

            if (keys & KEY_DOWN)
                menus[activeMenu].choice = (menus[activeMenu].choice + 1) % menus[activeMenu].itemCount;

            if (keys & KEY_A)
                menus[activeMenu].function(menus[activeMenu].choice);

            if (keys & KEY_B)
                activeMenu = -1;
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

        syncHeldItem(player.x, player.y, player.z, player.yaw, player.pitch, player.inventory.modelID, 0.1f);

        // ========================= Update NPCs =====================================

        for (int i = 0; i < MAX_NPCS; i++)
        {
            if (npcs[i].active)
            {
                updateNpc(&npcs[i], i);
                syncHeldItem(npcs[i].x, npcs[i].y, npcs[i].z, npcs[i].yaw, 0, npcs[i].inventory.modelID, 0.05f);
            }
        }

        // ========================= Update Trees ====================================

        for (int i = 0; i < MAX_TREES; i++)
        {
            if (trees[i].active) // Split work across frames
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
        if (activeMenu == -1)
        {
            thePrint("%d$ | ", player.money);
            if (player.inventory.quantity > 0 && player.inventory.itemID != ITEM_NONE)
            {
                thePrint("Holding: %d %s%s",
                         player.inventory.quantity,
                         itemNames[player.inventory.itemID],
                         player.inventory.quantity > 1 ? "s" : "");
            }
            else
            {
                thePrint("Holding: %s", itemNames[player.inventory.itemID]);
            }

            thePrint("\n");

            if (selectionType == SELECTION_ITEM)
            {
                thePrint("\nL: pickup %d %s%s",
                         items[selectionParam].inventory.quantity,
                         itemNames[items[selectionParam].inventory.itemID],
                         items[selectionParam].inventory.quantity > 1 && items[selectionParam].inventory.itemID != ITEM_NONE ? "s" : "");
                if (player.inventory.itemID != ITEM_NONE)
                    goto dropMsg;
            }
            else if (selectionType == SELECTION_NPC)
            {
                if (npcs[selectionParam].inventory.itemID == ITEM_NONE || npcs[selectionParam].inventory.itemID == player.inventory.itemID)
                {
                    thePrint("\nR: give %d %s%s to %s",
                             player.inventory.quantity,
                             itemNames[player.inventory.itemID],
                             player.inventory.quantity > 1 && player.inventory.itemID != ITEM_NONE ? "s" : "",
                             npcs[selectionParam].name);
                }
                if (player.inventory.itemID == ITEM_NONE || player.inventory.itemID == npcs[selectionParam].inventory.itemID)
                {
                    thePrint("\nL: take %d %s%s from %s",
                             npcs[selectionParam].inventory.quantity,
                             itemNames[npcs[selectionParam].inventory.itemID],
                             npcs[selectionParam].inventory.quantity > 1 && npcs[selectionParam].inventory.itemID != ITEM_NONE ? "s" : "",
                             npcs[selectionParam].name);
                }
            }
            else if (selectionType == SELECTION_TREE)
            {
                thePrint("\n%s tree", itemNames[trees[selectionParam].itemType]);
                if (trees[selectionParam].water > 0)
                {
                    if (trees[selectionParam].level < 3)
                    {
                        thePrint("\nTime to grow: %d seconds",
                                 (int)(trees[selectionParam].ageTime + (int)TREE_TRANSITION_TIME * (3 - trees[selectionParam].level) - time(NULL) + 1));
                    }
                }
                else
                {
                    thePrint("\nTree needs water!");
                }
                thePrint("\nWater: %dL / %dL",
                         (int)trees[selectionParam].water,
                         (trees[selectionParam].level + 1) * (int)TREE_TRANSITION_TIME / 4);
                if (trees[selectionParam].inventory.quantity > 0)
                {
                    thePrint("\n\nL: harvest %d %s%s from the tree",
                             trees[selectionParam].inventory.quantity,
                             itemNames[trees[selectionParam].inventory.itemID],
                             trees[selectionParam].inventory.quantity > 1 && trees[selectionParam].inventory.itemID != ITEM_NONE ? "s" : "");
                }
                if (trees[selectionParam].water < (trees[selectionParam].level + 1) * (int)TREE_TRANSITION_TIME / 4 && player.inventory.itemID == ITEM_WATERING_CAN)
                {
                    thePrint("\nHold R: water the tree");
                }
            }
            else if (player.inventory.itemID != ITEM_NONE)
            {
                if (player.inventory.itemID >= ITEM_APPLE_SEED_PACK && player.inventory.itemID <= ITEM_ORANGE_SEED_PACK)
                {
                    thePrint("\nL: plant %s seed", itemNames[player.inventory.itemID - ITEM_APPLE_SEED_PACK + ITEM_APPLE]);
                    syncPlacement(tree_bin);
                }
            dropMsg:
                thePrint("\nR: drop %d %s%s",
                         player.inventory.quantity,
                         itemNames[player.inventory.itemID],
                         player.inventory.quantity > 1 && player.inventory.itemID != ITEM_NONE ? "s" : "");
            }

            if (time(NULL) < alertTime)
            {
                thePrint("\n\n%s", alertText);
            }

            print(0, 11, "Mapster3D v" VERSION " by AzizBgBoss");
        }
        else
        {
            thePrint(menus[activeMenu].title);
            for (int i = 0; i < menus[activeMenu].itemCount; i++)
            {
                thePrint("\n%s %s", i == menus[activeMenu].choice ? "-" : " ", menus[activeMenu].items[i]);
            }

            if (time(NULL) < alertTime)
            {
                thePrint("\n\n%s", alertText);
            }

            print(0, 11, "A: select     B: close");
        }

        // ========================= Render Scene ====================================

        NE_ProcessArg(Draw3DScene, &Scene);
        // fpscount++;
        frames++;
    }

    return 0;
}
