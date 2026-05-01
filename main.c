#include "raylib.h"

typedef struct {
    float x;
    float y;    
    float speed;
    float width;
} car;

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Arcade Frogger - Amine Sevval");

    // --- 1. KURULUMLAR ---
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

    // --- HAFTA 3: HEDEF YUVALARI TANIMLAMA ---
    Rectangle finishZones[5];
    bool zoneOccupied[5] = { false, false, false, false, false };
    for (int i = 0; i < 5; i++) {
        finishZones[i] = (Rectangle){ (float)i * 160 + 40, 20, 80, 80 }; 
    }

    SetTargetFPS(60);

    // --- 2. ANA OYUN DÖNGÜSÜ ---
    while (!WindowShouldClose()) {
        // HAREKET
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) frogPos.x += speed;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) frogPos.x -= speed;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) frogPos.y -= speed;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) frogPos.y += speed;

        // SINIRLAR
        if (frogPos.x < 0) frogPos.x = 0;
        if (frogPos.x + frogImage.width > screenWidth) frogPos.x = (float)screenWidth - frogImage.width;
        if (frogPos.y < 0) frogPos.y = 0;
        if (frogPos.y + frogImage.height > screenHeight) frogPos.y = (float)screenHeight - frogImage.height;

        // NESNE HAREKETLERİ
        for(int i=0; i<3;i++){
            for(int j=0; j<2; j++){
                my_cars[i][j].x += my_cars[i][j].speed;
                if (my_cars[i][j].x > screenWidth) my_cars[i][j].x = -my_cars[i][j].width;
                logs[i][j].x += logs[i][j].speed;
                if (logs[i][j].x < -logs[i][j].width) logs[i][j].x = (float)screenWidth;
            }
        }

        // --- ÇARPIŞMA SENSÖRLERİ ---
        Rectangle frogRec = { frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height };

        // Araba Çarpışması
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 2; j++) {
                if (CheckCollisionRecs(frogRec, (Rectangle){ my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30 })) {
                    frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
                }
            }
        }

        // Su ve Kütük Kontrolü
        if (frogPos.y < 430 && frogPos.y > 320) {
            bool onLog = false;
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 2; j++) {
                    if (CheckCollisionRecs(frogRec, (Rectangle){ logs[i][j].x, logs[i][j].y, logs[i][j].width, 30 })) {
                        onLog = true;
                        frogPos.x += logs[i][j].speed; 
                    }
                }
            }
            if (!onLog) frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
        }

        // --- HAFTA 3: HEDEF VARIS SENSÖRÜ ---
        for (int i = 0; i < 5; i++) {
            if (CheckCollisionRecs(frogRec, finishZones[i]) && !zoneOccupied[i]) {
                zoneOccupied[i] = true;
                frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
            }
        }

        // --- 3. ÇİZİM AŞAMASI ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Zeminler
        DrawRectangle(0, 0, 800, 120, DARKGRAY);   // Finish
        DrawRectangle(0, 120, 800, 200, GRAY);     // Yol
        DrawRectangle(0, 320, 800, 110, SKYBLUE);  // Nehir
        DrawRectangle(0, 430, 800, 70, GRAY);      // Ara Yol
        DrawRectangle(0, 500, 800, 100, DARKGRAY); // Start

        // Yuvaları Çiz
        for (int i = 0; i < 5; i++) {
            if (zoneOccupied[i]) DrawRectangleRec(finishZones[i], DARKGREEN); 
            else DrawRectangleLinesEx(finishZones[i], 3, GREEN);
        }

        // Nesneleri ve Kurbağayı Çiz
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 2; j++) {
                DrawRectangle(my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30, RED);
                DrawRectangle(logs[i][j].x, logs[i][j].y, logs[i][j].width, 30, BROWN);
            }
        }
        DrawTextureV(frogImage, frogPos, WHITE);
        
        // --- HAFTA 3: KAZANMA EKRANI ---
        bool allFull = true;
        for (int i = 0; i < 5; i++) { if (!zoneOccupied[i]) allFull = false; }
        if (allFull) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(GOLD, 0.8f));
            DrawText("TEBRİKLER! BÖLÜM TAMAMLANDI", 180, 280, 30, DARKGRAY);
            DrawText("Tekrar oynamak icin R'ye bas", 250, 330, 20, DARKGRAY);
            if (IsKeyPressed(KEY_R)) { for (int i = 0; i < 5; i++) zoneOccupied[i] = false; }
        }

        EndDrawing();
    }

    // --- 4. KAPANIŞ ---
    UnloadTexture(frogImage);
    CloseWindow();
    return 0;
}
 /* 
}#include "raylib.h"
typedef struct 
{
    float x;
    float y;    
    float speed;
    float width;
}car;


int main() {
    // Ekran Boyutları
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int gridSize = 40; 

    InitWindow(screenWidth, screenHeight, "Frogger - 2. Gelistirici Grid");
    SetTargetFPS(60);

    car my_cars[3][2];
    for (int i = 0; i < 3; i++) {
        for(int j=0; j<2; j++){
            my_cars[i][j].x = j * 300+ (i * 50);
            my_cars[i][j].y = 120 + i * 40;
            my_cars[i][j].width = 80;
            my_cars[i][j].speed = 2.0f;
        }
    }
    car logs[3][2];
    for(int i=0; i<3; i++){
        for(int j=0; j<2; j++){
            logs[i][j].x = j * 400;
            logs[i][j].y = 320 + i * 40;
            logs[i][j].width = 160;
            logs[i][j].speed = -2.00f;
        }
    }
    while (!WindowShouldClose()) {
        for(int i=0; i<3;i++){
            for(int j=0; j<2; j++){
                my_cars[i][j].x += my_cars[i][j].speed;
                if (my_cars[i][j].x > screenWidth) {
                    my_cars[i][j].x = -my_cars[i][j].width; 
                }
        }
        }
        for(int i=0; i<3;i++){
            for(int j=0; j<2; j++){
                logs[i][j].x += logs[i][j].speed;
                if (logs[i][j].x < -logs[i][j].width) {
                    logs[i][j].x = screenWidth; 
                }
            }
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for(int i=0; i<3;i++){
            for(int j=0; j<2; j++){
                DrawRectangle(my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 40, RED);
        }
        }
        for(int i=0; i<3;i++){
            for(int j=0; j<2; j++){
                DrawRectangle(logs[i][j].x, logs[i][j].y, logs[i][j].width, 40, BROWN);
        }
        }
        // Dikey Çizgiler
        for (int i = 0; i <= screenWidth; i += gridSize) {
            DrawLine(i, 0, i, screenHeight, LIGHTGRAY);
        }
        // Yatay Çizgiler
        for (int i = 0; i <= screenHeight; i += gridSize) {
            DrawLine(0, i, screenWidth, i, LIGHTGRAY);
        }

        DrawText("Grid Sistemi Hazir!", 20, 20, 20, DARKGREEN);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
    */