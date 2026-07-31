#include <tiny3d.h>
#include <libfont.h>
#include <pngdec/pngdec.h>
#include <io/pad.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "KittyTetris.h"

int cherryBombFont;
u32 kittyTexture;
u32 kittyTextureOffset;

void initAll() {
    tiny3d_Init(1024 * 1024);
    
    ResetFont();
    InitFont();
    // EBOOT.BIN icinden cagrilirken dogru konumlanmasi icin USRDIR yolu
    cherryBombFont = LoadFont("CherryBomb", "/dev_hdd0/game/KITTY0001/USRDIR/fonts/CherryBombOne.ttf");

    pngData png;
    pngLoadFromFile("/dev_hdd0/game/KITTY0001/USRDIR/images/kitty.png", &png);
    
    if (png.bmp_out) {
        kittyTexture = tiny3d_AllocTexture(png.width * png.height * 4);
        kittyTextureOffset = tiny3d_TextureOffset(kittyTexture);
        memcpy((void*)kittyTexture, png.bmp_out, png.width * png.height * 4);
        free(png.bmp_out);
    }
}

void drawNeonRect(float x, float y, float width, float height, u32 color) {
    tiny3d_SetPolygon(TINY3D_LINES);
    tiny3d_VertexPos(x, y, 0);                 tiny3d_VertexColor(color);
    tiny3d_VertexPos(x + width, y, 0);         tiny3d_VertexColor(color);
    
    tiny3d_VertexPos(x + width, y, 0);         tiny3d_VertexColor(color);
    tiny3d_VertexPos(x + width, y + height, 0); tiny3d_VertexColor(color);
    
    tiny3d_VertexPos(x + width, y + height, 0); tiny3d_VertexColor(color);
    tiny3d_VertexPos(x, y + height, 0);         tiny3d_VertexColor(color);
    
    tiny3d_VertexPos(x, y + height, 0);         tiny3d_VertexColor(color);
    tiny3d_VertexPos(x, y, 0);                 tiny3d_VertexColor(color);
    tiny3d_End();
}

void drawNeonBlock(float x, float y, float size, u32 color) {
    drawNeonRect(x, y, size, size, color);
    
    tiny3d_SetPolygon(TINY3D_QUADS);
    u32 fillColor = (color & 0xFFFFFF00) | 0x33; // %20 Opaklik dolgusu
    tiny3d_VertexPos(x + 3, y + 3, 0);             tiny3d_VertexColor(fillColor);
    tiny3d_VertexPos(x + size - 3, y + 3, 0);     tiny3d_VertexColor(fillColor);
    tiny3d_VertexPos(x + size - 3, y + size - 3, 0); tiny3d_VertexColor(fillColor);
    tiny3d_VertexPos(x + 3, y + size - 3, 0);     tiny3d_VertexColor(fillColor);
    tiny3d_End();
}

void drawImage(float x, float y, float width, float height) {
    tiny3d_SetTexture(0, kittyTextureOffset, width, height, width * 4, TINY3D_TEX_FORMAT_A8R8G8B8, 1);
    tiny3d_SetPolygon(TINY3D_QUADS);
    
    tiny3d_VertexPos(x, y, 0);                 tiny3d_VertexTexture(0.0f, 0.0f);
    tiny3d_VertexPos(x + width, y, 0);         tiny3d_VertexTexture(1.0f, 0.0f);
    tiny3d_VertexPos(x + width, y + height, 0);tiny3d_VertexTexture(1.0f, 1.0f);
    tiny3d_VertexPos(x, y + height, 0);        tiny3d_VertexTexture(0.0f, 1.0f);
    tiny3d_End();
}

void drawNeonPanel(float x, float y, float width, float height, const char* title, const char* value, u32 color) {
    drawNeonRect(x, y, width, height, color);
    SetCurrentFont(cherryBombFont);
    
    SetFontColor(0x5A6E85FF, 0x00000000); 
    SetFontSize(16, 22);
    DrawString(x + 20, y + 15, title);
    
    SetFontColor(0xFFFFFFFF, 0x00000000);
    SetFontSize(28, 38);
    DrawString(x + 20, y + 45, value);
}

