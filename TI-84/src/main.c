#include <graphx.h>
#include <keypadc.h>
#include <sys/timers.h>

/* TODO: (priority top to bottom)
- Make maze generator
*/

typedef enum {
        SHOW_MAIN_MENU,
        SHOW_HELP_MENU,
        RUN_GAME,
        SHOW_WIN_MENU,
} GameState;

typedef struct {
        int center;
        int left;
        int right;
        int up;
        int down;
        int upLeft;
        int upRight;
        int downLeft;
        int downRight;
} Tiles;

typedef enum {
        UP,
        RIGHT,
        DOWN,
        LEFT,
} Direction;

typedef struct {
        int screenWidth;
        int screenHeight;
        int posX;
        int posY;
        int winX;
        int winY;
        int editMode;
        int darkMode;
        int runIn3d;
        Direction dir;
} GameData;

typedef struct {
        int margin;
        int paddingHoriz;
        int paddingVert;
        int textX;
        int rectWidth;
} WindowConfig;

void toggle(int *value) {
        *value = 1 - *value;
}

void updateDarkModeText(int darkMode) {
        gfx_SetTextFGColor((darkMode) ? 254 : 0);
        gfx_SetTextBGColor((darkMode) ? 0 : 255);
}

void updateDarkModeBG(int darkMode) {
        gfx_FillScreen((darkMode) ? 0 : 255);
        gfx_SetColor((darkMode) ? 255 : 0);
}

void checkBounds(int arrayWidth, int arrayHeight, int array[][arrayWidth], Tiles *tiles, int posX, int posY) {
        tiles->center = array[posY][posX];

        if (posX > 0) {
                tiles->left = array[posY][posX - 1];
        }
        if (posX < arrayWidth) {
                tiles->right = array[posY][posX + 1];
        }
        if (posY > 0) {
                tiles->up = array[posY - 1][posX];
        }
        if (posY < arrayHeight) {
                tiles->down = array[posY + 1][posX];
        }
        if (posX > 0 && posY > 0) {
                tiles->upLeft = array[posY - 1][posX - 1];
        }
        if (posX < arrayWidth && posY > 0) {
                tiles->upRight = array[posY - 1][posX + 1];
        }
        if (posX > 0 && posY < arrayHeight) {
                tiles->downLeft = array[posY + 1][posX - 1];
        }
        if (posX < arrayWidth && posY < arrayHeight) {
                tiles->downRight = array[posY + 1][posX + 1];
        }
}

void pointInDirection(int dir, Tiles *tiles, Tiles *relative) {
        relative->center = tiles->center;

        relative->left = tiles->left;
        relative->right = tiles->right;
        relative->up = tiles->up;
        relative->down = tiles->down;
        relative->upLeft = tiles->upLeft;
        relative->upRight = tiles->upRight;
        relative->downLeft = tiles->downLeft;
        relative->downRight = tiles->downRight;

        switch (dir) {
        case LEFT:
                relative->left = tiles->down;
                relative->right = tiles->up;
                relative->up = tiles->left;
                relative->down = tiles->right;
                relative->upLeft = tiles->downLeft;
                relative->upRight = tiles->upLeft;
                relative->downLeft = tiles->downRight;
                relative->downRight = tiles->upRight;
                break;
        case RIGHT:
                relative->left = tiles->up;
                relative->right = tiles->down;
                relative->up = tiles->right;
                relative->down = tiles->left;
                relative->upLeft = tiles->upRight;
                relative->upRight = tiles->downRight;
                relative->downLeft = tiles->upLeft;
                relative->downRight = tiles->downLeft;
                break;
        case UP:
                // Up orientation is normal, so keep the values the same
                break;
        case DOWN:
                relative->left = tiles->right;
                relative->right = tiles->left;
                relative->up = tiles->down;
                relative->down = tiles->up;
                relative->upLeft = tiles->downRight;
                relative->upRight = tiles->downLeft;
                relative->downLeft = tiles->upRight;
                relative->downRight = tiles->upLeft;
                break;
        }
}

