#include <raylib.h>

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
        UP = 1,
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
        int showFPS;
        Direction dir;
} GameData;

typedef struct {
        int textSize;
        int margin;
        int paddingHoriz;
        int paddingVert;
        int textX;
        int rectWidth;
} WindowConfig;

void toggle(int *value) {
        *value = 1 - *value;
}

Color getDarkModeTextColor(int darkMode) {
        return (darkMode) ? RAYWHITE : BLACK;
}

void updateDarkModeBG(int darkMode) {
        ClearBackground((darkMode) ? BLACK : RAYWHITE);
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
        Color lineColor = getDarkModeTextColor(gameData->darkMode);

        if ((relative->left && relative->center == 0) || (relative->left == 0 && relative->center))
                DrawLine((screenWidth - wallWidth) / 2, (screenHeight - wallHeight) / 2, (screenWidth - wallWidth) / 2, (screenHeight - wallHeight) / 2 + wallHeight, lineColor);

        if ((relative->left == 0 && relative->upLeft) || (relative->left && relative->upLeft == 0))
                DrawLine((screenWidth - wallWidth) / 2 - wallWidth, (screenHeight - wallHeight) / 2, (screenWidth - wallWidth) / 2, (screenHeight - wallHeight) / 2, lineColor);
        if ((relative->left == 0 && relative->downLeft) || (relative->left && relative->downLeft == 0))
                DrawLine((screenWidth - wallWidth) / 2 - wallWidth, (screenHeight + wallHeight) / 2, (screenWidth - wallWidth) / 2, (screenHeight + wallHeight) / 2, lineColor);

        if ((relative->right && relative->center == 0) || (relative->right == 0 && relative->center))
                DrawLine((screenWidth + wallWidth) / 2, (screenHeight - wallHeight) / 2, (screenWidth + wallWidth) / 2, (screenHeight - wallHeight) / 2 + wallHeight, lineColor);

        if ((relative->right == 0 && relative->upRight) || (relative->right && relative->upRight == 0))
                DrawLine((screenWidth + wallWidth) / 2, (screenHeight - wallHeight) / 2, (screenWidth + wallWidth) / 2 + wallWidth, (screenHeight - wallHeight) / 2, lineColor);
        if ((relative->right == 0 && relative->downRight) || (relative->right && relative->downRight == 0))
                DrawLine((screenWidth + wallWidth) / 2, (screenHeight + wallHeight) / 2, ((screenWidth + wallWidth) / 2) + wallWidth, (screenHeight + wallHeight) / 2, lineColor);

        if ((relative->up && relative->center == 0) || (relative->up == 0 && relative->center))
                DrawLine((screenWidth - wallWidth) / 2, (screenHeight - wallHeight) / 2, ((screenWidth - wallWidth) / 2) + wallWidth, (screenHeight - wallHeight) / 2, lineColor);

        if ((relative->up == 0 && relative->upLeft) || (relative->up && relative->upLeft == 0))
                DrawLine((screenWidth - wallWidth) / 2, ((screenHeight - wallHeight) / 2) - wallHeight, (screenWidth - wallWidth) / 2, ((screenHeight - wallHeight) / 2), lineColor);
        if ((relative->up == 0 && relative->upRight) || (relative->up && relative->upRight == 0))
                DrawLine((screenWidth + wallWidth) / 2, (screenHeight - wallHeight) / 2 - wallHeight, (screenWidth + wallWidth) / 2, (screenHeight - wallHeight) / 2, lineColor);

        if ((relative->down && relative->center == 0) || (relative->down == 0 && relative->center))
                DrawLine((screenWidth - wallWidth) / 2, (screenHeight + wallHeight) / 2, (screenWidth - wallWidth) / 2 + wallWidth, (screenHeight + wallHeight) / 2, lineColor);

        if ((relative->down == 0 && relative->downLeft) || (relative->down && relative->downLeft == 0))
                DrawLine((screenWidth - wallWidth) / 2, (screenHeight + wallHeight) / 2, (screenWidth - wallWidth) / 2, (screenHeight + wallHeight) / 2 + wallHeight, lineColor);
        if ((relative->down == 0 && relative->downRight) || (relative->down && relative->downRight == 0))
                DrawLine((screenWidth + wallWidth) / 2, (screenHeight + wallHeight) / 2, (screenWidth + wallWidth) / 2, (screenHeight + wallHeight) / 2 + wallHeight, lineColor);
}

