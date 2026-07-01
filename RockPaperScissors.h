#pragma once

class RockPaperScissorsGame : public ArcadeGame {
private:
    int pScore, aiScore;
    int state; // 0=pick, 1=show, 2=result
    int pPick, aiPick; // 0=Rock, 1=Paper, 2=Scissors
    int timer;

public:
    const char* getName() override { return "R-P-S"; }
    int getScore() override { return pScore; }

    void init() override {
        pScore = 0; aiScore = 0; state = 0; timer = 100;
    }

    void drawHand(int pick, int x, int y) {
        if (pick == 0) { // Rock
            display.fillCircle(x, y, 8, WHITE);
        } else if (pick == 1) { // Paper
            display.fillRect(x-6, y-8, 12, 16, WHITE);
        } else if (pick == 2) { // Scissors
            display.drawLine(x, y+8, x-6, y-6, WHITE);
            display.drawLine(x, y+8, x+6, y-6, WHITE);
            display.drawCircle(x-6, y-6, 2, WHITE);
            display.drawCircle(x+6, y-6, 2, WHITE);
        }
    }

    void update() override {
        if (state == 0) {
            timer--;
            bool cL = (digitalRead(btnLeft) == LOW);
            bool cS = (digitalRead(btnSelect) == LOW);
            bool cR = (digitalRead(btnRight) == LOW);

            pPick = -1;
            if (cL) pPick = 0;
            if (cS) pPick = 1;
            if (cR) pPick = 2;

            if (pPick != -1 || timer <= 0) {
                if (timer <= 0) pPick = random(3);
                aiPick = random(3);
                state = 1;
                timer = 100; // wait time
                delay(200);
            }

            display.clearDisplay();
            display.setCursor(10, 0); display.print("ROCK PAPER SCISSORS");
            display.setCursor(10, 20); display.print("Left:   ROCK");
            display.setCursor(10, 30); display.print("Select: PAPER");
            display.setCursor(10, 40); display.print("Right:  SCISSORS");
            display.setCursor(50, 55); display.print(timer/10);
            display.display();
            delay(15);
        } else if (state == 1) { // Show
            timer--;
            if (timer <= 0) {
                if (pPick == aiPick) {
                    // Tie
                } else if ((pPick == 0 && aiPick == 2) || (pPick == 1 && aiPick == 0) || (pPick == 2 && aiPick == 1)) {
                    pScore++;
                } else {
                    aiScore++;
                }
                
                if (pScore >= 3 || aiScore >= 3) state = 2;
                else state = 0; timer = 100;
            }

            display.clearDisplay();
            display.setCursor(10, 0); display.print("YOU");
            display.setCursor(90, 0); display.print("AI");
            
            drawHand(pPick, 20, 30);
            drawHand(aiPick, 95, 30);

            if (pPick == aiPick) { display.setCursor(50, 25); display.print("TIE!"); }
            else if ((pPick == 0 && aiPick == 2) || (pPick == 1 && aiPick == 0) || (pPick == 2 && aiPick == 1)) {
                display.setCursor(50, 25); display.print("WIN!");
            } else {
                display.setCursor(50, 25); display.print("LOSE");
            }
            
            display.setCursor(10, 50); display.print(pScore);
            display.setCursor(90, 50); display.print(aiScore);
            display.display();
            delay(15);
        } else if (state == 2) {
            display.clearDisplay();
            display.setCursor(30, 20); 
            if (pScore >= 3) display.print("YOU WON!");
            else display.print("AI WON!");
            if (digitalRead(btnSelect) == LOW) { init(); delay(200); }
            if (digitalRead(btnLeft) == LOW) { returnToMenu(); delay(200); }
            display.display();
        }
    }
};

REGISTER_GAME(RockPaperScissorsGame)
