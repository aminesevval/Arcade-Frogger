#include "raylib.h"

typedef struct {
    float x; float y; float speed; float width;
} car;

int main(void) {
    int lives = 3;
    const int screenWidth = 800;
    const int screenHeight = 600;
    bool allFull = false; // Değişkeni buraya ekledik

    InitWindow(screenWidth, screenHeight, "Arcade Frogger - Amine Sevval");

    // --- KURULUMLAR ---
    car my_cars[3][2];
    for (int i = 0; i < 3; i++) {
        for(int j=0; j<2; j++){
            my_cars[i][j].x = j * 300 + (i * 50);
            my_cars[i][j].y = 120 + i * 40;
            my_cars[i][j].width = 80;
            my_cars[i][j].speed = 2.0f;
        }
    }

    car logs[3][2];
    for(int i=0; i<3; i++){
        for(int j=0; j<2; j++){
            logs[i][j].x = (float)j * 400;
            logs[i][j].y = 320 + i * 40;
            logs[i][j].width = 160;
            logs[i][j].speed = -2.00f;
        }
    }

    Texture2D frogImage = LoadTexture("assets/frog.png");
    Vector2 frogPos = { (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
    float speed = 5.0f;

    Rectangle finishZones[5];
    bool zoneOccupied[5] = { false, false, false, false, false };
    for (int i = 0; i < 5; i++) {
        finishZones[i] = (Rectangle){ (float)i * 160 + 40, 20, 80, 80 }; 
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- GÜNCELLEME ---
        
        // Sadece can varsa ve oyun bitmediyse hareket et
        if (lives > 0 && !allFull) {
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) frogPos.x += speed;
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) frogPos.x -= speed;
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) frogPos.y -= speed;
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) frogPos.y += speed;
        }

        // Sınırlar
        if (frogPos.x < 0) frogPos.x = 0;
        if (frogPos.x + frogImage.width > screenWidth) frogPos.x = (float)screenWidth - frogImage.width;
        if (frogPos.y < 0) frogPos.y = 0;
        if (frogPos.y + frogImage.height > screenHeight) frogPos.y = (float)screenHeight - frogImage.height;

        // Nesne Hareketleri ve Araba Çarpışması
        for(int i=0; i<3;i++){
            for(int j=0; j<2; j++){
                my_cars[i][j].x += my_cars[i][j].speed;
                if (my_cars[i][j].x > screenWidth) my_cars[i][j].x = -my_cars[i][j].width;

                logs[i][j].x += logs[i][j].speed;
                if (logs[i][j].x < -logs[i][j].width) logs[i][j].x = (float)screenWidth;

                // Çarpışma Kontrolü
                if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height}, 
                                       (Rectangle){my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30})) {
                    frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
                    lives--;
                }
            }
        }

        // Nehir Kontrolü
        if (frogPos.y < 430 && frogPos.y > 320) {
            bool onLog = false;
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 2; j++) {
                    if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height}, 
                                           (Rectangle){logs[i][j].x, logs[i][j].y, logs[i][j].width, 30})) {
                        onLog = true;
                        frogPos.x += logs[i][j].speed;
                    }
                }
            }
            if (!onLog) {
                frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
                lives--;
            }
        }

        // Kazanma Kontrolü
        allFull = true;
        for (int i = 0; i < 5; i++) {
            if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height}, finishZones[i]) && !zoneOccupied[i]) {
                zoneOccupied[i] = true;
                frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
            }
            if (!zoneOccupied[i]) allFull = false;
        }

        // --- ÇİZİM ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(0, 0, 800, 120, DARKGRAY);   // Finish
        DrawRectangle(0, 120, 800, 200, GRAY);     // Yol
        DrawRectangle(0, 320, 800, 110, SKYBLUE);  // Nehir
        DrawRectangle(0, 430, 800, 70, GRAY);      // Ara Yol
        DrawRectangle(0, 500, 800, 100, DARKGRAY); // Start

        for (int i = 0; i < 5; i++) {
            if (zoneOccupied[i]) DrawRectangleRec(finishZones[i], DARKGREEN); 
            else DrawRectangleLinesEx(finishZones[i], 3, GREEN);
        }

        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 2; j++) {
                DrawRectangle(my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30, RED);
                DrawRectangle(logs[i][j].x, logs[i][j].y, logs[i][j].width, 30, BROWN);
            }
        }
        
        if (lives > 0) DrawTextureV(frogImage, frogPos, WHITE);
        
        // Can Göstergesi
        DrawText(TextFormat("CAN: %d", lives), 20, 560, 20, RED);

        if (allFull) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(GOLD, 0.8f));
            DrawText("TEBRİKLER!", 300, 280, 30, DARKGRAY);
            if (IsKeyPressed(KEY_R)) { 
                for (int i = 0; i < 5; i++) zoneOccupied[i] = false; 
                lives = 3;
            }
        }

        if (lives <= 0) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
            DrawText("OYUN BITTI!", 300, 280, 40, RED);
            DrawText("Yeniden baslamak icin R'ye bas", 260, 340, 20, LIGHTGRAY);
            if (IsKeyPressed(KEY_R)) {
                lives = 3;
                frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
                for (int i = 0; i < 5; i++) zoneOccupied[i] = false;
                allFull = false;
            }
        }

        EndDrawing();
    }

    UnloadTexture(frogImage);
    CloseWindow();
    return 0;
}