void render2d(Tiles *relative, GameData *gameData) {
        int screenWidth = gameData->screenWidth;
        int screenHeight = gameData->screenHeight;

        int wallWidth = screenWidth / 3 - 1;
        int wallHeight = screenHeight / 3 - 1;

        updateDarkModeBG(gameData->darkMode);
        updateDarkModeText(gameData->darkMode);

        if ((relative->left && relative->center == 0) || (relative->left == 0 && relative->center))
                gfx_VertLine_NoClip((screenWidth - wallWidth) >> 1, (screenHeight - wallHeight) >> 1, wallHeight);

        if ((relative->left == 0 && relative->upLeft) || (relative->left && relative->upLeft == 0))
                gfx_HorizLine_NoClip(((screenWidth - wallWidth) >> 1) - wallWidth, (screenHeight - wallHeight) >> 1, wallWidth);
        if ((relative->left == 0 && relative->downLeft) || (relative->left && relative->downLeft == 0))
                gfx_HorizLine_NoClip(((screenWidth - wallWidth) >> 1) - wallWidth, (screenHeight + wallHeight) >> 1, wallWidth);

        if ((relative->right && relative->center == 0) || (relative->right == 0 && relative->center))
                gfx_VertLine_NoClip((screenWidth + wallWidth) >> 1, (screenHeight - wallHeight) >> 1, wallHeight);

        if ((relative->right == 0 && relative->upRight) || (relative->right && relative->upRight == 0))
                gfx_HorizLine_NoClip((screenWidth + wallWidth) >> 1, (screenHeight - wallHeight) >> 1, wallWidth);
        if ((relative->right == 0 && relative->downRight) || (relative->right && relative->downRight == 0))
                gfx_HorizLine_NoClip((screenWidth + wallWidth) >> 1, (screenHeight + wallHeight) >> 1, wallWidth);

        if ((relative->up && relative->center == 0) || (relative->up == 0 && relative->center))
                gfx_HorizLine_NoClip((screenWidth - wallWidth) >> 1, (screenHeight - wallHeight) >> 1, wallWidth);

        if ((relative->up == 0 && relative->upLeft) || (relative->up && relative->upLeft == 0))
                gfx_VertLine_NoClip((screenWidth - wallWidth) >> 1, ((screenHeight - wallHeight) >> 1) - wallHeight, wallHeight);
        if ((relative->up == 0 && relative->upRight) || (relative->up && relative->upRight == 0))
                gfx_VertLine_NoClip((screenWidth + wallWidth) >> 1, ((screenHeight - wallHeight) >> 1) - wallHeight, wallHeight);

        if ((relative->down && relative->center == 0) || (relative->down == 0 && relative->center))
                gfx_HorizLine_NoClip((screenWidth - wallWidth) >> 1, (screenHeight + wallHeight) >> 1, wallWidth);

        if ((relative->down == 0 && relative->downLeft) || (relative->down && relative->downLeft == 0))
                gfx_VertLine_NoClip((screenWidth - wallWidth) >> 1, (screenHeight + wallHeight) >> 1, wallHeight);
        if ((relative->down == 0 && relative->downRight) || (relative->down && relative->downRight == 0))
                gfx_VertLine_NoClip((screenWidth + wallWidth) >> 1, (screenHeight + wallHeight) >> 1, wallHeight);
}

