#pragma once

class ColorMatchGame : public ArcadeGame {
private:
    int playerShape; // 0=Square, 1=Circle, 2=Triangle
    float yPos;
    int targetShape;
    int score;
    int state;
    bool lastL, lastR;

public:
    const char* getName() override { return "Color Match"; }
    int getScore() override { return score; }

    void init() override {
        score = 0; state = 0; playerShape = 0;
        yPos = 0; targetShape = random(3);
        lastL = true; lastR = true;
    }

    void drawShape(int shape, int x, int y, bool filled) {
        if (shape == 0) { // Square
            if (filled) display.fillRect(x-5, y-5, 10, 10, WHITE);
            else display.drawRect(x-5, y-5, 10, 10, WHITE);
        } else if (shape == 1) { // Circle
            if (filled) display.fillCircle(x, y, 5, WHITE);
            else display.drawCircle(x, y, 5, WHITE);
        } else if (shape == 2) { // Triangle
            if (filled) display.fillTriangle(x, y-5, x-5, y+5, x+5, y+5, WHITE);
            else display.drawTriangle(x, y-5, x-5, y+5, x+5, y+5, WHITE);
        }
    }

    void update() override {
        if (state == 0) {
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            if (cL && !lastL) { playerShape--; if (playerShape < 0) playerShape = 2; }
            if (cR && !lastR) { playerShape++; if (playerShape > 2) playerShape = 0; }
            
            lastL = cL; lastR = cR;

            yPos += 1.5 + (score * 0.1);
            if (yPos > 45) { // Collision zone
                if (playerShape == targetShape) {
                    score++;
                    yPos = 0;
                    targetShape = random(3);
                } else {
                    state = 1; delay(500);
                }
            }

            display.clearDisplay();
            
            // Draw Target Shape (falling)
            drawShape(targetShape, 64, (int)yPos, true);

            // Draw Player Shape
            drawShape(playerShape, 64, 55, false);

            display.setCursor(0, 0); display.print("Score:"); display.print(score);
            display.display();
            delay(15);
        } else {
            display.clearDisplay();
            display.setCursor(30, 20); display.print("MISMATCH!");
            display.setCursor(30, 35); display.print("Score: "); display.print(score);
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(ColorMatchGame)
