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

void DrawHeart(float x, float y, float size, Color color)
{
    float radius = size * 0.30f;

    // Sol üst daire
    DrawCircle(
        x - radius,
        y - radius / 2,
        radius,
        color
    );

    // Sağ üst daire
    DrawCircle(
        x + radius,
        y - radius / 2,
        radius,
        color
    );

    // Alt üçgen
    DrawTriangle(
        (Vector2){x - size * 0.60f, y},
        (Vector2){x + size * 0.60f, y},
        (Vector2){x, y + size * 0.85f},
        color
    );
}

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
    float hitTimer = 0.0f;
    float denizKaymaX = 0.0f; // Denizin yatayda kayma miktarını tutacak
    float frogScale = 0.12f;
    float speed = 5.0f;
    float waterGraceTimer = 0.1f;


    // Görsel ve Ses Yükleme
    Texture2D yolDokusu = LoadTexture("assets/yol.jpg");
    Texture2D frogImage = LoadTexture("assets/frog.png");
    Texture2D asfaltResmi = LoadTexture("assets/asfalt.png");
    Texture2D girisResmi = LoadTexture("assets/giris.png");
    Texture2D kalpTexture = LoadTexture("assets/kalp.png");
    // Değişken ismini denizResmi yapıyoruz (Senin çizim kodunla uyumlu olsun)
    Texture2D denizResmi = LoadTexture("assets/deniz.png");
    
