// Background utilities

void BgSetTile(int x, int y, int tile)
{
    if (x < 0 || x >= 32 || y < 0 || y >= 32)
        return; // Prevent out of bounds access
    bgGetMapPtr(bgsub)[x + y * 32] = tile;
}

void BgSet4Tile(int x, int y, int tile)
{
    BgSetTile(x, y * 2, tile * 2);
    BgSetTile(x, y * 2 + 1, tile * 2 + 1);
}

int cx, cy; // Current cursor position

void clearPrint()
{
    cx = 0;
    cy = 0;
    for (int y = 0; y < SCREEN_HEIGHT / 16; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH / 16; x++)
        {
            BgSet4Tile(x, y, ' '); // Clear the screen
        }
    }
}

void print(int x, int y, const char *text)
{
    while (*text)
    {
        if (*text == '\n') // If we reach a newline character, move to the next line
        {
            x = 0;                          // Reset x to the beginning of the line
            y++;                            // Move to the next line
            if (y > SCREEN_HEIGHT / 16 - 1) // If we reach the bottom of the screen, stop printing
                return;
            text++;
            continue; // Skip the newline character
        }
        BgSet4Tile(x, y, (char)(*text));
        x++;
        if (x > SCREEN_WIDTH / 8 - 1)
        {
            x = 0;
            y++;
        }
        text++;
    }
    cx = x; // Update current cursor position
    cy = y;
}

void printDirect(const char *text)
{
    print(cx, cy, text);
}

void printVal(int x, int y, int value)
{
    char buffer[32];
    itoa(value, buffer, 10);
    print(x, y, buffer);
}

void printValDirect(int value)
{
    char buffer[32];
    itoa(value, buffer, 10);
    printDirect(buffer);
}

void printSmart(int x, int y, const char *text)
{
    const int maxW = SCREEN_WIDTH / 8;
    const int maxH = SCREEN_HEIGHT / 16;

    while (*text)
    {
        if (*text == '\n')
        {
            x = 0;
            y++;
            if (y >= maxH)
                return;
            text++;
            continue;
        }

        int wordLen = 0;
        const char *tmp = text;
        while (*tmp && *tmp != ' ' && *tmp != '\n' && *tmp != '/')
        {
            wordLen++;
            tmp++;
        }

        if (x + wordLen > maxW && x != 0)
        {
            x = 0;
            y++;
            if (y >= maxH)
                return;
        }

        print(x, y, (char[]){*text, '\0'});
        x++;

        if (x >= maxW)
        {
            x = 0;
            y++;
            if (y >= maxH)
                return;
        }

        text++;
    }

    cx = x;
    cy = y;
}

void printSmartDirect(const char *text)
{
    printSmart(cx, cy, text);
}