void render3d(Tiles *relative, GameData *gameData) {
        int screenWidth = gameData->screenWidth;
        int screenHeight = gameData->screenHeight;

        int wallWidth = screenWidth / 8 - 1;
        int wallHeight = screenHeight / 8 - 1;

        int screenWidthMid = screenWidth >> 1;
        int screenHeightMid = screenHeight >> 1;

        updateDarkModeBG(gameData->darkMode);
        updateDarkModeText(gameData->darkMode);

        // Big lines separating left and upLeft & right and upRight
        if ((((relative->left == 0 && relative->upLeft) || (relative->left && relative->upLeft == 0)) && relative->up == 0) ||
              (relative->up && relative->left))
                gfx_VertLine_NoClip(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2, wallHeight * 4);

        if ((((relative->right == 0 && relative->upRight) || (relative->right && relative->upRight == 0)) && relative->up == 0) ||
               (relative->up && relative->right))
                gfx_VertLine_NoClip(screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2, wallHeight * 4);

        // left open
        if (relative->left == 0) {
                if (relative->upLeft) {
                        gfx_HorizLine_NoClip(screenWidthMid - wallWidth * 4, screenHeightMid - wallHeight * 2, wallWidth * 2);
                        gfx_HorizLine_NoClip(screenWidthMid - wallWidth * 4, screenHeightMid + wallHeight * 2, wallWidth * 2);
                }
                else {
                        gfx_HorizLine_NoClip(screenWidthMid - wallWidth * 4, screenHeightMid - wallHeight, wallWidth * 2);
                        gfx_HorizLine_NoClip(screenWidthMid - wallWidth * 4, screenHeightMid + wallHeight, wallWidth * 2);
                }
        }
        else {
                gfx_Line_NoClip(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2, screenWidthMid - wallWidth * 4, screenHeightMid - wallHeight * 4);
                gfx_Line_NoClip(screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight * 2, screenWidthMid - wallWidth * 4, screenHeightMid + wallHeight * 4);
        }

        // right open
        if (relative->right == 0) {
                if (relative->upRight) {
                        gfx_HorizLine_NoClip(screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2, wallWidth * 2);
                        gfx_HorizLine_NoClip(screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight * 2, wallWidth * 2);
                }
                else {
                        gfx_HorizLine_NoClip(screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight, wallWidth * 2);
                        gfx_HorizLine_NoClip(screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight, wallWidth * 2);
                }
        }
        else {
                gfx_Line_NoClip(screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2, screenWidthMid + wallWidth * 4, screenHeightMid - wallHeight * 4);
                gfx_Line_NoClip(screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight * 2, screenWidthMid + wallWidth * 4, screenHeightMid + wallHeight * 4);
        }

        // up open
        if (relative->up == 0) {
                gfx_HorizLine_NoClip(screenWidthMid - wallWidth, screenHeightMid - wallHeight, wallWidth * 2);
                gfx_HorizLine_NoClip(screenWidthMid - wallWidth, screenHeightMid + wallHeight, wallWidth * 2);

                if (relative->upLeft)
                        gfx_VertLine_NoClip(screenWidthMid - wallWidth, screenHeightMid - wallHeight, wallHeight * 2);
                if (relative->upRight)
                        gfx_VertLine_NoClip(screenWidthMid + wallWidth, screenHeightMid - wallHeight, wallHeight * 2);
        }
        else {
                gfx_HorizLine_NoClip(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2, wallWidth * 4);
                gfx_HorizLine_NoClip(screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight * 2, wallWidth * 4);
        }

        // up left open
        if (relative->upLeft == 0 && relative->up == 0) {
                gfx_HorizLine_NoClip(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight, wallWidth);
                gfx_HorizLine_NoClip(screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight, wallWidth);
        }
        else if (relative->up == 0) {
                gfx_Line_NoClip(screenWidthMid - wallWidth, screenHeightMid - wallHeight, screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2);
                gfx_Line_NoClip(screenWidthMid - wallWidth, screenHeightMid + wallHeight, screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight * 2);
        }

        // up right open
        if (relative->upRight == 0 && relative->up == 0) {
                gfx_HorizLine_NoClip(screenWidthMid + wallWidth, screenHeightMid - wallHeight, wallWidth);
                gfx_HorizLine_NoClip(screenWidthMid + wallWidth, screenHeightMid + wallHeight, wallWidth);
        }
        else if (relative->up == 0) {
                gfx_Line_NoClip(screenWidthMid + wallWidth, screenHeightMid - wallHeight, screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2);
                gfx_Line_NoClip(screenWidthMid + wallWidth, screenHeightMid + wallHeight, screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight * 2);
        }
}

