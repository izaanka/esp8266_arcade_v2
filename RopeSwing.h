#pragma once

class RopeSwingGame : public ArcadeGame {
private:
    float angle, aVel;
    float rx, ry; // Rope anchor
    float px, py; // Player pos when falling
    int score;
    int state; // 0=swing, 1=fall, 2=loss
    bool lastS;

public:
    const char* getName() override { return "Rope Swing"; }
    int getScore() override { return score; }

    void init() override {
        rx = 64; ry = -20; angle = PI/4; aVel = 0; score = 0; state = 0;
        lastS = true;
    }

    void update() override {
        bool cS = (digitalRead(btnSelect) == LOW);

        if (state == 0) { // Swinging
            aVel -= sin(angle) * 0.01;
            angle += aVel;
            
            px = rx + sin(angle) * 60;
            py = ry + cos(angle) * 60;

            if (cS && !lastS) { // Detach
                state = 1;
                // simplified physics for jump
                aVel *= 15; // velocity multiplier for falling
            }
        } else if (state == 1) { // Falling
            px += cos(angle) * aVel; 
            py += 2.0; // gravity

            // Generate next rope logic
            float nextRx = 64; float nextRy = ry;
            float dx = px - nextRx; float dy = py - nextRy;
            if (dx*dx + dy*dy < 60*60 + 200) { // Caught next rope
                // Snap to new rope
                angle = atan2(dx, dy);
                aVel = 0; // reset momentum
                state = 0;
                score++;
            }

            if (py > 64) { state = 2; delay(500); } // Fell
        } else if (state == 2) {
            display.clearDisplay();
            display.setCursor(40, 20); display.print("SPLAT!");
            display.setCursor(40, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
            return;
        }

        lastS = cS;

        display.clearDisplay();
        
        // Draw active rope
        if (state == 0 || (state == 1 && py < 40)) { // simplify drawing
            display.drawLine((int)rx, (int)ry, (int)(rx+sin(angle)*60), (int)(ry+cos(angle)*60), WHITE);
        }

        // Draw Player
        display.fillRect((int)px - 3, (int)py - 3, 6, 6, WHITE);

        display.setCursor(0, 0); display.print(score);
        display.display();
        delay(15);
    }
};

REGISTER_GAME(RopeSwingGame)