void render3d(Tiles *relative, GameData *gameData) {
        int screenWidth = gameData->screenWidth;
        int screenHeight = gameData->screenHeight;

        int wallWidth = screenWidth / 8 - 1;
        int wallHeight = screenHeight / 8 - 1;

        int screenWidthMid = screenWidth >> 1;
        int screenHeightMid = screenHeight >> 1;

        updateDarkModeBG(gameData->darkMode);
        Color lineColor = getDarkModeTextColor(gameData->darkMode);

        // Big lines separating left and upLeft & right and upRight
        if ((((relative->left == 0 && relative->upLeft) || (relative->left && relative->upLeft == 0)) && relative->up == 0) ||
              (relative->up && relative->left) ||
              (relative->left == 0 && relative->upLeft == 0 && relative->up))
                DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2, screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight * 2, lineColor);

        if ((((relative->right == 0 && relative->upRight) || (relative->right && relative->upRight == 0)) && relative->up == 0) ||
              (relative->up && relative->right) ||
              (relative->right == 0 && relative->upRight == 0 && relative->up))
                DrawLine(screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2, screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight * 2, lineColor);

        // left open
        if (relative->left == 0) {
                if (relative->upLeft) {
                        DrawLine(1, screenHeightMid - wallHeight * 2, screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2, lineColor);
                        DrawLine(1, screenHeightMid + wallHeight * 2, screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight * 2, lineColor);
                }
                else {
                        DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight, 1, screenHeightMid - wallHeight, lineColor);
                        DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight, 1, screenHeightMid + wallHeight, lineColor);
                }
        }
        else {
                DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2, 1, 1, lineColor);
                DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight * 2, 1, screenHeight - 1, lineColor);
        }

        // right open
        if (relative->right == 0) {
                if (relative->upRight) {
                        DrawLine(screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2, screenWidth - 1, screenHeightMid - wallHeight * 2, lineColor);
                        DrawLine(screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight * 2, screenWidth - 1, screenHeightMid + wallHeight * 2, lineColor);
                }
                else {
                        DrawLine(screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight, screenWidth - 1, screenHeightMid - wallHeight, lineColor);
                        DrawLine(screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight, screenWidth - 1, screenHeightMid + wallHeight, lineColor);
                }
        }
        else {
                DrawLine(screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2, screenWidth - 1, 1, lineColor);
                DrawLine(screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight * 2, screenWidth - 1, screenHeight - 1, lineColor);
        }

        // up open
        if (relative->up == 0) {
                DrawLine(screenWidthMid - wallWidth, screenHeightMid - wallHeight, screenWidthMid + wallWidth, screenHeightMid - wallHeight, lineColor);
                DrawLine(screenWidthMid - wallWidth, screenHeightMid + wallHeight, screenWidthMid + wallWidth, screenHeightMid + wallHeight, lineColor);

                if (relative->upLeft)
                        DrawLine(screenWidthMid - wallWidth, screenHeightMid - wallHeight, screenWidthMid - wallWidth, screenHeightMid + wallHeight, lineColor);
                if (relative->upRight)
                        DrawLine(screenWidthMid + wallWidth, screenHeightMid - wallHeight, screenWidthMid + wallWidth, screenHeightMid + wallHeight, lineColor);
        }
        else {
                DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2, screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2, lineColor);
                DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight * 2, screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight * 2, lineColor);
        }

        // up left open
        if (relative->upLeft == 0 && relative->up == 0) {
                DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight, screenWidthMid - wallWidth, screenHeightMid - wallHeight, lineColor);
                DrawLine(screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight, screenWidthMid - wallWidth, screenHeightMid + wallHeight, lineColor);
        }
        else if (relative->up == 0) {
                DrawLine(screenWidthMid - wallWidth, screenHeightMid - wallHeight, screenWidthMid - wallWidth * 2, screenHeightMid - wallHeight * 2, lineColor);
                DrawLine(screenWidthMid - wallWidth, screenHeightMid + wallHeight, screenWidthMid - wallWidth * 2, screenHeightMid + wallHeight * 2, lineColor);
        }

        // up right open
        if (relative->upRight == 0 && relative->up == 0) {
                DrawLine(screenWidthMid + wallWidth, screenHeightMid - wallHeight, screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight, lineColor);
                DrawLine(screenWidthMid + wallWidth, screenHeightMid + wallHeight, screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight, lineColor);
        }
        else if (relative->up == 0) {
                DrawLine(screenWidthMid + wallWidth, screenHeightMid - wallHeight, screenWidthMid + wallWidth * 2, screenHeightMid - wallHeight * 2, lineColor);
                DrawLine(screenWidthMid + wallWidth, screenHeightMid + wallHeight, screenWidthMid + wallWidth * 2, screenHeightMid + wallHeight * 2, lineColor);
        }
}