Texture2D carTextures[3]; // 3 tane araba için yer açtık
carTextures[0] = LoadTexture("assets/araba1.png"); // Dosya adların neyse onu yaz
carTextures[1] = LoadTexture("assets/araba2.png");
carTextures[2] = LoadTexture("assets/araba3.png");
Texture2D logTexture = LoadTexture("assets/log.png");
Texture2D yuvaBos = LoadTexture("assets/yuva_bos.png");
Texture2D yuvaDolu = LoadTexture("assets/yuva_dolu.png");
    bool inMenu = true;
    float yolKaymaX = 0.0f;
    float yolHizi = 100.0f;

    Sound jumpSound = LoadSound("assets/atlama_sesi.wav");
    SetSoundVolume(jumpSound, 0.1f);
    Sound crashSound = LoadSound("assets/araba_carpmasi.wav");
    Sound waterSound = LoadSound("assets/suya_dusme_sesi.wav");
    Sound successSound = LoadSound("assets/yuvaya_girme_sesi.wav");
    Sound levelUpSound = LoadSound("assets/level_atlama_sesi.wav");

    // --- KURULUMLAR ---
    car my_cars[3][2];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            my_cars[i][j].x = (float)j * 340 + (i * 60);
            my_cars[i][j].y = 335.0f + (i * 60);
            my_cars[i][j].width = 80.0f;
            my_cars[i][j].speed = 2.5f;
        }
    }

    log logs[3][2];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            logs[i][j].x = (float)j * 400;
            logs[i][j].y = 140.0f + (i * 65);
            logs[i][j].width = 160.0f;
            logs[i][j].speed = -2.00f;
            logs[i][j].sinkLevel = 0.0f;
            logs[i][j].isBeingSteppedOn = false;
        }
    }


        // frogPos başlangıç noktasını tamir ediyoruz:
    Vector2 frogPos = {
        (float)screenWidth / 2 - (frogImage.width * frogScale) / 2, 
        (float)screenHeight - (frogImage.height * frogScale) - 15 
    };

    Rectangle finishZones[5];
    bool zoneOccupied[5] = {false, false, false, false, false};
    for (int i = 0; i < 5; i++) {
        finishZones[i] = (Rectangle){(float)i * 160 + 40, 20, 80, 80};
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- GÜNCELLEME ---
        if (inMenu) {
            if (IsKeyPressed(KEY_ENTER)) {
                inMenu = false;
            }
        } 
        else {
            if (hitTimer > 0)
                hitTimer -= GetFrameTime();
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

            // 1. Ekran Sınırları Kontrolü
            // frogImage.width yerine (frogImage.width * frogScale) kullanıyoruz
            if (frogPos.x < 0) frogPos.x = 0;
            if (frogPos.x + (frogImage.width * frogScale) > screenWidth) 
                frogPos.x = screenWidth - (frogImage.width * frogScale);
            if (frogPos.y < 0) frogPos.y = 0;
            if (frogPos.y + (frogImage.height * frogScale) > screenHeight) 
                frogPos.y = screenHeight - (frogImage.height * frogScale);

            if (lives > 0 && !allFull) gameTimer -= GetFrameTime();

            if (gameTimer <= 0 && lives > 0) {

                gameTimer = maxTimer;

                // Can kaybetme bloklarının (lives--) içindeki frogPos atamasını şu yap:
            frogPos = (Vector2){
                (float)screenWidth / 2 - (frogImage.width * frogScale) / 2,
                (float)screenHeight - (frogImage.height * frogScale) - 10
            };

                lives--;
                hitTimer = 1.0f;
            }

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 2; j++) {
                    my_cars[i][j].x += my_cars[i][j].speed;
                    if (my_cars[i][j].x > screenWidth) my_cars[i][j].x = -my_cars[i][j].width;

                    logs[i][j].x += logs[i][j].speed;
                    if (logs[i][j].x < -logs[i][j].width) logs[i][j].x = (float)screenWidth;

                    // 2. Araba Çarpışma Kontrolü
                    if (CheckCollisionRecs(
                        (Rectangle){frogPos.x, frogPos.y, (frogImage.width * frogScale), (frogImage.height * frogScale)},
                        (Rectangle){my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30}))
                    {
                        PlaySound(crashSound);

                        // Öldüğünde veya resetlendiğinde burayı kullan:
                        frogPos = (Vector2){
                            (float)screenWidth / 2 - (frogImage.width * frogScale) / 2,
                            (float)screenHeight - (frogImage.height * frogScale) - 15
                        };

                        lives--;

                        hitTimer = 1.0f;
                    }
                }
            }

            // --- NEHİR VE KÜTÜK SİSTEMİ (KESİN ÇÖZÜM) ---
            bool onLog = false;
            // Nehir alanı sınırları: 120 (üst) ile 320 (asfaltın başı) arası
            if (frogPos.y < 310 && frogPos.y > 115) { 
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 2; j++) {
                        // KÜTÜK ÇARPIŞMA KONTROLÜ (70 piksel yüksekliğinde, seni asla ıskalamaz)
                        if (CheckCollisionRecs(
                            (Rectangle){frogPos.x, frogPos.y, (frogImage.width * frogScale), (frogImage.height * frogScale)},
                            (Rectangle){logs[i][j].x, logs[i][j].y + 10, logs[i][j].width, 50})) 
                        {
                            logs[i][j].isBeingSteppedOn = true;
                            if (logs[i][j].sinkLevel < 0.8f) {
                                onLog = true;
                                frogPos.x += logs[i][j].speed; // Kütükle beraber kaydır
                            }
                        } else {
                            logs[i][j].isBeingSteppedOn = false;
                        }

                        // Batma mekaniği
                        if (logs[i][j].isBeingSteppedOn) logs[i][j].sinkLevel += GetFrameTime() * 0.7f;
                        else logs[i][j].sinkLevel -= GetFrameTime() * 0.8f;

                        if (logs[i][j].sinkLevel > 1.0f) logs[i][j].sinkLevel = 1.0f;
                        if (logs[i][j].sinkLevel < 0.0f) logs[i][j].sinkLevel = 0.0f;
                    }
                }

                // EĞER NEHİRDEYSEK VE HİÇBİR KÜTÜĞE BASMIYORSAK:
                if (!onLog && hitTimer <= 0 && frogPos.y < 300) {
                    PlaySound(waterSound); // SES ŞİMDİ GELECEK
                    
                    // Başlangıç noktasına ışınla
                    frogPos = (Vector2){
                        (float)screenWidth / 2 - (frogImage.width * frogScale) / 2,
                        (float)screenHeight - (frogImage.height * frogScale) - 15
                    };
                    lives--;
                    hitTimer = 1.0f;
                }
            }

            allFull = true;
            for (int i = 0; i < 5; i++) {
                if (CheckCollisionRecs((Rectangle){
                    frogPos.x,
                    frogPos.y,
                    frogImage.width * frogScale,
                    frogImage.height * frogScale
                }, finishZones[i]) && !zoneOccupied[i]) {
                    PlaySound(successSound);
                    zoneOccupied[i] = true;
                    score += 100;
                    frogPos = (Vector2){
                        (float)screenWidth / 2 - (frogImage.width * frogScale) / 2,
                        (float)screenHeight - (frogImage.height * frogScale) - 15
                    };
                }
                if (!zoneOccupied[i]) allFull = false;
            }

            if (allFull) {
                PlaySound(levelUpSound);
                level++;
                if (level % 3 == 0 && lives < 5) {
                    lives++;
                }
                score += 500;
                gameTimer = maxTimer;
                for (int i = 0; i < 5; i++) zoneOccupied[i] = false;
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 2; j++) {
                        my_cars[i][j].speed += 0.5f;
                        logs[i][j].speed -= 0.5f;
                    }
                }
                allFull = false;
            }
        }

        // --- ÇİZİM ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (inMenu) {
           // Resmi ekran boyutuna (800x600) sığdırarak çiziyoruz
            Rectangle sourceRec = { 0.0f, 0.0f, (float)girisResmi.width, (float)girisResmi.height };
            Rectangle destRec = { 0.0f, 0.0f, 800.0f, 600.0f };
            DrawTexturePro(girisResmi, sourceRec, destRec, (Vector2){ 0, 0 }, 0.0f, WHITE);
            DrawText("BASLAMAK ICIN ENTER'A BAS", 200, 500, 25, DARKGRAY);
        } 
        else {
            // --- DENİZ AKIŞI (TAMİR EDİLDİ) ---
            if (denizResmi.id > 0) {
                // Denizi kütüklerin hızına göre kaydırıyoruz
                // logs[0][0].speed değeri -2.0f olduğu için deniz sola kayacak
                denizKaymaX += logs[0][0].speed; 

                // EĞER RESİM SOLA KAYIYORSA (Hız negatifse)
                if (denizKaymaX <= -denizResmi.width) {
                    denizKaymaX = 0;
                }
                
                // EĞER RESİM SAĞA KAYIYORSA (Hız pozitifse)
                if (denizKaymaX >= denizResmi.width) {
                    denizKaymaX = 0;
                }

                // Yan yana iki parça çizerek sonsuz döngü sağlıyoruz
                // Y konumunu senin koduna göre 120'ye sabitledim
                DrawTextureEx(denizResmi, (Vector2){denizKaymaX, 120}, 0.0f, 1.0f, WHITE);
                DrawTextureEx(denizResmi, (Vector2){denizKaymaX + denizResmi.width, 120}, 0.0f, 1.0f, WHITE);
                DrawTextureEx(denizResmi, (Vector2){denizKaymaX - denizResmi.width, 120}, 0.0f, 1.0f, WHITE); // Güvenlik için bir tane de sola
            } else {
                DrawRectangle(0, 120, 800, 200, SKYBLUE);
            }
            DrawRectangle(0, 0, 800, 120, DARKGRAY);

            // --- KÜTÜKLERİ GÖRSELLE ÇİZ ---
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
        if (logTexture.id > 0) {
            // Kaynak resmin tamamını alıyoruz
            Rectangle sourceRec = { 0.0f, 0.0f, (float)logTexture.width, (float)logTexture.height };
            // Kütüğü tam olarak kodundaki genişliğe (width) sığdırıyoruz
            Rectangle destRec = { logs[i][j].x, logs[i][j].y, logs[i][j].width, 70 }; 
            
            // Çizim ve batma efekti
            DrawTexturePro(logTexture, sourceRec, destRec, (Vector2){0, 0}, 0.0f, Fade(WHITE, 1.0f - logs[i][j].sinkLevel));
        } else {
            // Görsel yüklenmezse diye eski kahverengi kutu (güvenlik önlemi)
            DrawRectangle(logs[i][j].x, logs[i][j].y, logs[i][j].width, 70, Fade(BROWN, 1.0f - logs[i][j].sinkLevel));
        }
    }
}
            // --- YENİ NEON ASFALTI ÇİZ ---
            if (asfaltResmi.id > 0) {
                // Kaynak resmin (asfalt.png) tamamını alıyoruz
                Rectangle sourceRec = { 0.0f, 0.0f, (float)asfaltResmi.width, (float)asfaltResmi.height };
                // Yolu ekranda nereye (Y:320) ve hangi boyutta (800x200) çizeceğimiz
                Rectangle destRec = { 0.0f, 320.0f, 800.0f, 200.0f }; 
                
                DrawTexturePro(asfaltResmi, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
            } else {
                // Görsel yüklenemezse yedek gri zemin
                DrawRectangle(0, 320, 800, 200, GRAY);
            }

           

            DrawRectangle(0, 520, 800, 80, DARKGRAY);

            // --- ARAÇLARI ÇİZ ---
for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
        if (carTextures[i].id > 0) {
            // Arabalar çok büyükse 0.1f yap, çok küçükse 0.2f veya 0.3f yap
            float scale = 0.008f; 
            DrawTextureEx(carTextures[i], (Vector2){my_cars[i][j].x, my_cars[i][j].y}, 0.0f, scale, WHITE);
        } else {
            // Görsel yüklenmezse diye yedek kırmızı kutu
            DrawRectangle(my_cars[i][j].x, my_cars[i][j].y, my_cars[i][j].width, 30, RED);
        }
    }
}

            DrawRectangle(20, 530, 200, 15, BLACK);
            DrawRectangle(20, 530, (int)(200 * (gameTimer / maxTimer)), 15, GREEN);

            // --- HEDEFLERİ (YUVANIN KENDİSİNİ) ÇİZ ---
            for (int i = 0; i < 5; i++) {
                
                // GÖRSELİ BÜYÜTME AYARI:
                // Normal kutudan daha büyük çizmek için yeni bir dikdörtgen (destRec) oluşturuyoruz.
                // Ortalı durması için X ve Y'den 15 piksel geriye çektik, genişlik ve yüksekliği 80 yaptık.
                // Eğer hala küçük gelirse 80 sayılarını 90 veya 100 yapabilirsin, -15'leri de -20 falan yaparsın.
                Rectangle destRec = { finishZones[i].x - 15, finishZones[i].y - 15, 120, 120 };

                if (zoneOccupied[i]) {
                    // YUVA DOLUYSA: Dolu görselini çiz
                    if (yuvaDolu.id > 0) {
                        Rectangle sourceRec = { 0.0f, 0.0f, (float)yuvaDolu.width, (float)yuvaDolu.height };
                        // finishZones[i] yerine kendi hazırladığımız büyük destRec'i kullanıyoruz
                        DrawTexturePro(yuvaDolu, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
                    } else {
                        DrawRectangleRec(finishZones[i], DARKGREEN); 
                    }
                } 
                else {
                    // YUVA BOŞSA: Boş görselini çiz
                    if (yuvaBos.id > 0) {
                        Rectangle sourceRec = { 0.0f, 0.0f, (float)yuvaBos.width, (float)yuvaBos.height };
                        // Burada da aynı büyük destRec'i kullanıyoruz
                        DrawTexturePro(yuvaBos, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
                    } else {
                        DrawRectangleLinesEx(finishZones[i], 3, GREEN); 
                    }
                }
            }

            if (lives > 0) {
                if (hitTimer > 0 && ((int)(hitTimer * 10)) % 2 == 0) {
                    // Hasar aldığında yanıp sönme efektiyle çiz
                    DrawTextureEx(frogImage, frogPos, 0.0f, frogScale, WHITE);
                } else {
                    // Normal çizim
                    DrawTextureEx(frogImage, frogPos, 0.0f, frogScale, WHITE);
                }
            }
// --- KALPLERİ ÇİZ ---
            for (int i = 1; i <= lives; i++) {
                float posX = -70.0f + (i * 45.0f);
                float posY = 525.0f; // Level yazısıyla aynı hizaya çektik

                // Görsel başarıyla yüklendiyse
                if (kalpTexture.id > 0) {
                    // O devasa resmi (ne boyutta olursa olsun) zorla 50x50'lik bir alana sıkıştırıyoruz
                    Rectangle sourceRec = { 0.0f, 0.0f, (float)kalpTexture.width, (float)kalpTexture.height };
                    Rectangle destRec = { posX, posY, 150.0f, 100.0f }; 
                    
                    DrawTexturePro(kalpTexture, sourceRec, destRec, (Vector2){0, 0}, 0.0f, WHITE);
                } 
                // Eğer görsel DOSYA YOLU veya UZANTISI yüzünden okunamadıysa
                else {
                    // Ekrana KIRMIZI KARELER çiz ki görselin hiç yüklenemediğini kör olmadan görelim
                    DrawRectangle((int)posX, (int)posY, 40, 40, RED);
                }
            }
            
            DrawText(TextFormat("LEVEL: %d", level), 250, 565, 25, GOLD);
            DrawText(TextFormat("SKOR: %05d", score), 580, 565, 25, GREEN);

            if (lives <= 0) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
                DrawText("OYUN BITTI! RESTART ICIN 'R'", 200, 280, 30, RED);
                if (IsKeyPressed(KEY_R)) {
                    lives = 3; level = 1; score = 0; gameTimer = maxTimer;
                    for (int i = 0; i < 5; i++) zoneOccupied[i] = false;
                    frogPos = (Vector2){
                        (float)screenWidth / 2 - (frogImage.width * frogScale) / 2,
                        (float)screenHeight - (frogImage.height * frogScale) - 15
                    };
                }
            }
        }
        EndDrawing();
    }
    UnloadTexture(logTexture);
    for (int i = 0; i < 3; i++) {
        UnloadTexture(carTextures[i]); // 3 arabayı da hafızadan siliyoruz
    }
    UnloadTexture(kalpTexture); // Kalbi hafızadan sil
    UnloadTexture(frogImage);
    UnloadTexture(asfaltResmi);
    UnloadTexture(yolDokusu);
    UnloadTexture(girisResmi);
    UnloadSound(jumpSound);
    UnloadSound(crashSound);
    UnloadSound(waterSound);
    UnloadSound(successSound);
    UnloadSound(levelUpSound);
    UnloadTexture(yuvaBos);
    UnloadTexture(yuvaDolu);
    UnloadTexture(denizResmi);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}