typedef void (*function_t)(uint8_t choice);

typedef struct
{
    char title[32];
    char items[8][32];
    uint8_t itemCount;
    function_t function;
    uint8_t choice;
} Menu;

int activeMenu = -1;

void phoneMenuHandler(uint8_t choice)
{
    switch (choice)
    {
    case 1:
        activeMenu = 1;
        return;
    case 2:
        activeMenu = 2;
        return;
    }
    activeMenu = -1;
}

void workerMenuHandler(uint8_t choice)
{
    switch (choice)
    {
    case 0:
        if (playerPay(100))
        {
            int id = spawnNpc(0, 0);
            if (id != -1)
                alert("Hired new worker, %s!", npcs[id].name);
            else
            {
                alert("Maximum number of workers reached (%d)!", MAX_NPCS);
                playerPay(-100);
            }
        } else
            alert("You don't have enough funds!");
        break;
    case 1:
        bool found = false;
        for (int i = MAX_NPCS - 1; i >= 0; i--)
        {
            if (npcs[i].active)
            {
                found = true;
                if (npcs[i].inventory.itemID == ITEM_NONE)
                {
                    npcs[i].active = false;
                    Scene.activeModel[npcs[i].modelID] = false;
                    alert("Dismissed %s...", npcs[i].name);
                    activeMenu = -1;
                    return;
                }
            }
        }
        if (!found)
            alert("No workers to dismiss!");
        else
            alert("Can't find a worker with an empty inventory to dismiss!");
        break;
    }
    activeMenu = -1;
}

void shopMenuHandler(uint8_t choice)
{
    switch (choice)
    {
    case 0:
        activeMenu = 3;
        return;
    case 1:
        activeMenu = 4;
        return;
    }
    activeMenu = -1;
}

typedef struct
{
    uint8_t itemID;
    int32_t price;
} ShopItem;

const ShopItem buyItems[] = {
    {ITEM_APPLE_SEED_PACK, 1},
    {ITEM_ORANGE_SEED_PACK, 3},
};

const ShopItem sellItems[] = {
    {ITEM_APPLE, 1},
    {ITEM_ORANGE, 3},
};

void buyMenuHandler(uint8_t choice)
{
    if (playerPay(buyItems[choice].price))
    {
        if (createItem(0, 0, 0, buyItems[choice].itemID, 1) != -1)
            alert("1 %s delivered to the spawn point!", itemNames[buyItems[choice].itemID]);
        else
        {
            playerPay(-buyItems[choice].price);
            alert("Maximum dropped items limit reached (%d)!", MAX_ITEMS);
        }
    }
    else
        alert("You don't have enough funds!");
}

void sellMenuHandler(uint8_t choice)
{
    if (player.inventory.itemID == sellItems[choice].itemID)
    {
        giveInventory(&player.inventory, player.inventory.itemID, -1);
        playerPay(-sellItems[choice].price);
        alert("1 %s sold for %d$!", itemNames[sellItems[choice].itemID], sellItems[choice].price);
        return;
    }
    for (int i = MAX_ITEMS - 1; i >= 0; i--)
    {
        if (items[i].active && items[i].inventory.itemID == sellItems[choice].itemID)
        {
            addItemQuantity(i, -1);
            playerPay(-sellItems[choice].price);
            alert("1 %s sold for %d$!", itemNames[sellItems[choice].itemID], sellItems[choice].price);
            return;
        }
    }
    alert("You don't have any %ss to sell!", itemNames[sellItems[choice].itemID]);
}

Menu menus[] = {
    {"Phone", {"Stats", "Workers", "Online Shop"}, 3, phoneMenuHandler, 0},
    {"Workers", {"Hire Worker - 100$", "Dismiss Worker"}, 2, workerMenuHandler, 0},
    {"Online Shop", {"Buy Products", "Sell Products"}, 2, shopMenuHandler, 0},
    {"Buy Products", {}, 0, buyMenuHandler, 0},
    {"Sell Products", {}, 0, sellMenuHandler, 0},
};