int processInput(int arrayWidth, int arrayHeight, int array[][arrayWidth], Tiles *relative, GameData *gameData) {
        int posX = gameData->posX;
        int posY = gameData->posY;

        kb_Scan();

        int kb3 = kb_Data[3];
        int kb4 = kb_Data[4];
        int kb5 = kb_Data[5];
        int kb6 = kb_Data[6];
        int kb7 = kb_Data[7];

        // Keyboard toggles
        if (kb_Data[1] & kb_2nd) {
                toggle(&(gameData->editMode));
        }
        else if (kb_Data[2] & kb_Alpha) {
                toggle(&(gameData->darkMode));
        }
        else if (kb_Data[1] & kb_Mode) {
                toggle(&(gameData->runIn3d));
        }

        else if (kb4 & kb_LParen) {
                if (gameData->dir == UP)
                        gameData->dir = LEFT;
                else
                        gameData->dir--;
        }
        else if (kb5 & kb_RParen) {
                gameData->dir++;
                gameData->dir %= LEFT + 1;
        }

        else if (kb7 & kb_Left) {
                if (relative->left == 0 || (gameData->editMode && relative->left != 2)) {
                        switch (gameData->dir) {
                        case LEFT:
                                gameData->posY += 1;
                                break;
                        case RIGHT:
                                gameData->posY -= 1;
                                break;
                        case UP:
                                gameData->posX -= 1;
                                break;
                        case DOWN:
                                gameData->posX += 1;
                                break;
                        }
                }
        }
        else if (kb7 & kb_Right) {
                if (relative->right == 0 || (gameData->editMode && relative->right != 2)) {
                        switch (gameData->dir) {
                        case LEFT:
                                gameData->posY -= 1;
                                break;
                        case RIGHT:
                                gameData->posY += 1;
                                break;
                        case UP:
                                gameData->posX += 1;
                                break;
                        case DOWN:
                                gameData->posX -= 1;
                                break;
                        }
                }
        }
        else if (kb7 & kb_Up) {
                if (relative->up == 0 || (gameData->editMode && relative->up != 2)) {
                        switch (gameData->dir) {
                        case LEFT:
                                gameData->posX -= 1;
                                break;
                        case RIGHT:
                                gameData->posX += 1;
                                break;
                        case UP:
                                gameData->posY -= 1;
                                break;
                        case DOWN:
                                gameData->posY += 1;
                                break;
                        }
                }
        }
        else if (kb7 & kb_Down) {
                if (relative->down == 0 || (gameData->editMode && relative->down != 2)) {
                        switch (gameData->dir) {
                        case LEFT:
                                gameData->posX += 1;
                                break;
                        case RIGHT:
                                gameData->posX -= 1;
                                break;
                        case UP:
                                gameData->posY += 1;
                                break;
                        case DOWN:
                                gameData->posY -= 1;
                                break;
                        }
                }
        }

        if (kb6 & kb_Clear) {
                return -1;
        }

        else if (gameData->editMode) {
                if (kb3 & kb_4 && relative->left != 2) {
                        switch (gameData->dir) {
                        case LEFT:
                                toggle(&(array[posY + 1][posX]));
                                break;
                        case RIGHT:
                                toggle(&(array[posY - 1][posX]));
                                break;
                        case UP:
                                toggle(&(array[posY][posX - 1]));
                                break;
                        case DOWN:
                                toggle(&(array[posY][posX + 1]));
                                break;
                        }
                }
                else if (kb5 & kb_6 && relative->right != 2) {
                        switch (gameData->dir) {
                        case LEFT:
                                toggle(&(array[posY - 1][posX]));
                                break;
                        case RIGHT:
                                toggle(&(array[posY + 1][posX]));
                                break;
                        case UP:
                                toggle(&(array[posY][posX + 1]));
                                break;
                        case DOWN:
                                toggle(&(array[posY][posX - 1]));
                                break;
                        }
                }
                else if (kb4 & kb_8 && relative->up != 2) {
                        switch (gameData->dir) {
                        case LEFT:
                                toggle(&(array[posY][posX - 1]));
                                break;
                        case RIGHT:
                                toggle(&(array[posY][posX + 1]));
                                break;
                        case UP:
                                toggle(&(array[posY - 1][posX]));
                                break;
                        case DOWN:
                                toggle(&(array[posY + 1][posX]));
                                break;
                        }
                }
                else if (kb4 & kb_2 && relative->down != 2) {
                        switch (gameData->dir) {
                        case LEFT:
                                toggle(&(array[posY][posX + 1]));
                                break;
                        case RIGHT:
                                toggle(&(array[posY][posX - 1]));
                                break;
                        case UP:
                                toggle(&(array[posY + 1][posX]));
                                break;
                        case DOWN:
                                toggle(&(array[posY - 1][posX]));
                                break;
                        }
                }
                else if (kb3 & kb_7 && relative->upLeft != 2) {
                        switch (gameData->dir) {
                        case LEFT:
                                toggle(&(array[posY + 1][posX - 1]));
                                break;
                        case RIGHT:
                                toggle(&(array[posY - 1][posX + 1]));
                                break;
                        case UP:
                                toggle(&(array[posY - 1][posX - 1]));
                                break;
                        case DOWN:
                                toggle(&(array[posY + 1][posX + 1]));
                                break;
                        }
                }
                else if (kb5 & kb_9 && relative->upRight != 2) {
                        switch (gameData->dir) {
                        case LEFT:
                                toggle(&(array[posY - 1][posX - 1]));
                                break;
                        case RIGHT:
                                toggle(&(array[posY + 1][posX + 1]));
                                break;
                        case UP:
                                toggle(&(array[posY - 1][posX + 1]));
                                break;
                        case DOWN:
                                toggle(&(array[posY + 1][posX - 1]));
                                break;
                        }
                }
                else if (kb3 & kb_1 && relative->downLeft != 2) {
                        switch (gameData->dir) {
                        case LEFT:
                                toggle(&(array[posY + 1][posX + 1]));
                                break;
                        case RIGHT:
                                toggle(&(array[posY - 1][posX - 1]));
                                break;
                        case UP:
                                toggle(&(array[posY + 1][posX - 1]));
                                break;
                        case DOWN:
                                toggle(&(array[posY - 1][posX + 1]));
                                break;
                        }
                }
                else if (kb5 & kb_3 && relative->downRight != 2) {
                        switch (gameData->dir) {
                        case LEFT:
                                toggle(&(array[posY - 1][posX + 1]));
                                break;
                        case RIGHT:
                                toggle(&(array[posY + 1][posX - 1]));
                                break;
                        case UP:
                                toggle(&(array[posY + 1][posX + 1]));
                                break;
                        case DOWN:
                                toggle(&(array[posY - 1][posX - 1]));
                                break;
                        }
                }

                else if (kb3 & kb_0) {
                        for (int i = 1; i < arrayHeight - 1; i++) {
                                for (int j = 1; j < arrayWidth - 1; j++) {
                                        array[i][j] = 0;
                                }
                        }
                        gameData->posX = gameData->posY = 1;
                        gameData->winX = gameData->winY = 7;
                }
                else if (kb4 & kb_DecPnt) {
                        gameData->winX = gameData->posX;
                        gameData->winY = gameData->posY;
                }
        }

        return 0;
}

