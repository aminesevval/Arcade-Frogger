#include "raylib.h"

typedef struct {
    float x; float y; float speed; float width;
} car;

int main(void) {
    int lives = 3;
    int level = 1; // Seviye değişkenini ekledik
    int score = 0; // Skor değişkeni 
    const int screenWidth = 800;
    const int screenHeight = 600;
    bool allFull = false; 

    InitWindow(screenWidth, screenHeight, "Arcade Frogger - Amine Sevval");

    // --- 1. KURULUMLAR ---
    car my_cars[3][2];
    for (int i = 0; i < 3; i++) {
        for(int j=0; j<2; j++){
            my_cars[i][j].x = j * 340 + (i * 60);
            my_cars[i][j].y = 335 + (i * 60); 
            my_cars[i][j].width = 80;
            my_cars[i][j].speed = 2.5f; // Başlangıç hızı
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

        // Nesne Hareketleri ve Çarpışmalar
        for(int i=0; i<3;i++){
            for(int j=0; j<2; j++){
                my_cars[i][j].x += my_cars[i][j].speed;
                if (my_cars[i][j].x > screenWidth) my_cars[i][j].x = -my_cars[i][j].width;

                logs[i][j].x += logs[i][j].speed;
                if (logs[i][j].x < -logs[i][j].width) logs[i][j].x = (float)screenWidth;

                // Araba Çarpışması
                if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height}, 
                                       (Rectangle){my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30})) {
                    frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
                    lives--;
                }
            }
        }

        // Nehir Kontrolü
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
                frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
                lives--;
            }
        }

        // Yuva Kontrolü ve Level Atlama Mantığı
        allFull = true;
        for (int i = 0; i < 5; i++) {
            if (CheckCollisionRecs((Rectangle){frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height}, finishZones[i]) && !zoneOccupied[i]) {
                zoneOccupied[i] = true;
                score += 100;
                frogPos = (Vector2){ (float)screenWidth/2 - (float)frogImage.width/2, (float)screenHeight - (float)frogImage.height - 20 };
            }
            if (!zoneOccupied[i]) allFull = false;
        }

        // EĞER HER YER DOLDUYSA: Level Atlat
        if (allFull) {
            level++; // Seviyeyi artır
            score += 500; // Bonus puan
            for (int i = 0; i < 5; i++) zoneOccupied[i] = false; // Yuvaları boşalt
            allFull = false; // Kazanma bayrağını indir
            
            // Hızları Artır
            for(int i=0; i<3; i++) {
                for(int j=0; j<2; j++) {
                    my_cars[i][j].speed += 0.5f; 
                    logs[i][j].speed -= 0.5f; 
                }
            }
        }

        // --- 3. ÇİZİM ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(0, 0, 800, 120, DARKGRAY);   // Finish
        DrawRectangle(0, 120, 800, 200, SKYBLUE);  // Nehir
        DrawRectangle(0, 320, 800, 200, GRAY);     // Yol
        DrawTexture(asfaltResmi, 0, 320, WHITE);   // Yol Görseli
        DrawRectangle(0, 520, 800, 80, DARKGRAY); // Start

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
        
        // Göstergeler - Temiz ve Aralıklı Düzen
        DrawText(TextFormat("CAN: %d", lives), 20, 565, 25, RED);
        DrawText(TextFormat("LEVEL: %d", level), 250, 565, 25, GOLD); // 200 yerine 250 yaparak boşluk bıraktık
        DrawText(TextFormat("SKOR: %05d", score), 580, 565, 25, GREEN);

        // Game Over Ekranı
        if (lives <= 0) {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
            DrawText("OYUN BITTI!", 300, 280, 40, RED);
            DrawText("Tekrar oynamak icin R'ye bas", 250, 330, 20, LIGHTGRAY);
            if (IsKeyPressed(KEY_R)) {
                lives = 3;
                level = 1;
                score = 0;
                for (int i = 0; i < 5; i++) zoneOccupied[i] = false;
                // Arabaların hızlarını başlangıca çek
                for(int i=0; i<3; i++) {
                    for(int j=0; j<2; j++) {
                        my_cars[i][j].speed = 2.5f;
                        logs[i][j].speed = -2.0f;
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