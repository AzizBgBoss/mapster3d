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
    float h10 = terrainVertices[x0 + 1][z0][1];
    float h01 = terrainVertices[x0][z0 + 1][1];
    float h11 = terrainVertices[x0 + 1][z0 + 1][1];

    float h0 = h00 + (h10 - h00) * tx;
    float h1 = h01 + (h11 - h01) * tx;

    return h0 + (h1 - h0) * tz;
}

int compareModelDist(const void *a, const void *b)
{
    float da = ((const VisibleModel *)a)->distSq;
    float db = ((const VisibleModel *)b)->distSq;
    if (da < db)
        return -1;
    if (da > db)
        return 1;
    return 0;
}

bool isInSight(float camX, float camY, float camZ,
               float dirX, float dirY, float dirZ,
               float px, float py, float pz)
{
    float testX = camX - dirX * SIGHT_BACKUP;
    float testY = camY - dirY * SIGHT_BACKUP;
    float testZ = camZ - dirZ * SIGHT_BACKUP;

    float dx = px - testX;
    float dy = py - testY;
    float dz = pz - testZ;

    float rdx = px - camX;
    float rdy = py - camY;
    float rdz = pz - camZ;
    float distSq = rdx * rdx + rdy * rdy + rdz * rdz;
    if (distSq > RENDER * RENDER)
        return false;

    float testDistSq = dx * dx + dy * dy + dz * dz;
    float dist = sqrtf(testDistSq);
    float dot;
    if (dist > 0.00001f)
        dot = (dx * dirX + dy * dirY + dz * dirZ) / dist;
    else
        dot = 1.0f;

    if (dot < 0.4f)
        return false;

    return true;
}

bool isInRange(float x1, float z1, float x2, float z2, float range)
{
    float dx = x1 - x2;
    float dz = z1 - z2;
    return (dx * dx + dz * dz) <= (range * range);
}

bool isInPlayerRange(float x, float z, float range)
{
    return isInRange(x, z, player.x, player.z, range);
}