int run(GameData *gameData, int arrayWidth, int arrayHeight, int array[][arrayWidth]) {
        Tiles tiles;
        tiles.center = tiles.left = tiles.right = tiles.up = tiles.down = tiles.upLeft = tiles.upRight = tiles.downLeft = tiles.downRight = 0;

        checkBounds(arrayWidth, arrayHeight, array, &(tiles), gameData->posX, gameData->posY);

        Tiles relative;
        pointInDirection(gameData->dir, &(tiles), &(relative));

        if (processInput(arrayWidth, arrayHeight, array, &(relative), gameData) != 0)
                return -1; // Go back to main menu

        if (gameData->runIn3d)
                render3d(&(relative), gameData);
        else
                render2d(&(relative), gameData);

        // Text in top left corner to signify edit mode
        if (gameData->editMode) {
                updateDarkModeText(gameData->darkMode);
                gfx_PrintStringXY("EDIT", 5, 5);
        }

        gfx_BlitBuffer();

        delay(150);

        // Win maze
        if (gameData->posX == gameData->winX && gameData->posY == gameData->winY && !gameData->editMode)
                return 1;

        return 0;
}

void showInfoWindow(char *text[], int textLen, GameData *gameData, WindowConfig *windowData, GameState *gameState) {
        int screenHeight = gameData->screenHeight;

        int margin = windowData->margin;
        int paddingVert = windowData->paddingVert;

        int textX = windowData->textX;

        int rectWidth = windowData->rectWidth;

        kb_Scan();
        if (kb_Data[6] & kb_Clear)
                *gameState = SHOW_MAIN_MENU;

        updateDarkModeBG(gameData->darkMode);
        updateDarkModeText(gameData->darkMode);

        gfx_Rectangle_NoClip(margin, margin, rectWidth, screenHeight - (margin << 1));
        int cursorPos = margin + paddingVert;

        for (int i = 0; i < textLen; i++) {
                gfx_PrintStringXY(text[i], textX, cursorPos);
                cursorPos += paddingVert;
        }

        gfx_BlitBuffer();

        delay(300);
}