void drawLobbyScreen() {
    SetCurrentFont(cherryBombFont);
    SetFontColor(0xFFFFFFFF, 0x00000000);
    SetFontSize(50, 65);
    DrawString(280, 240, "KITTY TETRIS");

    SetFontColor(0xFF0066FF, 0x00000000);
    SetFontSize(25, 35);
    DrawString(330, 310, "KITTY STUDIOS");

    drawImage(570, 305, 40, 40);

    drawNeonRect(250, 400, 350, 80, 0x00F0FFFF); 
    SetFontColor(0xFFFFFFFF, 0x00000000);
    SetFontSize(30, 40);
    DrawString(370, 420, "BASLA X");
}

void drawGameScreen(const KittyTetris& game) {
    float panelX = 80.0f;
    float panelW = 300.0f;

    char scoreStr[16], levelStr[16], linesStr[16];
    sprintf(scoreStr, "%06d", game.getScore());
    sprintf(levelStr, "%d", game.getLevel());
    sprintf(linesStr, "%d", game.getLinesCleared());

    drawNeonPanel(panelX, 100, panelW, 110, "SKOR", scoreStr, 0x00F0FFFF);
    drawNeonPanel(panelX, 240, panelW, 110, "SEVIYE", levelStr, 0xFFFF00FF);
    drawNeonPanel(panelX, 380, panelW, 110, "SATIR", linesStr, 0x00FF00FF);

    float boardTop = 80.0f;
    float availableHeight = 600.0f;
    float blockSize = availableHeight / 20.0f;
    float boardWidth = blockSize * 10.0f;
    float boardStartX = 550.0f;

    drawNeonRect(boardStartX, boardTop, boardWidth, availableHeight, 0xFF0066FF);

    for (int r = 0; r < 20; r++) {
        for (int c = 0; c < 10; c++) {
            u32 color = game.getColorAt(r, c);
            if (color != 0) {
                drawNeonBlock(boardStartX + c * blockSize, boardTop + r * blockSize, blockSize, color);
            }
        }
    }

    if (!game.getIsGameOver()) {
        auto piece = game.getCurrentPiece();
        int pX = game.getPieceX();
        int pY = game.getPieceY();
        u32 color = game.getCurrentPieceColor();

        for (size_t r = 0; r < piece.size(); r++) {
            for (size_t c = 0; c < piece[r].size(); c++) {
                if (piece[r][c] != 0 && (pY + (int)r >= 0)) {
                    drawNeonBlock(boardStartX + (pX + c) * blockSize, boardTop + (pY + r) * blockSize, blockSize, color);
                }
            }
        }
    } else {
        SetCurrentFont(cherryBombFont);
        SetFontColor(0xFF3333FF, 0x00000000);
        SetFontSize(40, 55);
        DrawString(560, 300, "GAME OVER");
        SetFontSize(20, 30);
        SetFontColor(0xFFFFFFFF, 0x00000000);
        DrawString(560, 370, "START BAS RESET");
    }
}

int main() {
    padInit(7);
    initAll();
    
    KittyTetris game;
    int currentScreen = 0; // 0: Lobby, 1: Game
    int dropFrameCounter = 0;
    int inputCooldown = 0;

    while (1) {
        tiny3d_Clear(0xff0b0e14, TINY3D_CLEAR_ALL);
        tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);
        tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
                            TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO);

        padData pad;
        padGetData(0, &pad);

        if (inputCooldown > 0) inputCooldown--;

        if (currentScreen == 0) {
            drawLobbyScreen();
            if (pad.BTN_CROSS && inputCooldown == 0) {
                currentScreen = 1;
                game.resetGame();
                inputCooldown = 15;
            }
        } else {
            // Oyun Dongusu Girdileri
            if (inputCooldown == 0) {
                if (pad.BTN_LEFT) { game.moveLeft(); inputCooldown = 7; }
                else if (pad.BTN_RIGHT) { game.moveRight(); inputCooldown = 7; }
                else if (pad.BTN_DOWN) { game.moveDown(); inputCooldown = 4; }
                else if (pad.BTN_CROSS) { game.rotate(); inputCooldown = 12; }
                
                if (game.getIsGameOver() && pad.BTN_START) {
                    game.resetGame();
                    inputCooldown = 15;
                }
            }

            // Yercekimi Tetikleyicisi
            dropFrameCounter++;
            if (dropFrameCounter >= game.getGameSpeedFrames()) {
                game.moveDown();
                dropFrameCounter = 0;
            }

            drawGameScreen(game);
        }

        tiny3d_Flip();
    }
    
    return 0;
}