bool isSolid(float x, float z, int param) // if param == -1, we're checking the player, else it's an npc id. we do this so the collision doesn't count the owner's own box.
{
    const float dist = 0.15f;
    if (isInPlayerRange(x, z, dist) && param != -1)
        return true;
    for (int i = 0; i < MAX_TREES; i++)
    {
        if (trees[i].active && isInRange(x, z, trees[i].x, trees[i].z, dist * trees[i].level / 4.0f))
            return true;
    }
    /*
    for (int i = 0; i < MAX_NPCS; i++)
    {
        if (npcs[i].active && isInRange(x, z, npcs[i].x, npcs[i].z, dist) && i != param)
            return true;
    }
    */
    if (x > (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE || x < -(TERRAIN_SIZE / 2.0f) * SCALE)
        return true;
    if (z > (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE || z < -(TERRAIN_SIZE / 2.0f) * SCALE)
        return true;
    return false;
}

int createModel(float x, float y, float z, float pitch, float yaw, float roll, const void *data, NE_Material *mat)
{
    for (int i = 0; i < NUM_MODELS; i++)
    {
        if (!Scene.activeModel[i])
        {
            Scene.activeModel[i] = true;
            Scene.modelsRef[i] = data;
            NE_ModelLoadStaticMesh(Scene.Model[i], data);
            NE_ModelSetMaterial(Scene.Model[i], mat);
            NE_ModelSetCoord(Scene.Model[i], x, y, z);
            NE_ModelSetRot(Scene.Model[i], pitch, yaw, roll);
            NE_ModelScale(Scene.Model[i], MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);
            return i;
        }
    }
    return -1;
}

void moveForward(float *x, float *z, float yaw, float speed, int param)
{
    float fx = x[0] + sinf(yaw) * speed * delta;
    float fz = z[0] + cosf(yaw) * speed * delta;

    if (!isSolid(fx, fz, param))
    {
        x[0] = fx;
        z[0] = fz;
    }
    else if (!isSolid(fx, z[0], param))
    {
        x[0] = fx;
    }
    else if (!isSolid(x[0], fz, param))
    {
        z[0] = fz;
    }

    // add this just in case i spawn something out of bounds

    if (x[0] > (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE)
        x[0] = (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE;
    else if (x[0] < -(TERRAIN_SIZE / 2.0f) * SCALE)
        x[0] = -(TERRAIN_SIZE / 2.0f) * SCALE;

    if (z[0] > (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE)
        z[0] = (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE;
    else if (z[0] < -(TERRAIN_SIZE / 2.0f) * SCALE)
        z[0] = -(TERRAIN_SIZE / 2.0f) * SCALE;
}

int giveInventoryPos(Inventory *inventory, uint8_t itemID, int quantity, float x, float y, float z)
{
    int space = 0; // number of items successfully taken
    if (inventory->itemID == ITEM_NONE)
    {
        inventory->itemID = itemID;
        if (quantity > 3)
        {
            inventory->quantity = 3;
            space = 3;
        }
        else
        {
            inventory->quantity = quantity;
            space = quantity;
        }
        inventory->modelID = createModel(x, y, z, 0, 0, 0, itemModels[itemID].model, itemModels[itemID].mat); // it is mathematically impossible for this to return -1
    }
    else if (inventory->itemID == itemID)
    {
        int add = quantity;
        if (inventory->quantity + add > 3)
            add = 3 - inventory->quantity;
        else if (inventory->quantity + add < 0)
            add = -inventory->quantity;
        inventory->quantity += add;
        space = add;
    }
    else if (inventory->itemID != itemID)
    {
        space = 0;
    }

    if (inventory->quantity <= 0 && inventory->itemID != ITEM_NONE)
    {
        inventory->itemID = ITEM_NONE;
        inventory->quantity = 0;
        Scene.activeModel[inventory->modelID] = false;
        inventory->modelID = -1;
    }

    return space;
}

int giveInventory(Inventory *inventory, uint8_t itemID, int quantity)
{
    return giveInventoryPos(inventory, itemID, quantity, 0, 0, 0);
}

bool createTree(float x, float z, uint8_t itemType)
{
    for (int i = 0; i < MAX_TREES; i++)
    {
        if (!trees[i].active)
        {
            float yaw = rand() / (float)RAND_MAX * 2.0f * M_PI;
            int id = createModel(x, getHeightAt(x, z), z, 0, RAD2ANG(yaw), 0, plant_0_bin, Plant0Material);
            if (id != -1)
            {
                trees[i].active = true;
                trees[i].modelID = id;
                trees[i].x = x;
                trees[i].y = getHeightAt(x, z);
                trees[i].z = z;
                trees[i].yaw = yaw;
                trees[i].itemType = itemType;
                trees[i].ageTime = time(NULL);
                trees[i].level = 0;
                trees[i].inventory.itemID = ITEM_NONE;
                trees[i].inventory.quantity = 0;
                trees[i].water = 67;
                return true;
            }
            return false;
        }
    }
    return false;
}

uint32_t oldTimer = 0;

void updateTree(Tree *tree, uint8_t id)
{
    if (tree->ageTime + (int)TREE_TRANSITION_TIME < time(NULL) && tree->water > 0)
    {
        if (tree->level < 3)
        {
            tree->level++;
            tree->ageTime = time(NULL);
            switch (tree->level)
            {
            case 1:
                Scene.activeModel[tree->modelID] = false;
                tree->modelID = createModel(tree->x, tree->y, tree->z, 0, RAD2ANG(tree->yaw), 0, plant_1_bin, Plant1Material);
                break;
            case 2:
                Scene.activeModel[tree->modelID] = false;
                tree->modelID = createModel(tree->x, tree->y, tree->z, 0, RAD2ANG(tree->yaw), 0, plant_2_bin, Plant2Material);
                break;
            case 3:
                Scene.activeModel[tree->modelID] = false;
                tree->modelID = createModel(tree->x, tree->y, tree->z, 0, RAD2ANG(tree->yaw), 0, tree_bin, TreeMaterial);
                break;
            }
        }
        else
        {
            const float treeItemPos[3][3] =
                {
                    {0.1f, 0.1f, 0.1f},
                    {0.2f, 0.2f, 0.2f},
                    {0.3f, 0.3f, 0.3f},
                };
            giveInventoryPos(&tree->inventory, tree->itemType, 1, tree->x + treeItemPos[tree->inventory.quantity][0] * sinf(tree->yaw), tree->y + treeItemPos[tree->inventory.quantity][1], tree->z + treeItemPos[tree->inventory.quantity][2] * cosf(tree->yaw));
        }
    }
    if (tree->water > 0 && time(NULL) != oldTime && tree->inventory.quantity < 3)
    {
        tree->water--;
        oldTimer = time(NULL);
    }
    if (tree->level > 0 && tree->level < 3)
    {
        float diff = ((time(NULL) - tree->ageTime) / TREE_TRANSITION_TIME) * GROWTH_FACTOR;
        NE_ModelScale(Scene.Model[tree->modelID], MODEL_SCALE * (1 + diff), MODEL_SCALE * (1 + diff), MODEL_SCALE * (1 + diff));
    }
}

bool createItem(float x, float z, uint8_t itemID, uint8_t quantity)
{
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (!items[i].active)
        {
            int id = createModel(x, getHeightAt(x, z), z, 0, rando(0, 512), 0, itemModels[itemID].model, itemModels[itemID].mat);
            if (id != -1)
            {
                items[i].active = true;
                items[i].modelID = id;
                items[i].x = x;
                items[i].y = getHeightAt(x, z);
                items[i].z = z;
                items[i].inventory.itemID = itemID;
                items[i].inventory.quantity = quantity;
                return true;
            }
            return false;
        }
    }
    return false;
}

void destroyItem(int id)
{
    items[id].active = false;
    Scene.activeModel[items[id].modelID] = false;
    return;
}

void addItemQuantity(uint8_t id, uint8_t quantity)
{
    items[id].inventory.quantity += quantity;
    if (items[id].inventory.quantity <= 0)
        destroyItem(id);
    return;
}

bool spawnNpc(float x, float z)
{
    for (int i = 0; i < MAX_NPCS; i++)
    {
        if (!npcs[i].active)
        {
            int id = createModel(x, getHeightAt(x, z), z, 0, 0, 0, npc_bin, NpcMaterial);
            if (id != -1)
            {
                npcs[i].active = true;
                npcs[i].modelID = id;
                npcs[i].x = x;
                npcs[i].y = getHeightAt(x, z);
                npcs[i].z = z;
                npcs[i].target = TARGET_RANDOM;
                npcs[i].inventory.itemID = ITEM_NONE;
                npcs[i].inventory.quantity = 0;
                npcs[i].inventory.modelID = -1;
                strcpy(npcs[i].name, names[rando(0, NAMES)]);
                return true;
            }
            return false;
        }
    }
    return false;
}

void updateNpc(Npc *npc, uint8_t id)
{
    if (selectionType == SELECTION_NPC && selectionParam == id)
    {
        float diff = atan2f(player.x - npc->x, player.z - npc->z) - npc->yaw;
        diff -= 2.0f * M_PI * floorf((diff + M_PI) / (2.0f * M_PI)); // wrap to [-pi, pi]
        npc->yaw += clamp(diff, -0.1f, 0.1f);
        npc->speed = 0.0f;
    }
    else
    {
        if (npc->target == TARGET_PLAYER)
        {
            npc->yaw = atan2f(player.x - npc->x, player.z - npc->z);
            npc->speed = 0.01f;
        }
        else if (npc->target == TARGET_RANDOM)
        {
            if (frames % F_SECOND * 5 == 0)
            {
                npc->yaw = (float)rand() / RAND_MAX * 2.0f * M_PI;
            }
            npc->speed = 0.01f;
        }
    }
    npc->yaw = fmodf(npc->yaw, 2.0f * (float)M_PI);

    moveForward(&npc->x, &npc->z, npc->yaw, npc->speed, id);

    npc->y = getHeightAt(npc->x, npc->z);

    // Update the model's position

    NE_ModelSetCoord(Scene.Model[npc->modelID], npc->x, npc->y, npc->z);
    NE_ModelSetRot(Scene.Model[npc->modelID], 0, RAD2ANG(npc->yaw), 0);
}

void syncHeldItem(float x, float y, float z, float yaw, float pitch, int modelID, float dist)
{
    if (modelID == -1)
        return;

    float newX = x + sinf(yaw) * cosf(pitch) * dist;
    float newZ = z + cosf(yaw) * cosf(pitch) * dist; // multiply pitch so we feel the pitch :)
    float newY = y + 0.04f + sinf(pitch * 0.8f) * dist;

    newY = max(newY, getHeightAt(newX, newZ));

    NE_ModelSetCoord(Scene.Model[modelID], newX, newY, newZ);
    NE_ModelSetRot(Scene.Model[modelID], 0, RAD2ANG(yaw), 0);
}

void setHighlightedModel(int id)
{
    if (id == -1)
    {
        Scene.activeModel[highlightedModelID] = false;
        highlightedModelID = -1;
        return;
    }
    if (highlightedModelID == -1)
    {
        highlightedModelID = createModel(0, 0, 0, 0, 0, 0, Scene.modelsRef[id], HighlightMaterial);
    }
    else
    {
        Scene.activeModel[highlightedModelID] = false;
        highlightedModelID = createModel(0, 0, 0, 0, 0, 0, Scene.modelsRef[id], HighlightMaterial);
    }
    NE_ModelScale(Scene.Model[highlightedModelID], Scene.Model[id]->sx / 4096.0f * 1.08f, Scene.Model[id]->sy / 4096.0f * 1.08f, Scene.Model[id]->sz / 4096.0f * 1.08f);
    highlightedModel = id;
}

void alert(const char *message)
{
    strcpy(alertText, message);
    alertTime = time(NULL) + 5;
}