int showStartMenu(GameData *gameData, WindowConfig *windowData, int *selectedOption, GameState *gameState) {
        int screenHeight = gameData->screenHeight;

        int margin = windowData->margin;
        int paddingHoriz = windowData->paddingHoriz;
        int paddingVert = windowData->paddingVert;

        int textX = windowData->textX;

        int rectWidth = windowData->rectWidth;

        char *menuOptions[] = {
                "PLAY",
                "TOGGLE EDIT MODE",
                "TOGGLE DARK MODE",
                "TOGGLE 2D/3D MODE",
                "HELP/KEYBINDS",
                "EXIT",
        };
        int menuOptionsLen = sizeof(menuOptions) / sizeof(menuOptions[0]);

        int status = 0;

        kb_Scan();

        int kbArrow = kb_Data[7];
        if (kbArrow & kb_Up && selectedOption > 0) {
                *selectedOption -= 1;
        }
        else if (kbArrow & kb_Down && *selectedOption < menuOptionsLen - 1) {
                *selectedOption += 1;
        }
        else if (kb_Data[6] & kb_Enter) {
                switch (*selectedOption) {
                case 0:
                        // Reset player position
                        gameData->posX = gameData->posY = 1;
                        *gameState = RUN_GAME;
                        break;
                case 1:
                        toggle(&(gameData->editMode));
                        break;
                case 2:
                        toggle(&(gameData->darkMode));
                        break;
                case 3:
                        toggle(&(gameData->runIn3d));
                        break;
                case 4:
                        *gameState = SHOW_HELP_MENU;
                        break;
                case 5:
                        status = -1;
                        break;
                }
        }

        updateDarkModeBG(gameData->darkMode);
        updateDarkModeText(gameData->darkMode);

        gfx_Rectangle_NoClip(margin, margin, rectWidth, screenHeight - (margin << 1));

        int cursorPos = margin + paddingVert;

        gfx_PrintStringXY("CMAZE MENU", textX, cursorPos);

        gfx_PrintStringXY((gameData->runIn3d) ? "Running in 3D mode" : "Running in 2D mode", gfx_GetTextX() + paddingHoriz, cursorPos);
        if (gameData->editMode)
                gfx_PrintStringXY("EditMode", gfx_GetTextX() + paddingHoriz, cursorPos);

        cursorPos += paddingVert;

        // Line to separate title and options
        gfx_HorizLine_NoClip(margin, cursorPos, rectWidth);

        cursorPos += paddingVert;

        for (int i = 0; i < menuOptionsLen; i++) {
                if (i == *selectedOption)
                        gfx_PrintStringXY(">", margin + (paddingHoriz >> 1), cursorPos);

                gfx_PrintStringXY(menuOptions[i], textX, cursorPos);
                cursorPos += paddingVert;
        }
        gfx_BlitBuffer();

        delay(150);

        return status;
}

