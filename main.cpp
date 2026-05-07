#include "raylib.h"
typedef struct 
{
    float x;
    float y;    
    float speed;
    float width;
}car;
int main(void) {
  
    int lives = 3;
    int level = 1; 
    int score = 0; 
    float gameTimer = 30.0f;
    float maxTimer = 30.0f;
    const int screenWidth = 800;
    const int screenHeight = 600;
    bool allFull = false; 

    InitWindow(screenWidth, screenHeight, "Arcade Frogger - Amine Sevval");
    InitAudioDevice(); // Ses cihazını başlat

    // Sesleri Yükle
    Sound jumpSound = LoadSound("assets/atlama_sesi.wav");
    Sound crashSound = LoadSound("assets/araba_carpmasi.wav");
    Sound waterSound = LoadSound("assets/suya_dusme_sesi.wav");
    Sound successSound = LoadSound("assets/yuvaya_girme_sesi.wav");
    Sound levelUpSound = LoadSound("assets/level_atlama_sesi.wav");

    // --- 1. KURULUMLAR ---
    car my_cars[3][2];
    for (int i = 0; i < 3; i++) {
        for(int j=0; j<2; j++){
            my_cars[i][j].x = j * 340 + (i * 60);
            my_cars[i][j].y = 335 + (i * 60); 
            my_cars[i][j].width = 80;
            my_cars[i][j].speed = 2.5f;
        }
    }

    car logs[3][2];
    for(int i=0; i<3; i++){
        for(int j=0; j<2; j++){
            logs[i][j].x = (float)j * 400;
            logs[i][j].y = 140 + (i * 65); 
            logs[i][j].width = 160;
            logs[i][j].speed = -2.00f;
        }
    }

    Texture2D frogImage = LoadTexture("assets/frog.png");
    Texture2D asfaltResmi = LoadTexture("assets/asfalt.png");
    Vector2 frogPos = { (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
    float speed = 5.0f;

    Rectangle finishZones[5];
    bool zoneOccupied[5] = { false, false, false, false, false };
    for (int i = 0; i < 5; i++) {
        finishZones[i] = (Rectangle){ (float)i * 160 + 40, 20, 80, 80 }; 
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- 2. GÜNCELLEME ---
        if (lives > 0) {
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || 
                IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                PlaySound(jumpSound); // Her harekette atlama sesi
            }

            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) frogPos.x += speed;
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) frogPos.x -= speed;
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) frogPos.y -= speed;
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) frogPos.y += speed;
        }

        if (frogPos.x < 0) frogPos.x = 0;
        if (frogPos.x + frogImage.width > screenWidth) frogPos.x = (float)screenWidth - frogImage.width;
        if (frogPos.y < 0) frogPos.y = 0;
        if (frogPos.y + frogImage.height > screenHeight) frogPos.y = (float)screenHeight - frogImage.height;

        if (lives > 0 && !allFull) {
            gameTimer -= GetFrameTime();
        }

        for(int i=0; i<3;i++){
            for(int j=0; j<2; j++){
                my_cars[i][j].x += my_cars[i][j].speed;
                if (my_cars[i][j].x > screenWidth) my_cars[i][j].x = -my_cars[i][j].width;

                logs[i][j].x += logs[i][j].speed;
                if (logs[i][j].x < -logs[i][j].width) logs[i][j].x = (float)screenWidth;

                if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height}, 
                                       (Rectangle){my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30})) {
                    PlaySound(crashSound); // Araba çarpma sesi
                    frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
                    lives--;
                }
            }
        }

        if (frogPos.y + frogImage.height < 320 && frogPos.y > 120) {
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
                PlaySound(waterSound); // Suya düşme sesi
                frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
                lives--;
            }
        }

        allFull = true;
        for (int i = 0; i < 5; i++) {
            if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height}, finishZones[i]) && !zoneOccupied[i]) {
                PlaySound(successSound); // Yuvaya girme sesi
                zoneOccupied[i] = true;
                score += 100;
                frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
            }
            if (!zoneOccupied[i]) allFull = false;
        }

        if (gameTimer <= 0) {
            PlaySound(waterSound); // Süre bitince (suya düşme sesiyle aynı efekt)
            lives--;
            gameTimer = maxTimer;
            frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
        }

        if (allFull) {
            PlaySound(levelUpSound); // Level atlama sesi
            level++;
            score += 500;
            gameTimer = maxTimer;
            for (int i = 0; i < 5; i++) zoneOccupied[i] = false;
            allFull = false;
            for(int i=0; i<3; i++) {
                for(int j=0; j<2; j++) {
                    my_cars[i][j].speed += 0.5f; 
                    logs[i][j].speed -= 0.5f; 
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 0, 800, 120, DARKGRAY); 
        DrawRectangle(0, 120, 800, 200, SKYBLUE);
        DrawRectangle(0, 320, 800, 200, GRAY); 
        DrawTexture(asfaltResmi, 0, 320, WHITE);
        DrawRectangle(0, 520, 800, 80, DARKGRAY);
        DrawRectangle(20, 530, 200, 15, BLACK);
        DrawRectangle(20, 530, (int)(200 * (gameTimer / maxTimer)), 15, GREEN);
        DrawText("SURE", 230, 528, 18, RAYWHITE);

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
        
if (lives > 0) 
{  // <-- Bu parantezi açmamız lazım!
    
    // Çarpışma Kontrolü (Arabalar için)
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 2; j++) { // Senin arabaların 2 katlı olduğu için bunu ekledik
           if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, 50, 50}, my_cars[i][j].rec)) {
                lives--;              
                frogPos.x = 400;      // Kurbağayı ışınla
                frogPos.y = 400;
            }
        }
    }
// ... (burada çarpışma kontrolü for döngüleri bitti)
    } // Bu parantez çarpışma for döngüsünün sonu (yaklaşık 190. satır)

    // --- 4. HAFTA: BURAYA EKLE (Satır 191 gibi) ---
    if (frogPos.y <= 50) { 
        score += 100;      
        level++;           
        frogPos.x = 400;   // Başlangıca ışınla
        frogPos.y = 400;
        
        // Arabaları biraz hızlandır (Zorluk artsın)
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 2; j++) {
                my_cars[i][j].speed += 0.5f; 
            }
        }
    }
    // ----------------------------------------------

    // Kurbağayı Çiz
    DrawTextureV(frogImage, frogPos, WHITE);
    // Kurbağayı Çiz
    DrawTextureV(frogImage, frogPos, WHITE);
} 
        DrawText(TextFormat("CAN: %d", lives), 20, 565, 25, RED);
        DrawText(TextFormat("LEVEL: %d", level), 250, 565, 25, GOLD);
        DrawText(TextFormat("SKOR: %05d", score), 580, 565, 25, GREEN);

        if (lives <= 0) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
            DrawText("OYUN BITTI!", 300, 280, 40, RED);
            if (IsKeyPressed(KEY_R)) {
                lives = 3; level = 1; score = 0; gameTimer = maxTimer;
                for (int i = 0; i < 5; i++) zoneOccupied[i] = false;
                for(int i=0; i<3; i++) {
                    for(int j=0; j<2; j++) {
                        my_cars[i][j].speed = 2.5f; logs[i][j].speed = -2.0f;
                    }
                }
            }
        }
        EndDrawing();
    }

    UnloadTexture(frogImage);
    UnloadTexture(asfaltResmi);
    CloseWindow();
    return 0;
}