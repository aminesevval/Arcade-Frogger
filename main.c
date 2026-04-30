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
        // --- HAFTA 2: ÇARPIŞMA KONTROLLERİ ---
        Rectangle frogRec = { frogPos.x, frogPos.y, (float)frogImage.width, (float)frogImage.height };

        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 2; j++) {
                Rectangle carRec = { my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30 };
                if (CheckCollisionRecs(frogRec, carRec)) {
                    // Kurbağa ezildi, başa dön!
                    frogPos.x = (float)screenWidth/2 - (float)frogImage.width/2;
                    frogPos.y = (float)screenHeight - (float)frogImage.height - 20;
                }
            }
        }
        // -------------------------------------
        // --- HAFTA 2: SUYA DÜŞME KONTROLLERİ ---
        
        
        // Bizim örneğimizde kaldırım 500'de, asfalt 300'de başlıyor. 
        // 300'ün altı su bölgesi demektir (Y ekseni yukarı çıktıkça azalır).
        // 1. Kurbağa suyun olduğu bölgede mi? 
       // Kurbağa gerçekten kütüklerin olduğu nehir hizasında mı?
        if (frogPos.y < 430 && frogPos.y > 320) {
            
            bool kutugunUzerindeMi = false; // Başlangıçta kütükte değil sayıyoruz

            // 2. Acaba herhangi bir kütüğe temas ediyor mu?
            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 2; j++) {
                    // Kütüğün görünmez çarpışma kutusu (Yüksekliği yine 30 alalım)
                    Rectangle logRec = { logs[i][j].x, logs[i][j].y, logs[i][j].width, 30 };
                    
                    // Eğer kurbağanın kutusu bir kütüğe değerse:
                    if (CheckCollisionRecs(frogRec, logRec)) {
                        kutugunUzerindeMi = true; // Evet, kurtuldu!
                        
                        // EKSTRA GÖREV: Kurbağa kütüğün üzerindeyse, kütükle aynı hızda kaymalı!
                        frogPos.x += logs[i][j].speed; 
                    }
                }
            }

            // 3. Eğer su bölgesindeyse VE hiçbir kütüğe değmiyorsa:
            if (!kutugunUzerindeMi) {
                // KURBAĞA BOĞULDU! Başa dön...
                frogPos.x = (float)screenWidth/2 - (float)frogImage.width/2;
                frogPos.y = (float)screenHeight - (float)frogImage.height - 20;
            }
        }
        // ---------------------------------------
        // --- ÇİZİM AŞAMASI ---
        BeginDrawing();
        
        ClearBackground(RAYWHITE); 

        // ZEMİN BOYA İŞLEMLERİ
        // --- ÇİZİM AŞAMASI ---
        BeginDrawing();
        ClearBackground(RAYWHITE); 

        // 1. EN ÜST GÜVENLİ BÖLGEL (Finish Zone) - 0'dan 120'ye kadar
        DrawRectangle(0, 0, 800, 120, DARKGRAY);

        // 2. ÜST ASFALT (Arabaların yolu) - 120'den 320'ye kadar
        DrawRectangle(0, 120, 800, 200, GRAY); 

        // 3. NEHİR (Kütüklerin yolu) - 320'den 430'a kadar (Daha önce sabitlemiştik)
        DrawRectangle(0, 320, 800, 110, SKYBLUE);

        // 4. ALT ASFALT (Nehir ile Başlangıç arasındaki boşluk) - 430'dan 500'e kadar
        DrawRectangle(0, 430, 800, 70, GRAY);

        // 5. ALT KALDIRIM (Başlangıç bölgesi) - 500'den en aşağıya kadar
        DrawRectangle(0, 500, 800, 100, DARKGRAY);
        
        // ... arkadaşının yaptığı kırmızı arabaları çizen döngüler burada kalmaya devam edecek // Ekranı her karede temizle (Beyaz arka plan)
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