int processInput(int arrayWidth, int arrayHeight, int array[][arrayWidth], Tiles *relative, GameData *gameData, bool btnLeftAction, bool btnRightAction, bool btnUpAction, bool btnDownAction, bool btnEditAction, bool btnDarkModeAction, bool btn3dAction) {
        int posX = gameData->posX;
        int posY = gameData->posY;

        // Keyboard toggles
        if (IsKeyPressed(KEY_ONE) || btnEditAction) {
                toggle(&(gameData->editMode));
        }
        else if (IsKeyPressed(KEY_TWO) || btnDarkModeAction) {
                toggle(&(gameData->darkMode));
        }
        else if (IsKeyPressed(KEY_THREE) || btn3dAction) {
                toggle(&(gameData->runIn3d));
        }
        else if (IsKeyPressed(KEY_FOUR)) {
                toggle(&(gameData->showFPS));
        }

        else if (IsKeyPressed(KEY_V) || (gameData->runIn3d && (IsKeyPressed(KEY_LEFT) || btnLeftAction))) {
                if (gameData->dir == UP)
                        gameData->dir = LEFT;
                else
                        gameData->dir--;
        }
        else if (IsKeyPressed(KEY_B) || (gameData->runIn3d && (IsKeyPressed(KEY_RIGHT) || btnRightAction))) {
                gameData->dir++;
                gameData->dir %= LEFT + 1;
        }

        else if (IsKeyPressed(KEY_LEFT) || (!gameData->runIn3d && btnLeftAction)) {
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
        else if (IsKeyPressed(KEY_RIGHT) || (!gameData->runIn3d && btnRightAction)) {
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
        else if (IsKeyPressed(KEY_UP) || btnUpAction) {
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
        else if (IsKeyPressed(KEY_DOWN) || btnDownAction) {
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

        else if (IsKeyPressed(KEY_BACKSPACE)) {
                return -1;
        }

        else if (gameData->editMode) {
                if (IsKeyPressed(KEY_A) && relative->left != 2) {
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
                else if (IsKeyPressed(KEY_D) && relative->right != 2) {
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
                else if (IsKeyPressed(KEY_W) && relative->up != 2) {
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
                else if (IsKeyPressed(KEY_X) && relative->down != 2) {
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
                else if (IsKeyPressed(KEY_Q) && relative->upLeft != 2) {
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
                else if (IsKeyPressed(KEY_E) && relative->upRight != 2) {
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
                else if (IsKeyPressed(KEY_Z) && relative->downLeft != 2) {
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
                else if (IsKeyPressed(KEY_C) && relative->downRight != 2) {
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

                else if (IsKeyPressed(KEY_ZERO)) {
                        for (int i = 1; i < arrayHeight - 1; i++) {
                                for (int j = 1; j < arrayWidth - 1; j++) {
                                        array[i][j] = 0;
                                }
                        }
                        gameData->posX = gameData->posY = 1;
                        gameData->winX = gameData->winY = 7;
                }
                else if (IsKeyPressed(KEY_NINE)) {
                        gameData->winX = gameData->posX;
                        gameData->winY = gameData->posY;
                }
        }

        return 0;
}

bool checkButtonPressed(Rectangle *rect) {
        if (CheckCollisionPointRec(GetMousePosition(), *rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                return true;

        return false;
}

int run(GameData *gameData, WindowConfig *windowData, int arrayWidth, int arrayHeight, int array[][arrayWidth]) {
        Tiles tiles;
        tiles.center = tiles.left = tiles.right = tiles.up = tiles.down = tiles.upLeft = tiles.upRight = tiles.downLeft = tiles.downRight = 0;

        checkBounds(arrayWidth, arrayHeight, array, &(tiles), gameData->posX, gameData->posY);

        Tiles relative;
        pointInDirection(gameData->dir, &(tiles), &(relative));

        int buttonMargin = 10;
        int buttonLength = 50;

        Rectangle btnLeft = { buttonMargin, gameData->screenHeight - buttonMargin - buttonLength, buttonLength, buttonLength };
        bool btnLeftAction = checkButtonPressed(&btnLeft);

        Rectangle btnRight = { buttonMargin * 3 + buttonLength * 2, gameData->screenHeight - buttonMargin - buttonLength, buttonLength, buttonLength };
        bool btnRightAction = checkButtonPressed(&btnRight);

        Rectangle btnUp = { buttonMargin * 2 + buttonLength, gameData->screenHeight - buttonMargin * 2 - buttonLength * 2, buttonLength, buttonLength };
        bool btnUpAction = checkButtonPressed(&btnUp);

        Rectangle btnDown = { buttonMargin * 2 + buttonLength, gameData->screenHeight - buttonMargin - buttonLength, buttonLength, buttonLength };
        bool btnDownAction = checkButtonPressed(&btnDown);

        Rectangle btnEdit = { gameData->screenWidth - buttonMargin * 3 - buttonLength * 3, gameData->screenHeight - buttonMargin - buttonLength, buttonLength, buttonLength };
        bool btnEditAction = checkButtonPressed(&btnEdit);

        Rectangle btnDarkMode = { gameData->screenWidth - buttonMargin * 2 - buttonLength * 2, gameData->screenHeight - buttonMargin - buttonLength, buttonLength, buttonLength };
        bool btnDarkModeAction = checkButtonPressed(&btnDarkMode);

        Rectangle btn3d = { gameData->screenWidth - buttonMargin - buttonLength, gameData->screenHeight - buttonMargin - buttonLength, buttonLength, buttonLength };
        bool btn3dAction = checkButtonPressed(&btn3d);

        if (processInput(arrayWidth, arrayHeight, array, &(relative), gameData, btnLeftAction, btnRightAction, btnUpAction, btnDownAction, btnEditAction, btnDarkModeAction, btn3dAction) != 0)
                return -1; // Go back to main menu

        BeginDrawing();
                if (gameData->runIn3d)
                        render3d(&(relative), gameData);
                else
                        render2d(&(relative), gameData);

                Color textColor = getDarkModeTextColor(gameData->darkMode);

                // Buttons
                DrawRectangleLines(btnLeft.x, btnLeft.y, buttonLength, buttonLength, textColor);
                DrawRectangleLines(btnRight.x, btnRight.y, buttonLength, buttonLength, textColor);
                DrawRectangleLines(btnUp.x, btnUp.y, buttonLength, buttonLength, textColor);
                DrawRectangleLines(btnDown.x, btnDown.y, buttonLength, buttonLength, textColor);

                DrawRectangleLines(btnEdit.x, btnEdit.y, buttonLength, buttonLength, textColor);
                DrawRectangleLines(btnDarkMode.x, btnDarkMode.y, buttonLength, buttonLength, textColor);
                DrawRectangleLines(btn3d.x, btn3d.y, buttonLength, buttonLength, textColor);

                // Text in top left corner to signify edit mode
                if (gameData->editMode) {
                        DrawText("EDIT", 5, 5, windowData->textSize, textColor);
                }

                if (gameData->showFPS)
                        DrawFPS(5, 30);
        EndDrawing();

        // Win maze
        if (gameData->posX == gameData->winX && gameData->posY == gameData->winY && !gameData->editMode)
                return 1;

        return 0;
}

void showInfoWindow(char *text[], int textLen, GameData *gameData, WindowConfig *windowData, GameState *gameState) {
        int screenHeight = gameData->screenHeight;

        int textSize = windowData->textSize;
        int margin = windowData->margin;
        int paddingVert = windowData->paddingVert;

        int textX = windowData->textX;

        int rectWidth = windowData->rectWidth;

        if (IsKeyPressed(KEY_BACKSPACE) || IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                *gameState = SHOW_MAIN_MENU;

        BeginDrawing();
                updateDarkModeBG(gameData->darkMode);
                Color textColor = getDarkModeTextColor(gameData->darkMode);

                DrawRectangleLines(margin, margin, rectWidth, screenHeight - (margin << 1), textColor);
                int cursorPos = margin + paddingVert;

                for (int i = 0; i < textLen; i++) {
                        DrawText(text[i], textX, cursorPos, textSize, textColor);
                        cursorPos += paddingVert;
                }

                if (gameData->showFPS)
                        DrawFPS(5, 5);
        EndDrawing();
}

int showStartMenu(GameData *gameData, WindowConfig *windowData, int *selectedOption, GameState *gameState) {
        int screenHeight = gameData->screenHeight;

        int textSize = windowData->textSize;
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
                "TOGGLE SHOW FPS",
                "HELP/KEYBINDS",
                "EXIT",
        };
        int menuOptionsLen = sizeof(menuOptions) / sizeof(menuOptions[0]);

        int buttonMargin = 10;
        int buttonLength = 50;

        Rectangle btnUp = { buttonMargin + windowData->margin, gameData->screenHeight - buttonMargin * 2 - buttonLength * 2 - windowData->margin, buttonLength, buttonLength };
        bool btnUpAction = checkButtonPressed(&btnUp);

        Rectangle btnDown = { buttonMargin + windowData->margin, gameData->screenHeight - buttonMargin - buttonLength - windowData->margin, buttonLength, buttonLength };
        bool btnDownAction = checkButtonPressed(&btnDown);

        Rectangle btnEnter = { gameData->screenWidth - buttonMargin - windowData->margin - buttonLength, gameData->screenHeight - buttonMargin - buttonLength - windowData->margin, buttonLength, buttonLength };
        bool btnEnterAction = checkButtonPressed(&btnEnter);

        int status = 0;

        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_K) || btnUpAction) && *selectedOption > 0) {
                *selectedOption -= 1;
        }
        else if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_J) || btnDownAction) && *selectedOption < menuOptionsLen - 1) {
                *selectedOption += 1;
        }
        else if (IsKeyPressed(KEY_ENTER) || btnEnterAction) {
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
                        toggle(&(gameData->showFPS));
                        break;
                case 5:
                        *gameState = SHOW_HELP_MENU;
                        break;
                case 6:
                        status = -1;
                        break;
                }
        }

        BeginDrawing();
                updateDarkModeBG(gameData->darkMode);
                Color textColor = getDarkModeTextColor(gameData->darkMode);

                DrawRectangleLines(margin, margin, rectWidth, screenHeight - (margin << 1), textColor);

                // Buttons
                DrawRectangleLines(btnUp.x, btnUp.y, buttonLength, buttonLength, textColor);
                DrawRectangleLines(btnDown.x, btnDown.y, buttonLength, buttonLength, textColor);
                DrawRectangleLines(btnEnter.x, btnEnter.y, buttonLength, buttonLength, textColor);

                int cursorPos = margin + paddingVert;

                DrawText("CMAZE MENU", textX, cursorPos, textSize, textColor);

                DrawText((gameData->runIn3d) ? "Running in 3D mode" : "Running in 2D mode", paddingHoriz + 200, cursorPos, 20, textColor);
                if (gameData->editMode)
                        DrawText("EditMode", paddingHoriz + 430, cursorPos, textSize, textColor);

                cursorPos += paddingVert * 2;

                // Line to separate title and options
                DrawLine(margin, cursorPos, margin + rectWidth, cursorPos, textColor);

                cursorPos += paddingVert;

                for (int i = 0; i < menuOptionsLen; i++) {
                        if (i == *selectedOption)
                                DrawText(">", margin + (paddingHoriz >> 1), cursorPos, textSize, textColor);

                        DrawText(menuOptions[i], textX, cursorPos, textSize, textColor);
                        cursorPos += paddingVert;
                }

                if (gameData->showFPS)
                        DrawFPS(5, 5);
        EndDrawing();

        return status;
}

void init(GameData *gameData) {
        gameData->screenWidth = GetScreenWidth();
        gameData->screenHeight = GetScreenHeight();

        gameData->posX = gameData->posY = 1;
        gameData->winX = gameData->winY = 7;

        gameData->editMode = 0;
        gameData->darkMode = 1;
        gameData->runIn3d = 1;
        gameData->showFPS = 0;
        gameData->dir = UP;
}

void initWindowConfig(WindowConfig *windowData, GameData *gameData) {
        windowData->textSize = 20;

        windowData->margin = 30;
        windowData->paddingHoriz = 22;
        windowData->paddingVert = 22;

        windowData->textX = windowData->margin + windowData->paddingHoriz;

        windowData->rectWidth = gameData->screenWidth - (windowData->margin << 1);
}

int main() {
        InitWindow(1024, 768, "cmaze");

        // Broken on web
        //ToggleFullscreen();

        SetTargetFPS(60);

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

        while (!WindowShouldClose()) {
                if (gameState == SHOW_MAIN_MENU) {
                        status = showStartMenu(&(gameData), &(windowData), &selectedOption, &gameState);
                        if (status == -1)
                                break;
                }
                else if (gameState == SHOW_HELP_MENU) {
                        char *help[] = {
                                "Arrow keys to move",
                                "Bracket keys or v,b to turn",
                                "'1' to toggle edit mode",
                                "'2' to toggle dark mode",
                                "'3' to switch to 2D/3D",
                                "'4' to toggle FPS counter",
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
                        status = run(&(gameData), &(windowData), arrayWidth, arrayHeight, array);

                        if (status == -1)
                                gameState = SHOW_MAIN_MENU;
                        else if (status == 1)
                                gameState = SHOW_WIN_MENU;
                }
                else if (gameState == SHOW_WIN_MENU) {
                        char *winText[] = {
                                "You got out of the maze!",
                                "Press esc to exit",
                        };
                        int winTextLen = sizeof(winText) / sizeof(winText[0]);
                        showInfoWindow(winText, winTextLen, &(gameData), &(windowData), &gameState);
                }
        }

        CloseWindow();

        return 0;
}
