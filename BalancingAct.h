#pragma once

class BalancingActGame : public ArcadeGame {
private:
    float cx, angle, aVel;
    int score;
    int state;

public:
    virtual int getScore() override { return score; }
    const char* getName() override { return "Balancing Act"; }

    void init() override {
        cx = 64; angle = 0; aVel = 0; score = 0; state = 0;
    }

    void update() override {
        if (state == 0) {
            if (digitalRead(btnLeft) == LOW) { cx -= 2.0; aVel -= 0.05; }
            if (digitalRead(btnRight) == LOW) { cx += 2.0; aVel += 0.05; }
            
            // Inverted pendulum physics
            aVel += sin(angle) * 0.1; // gravity tips it further
            angle += aVel;
            
            // Random wind
            if (random(100) < 5) aVel += random(-5, 6) * 0.01;

            if (cx < 10) cx = 10;
            if (cx > 118) cx = 118;

            score++;

            if (abs(angle) > 1.2) { state = 1; delay(500); }
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("FELL OVER!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score/10);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }

        display.clearDisplay();
        display.drawLine(0, 50, 128, 50, WHITE); // Ground
        display.fillRect((int)cx - 6, 46, 12, 4, WHITE); // Cart
        
        // Pole
        float px = cx + sin(angle) * 30;
        float py = 46 - cos(angle) * 30;
        display.drawLine((int)cx, 46, (int)px, (int)py, WHITE);
        display.fillCircle((int)px, (int)py, 2, WHITE); // Weight

        display.setCursor(0, 0); display.print(score/10);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(BalancingActGame)
