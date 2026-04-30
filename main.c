#include "raylib.h"
typedef struct 
{
    float x;
    float y;    
    float speed;
    float width;
}car;

int main(void)
{
    // --- 1. AYARLAR VE PENCERE AÇILIŞI ---
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Pencereyi oluşturuyoruz
    InitWindow(screenWidth, screenHeight, "Arcade Frogger - Amine Sevval");
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

    // --- 2. GÖRSEL YÜKLEME ---
    // Dikkat: assets klasöründe frog.png dosyanın olduğundan emin ol!
    Texture2D frogImage = LoadTexture("assets/frog.png");

    // Kurbağanın başlangıç pozisyonu (Ekranın alt-orta kısmı)
    // Resmin boyutlarını hesaba katarak tam merkeze oturtuyoruz
    Vector2 frogPos = { 
        (float)screenWidth/2 - (float)frogImage.width/2, 
        (float)screenHeight - (float)frogImage.height - 20 
    };

    float speed = 5.0f; // Hareket hızı

    SetTargetFPS(60); // Oyunun saniyede 60 kare çalışmasını sağlar

    // --- 3. ANA OYUN DÖNGÜSÜ ---
    while (!WindowShouldClose())
    {
        
        // --- GÜNCELLEME (HAREKET) ---
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) frogPos.x += speed;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) frogPos.x -= speed;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) frogPos.y -= speed;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) frogPos.y += speed;

        // --- SINIR KONTROLLERİ (Kaçış Engelleme) ---
        // Sol sınır
        if (frogPos.x < 0) frogPos.x = 0;
        
        // Sağ sınır (Ekran genişliğinden kurbağanın genişliğini çıkarıyoruz)
        if (frogPos.x + frogImage.width > screenWidth) {
            frogPos.x = screenWidth - frogImage.width;
        }

        // Üst sınır
        if (frogPos.y < 0) frogPos.y = 0;

        // Alt sınır (Ekran yüksekliğinden kurbağanın yüksekliğini çıkarıyoruz)
        if (frogPos.y + frogImage.height > screenHeight) {
            frogPos.y = screenHeight - frogImage.height;
        }
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

        // --- ÇİZİM AŞAMASI ---
        BeginDrawing();

            ClearBackground(RAYWHITE); // Ekranı her karede temizle (Beyaz arka plan)
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 2; j++) {
                    // Arabaları kırmızı dikdörtgen olarak çiziyoruz
                    DrawRectangle(my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30, RED);
                }
            }

            // 2. KÜTÜKLERİ ÇİZ (Senin döngün)
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 2; j++) {
                    // Kütükleri kahverengi dikdörtgen olarak çiziyoruz
                    DrawRectangle(logs[i][j].x, logs[i][j].y, logs[i][j].width, 30, BROWN);
                }
            }
            
            // Kurbağayı yeni pozisyonunda çiz
            DrawTextureV(frogImage, frogPos, WHITE);
            
            // Yardımcı metinler
            DrawText("Yon tuslari veya WASD ile hareket et", 10, 10, 20, DARKGRAY);
            DrawText("Sinirlar artik guvende!", 10, 40, 20, GREEN);

        EndDrawing();
    }

    // --- 4. KAPANIŞ ---
    UnloadTexture(frogImage); // Hafızayı boşalt (Mekatronik disiplini!)
    CloseWindow();

    return 0;
}