void init(GameData *gameData) {
        gameData->screenWidth = GFX_LCD_WIDTH;
        gameData->screenHeight = GFX_LCD_HEIGHT;

        gameData->posX = gameData->posY = 1;
        gameData->winX = gameData->winY = 7;

        gameData->editMode = 0;
        gameData->darkMode = 1;
        gameData->runIn3d = 1;
        gameData->dir = UP;
}

void initWindowConfig(WindowConfig *windowData, GameData *gameData) {
        windowData->margin = 30;
        windowData->paddingHoriz = 16;
        windowData->paddingVert = 16;

        windowData->textX = windowData->margin + windowData->paddingHoriz;

        windowData->rectWidth = gameData->screenWidth - (windowData->margin << 1);
}

int main() {
        gfx_Begin();
        gfx_SetDrawBuffer();

        gfx_SetTextConfig(gfx_text_noclip);

        GameData gameData;
        init(&(gameData));

        WindowConfig windowData;
        initWindowConfig(&(windowData), &(gameData));

        int array[][9] = {
                {2, 2, 2, 2, 2, 2, 2, 2, 2},
                {2, 0, 1, 0, 0, 0, 1, 0, 2},
                {2, 0, 1, 1, 1, 0, 0, 0, 2},
                {2, 0, 0, 0, 1, 0, 1, 0, 2},
                {2, 1, 1, 0, 1, 0, 1, 0, 2},
                {2, 0, 1, 0, 0, 0, 1, 0, 2},
                {2, 0, 1, 0, 1, 0, 1, 0, 2},
                {2, 0, 0, 0, 0, 0, 1, 0, 2},
                {2, 2, 2, 2, 2, 2, 2, 2, 2},
        };

        int arrayHeight = sizeof(array) / sizeof(array[0]);
        int arrayWidth = sizeof(array[0]) / sizeof(array[0][0]);

        int status = 0;
        int selectedOption = 0;
        GameState gameState = SHOW_MAIN_MENU;

        while (status == 0) {
                if (gameState == SHOW_MAIN_MENU) {
                        gfx_SetTextScale(1, 1);
                        status = showStartMenu(&(gameData), &(windowData), &selectedOption, &gameState);
                        if (status == -1)
                                break;
                }
                else if (gameState == SHOW_HELP_MENU) {
                        gfx_SetTextScale(1, 1);
                        char *help[] = {
                                "Arrow keys to move",
                                "Bracket keys or v,b to turn",
                                "'1' to toggle edit mode",
                                "'2' to toggle dark mode",
                                "'3' to switch to 2D/3D",
                                "'backspace' in-game to return to main menu",
                                "",
                                "In edit mode:",
                                "",
                                "'0' to empty the maze",
                                "'9' to change win position",
                                "q,w,e, a,s,d, z,x,c to place/remove maze tiles",
                                "",
                                "Press 'backspace' to go back",
                        };
                        int helpLen = sizeof(help) / sizeof(help[0]);
                        showInfoWindow(help, helpLen, &(gameData), &(windowData), &gameState);
                }
                else if (gameState == RUN_GAME) {
                        gfx_SetTextScale(2, 2);

                        status = run(&(gameData), arrayWidth, arrayHeight, array);

                        if (status == -1)
                                gameState = SHOW_MAIN_MENU;
                        else if (status == 1) {
                                gameState = SHOW_WIN_MENU;
                        }
                }
                else if (gameState == SHOW_WIN_MENU) {
                        gfx_SetTextScale(1, 1);

                        char *winText[] = {
                                "You got out of the maze!",
                                "Press esc to exit",
                        };
                        int winTextLen = sizeof(winText) / sizeof(winText[0]);
                        showInfoWindow(winText, winTextLen, &(gameData), &(windowData), &gameState);
                }
        }

        kb_Reset();
        gfx_End();

        return 0;
}
