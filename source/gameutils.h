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

/*

    // Load model
    for (int i = 0; i < NUM_MODELS; i++)
        NE_ModelLoadStaticMesh(Scene.Model[i], cube_bin);

    for (int i = 0; i < NUM_MODELS; i++)
    {
        NE_ModelSetRot(Scene.Model[i], i, i * 30, i * 20);
        NE_ModelSetCoord(Scene.Model[i], 0, i % 4, i / 4);
    }
*/

bool createModel(float x, float y, float z, float pitch, float yaw, float roll, const void *data, NE_Material *mat)
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
            return true;
        }
    }
}

bool createTree(float x, float z)
{
    for (int i = 0; i < MAX_TREES; i++)
    {
        if (!trees[i].active)
        {
            if (createModel(x, getHeightAt(x, z), z, 0, 0, 0, cube_bin, TreeMaterial))
            {
                trees[i].active = true;
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