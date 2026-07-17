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

void moveForward(float *x, float *z, float yaw, float speed)
{
    x[0] += sinf(yaw) * speed * delta;
    z[0] += cosf(yaw) * speed * delta;

    if (x[0] > (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE)
        x[0] = (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE;
    else if (x[0] < -(TERRAIN_SIZE / 2.0f) * SCALE)
        x[0] = -(TERRAIN_SIZE / 2.0f) * SCALE;

    if (z[0] > (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE)
        z[0] = (TERRAIN_SIZE / 2.0f - 1.0f) * SCALE;
    else if (z[0] < -(TERRAIN_SIZE / 2.0f) * SCALE)
        z[0] = -(TERRAIN_SIZE / 2.0f) * SCALE;
}

int createModel(float x, float y, float z, float pitch, float yaw, float roll, const void *data, NE_Material *mat)
{
    for (int i = 0; i < NUM_MODELS; i++)
    {
        if (!Scene.activeModel[i])
        {
            Scene.activeModel[i] = true;
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

bool createTree(float x, float z)
{
    for (int i = 0; i < MAX_TREES; i++)
    {
        if (!trees[i].active)
        {
            int id = createModel(x, getHeightAt(x, z), z, 0, rando(0, 512), 0, tree_bin, TreeMaterial);
            if (id != -1)
            {
                trees[i].active = true;
                trees[i].modelID = id;
                trees[i].x = x;
                trees[i].y = getHeightAt(x, z);
                trees[i].z = z;
                return true;
            }
            return false;
        }
    }
    return false;
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
                return true;
            }
            return false;
        }
    }
    return false;
}

void updateNpc(Npc *npc)
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

    moveForward(&npc->x, &npc->z, npc->yaw, npc->speed);

    npc->y = getHeightAt(npc->x, npc->z);

    // Update the model's position

    NE_ModelSetCoord(Scene.Model[npc->modelID], npc->x, npc->y, npc->z);
    NE_ModelSetRot(Scene.Model[npc->modelID], 0, RAD2ANG(npc->yaw), 0);
}