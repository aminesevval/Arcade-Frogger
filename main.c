#include "raylib.h"

typedef struct {
    float x;
    float y;
    float speed;
    float width;
} car;

typedef struct {
    float x;
    float y;
    float speed;
    float width;
    float sinkLevel;
    bool isBeingSteppedOn;
} log;

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Arcade Frogger - Amine Sevval");
    InitAudioDevice();

    int lives = 3;
    int level = 1;
    int score = 0;
    float gameTimer = 30.0f;
    float maxTimer = 30.0f;
    bool allFull = false;

    // Görsel ve Ses Yükleme
    Texture2D yolDokusu = LoadTexture("assets/yol.jpg");
    Texture2D frogImage = LoadTexture("assets/frog.png");
    Texture2D asfaltResmi = LoadTexture("assets/asfalt.png");

    float yolKaymaX = 0.0f;
    float yolHizi = 100.0f;

    Sound jumpSound = LoadSound("assets/atlama_sesi.wav");
    Sound crashSound = LoadSound("assets/araba_carpmasi.wav");
    Sound waterSound = LoadSound("assets/suya_dusme_sesi.wav");
    Sound successSound = LoadSound("assets/yuvaya_girme_sesi.wav");
    Sound levelUpSound = LoadSound("assets/level_atlama_sesi.wav");

    // --- KURULUMLAR ---
    car my_cars[3][2];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            my_cars[i][j].x = j * 340 + (i * 60);
            my_cars[i][j].y = 335 + (i * 60);
            my_cars[i][j].width = 80;
            my_cars[i][j].speed = 2.5f;
        }
    }

    log logs[3][2];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            logs[i][j].x = (float)j * 400;
            logs[i][j].y = 140 + (i * 65);
            logs[i][j].width = 160;
            logs[i][j].speed = -2.00f;
            logs[i][j].sinkLevel = 0.0f;
            logs[i][j].isBeingSteppedOn = false;
        }
    }

    Vector2 frogPos = {(float)screenWidth / 2 - (float)frogImage.width / 2, (float)screenHeight - (float)frogImage.height - 20};
    float speed = 5.0f;

    Rectangle finishZones[5];
    bool zoneOccupied[5] = {false, false, false, false, false};
    for (int i = 0; i < 5; i++) {
        finishZones[i] = (Rectangle){(float)i * 160 + 40, 20, 80, 80};
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- GÜNCELLEME ---
        if (yolDokusu.id > 0) {
            yolKaymaX += yolHizi * GetFrameTime();
            if (yolKaymaX >= (float)yolDokusu.width) yolKaymaX = 0;
        }

        if (lives > 0) {
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) ||
                IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                PlaySound(jumpSound);
            }

            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) frogPos.x += speed;
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) frogPos.x -= speed;
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) frogPos.y -= speed;
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) frogPos.y += speed;
        }

        if (frogPos.x < 0) frogPos.x = 0;
        if (frogPos.x + (float)frogImage.width > (float)screenWidth) frogPos.x = (float)screenWidth - (float)frogImage.width;
        if (frogPos.y < 0) frogPos.y = 0;
        if (frogPos.y + (float)frogImage.height > (float)screenHeight) frogPos.y = (float)screenHeight - (float)frogImage.height;

        if (lives > 0 && !allFull) gameTimer -= GetFrameTime();

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                my_cars[i][j].x += my_cars[i][j].speed;
                if (my_cars[i][j].x > screenWidth) my_cars[i][j].x = -my_cars[i][j].width;

                logs[i][j].x += logs[i][j].speed;
                if (logs[i][j].x < -logs[i][j].width) logs[i][j].x = (float)screenWidth;

                if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height},
                                       (Rectangle){my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30})) {
                    PlaySound(crashSound);
                    frogPos = (Vector2){(float)screenWidth / 2 - (float)frogImage.width / 2, (float)screenHeight - (float)frogImage.height - 20};
                    lives--;
                }
            }
        }

        bool onLog = false;
        if (frogPos.y + (float)frogImage.height < 320 && frogPos.y > 120) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 2; j++) {
                    if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height},
                                           (Rectangle){logs[i][j].x, logs[i][j].y, logs[i][j].width, 30})) {
                        logs[i][j].isBeingSteppedOn = true;
                        if (logs[i][j].sinkLevel < 0.8f) {
                            onLog = true;
                            frogPos.x += logs[i][j].speed;
                        }
                    } else logs[i][j].isBeingSteppedOn = false;

                    if (logs[i][j].isBeingSteppedOn) logs[i][j].sinkLevel += GetFrameTime() * 0.7f;
                    else logs[i][j].sinkLevel -= GetFrameTime() * 0.8f;

                    if (logs[i][j].sinkLevel > 1.0f) logs[i][j].sinkLevel = 1.0f;
                    if (logs[i][j].sinkLevel < 0.0f) logs[i][j].sinkLevel = 0.0f;
                }
            }
            if (!onLog) {
                PlaySound(waterSound);
                frogPos = (Vector2){(float)screenWidth / 2 - (float)frogImage.width / 2, (float)screenHeight - (float)frogImage.height - 20};
                lives--;
            }
        }

        allFull = true;
        for (int i = 0; i < 5; i++) {
            if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height}, finishZones[i]) && !zoneOccupied[i]) {
                PlaySound(successSound);
                zoneOccupied[i] = true;
                score += 100;
                frogPos = (Vector2){(float)screenWidth / 2 - (float)frogImage.width / 2, (float)screenHeight - (float)frogImage.height - 20};
                gameTimer = maxTimer;
            }
            if (!zoneOccupied[i]) allFull = false;
        }

        if (allFull) {
            PlaySound(levelUpSound);
            level++;
            score += 500;
            for (int i = 0; i < 5; i++) zoneOccupied[i] = false;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 2; j++) {
                    my_cars[i][j].speed += 0.5f;
                    logs[i][j].speed -= 0.5f;
                }
            }
            allFull = false;
        }

        // --- ÇİZİM ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(0, 0, 800, 120, DARKGRAY);
        DrawRectangle(0, 120, 800, 200, SKYBLUE);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                DrawRectangle(logs[i][j].x, logs[i][j].y, logs[i][j].width, 30, Fade(BROWN, 1.0f - logs[i][j].sinkLevel));
            }
        }

        DrawRectangle(0, 320, 800, 200, GRAY);
        if (yolDokusu.id > 0) {
            for (int i = 0; i < (screenWidth / yolDokusu.width) + 2; i++) {
                DrawTextureRec(yolDokusu,
                               (Rectangle){0, 0, (float)yolDokusu.width, (float)yolDokusu.height},
                               (Vector2){(float)(i * yolDokusu.width) - yolKaymaX, 320},
                               WHITE);
            }
        }

        for (int i = -1; i < 10; i++) {
            float xPos = i * 100 + ((int)yolKaymaX % 100);
            DrawRectangle(xPos, 385, 40, 5, WHITE);
            DrawRectangle(xPos, 455, 40, 5, WHITE);
        }

        DrawRectangle(0, 520, 800, 80, DARKGRAY);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                DrawRectangle(my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30, RED);
            }
        }

        DrawRectangle(20, 530, 200, 15, BLACK);
        DrawRectangle(20, 530, (int)(200 * (gameTimer / maxTimer)), 15, GREEN);

        for (int i = 0; i < 5; i++) {
            if (zoneOccupied[i]) DrawRectangleRec(finishZones[i], DARKGREEN);
            else DrawRectangleLinesEx(finishZones[i], 3, GREEN);
        }

        if (lives > 0) DrawTextureV(frogImage, frogPos, WHITE);
        
        DrawText(TextFormat("CAN: %d", lives), 20, 565, 25, RED);
        DrawText(TextFormat("LEVEL: %d", level), 250, 565, 25, GOLD);
        DrawText(TextFormat("SKOR: %05d", score), 580, 565, 25, GREEN);

        if (lives <= 0) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
            DrawText("OYUN BITTI! RESTART ICIN 'R'", 200, 280, 30, RED);
            if (IsKeyPressed(KEY_R)) {
                lives = 3; level = 1; score = 0; gameTimer = maxTimer;
                for (int i = 0; i < 5; i++) zoneOccupied[i] = false;
                frogPos = (Vector2){(float)screenWidth / 2 - (float)frogImage.width / 2, (float)screenHeight - (float)frogImage.height - 20};
            }
        }
        EndDrawing();
    }

    UnloadTexture(frogImage);
    UnloadTexture(asfaltResmi);
    UnloadTexture(yolDokusu);
    UnloadSound(jumpSound);
    UnloadSound(crashSound);
    UnloadSound(waterSound);
    UnloadSound(successSound);
    UnloadSound(levelUpSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}