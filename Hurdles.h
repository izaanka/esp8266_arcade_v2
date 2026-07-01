#pragma once

class HurdlesGame : public ArcadeGame {
private:
    float speed;
    float dist;
    float pY, pVY;
    bool jumping;
    int state; // 0=countdown, 1=race, 2=finish
    
    struct Hurdle { float x; };
    Hurdle hurdles[3];
    int lastBtn;
    int timeMS;
    unsigned long startTime;

public:
    const char* getName() override { return "Hurdles"; }

    void init() override {
        speed = 0; dist = 0; pY = 50; pVY = 0; jumping = false;
        state = 0; lastBtn = -1; timeMS = 0;
        for(int i=0; i<3; i++) hurdles[i].x = 100 + i*150;
    }

    void update() override {
        if (state == 0) {
            display.clearDisplay();
            display.setCursor(40, 25); display.print("READY!");
            if (millis() % 1000 < 100) { state = 1; startTime = millis(); delay(200); }
            display.display();
            return;
        } else if (state == 1) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);

            // Mash L/R to build speed
            if (cL && lastBtn != 0) { speed += 0.5; lastBtn = 0; }
            if (cR && lastBtn != 1) { speed += 0.5; lastBtn = 1; }
            speed -= 0.1; // friction
            if (speed < 0) speed = 0;
            if (speed > 8) speed = 8;

            if (cS && !jumping) { jumping = true; pVY = -4.5; }

            if (jumping) {
                pY += pVY; pVY += 0.4;
                if (pY >= 50) { pY = 50; jumping = false; }
            }

            dist += speed;

            for(int i=0; i<3; i++) {
                hurdles[i].x -= speed;
                if (hurdles[i].x < -10) hurdles[i].x += 450; // loop hurdles
                
                // Collision
                if (hurdles[i].x > 10 && hurdles[i].x < 18) {
                    if (pY > 40) { speed = 0; hurdles[i].x += 20; } // hit hurdle, penalty
                }
            }

            timeMS = millis() - startTime;
            if (dist > 1500) { state = 2; delay(500); } // 1500m race

        } else if (state == 2) {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("FINISH!");
            display.setCursor(30, 35); display.print("Time: "); display.print(timeMS / 1000.0);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }

        display.clearDisplay();
        display.drawLine(0, 50, 128, 50, WHITE);

        // Player
        display.fillRect(10, (int)pY - 10, 8, 10, WHITE);

        // Hurdles
        for(int i=0; i<3; i++) {
            if (hurdles[i].x >= 0 && hurdles[i].x <= 128) {
                display.drawLine((int)hurdles[i].x, 50, (int)hurdles[i].x, 40, WHITE);
                display.drawLine((int)hurdles[i].x-2, 40, (int)hurdles[i].x+2, 40, WHITE);
            }
        }

        // Distance bar
        display.drawRect(10, 5, 108, 4, WHITE);
        display.fillRect(10, 5, (int)(dist / 1500.0 * 108), 4, WHITE);

        display.display();
        delay(15);
    }
};

REGISTER_GAME(HurdlesGame)
