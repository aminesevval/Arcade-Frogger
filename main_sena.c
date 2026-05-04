#include "raylib.h"
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
     int lives = 3;

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
for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 2; j++) {
                // Arabayı yürüt
                my_cars[i][j].x += my_cars[i][j].speed;

                // Ekrandan çıkarsa başa döndür
                if (my_cars[i][j].x > screenWidth) {
                    my_cars[i][j].x = -my_cars[i][j].width;
                }

                // Çarpışma kontrolü
                if (CheckCollisionCircleRec(frogPos, 15, (Rectangle){my_cars[i][j].x, my_cars[i][j].y, 80, 40})) {
                    lives--;
                    frogPos = (Vector2){ 400, 560 };
                    if (lives <= 0) {
                        lives = 3;
                    }
                }
            }
        }// KAZANMA KONTROLÜ (4. Hafta Görevi)
        if (frogPos.y <= 40) { 
            frogPos = (Vector2){ 400, 560 }; // Karşıya ulaşınca başa dön
            // Buraya ileride skor ekleyebilirsin
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
        DrawText(TextFormat("CAN: %i", lives), 20, 20, 25, BLACK);
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